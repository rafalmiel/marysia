/*
    Marysia database server
    Copyright (C) 2011  Rafal Mielniczuk rafalm3@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "PageIndex.h"

#include <iostream>

#include "page/tableinfo/PageTableInfo.h"
#include "table/Column.h"
#include "table/KeyInfo.h"
#include "table/KeyValue.h"
#include "table/values/ValueInteger.h"
#include "utils/ByteData.h"

using namespace std;

PageIndex::PageIndex(ByteData* data) : Page( data )
{

}

void PageIndex::initEmpty()
{
    m_data->dataSetAt( PAGE_HEADER_SIZE, 0, m_data->size() - PAGE_HEADER_SIZE );
    setRowsOnPage( 0 );
    setRowsSize( 0 );
    setWritePos( OFFSET_ROWS_LIST );
    setNextPage( 0 );
    setPrevPage( 0 );
    setAutoincrementCounter( 0 );
}

void PageIndex::setPageTableinfo(PageTableInfo* page_tableinfo)
{
    m_page_tableinfo = page_tableinfo;
}

uint16_t PageIndex::rowsOnPage() const
{
    return m_data->readUInt16At( OFFSET_INDEX_INFO );
}

uint16_t PageIndex::rowsSize() const
{
    return m_data->readUInt16At( OFFSET_INDEX_INFO + 2 );
}

uint16_t PageIndex::writePos() const
{
    return m_data->readUInt16At( OFFSET_INDEX_INFO + 4 );
}

void PageIndex::setRowsOnPage( uint16_t rows )
{
    m_data->writeUInt16At( OFFSET_INDEX_INFO, rows );
    markDirty();
}

void PageIndex::setRowsSize( uint16_t size )
{
    m_data->writeUInt16At( OFFSET_INDEX_INFO + 2, size );
    markDirty();
}

void PageIndex::setWritePos( uint16_t pos )
{
    m_data->writeUInt16At( OFFSET_INDEX_INFO + 4, pos );
    markDirty();
}

uint32_t PageIndex::nextPage() const
{
    return m_data->readUInt32At(OFFSET_NEIGHBOUR_PAGES_ID + 4);
}

uint32_t PageIndex::prevPage() const
{
    return m_data->readUInt32At(OFFSET_NEIGHBOUR_PAGES_ID);
}

bool PageIndex::isRoot() const
{
    return (prevPage() == 0) && (nextPage() == 0);
}

bool PageIndex::isLeaf() const
{
    return pageType() == Page::INDEX_LEAF_NODE;
}

bool PageIndex::isIntern() const
{
    return pageType() == Page::INDEX_INTERN_NODE;
}

void PageIndex::setNextPage(page_id_t page_id)
{
    m_data->writeUInt32At(OFFSET_NEIGHBOUR_PAGES_ID + 4, page_id);
    markDirty();
}

void PageIndex::setPrevPage(page_id_t page_id)
{
    m_data->writeUInt32At(OFFSET_NEIGHBOUR_PAGES_ID, page_id);
    markDirty();
}

uint32_t PageIndex::autoincrementCounter() const
{
    return m_data->readUInt32At(OFFSET_AI_COUNTER);
}

void PageIndex::setAutoincrementCounter(uint32_t cntr)
{
    m_data->writeUInt32At(OFFSET_AI_COUNTER, cntr);
    markDirty();
}

uint16_t PageIndex::freeSpace() const
{
    return ( m_data->size() - OFFSET_ROWS_LIST ) - rowsSize() - (rowsOnPage() << 1);
}

uint16_t PageIndex::usedSpace() const
{
    return rowsSize() + (rowsOnPage() << 1);
}

int16_t PageIndex::fillFactor() const
{
    return usedSpace() - freeSpace();
}

uint16_t PageIndex::dictStartPos() const
{
    return m_data->size() - ( rowsOnPage() << 1 );
}

uint16_t PageIndex::rowSize() const
{
    uint16_t size = 2/*rowsize*/ + 2/*dictentry*/;

    if ( isLeaf() )
    {
        for (uint16_t i = 0; i < m_page_tableinfo->columnCount(); ++i )
        {
            Column* col = m_page_tableinfo->columnByNum( i );
            
            size += 2 /*value ptr*/ + 2/*value len*/ + col->valueStoreSize();
        }
    }
    else
    {
        size += 4;/*page ptr size*/
        for (uint16_t i = 0; i < m_page_tableinfo->primaryKey()->keyColumnNum(); ++i )
        {
            Column* col = m_page_tableinfo->primaryKey()->keyPart(i)->column();
            
            size += 2/*value len*/ + col->valueStoreSize();
        }
    }


    return size;
}

uint16_t PageIndex::maxRowsOnPage() const
{
    uint16_t head_size = OFFSET_ROWS_LIST;
    uint8_t rows_in_head = 0;
    if ( isIntern() )
    {
        head_size += 6;
        rows_in_head = 1;
    }
    return ( ( m_data->size() - head_size ) / rowSize() ) + rows_in_head;
    //return 6;
}

uint16_t PageIndex::minRowsOnPage() const
{
    return maxRowsOnPage() >> 1;
}

bool PageIndex::hasEnoughData() const
{
    if ( ! isRoot() )
    {
        return rowsOnPage() >= ( minRowsOnPage() );
    }
    return ( rowsOnPage() > 1 ) || ( isLeaf() );
}

KeyValue* PageIndex::keyValueAt( KeyInfo* key_info, uint16_t pos ) const
{
    uint16_t rowPtr = posFromDict( pos );
    return createKeyValueAtPtr( key_info, rowPtr );
}

bool PageIndex::canRedistributeData(bool fromEnd) const
{
    return rowsOnPage() > minRowsOnPage();
}

void PageIndex::storePosInDict(uint16_t place_num, uint16_t value)
{
    uint16_t dictStart = dictStartPos();
    m_data->shiftLeftAt( dictStart, place_num << 1, 2);
    m_data->writeUInt16At( dictStart - 2 + ( place_num << 1 ) , value );
    setRowsOnPage( rowsOnPage() + 1 );
    markDirty();
}

uint16_t PageIndex::removePosFromDict(uint16_t place_num, uint16_t count)
{
    uint16_t dict_pos = dictStartPos();
    uint16_t val = m_data->readUInt16At( dict_pos + ( place_num << 1 ) );
    m_data->shiftRightAt( dict_pos, place_num << 1, count << 1 );
    setRowsOnPage( rowsOnPage() - count );
    markDirty();
    return val;
}

/**
 * @brief odnajduje numer w slowniku. Jesli pozycji nie znaleziono, zwraca pozycje na ktorej nalezy wstawic nowa wartosc.
 *
 * @param key_value wartosc klucza, po ktorym odbywa sie szukanie
 * @param found_exact parametr wyjsciowy, mowiacy czy znaleziono dokladna wartosc
 * @return int8_t
 **/
uint16_t PageIndex::findDictEntryNum(KeyValue* key_value, bool& found_exact ) const
{
    int16_t low_b = 1;
    int16_t rows = rowsOnPage();
    int16_t top_b = rows;
    int16_t pos = ( low_b + top_b ) >> 1;
    uint16_t dictStart = dictStartPos();
    int8_t comp_res = 1;

    //binary search
    found_exact = false;
    while ( low_b <= top_b )
    {
        KeyValue* comp_value = createKeyValueAtPtr( key_value->keyInfo(), posFromDict( pos - 1)  );

        
        comp_res = key_value->compare( comp_value );
        delete comp_value;

        if (comp_res > 0)
        {
            low_b = pos + 1;
        }
        else if (comp_res < 0)
        {
            top_b = pos - 1;
        }
        else
        {
            found_exact = true;
            break;
        }

        if ( low_b <= top_b )
            pos = ( low_b + top_b ) >> 1;

    }

    if (comp_res > 0) {
        ++pos;
    }
    
    return --pos;
}

uint16_t PageIndex::posFromDict(uint16_t place_num) const
{
    return m_data->readUInt16At( dictStartPos() + (place_num << 1) );
}

uint16_t PageIndex::findInsertPos(KeyValue* key_value, bool& found_exact) const
{
    return findDictEntryNum(key_value, found_exact);
}

void PageIndex::deleteRowAt(uint16_t pos)
{
    uint16_t row_pos = removePosFromDict( pos );
    
    deleteDataAtPtr( row_pos );
}

bool PageIndex::deleteRow(KeyValue* key_value, bool findExact)
{

    bool found;
    uint16_t dictPos = findDictEntryNum( key_value, found );
    if (!found) dictPos--;

    if (found || ( findExact == false ) )
    {

        deleteRowAt( dictPos );
        return true;
    }

    return false;
}

void PageIndex::updateWriteValue(uint16_t searchStartPtr)
{
    uint16_t len = 0;
    m_data->seek( searchStartPtr );
    while ((len = m_data->readUInt16At( m_data->pos() )) != 0) {
        m_data->moveBy( len );
    }
    setWritePos( m_data->pos() );
}

void PageIndex::writeRawDataAtPtr(uint16_t pos, uint8_t* row_data, uint16_t size)
{
    m_data->seek( pos );
    m_data->writeData( row_data, size );
    
    updateWriteValue( pos + size );
    setRowsSize( rowsSize() + size );
}

uint8_t* PageIndex::rawEntryAt(uint16_t dictPos, uint16_t& size) const
{
    uint16_t rowPtr = posFromDict( dictPos );
    size = m_data->readUInt16At( rowPtr );
    m_data->seek( rowPtr );
    return m_data->dataPtr( true );
}

void PageIndex::deleteDataAtPtr(uint16_t pos)
{
    uint16_t row_len = m_data->readUInt16At( pos );
    m_data->dataSetAt(pos, 0, row_len);
    
    if ( pos < writePos() )
    {
        setWritePos( pos );
    }
    
    setRowsSize( rowsSize() - row_len );
}

KeyValue* PageIndex::findMatchingKeyValue(KeyValue* key_value) const
{
    bool found;
    uint16_t dictPos = findDictEntryNum( key_value, found );
    if (!found) dictPos--;
    
    return createKeyValueAtPtr( key_value->keyInfo(), posFromDict( dictPos ) );
}

uint16_t PageIndex::insertPos(KeyValue* key_value, bool& found) const
{
    uint16_t insert_pos = findInsertPos(key_value, found);
    
    return insert_pos;
}

uint16_t PageIndex::findMatchingEntry(KeyValue* key_value) const
{
    bool found;
    uint16_t dictPos = findDictEntryNum( key_value, found );
    if (!found) dictPos--;

    return dictPos;
}

void PageIndex::saveRawDataAtEnd(uint8_t* raw_data, uint16_t size)
{
    uint16_t writeP = writePos();

    storePosInDict( rowsOnPage(), writeP );
    writeRawDataAtPtr( writeP, raw_data, size );
}

void PageIndex::saveRawDataAtBegining(uint8_t* raw_data, uint16_t size)
{
    uint16_t writeP = writePos();
    storePosInDict( (isLeaf())?0:1, writeP );
    writeRawDataAtPtr( writeP, raw_data, size );
}

void PageIndex::mergeNode(PageIndex* pageIndex, bool atEnd)
{
    uint16_t freeSp = freeSpace();
    uint16_t rowsToStore = pageIndex->rowsOnPage();
    uint16_t startPos = (isLeaf())?0:1;
    if ( atEnd )
    {
        setNextPage( pageIndex->nextPage() );
        for ( uint16_t i = startPos; i < pageIndex->rowsOnPage(); ++i )
        {
            uint16_t size = 0;
            uint8_t* rawRow = pageIndex->rawEntryAt( i, size );
            saveRawDataAtEnd( rawRow, size );

        }
        
    }
    else
    {
        setPrevPage( pageIndex->prevPage() );
        for ( uint16_t i = startPos; i < pageIndex->rowsOnPage(); ++i )
        {
            uint16_t size = 0;
            uint8_t* rawRow = pageIndex->rawEntryAt( i, size );
            saveRawDataAtBegining( rawRow, size );
        }
    }

    
}
