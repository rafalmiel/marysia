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


#include "PageIndexLeaf.h"

#include <iostream>
#include <stdlib.h>

#include "io/File.h"
#include "page/data/PageIndexIntern.h"
#include "page/tableinfo/PageTableInfo.h"
#include "table/KeyInfo.h"
#include "table/KeyValue.h"
#include "table/Row.h"
#include "table/Value.h"
#include "table/ValueFactory.h"
#include "utils/ByteData.h"

using namespace std;

PageIndexLeaf::PageIndexLeaf( ByteData* data ): PageIndex( data )
{
    m_page_size = PAGE_SIZE;
    m_is_fixed_lenght_page = true;
}

KeyValue* PageIndexLeaf::firstKeyValue(KeyInfo* key_info) const
{
    return keyValueAt( key_info, 0 );
}

void PageIndexLeaf::writeRowDataAt(uint16_t pos, Row* row)
{
    storePosInDict(pos, writePos());
    
    writeRowDataAtPtr( writePos(), row );
}

void PageIndexLeaf::writeRowDataAtPtr(uint16_t pos, Row* row)
{
    m_data->seek( pos );
    
    m_data->writeUInt16( row->byteSize() );
    
    uint16_t ptr_pos = m_data->pos();
    
    m_data->moveBy( row->valuesCount() << 1 );
    
    for ( int i = 0; i < row->valuesCount(); ++i )
    {
        m_data->writeUInt16At( ptr_pos + ( i << 1 ), m_data->pos() );
        
        Value* value = row->valueAt( i );
        
        m_data->writeUInt16( value->valueLen() );
        m_data->writeData( value->rawData(), value->valueLen(), value->storeSize() );
    }

    updateWriteValue( m_data->pos() );
    setRowsSize( rowsSize() + row->byteSize() );
}

void PageIndexLeaf::insertRow(Row* row)
{
    bool found_exact = false;
    KeyValue* key_value = row->createKeyValueByName( m_page_tableinfo->primaryKey()->keyName() );
    uint16_t insert_pos = findInsertPos(key_value, found_exact);
    delete key_value;
    
    if (! found_exact)
    {
        writeRowDataAt( insert_pos, row );
    }
}

void PageIndexLeaf::insertRow(Row* row, uint16_t pos)
{
    writeRowDataAt( pos, row );
}

void PageIndexLeaf::splitNode(PageIndexLeaf* pageIndex)
{
    uint16_t rowsCount = rowsOnPage();
    uint16_t rowPos = 0;
    uint16_t pos = rowsCount >> 1;

    
    for (int i = pos; i < rowsCount; ++i)
    {
        rowPos = posFromDict( i );

        m_data->seek(rowPos);

        uint16_t size = m_data->readUInt16At( rowPos );
        uint8_t* raw_data = m_data->dataPtr( true );
        pageIndex->saveRawDataAtEnd( raw_data, size );
    }
    
    for (int i = rowsCount - 1; i >= pos; --i)
    {
        deleteRowAt( i );
    }
    
    pageIndex->setNextPage( nextPage() );
    setNextPage( pageIndex->pageId() );
    pageIndex->setPrevPage( pageId() );
}

void PageIndexLeaf::writeRawDataAtPtr(uint16_t pos, uint8_t* row_data, uint16_t size)
{
    PageIndex::writeRawDataAtPtr( pos, row_data, size );

    updateFieldPtrs( pos );
}

void PageIndexLeaf::updateFieldPtrs(uint16_t rowPtr)
{
    uint16_t columnCount = m_page_tableinfo->columnCount();
    uint16_t fieldPtrStart = rowPtr + 2;
    m_data->seek( fieldPtrStart + ( columnCount << 1 ) );
    for ( int i = 0; i < columnCount; ++i )
    {
        m_data->writeUInt16At(fieldPtrStart + (i << 1), m_data->pos());
        m_data->moveBy( m_page_tableinfo->columnByNum( i )->valueStoreSize() + 2 );
    }
    markDirty();
}

Row* PageIndexLeaf::rowAt(uint16_t pos) const
{
    uint16_t rowPtr = posFromDict( pos );
    return createRowFromPtr( rowPtr );
}

KeyValue* PageIndexLeaf::createKeyValueAtPtr(KeyInfo* key_info, uint16_t pos) const
{
    uint16_t oldPos = m_data->pos();
    
    m_data->seek( pos );
    KeyValue* key_value = new KeyValue( key_info );
    key_value->setOwnsValues( true );

    for ( int i = 0; i < key_info->keyColumnNum(); ++i )
    {
        m_data->seek( m_data->readUInt16At( pos + 2 + ( key_info->keyPart( i )->column()->columnNr() << 1 ) ) ) ;
        uint16_t len = m_data->readUInt16();

        key_value->addValue(
            ValueFactory::createValue( key_info->keyPart( i )->column()->type(), key_info->keyPart( i )->column()->typeLen(), m_data->dataPtr( true ), len )
        );
    }

    m_data->seek( oldPos );

    return key_value;
}

Row* PageIndexLeaf::createRowFromPtr(uint16_t pos) const
{
    uint16_t oldPos = m_data->pos();
    
    m_data->seek( pos );
    Row* row = new Row(m_page_tableinfo);
    
    for ( int i = 0; i < m_page_tableinfo->columnCount(); ++i )
    {
        m_data->seek( m_data->readUInt16At( pos + 2 + ( i << 1 ) ) ) ;
        uint16_t len = m_data->readUInt16();
        row->addValue(
            ValueFactory::createValue( m_page_tableinfo->columnByNum( i )->type(), m_page_tableinfo->columnByNum( i )->typeLen(), m_data->dataPtr( true ), len )
        );
    }
    
    m_data->seek( oldPos );
    
    return row;
}

void PageIndexLeaf::redistributeNode( PageIndex* pageIndex, PageIndex* anchorNode, KeyInfo* key_info, bool atEnd )
{
    uint16_t rOnPage = pageIndex->rowsOnPage();

    int16_t recDiff = rOnPage - rowsOnPage();
    KeyValue* oldKey = NULL;
    KeyValue* newKey = NULL;
    uint16_t toTransfer = ( ( recDiff >> 1 ) );

    if (toTransfer > 0)
    {

        if ( atEnd )
        {
            oldKey = pageIndex->firstKeyValue( key_info );
            for ( uint16_t i = 0; i < toTransfer; ++i )
            {
                uint16_t size = 0;
                uint8_t* rawRow = pageIndex->rawEntryAt( 0, size );
                saveRawDataAtEnd( rawRow, size );
                pageIndex->deleteRowAt( 0 );
            }
            newKey = pageIndex->firstKeyValue( key_info );
        } else
        {
            oldKey = firstKeyValue( key_info );
            for ( uint16_t i = 0; i < toTransfer; ++i )
            {
                uint16_t size = 0;
                uint8_t* rawRow = pageIndex->rawEntryAt( rOnPage - 1, size );
                saveRawDataAtBegining( rawRow, size );
                pageIndex->deleteRowAt( rOnPage - 1 );
                rOnPage--;
            }
            newKey = firstKeyValue( key_info );
        }
        static_cast<PageIndexIntern*>( anchorNode )->replaceKey(oldKey, newKey);

        delete oldKey;
        delete newKey;
    }
}

