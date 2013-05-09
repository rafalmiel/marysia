/*
*  Marysia database server
*  Copyright (C) 2011  Rafal Mielniczuk rafalm3@gmail.com
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "BTree.h"

#include <iostream>
#include <stdlib.h>

#include "buffer/Buffer.h"
#include "filestruct/BlockStorageManager.h"
#include "filestruct/Tablespace.h"
#include "page/data/PageIndexIntern.h"
#include "page/data/PageIndexLeaf.h"
#include "page/data/PageSystemSegment.h"
#include "page/PageFactory.h"
#include "page/tableinfo/PageTableInfo.h"
#include "table/KeyInfo.h"
#include "table/KeyValue.h"
#include "table/Row.h"
#include "table/ValueLongBlock.h"

using namespace std;

BTree::BTree(Buffer* buf):
    m_buffer( buf ),
    m_data_filename( "" ),
    m_tablespace( 0 ),
    m_block_storage_manager( 0 ),
    m_read_leaf_page( 0 )
{

}

/**
 * @brief inicjuje tabele do czytania sekwencyjnego
 *
 * @param key_name nazwa klucza, po ktorym bedziemy czytac
 * @return void
 **/
void BTree::readInit(const String& key_name)
{
    if (m_read_leaf_page)
    {
        m_buffer->savePage(m_read_leaf_page);
    }

    PageSystemSegment* syspage = m_tablespace->pageSystemSegment();
    key_entry_info_st* key_entry = syspage->keyEntryByName( key_name );
    m_read_leaf_page = m_tablespace->pageIndexLeaf( key_entry->data_page_id );
    m_read_pos = 0;
    m_buffer->savePage( syspage );
    delete key_entry;
}

/**
 * @brief pobiera wiersz o podanej wartosci klucza lub NULL gdy nie ma w tabeli wiersza o zadanej wartosci klucza
 *
 * @return Row*
 **/
Row* BTree::read(const String& key_name, KeyValue* key_value)
{
    PageIndex* cur_page = rootNode(key_name);

    page_id_t page_id;
    while (!cur_page->isLeaf()) {
        page_id = static_cast<PageIndexIntern*>(cur_page)->findMatchingNode(key_value);
        cur_page = m_tablespace->pageIndex( page_id );
    }

    bool found;
    uint16_t pos = cur_page->findDictEntryNum(key_value, found);
    if (found) {
        m_read_leaf_page = static_cast<PageIndexLeaf*>(cur_page);
        m_read_pos = pos;
        return m_read_leaf_page->rowAt(pos);
    }
    return NULL;
}

/**
 * @brief pobiera kolejny wiersz z tabeli, lub NULL gdy wewnetrzny kursor jest na koncu tabeli
 *
 * @return Row*
 **/
Row* BTree::readNext()
{
    if ( m_read_pos < m_read_leaf_page->rowsOnPage() )
    {
        /**
         * TODO: read long fields
         */
        return m_read_leaf_page->rowAt( m_read_pos++ );
    } else {
        page_id_t next_page_id = m_read_leaf_page->nextPage();
        if ( next_page_id != 0 )
        {
            m_buffer->savePage( m_read_leaf_page );
            m_read_leaf_page = m_tablespace->pageIndexLeaf( next_page_id );
            m_read_pos = 0;
            return readNext();
        } else
        {
            readClose();
            return NULL;
        }
    }
}

/**
 * @brief zamyka tabele do czytania sekwencyjnego
 *
 * @return void
 **/
void BTree::readClose()
{
    if ( m_read_leaf_page )
    {
        m_buffer->savePage( m_read_leaf_page );
    }
    m_read_leaf_page = 0;
    m_read_pos = 0;
}

/**
 * @brief Otwiera tabele do dalszych manipulacji
 *
 * @param filename nazwa pliku z danymi
 * @param page_table_info Strona ze strukturą tabeli
 * @param tablespace wskaznik na Tablespace
 * @return void
 **/
void BTree::openTable(const String& filename, PageTableInfo* page_table_info, Tablespace* tablespace, BlockStorageManager* block_storage_manager)
{
    m_data_filename = filename;
    m_page_tableinfo = page_table_info;
    m_tablespace = tablespace;
    m_block_storage_manager = block_storage_manager;
}

bool BTree::removeRebalance(page_id_t thisNodeId, page_id_t parentNodeId, page_id_t lAnchor, page_id_t rAnchor, KeyValue* key)
{
    page_id_t nextRAnchor = 0;
    page_id_t nextLAnchor = 0;
    page_id_t nextNodeId = 0;
    page_id_t doRebalance = false;

    PageIndex* thisNodePage = m_tablespace->pageIndex( thisNodeId );

    if ( thisNodePage->isIntern() )
    {
        uint16_t foundPos = thisNodePage->findMatchingEntry( key );
        nextNodeId = static_cast<PageIndexIntern*>( thisNodePage )->pagePtrAt( foundPos );

        if (foundPos == 0)
        {
            if ( thisNodePage->prevPage() != 0 )
            {
                nextLAnchor = lAnchor;
            }
        }
        else
        {
            nextLAnchor = thisNodeId;
        }

        if (foundPos == thisNodePage->rowsOnPage() - 1)
        {
            if ( thisNodePage->nextPage() != 0)
            {
                nextRAnchor = rAnchor;
            }
        }
        else
        {
            nextRAnchor = thisNodeId;
        }

        doRebalance = removeRebalance(nextNodeId, thisNodeId, nextLAnchor, nextRAnchor, key);
    }
    else
    {
        /**
         * === dealocate long fields ===
         */
        if ( m_page_tableinfo->longColumnsCount() > 0 )
        {
            bool found = false;
            uint16_t pos = thisNodePage->findInsertPos(key, found);
            if ( found )
            {
                Row* row = static_cast<PageIndexLeaf*>(thisNodePage)->rowAt( pos );

                for (uint16_t i = 0; i < m_page_tableinfo->longColumnsCount(); ++i )
                {
                    long_data_info_t* long_data = static_cast<ValueLongBlock*>( row->valueAt( m_page_tableinfo->longColumn( i )->columnNr() ) )->longDataInfo();
                    block_ptr_t block_ptr;
                    block_ptr.page_id = long_data->page_id;
                    block_ptr.nr = long_data->nr;
                    m_block_storage_manager->deleteData( block_ptr );
                }

                delete row;
                thisNodePage->deleteRowAt( pos );
            }
            doRebalance = found;
        }
        /**
         * === dealocate long fields ===
         */
        else
        {
            doRebalance = thisNodePage->deleteRow(key, true);
        }
    }


    if ( doRebalance )
    {
        if ( ! thisNodePage->hasEnoughData() )
        {
            if ( thisNodePage->isRoot() && thisNodePage->isIntern() )
            {
                PageIndexIntern* oldRoot = static_cast<PageIndexIntern*>(thisNodePage);
                PageSystemSegment* syspage = m_tablespace->pageSystemSegment();
                key_entry_info_st* key_entry = syspage->keyEntryByName( key->keyInfo()->keyName() );
                page_id_t newRootId = oldRoot->pagePtrAt( 0 );
                m_tablespace->deallocatePage( oldRoot );
                m_buffer->savePage( oldRoot );
                PageIndex* newRoot = m_tablespace->pageIndex( newRootId );
                if (newRoot->isLeaf())
                {
                    key_entry->nodes_id = 0;
                    key_entry->nodes_page_id = 0;
                    key_entry->data_id = newRoot->segmentId();
                    key_entry->data_page_id = newRoot->pageId();
                }
                else
                {
                    key_entry->nodes_id = newRoot->segmentId();
                    key_entry->nodes_page_id = newRoot->pageId();
                }

                syspage->updateKeyEntry( key_entry->keyName, key_entry );

                delete key_entry;
                m_buffer->savePage( syspage );
                m_buffer->savePage( newRoot );

                return false;
            }
            else
            {
                bool hasLeft = lAnchor != 0;
                bool hasRight = rAnchor != 0;
                bool atEnd = false;
                page_id_t redistributeAnchorId = 0;
                PageIndex* redistributeN = NULL;

                if ( hasLeft )
                {
                    PageIndex* leftSibling = m_tablespace->pageIndex( thisNodePage->prevPage() );
                    if ( leftSibling->canRedistributeData() )
                    {
                        redistributeN = leftSibling;
                        redistributeAnchorId = lAnchor;
                        atEnd = false;
                    }
                    else
                    {
                        m_buffer->savePage( leftSibling );
                    }
                }
                if ( ( redistributeN == NULL ) && hasRight )
                {
                    PageIndex* rightSibling = m_tablespace->pageIndex( thisNodePage->nextPage() );
                    if ( rightSibling->canRedistributeData() )
                    {
                        redistributeN = rightSibling;
                        redistributeAnchorId = rAnchor;
                        atEnd = true;
                    }
                    else
                    {
                        m_buffer->savePage( rightSibling );
                    }
                }
                if ( redistributeN != NULL )
                {
                    PageIndexIntern* anchorNode = m_tablespace->pageIndexIntern( redistributeAnchorId );
                    thisNodePage->redistributeNode( redistributeN, static_cast<PageIndexIntern*>(anchorNode),key->keyInfo(),  atEnd);

                    m_buffer->savePage( redistributeN );
                    m_buffer->savePage( thisNodePage );
                    m_buffer->savePage( anchorNode );
                    return false;
                }
                else
                {
                    PageIndex* leftNode = NULL;
                    PageIndex* rightNode = NULL;


                    if ( lAnchor == parentNodeId )
                    {
                        leftNode = m_tablespace->pageIndex( thisNodePage->prevPage() );
                        rightNode = thisNodePage;
                    }
                    else
                    {
                        rightNode = m_tablespace->pageIndex( thisNodePage->nextPage() );
                        leftNode = thisNodePage;
                    }
                    PageIndexIntern* anchorNode = m_tablespace->pageIndexIntern( parentNodeId );

                    merge(leftNode, rightNode, anchorNode, key->keyInfo(), true);

                    m_buffer->savePage( rightNode );
                    m_buffer->savePage( anchorNode );
                    m_buffer->savePage( leftNode );

                    return true;
                }
            }
        }
    }
    m_buffer->savePage(thisNodePage);
}

KeyValue* BTree::insertRebalance(page_id_t thisNodeId, Row* row, page_id_t& resPage, bool& insertResult)
{
    page_id_t nextNodeId = 0;
    KeyValue* resKey = NULL;
    page_id_t insertResPageId = 0;

    PageIndex* thisNodePage = m_tablespace->pageIndex( thisNodeId );

    KeyValue* rowKey = row->createKeyValueByName( row->tableinfo()->primaryKey()->keyName() );;

    if ( thisNodePage->isIntern() )
    {
        uint16_t foundPos = thisNodePage->findMatchingEntry( rowKey );
        nextNodeId = static_cast<PageIndexIntern*>( thisNodePage )->pagePtrAt( foundPos );

        resKey = insertRebalance(nextNodeId, row, insertResPageId, insertResult);
        delete rowKey;
    }
    else
    {
        resKey = rowKey;
    }

    if ( resKey != NULL )
    {
        if ( thisNodePage->isLeaf() )
        {
            bool found = false;
            PageIndexLeaf* thisLeafPage = static_cast<PageIndexLeaf*>(thisNodePage);

            uint16_t foundPos = thisLeafPage->insertPos(resKey, found);

            if ( ! found )
            {
                if ( ! thisLeafPage->isFull()) {
                    row->saveLongValues();
                    thisLeafPage->insertRow( row );
                    m_buffer->savePage( thisLeafPage );
                    insertResult = true;
                    delete resKey;
                    return NULL;
                }
                else
                {
                    PageSystemSegment* syspage = m_tablespace->pageSystemSegment();
                    key_entry_info_st* key_info = syspage->keyEntryByName( resKey->keyInfo()->keyName() );

                    page_info_t page_info = m_tablespace->allocatePage( thisLeafPage->segmentId() );
                    page_id_t new_page_id = page_info.page_id;
                    segment_id_t new_segment_id = page_info.segment_id;

                    PageIndexLeaf* newLeaf = PageFactory::createPageIndexLeaf( m_data_filename, new_page_id, new_segment_id, m_page_tableinfo );

                    resPage = newLeaf->pageId();

                    if ( thisLeafPage->nextPage() != 0 )
                    {
                        PageIndex* nextPage = m_tablespace->pageIndex( thisLeafPage->nextPage() );
                        nextPage->setPrevPage( newLeaf->pageId() );
                        m_buffer->savePage( nextPage );
                    }
                    thisLeafPage->splitNode( newLeaf );


                    KeyValue* borderKey = newLeaf->firstKeyValue( m_page_tableinfo->primaryKey() );

                    int8_t compare_res = resKey->compare( borderKey );

                    row->saveLongValues();
                    if (compare_res < 0)
                    {
                        thisLeafPage->insertRow( row );
                    }
                    else if ( compare_res > 0)
                    {
                        newLeaf->insertRow( row );
                    }

                    insertResult = true;

                    if (key_info->nodes_page_id == 0)
                    {
                        page_info = m_tablespace->allocatePage( m_tablespace->allocateSegment( 1 ).segment_id );
                        key_info->nodes_id = page_info.segment_id;
                        key_info->nodes_page_id = page_info.page_id;

                        syspage->updateKeyEntry( row->tableinfo()->primaryKey()->keyName(), key_info );

                        PageIndexIntern* internRootLeaf = PageFactory::createPageIndexIntern( m_data_filename, key_info->nodes_page_id, page_info.segment_id, m_page_tableinfo );

                        internRootLeaf->setInfimumKey( thisLeafPage->pageId() );
                        internRootLeaf->insertKey( borderKey, newLeaf->pageId() );

                        m_buffer->savePage( internRootLeaf );
                        delete borderKey;
                        borderKey = NULL;

                    }

                    delete key_info;
                    delete resKey;
                    m_buffer->savePage( newLeaf );
                    m_buffer->savePage( syspage );
                    m_buffer->savePage( thisLeafPage );

                    return borderKey;
                }
            }
            else
            {
                m_buffer->savePage( thisLeafPage );
                insertResult = false;
                delete resKey;
                return NULL;
            }
        }
        else
        {
            PageIndexIntern* internNode = static_cast<PageIndexIntern*>( thisNodePage );
            if ( ! internNode->isFull() )
            {
                internNode->insertKey( resKey, insertResPageId );
                m_buffer->savePage( internNode );
                delete resKey;
                return NULL;
            } else
            {
                page_info_t page_info = m_tablespace->allocatePage( internNode->segmentId() );

                PageIndexIntern* internNewNode = PageFactory::createPageIndexIntern( m_data_filename, page_info.page_id,  page_info.segment_id, m_page_tableinfo );

                bool isR = internNode->isRoot();

                if ( internNode->nextPage() != 0 )
                {
                    PageIndex* nextPage = m_tablespace->pageIndex( internNode->nextPage() );
                    nextPage->setPrevPage( internNewNode->pageId() );
                    m_buffer->savePage( nextPage );
                }
                bool ret_insert = false;
                key_info_t key_info = internNode->splitNode( internNewNode, resKey, insertResPageId, ret_insert );
                KeyValue* keyNewVal = key_info.key_val;

                resPage = internNewNode->pageId();

                /**
                 * wezel nie ma ojca, zwiekszamy wiec poziom drzewa o jeden, tworzac nowy korzen
                 */
                if ( isR )
                {
                    createNewRoot( internNode, internNewNode, keyNewVal );
                    delete keyNewVal;
                    keyNewVal = NULL;
                }

                m_buffer->savePage( internNewNode );
                m_buffer->savePage( internNode );

                if (!ret_insert)
                    delete resKey;

                return keyNewVal;
            }
        }
    }
    else
    {
        m_buffer->savePage(thisNodePage);
        return NULL;
    }

}

PageIndex* BTree::rootNode(const String& key_name) const
{
    PageSystemSegment* syspage = m_tablespace->pageSystemSegment();

    key_entry_info_st* key_entry = syspage->keyEntryByName( key_name );
    page_id_t node_page_id = key_entry->nodes_page_id;
    page_id_t data_page_id = key_entry->data_page_id;

    delete key_entry;
    m_buffer->savePage( syspage );

    if ( node_page_id != 0 ) {
        return m_tablespace->pageIndex( node_page_id );
    } else {
        return m_tablespace->pageIndex( data_page_id );
    }
}

/**
 * @brief wstawia rekord do b+drzewa
 *
 * @param row wiersz do wstawienia
 * @return bool
 **/
bool BTree::insertRow(Row* row)
{
    page_id_t resPage = 0;

    PageSystemSegment* syspage = m_tablespace->pageSystemSegment();
    key_entry_info_st* key_entry = syspage->keyEntryByName( m_page_tableinfo->primaryKey()->keyName() );

    if ( row->tableinfo()->primaryKey()->isAutoincrement() )
    {
        row->setAutoincVal( key_entry->autoinc_value );

        if ( key_entry->autoinc_value <= row->autoincVal() )
        {
            key_entry->autoinc_value = row->autoincVal() + 1;
            syspage->updateKeyEntry( key_entry->keyName, key_entry );
        }
    }


    page_id_t rootPageId = 0;
    if (key_entry->nodes_page_id != 0) rootPageId = key_entry->nodes_page_id;
    else rootPageId = key_entry->data_page_id;

    bool insertRes = false;

    insertRebalance( rootPageId, row, resPage, insertRes );


    delete key_entry;
    m_buffer->savePage( syspage );

    return insertRes;
}

/**
 * @brief usuwa rekord z b+drzewa
 *
 * @param key_value wartosc klucza, po ktorej szukamy wiersza
 * @return void
 **/
void BTree::deleteRow(KeyValue* key_value)
{
    PageSystemSegment* syspage = m_tablespace->pageSystemSegment();

    key_entry_info_st* key_entry = syspage->keyEntryByName( m_page_tableinfo->primaryKey()->keyName() );
    page_id_t rootPageId = 0;
    if (key_entry->nodes_page_id != 0) rootPageId = key_entry->nodes_page_id;
    else rootPageId = key_entry->data_page_id;
    delete key_entry;
    m_buffer->savePage( syspage );

    removeRebalance(rootPageId, 0, 0, 0, key_value);

}

void BTree::createNewRoot(PageIndex* indexIntern1, PageIndex* indexIntern2, KeyValue* key)
{
    page_info_t page_info = m_tablespace->allocatePage( indexIntern1->segmentId() );
    PageIndexIntern* rootNode = PageFactory::createPageIndexIntern( m_data_filename, page_info.page_id, page_info.segment_id, m_page_tableinfo );
    rootNode->setInfimumKey( indexIntern1->pageId() );
    rootNode->insertKey( key, indexIntern2->pageId() );
    m_buffer->savePage( rootNode );
    PageSystemSegment* syspage = m_tablespace->pageSystemSegment();
    key_entry_info_st* key_entry = syspage->keyEntryByName( key->keyInfo()->keyName() );
    key_entry->nodes_id = page_info.segment_id;
    key_entry->nodes_page_id = page_info.page_id;
    syspage->updateKeyEntry( key_entry->keyName, key_entry );
    delete key_entry;

    m_buffer->savePage(syspage);
}

void BTree::merge(PageIndex* leftNode, PageIndex* rightNode, PageIndexIntern* anchorNode, KeyInfo* key_info, bool atEnd)
{
    KeyValue* keyToDelete = NULL;
    keyToDelete = rightNode->firstKeyValue( key_info );

    if ( leftNode->isIntern() )
    {
        PageIndexIntern* indexIntern = static_cast<PageIndexIntern*>( leftNode );
        PageIndexIntern* mergeIntern = static_cast<PageIndexIntern*>( rightNode );
        KeyValue* keyToCopy = anchorNode->findMatchingKeyValue( keyToDelete );

        indexIntern->insertKey( keyToCopy, mergeIntern->pagePtrAt( 0 ) );
        delete keyToCopy;
    }

    if ( rightNode->nextPage() != 0 )
    {
        PageIndex* nextPage = m_tablespace->pageIndex( rightNode->nextPage() );
        nextPage->setPrevPage( leftNode->pageId() );
        m_buffer->savePage( nextPage );
    }

    /**
     * merge
     */
    uint16_t freeSp = leftNode->freeSpace();
    uint16_t rowsToStore = rightNode->rowsOnPage();
    uint16_t startPos = (leftNode->isLeaf())?0:1;

    leftNode->setNextPage( rightNode->nextPage() );
    for ( uint16_t i = startPos; i < rightNode->rowsOnPage(); ++i )
    {
        uint16_t size = 0;
        uint8_t* rawRow = rightNode->rawEntryAt( i, size );
        leftNode->saveRawDataAtEnd( rawRow, size );

    }

    anchorNode->deleteRow( keyToDelete, false );

    m_tablespace->deallocatePage( rightNode );
    delete keyToDelete;
}

void BTree::checkLeafCorrectness() const
{
    PageSystemSegment* syspage = m_tablespace->pageSystemSegment();
    key_entry_info_st* key_entry = syspage->keyEntryByName( m_page_tableinfo->primaryKey()->keyName() );
    PageIndexLeaf* leaf = m_tablespace->pageIndexLeaf( key_entry->data_page_id );
    m_buffer->savePage( syspage );
    delete key_entry;
    uint16_t nextCount = 0;
    uint16_t prevCount = 0;

    page_id_t page_id = 0;


    while ((page_id = leaf->nextPage()) != 0)
    {
        nextCount++;
        m_buffer->savePage( leaf );
        leaf = m_tablespace->pageIndexLeaf(page_id);
    }

    while ((page_id = leaf->prevPage()) != 0)
    {
        prevCount++;
        m_buffer->savePage( leaf );
        leaf = m_tablespace->pageIndexLeaf(page_id);
    }
    m_buffer->savePage( leaf );
    if (nextCount != prevCount)
        exit(1);
}
