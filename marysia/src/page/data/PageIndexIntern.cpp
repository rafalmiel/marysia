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


#include "PageIndexIntern.h"

#include <iostream>

#include "page/tableinfo/PageTableInfo.h"
#include "table/KeyInfo.h"
#include "table/KeyValue.h"
#include "table/Value.h"
#include "table/ValueFactory.h"
#include "utils/ByteData.h"

using namespace std;

PageIndexIntern::PageIndexIntern(ByteData* data): PageIndex(data)
{
    m_page_size = PAGE_SIZE;
    m_is_fixed_lenght_page = true;
}

void PageIndexIntern::initEmpty()
{
    PageIndex::initEmpty();

    setInfimumKey( 0 );
    storePosInDict( 0, OFFSET_INFIMUM );
    updateWriteValue( OFFSET_INFIMUM );
    setRowsOnPage( 1 );
    setRowsSize( 6 );
}

KeyValue* PageIndexIntern::firstKeyValue(KeyInfo* key_info) const
{
    return keyValueAt( key_info, 1 );
}

void PageIndexIntern::setInfimumKey(page_id_t page_id)
{
    m_data->seek( OFFSET_INFIMUM );
    m_data->writeUInt16( 6 );
    m_data->writeUInt32( page_id );
    markDirty();
}

page_id_t PageIndexIntern::infimumKey() const
{
    m_data->seek( OFFSET_INFIMUM + 2 );
    return m_data->readUInt32();
}

page_id_t PageIndexIntern::pagePtrAt(uint16_t pos) const
{
    
    uint16_t rowPtr = posFromDict( pos );
    return m_data->readUInt32At( rowPtr + 2 );
}

key_info_t PageIndexIntern::splitNode(PageIndexIntern* pageIndex, KeyValue* insert_key, page_id_t ref_page, bool& return_insert)
{
    uint16_t rowsCount = rowsOnPage();
    uint16_t rowPos = 0;
    uint16_t pos = ( (rowsCount - 1) >> 1 );

    if (pos < minRowsOnPage())
    {
        KeyValue* comp_val = keyValueAt(insert_key->keyInfo(), pos);
        if ( insert_key->compare( comp_val ) > 0 )
        {
            KeyValue* comp_val2 = keyValueAt(insert_key->keyInfo(), pos+1);
            if ( insert_key->compare( comp_val2 ) < 0 )
            {
                return_insert = true;
            }
            else
            {
                pos += 1;
            }
            delete comp_val2;
        }
        delete comp_val;
    }
    key_info_t key_info;
        
    for (int i = pos + 1; i < rowsCount; ++i)
    {
        rowPos = posFromDict( i );
        
        m_data->seek(rowPos);
        
        uint16_t size = m_data->readUInt16At( rowPos );
        uint8_t* raw_data = m_data->dataPtr( true );
        
        pageIndex->saveRawDataAtEnd( raw_data, size );
    }

    for (int i = rowsCount - 1; i >= pos + 1; --i)
    {
        deleteRowAt( i );
    }

    pageIndex->setNextPage( nextPage() );
    setNextPage( pageIndex->pageId() );
    pageIndex->setPrevPage( pageId() );
    
    /**
     * zwraca srodkowy klucz
     */
    if (return_insert)
    {
        key_info.key_val = insert_key;
        key_info.page_id = ref_page;

        pageIndex->setInfimumKey( ref_page );

        return key_info;
    }
    else
    {
        rowPos = posFromDict( pos );
        
        page_id_t page_id = m_data->readUInt32At( rowPos + 2 );
        
        pageIndex->setInfimumKey( page_id );
        
        KeyValue* key_val = createKeyValueAtPtr( m_page_tableinfo->primaryKey(), rowPos );

        deleteRowAt( pos );

        if ( insert_key->compare( key_val ) > 0 )
        {
            pageIndex->insertKey( insert_key, ref_page );
        }
        else
        {
            insertKey( insert_key, ref_page );
        }

        key_info.key_val = key_val;
        key_info.page_id = page_id;

        return key_info;
    }
}

page_id_t PageIndexIntern::findMatchingNode(KeyValue* key_value) const
{
    bool found;
    uint16_t dictPos = findDictEntryNum( key_value, found );
    if (!found) dictPos--;
    uint16_t pos = posFromDict( dictPos );

    m_data->seek( pos + 2);

    return m_data->readUInt32();
}

void PageIndexIntern::insertKey(KeyValue* key_value, page_id_t page_id)
{
    bool found_exact;
    
    uint16_t insert_pos = findInsertPos(key_value, found_exact);
    
    if (! found_exact)
    {
        writeKeyDataAt(insert_pos, key_value, page_id);
    }
}

void PageIndexIntern::writeKeyDataAt(uint16_t pos, KeyValue* key_value, page_id_t page_id)
{
    storePosInDict(pos, writePos());
    
    writeKeyDataAtPtr( writePos(), key_value, page_id );
}

void PageIndexIntern::replaceKey(KeyValue* valToReplace, KeyValue* replaceVal, page_id_t newPageId)
{
    uint16_t pos = findMatchingEntry( valToReplace );

    uint16_t keyPtr = posFromDict( pos );

    m_data->seek( keyPtr );
    m_data->writeUInt16( replaceVal->byteSize() + 4 );
    if (newPageId != 0)
    {
        m_data->writeUInt32( newPageId );
    }
    else
    {
        m_data->moveBy( 4 );
    }

    for ( uint8_t i = 0; i < replaceVal->keyInfo()->keyColumnNum(); ++i )
    {
        Value* value = replaceVal->valueAt( i );
        m_data->writeUInt16( value->valueLen() );
        m_data->writeData( value->rawData(), value->valueLen(), value->storeSize() );
    }
    
    markDirty();
}

void PageIndexIntern::writeKeyDataAtPtr(uint16_t pos, KeyValue* key_value, page_id_t page_id)
{
    m_data->seek( pos );
    
    m_data->writeUInt16( key_value->byteSize() + 4 );
    m_data->writeUInt32( page_id );

    for ( uint8_t i = 0; i < key_value->keyInfo()->keyColumnNum(); ++i )
    {
        Value* value = key_value->valueAt( i );
        m_data->writeUInt16( value->valueLen() );
        m_data->writeData( value->rawData(), value->valueLen(), value->storeSize() );
    }

    updateWriteValue( m_data->pos() );

    setRowsSize( rowsSize() + key_value->byteSize() + 4 );
}

KeyValue* PageIndexIntern::createKeyValueAtPtr(KeyInfo* key_info, uint16_t pos) const
{
    m_data->seek( pos + 2 + 4 );

    KeyValue* key_value = new KeyValue( key_info );
    key_value->setOwnsValues( true );

    if (pos == OFFSET_INFIMUM)
    {
        key_value->setIsInfimum( true );
        return key_value;
    }

    uint8_t i = 0;
    while ( i < key_info->keyColumnNum() )
    {
        uint16_t len = m_data->readUInt16();

        Value* value = ValueFactory::createValue( key_info->keyPart( i )->column()->type(), key_info->keyPart( i )->column()->typeLen(), m_data->dataPtr( true ), len );
        
        key_value->addValue(
            value
        );
        i++;

        m_data->moveBy( value->storeSize() );
    }

    return key_value;
}

void PageIndexIntern::redistributeNode( PageIndex* pageIndex, PageIndex* anchorNode, KeyInfo* key_info, bool atEnd )
{
    uint16_t rOnPage = pageIndex->rowsOnPage();

    int16_t recDiff = rOnPage - rowsOnPage();
    KeyValue* oldKey = NULL;
    KeyValue* newKey = NULL;
    uint16_t toTransfer = ( ( recDiff >> 1 ) );
    if (toTransfer > 0)
    {

        PageIndexIntern* leftNode = static_cast<PageIndexIntern*>( (atEnd)?this:pageIndex );
        PageIndexIntern* rightNode = static_cast<PageIndexIntern*>( (atEnd)?pageIndex:this );
        if (atEnd)
        {
            oldKey = rightNode->firstKeyValue(key_info);
            KeyValue* parentKeyVal = anchorNode->findMatchingKeyValue( oldKey );
            page_id_t infimumRight = rightNode->infimumKey();
            leftNode->insertKey( parentKeyVal, infimumRight );
            toTransfer--;
            for ( uint16_t i = 0; i < toTransfer; ++i )
            {
                uint16_t size = 0;
                uint8_t* rawRow = rightNode->rawEntryAt( 1, size );
                leftNode->saveRawDataAtEnd( rawRow, size );
                rightNode->deleteRowAt( 1 );
            }
            newKey = rightNode->firstKeyValue( key_info );
            page_id_t firstPage = rightNode->pagePtrAt( 1 );
            
            static_cast<PageIndexIntern*>( anchorNode )->replaceKey( parentKeyVal, newKey );
            rightNode->deleteRowAt( 1 );
            rightNode->setInfimumKey( firstPage );
            delete parentKeyVal;
        }
        else
        {
            oldKey = rightNode->firstKeyValue(key_info);
            KeyValue* parentKeyVal = anchorNode->findMatchingKeyValue( oldKey );
            page_id_t infimumKey = rightNode->infimumKey();
            rightNode->insertKey( parentKeyVal, infimumKey );
            toTransfer--;
            for ( uint16_t i = 0; i < toTransfer; ++i )
            {
                uint16_t size = 0;
                uint8_t* rawRow = leftNode->rawEntryAt( rOnPage - 1, size );
                rightNode->saveRawDataAtBegining( rawRow, size );
                leftNode->deleteRowAt( rOnPage - 1 );
                rOnPage--;
            }
            newKey = leftNode->keyValueAt( key_info, leftNode->rowsOnPage()-1 );
            page_id_t lastPage = leftNode->pagePtrAt( leftNode->rowsOnPage() - 1 );
            static_cast<PageIndexIntern*>( anchorNode )->replaceKey( parentKeyVal, newKey );
            leftNode->deleteRowAt( leftNode->rowsOnPage() - 1 );
            rightNode->setInfimumKey( lastPage );
            delete parentKeyVal;
        }
        
        delete oldKey;
        delete newKey;
    }
}

