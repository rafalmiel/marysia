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

#include "PageSegmentLongStorage.h"

#include "utils/ByteData.h"

#include <iostream>

using namespace std;

PageSegmentLongStorage::PageSegmentLongStorage(ByteData* data): PageSegment(data)
{
    m_page_size = PAGE_SIZE;
    m_is_fixed_lenght_page = true;
}

PageSegmentLongStorage::PageSegmentLongStorage(ByteData* data, uint8_t extentCount): PageSegment(data, extentCount)
{
    m_page_size = PAGE_SIZE;
    m_is_fixed_lenght_page = true;
}

void PageSegmentLongStorage::setListEntryCount(uint16_t count)
{
    m_data->writeUInt16At( OFFSET_LIST_ENTRY_COUNT, count );
    markDirty();
}

uint16_t PageSegmentLongStorage::listEntryCount() const
{
    return m_data->readUInt16At( OFFSET_LIST_ENTRY_COUNT );
}

uint8_t PageSegmentLongStorage::maxChunkCountAvail() const
{
    return m_data->readInt8At( OFFSET_LIST_ENTRY_COUNT + 2 );
}

void PageSegmentLongStorage::setMaxChunkCountAvail(uint8_t count)
{
    m_data->writeUInt8At( OFFSET_LIST_ENTRY_COUNT + 2, count );
    markDirty();
}

void PageSegmentLongStorage::updateMaxChunkCountAvail()
{
    uint32_t maxCount = 0;
    uint32_t rob;
    for (uint16_t i = 0; i < listEntryCount(); ++i)
    {
        if ( ( rob = listEntryAt( i ).max_free_chunk_block ) > maxCount )
        {
            
            maxCount = rob;
        }
    }
    setMaxChunkCountAvail( maxCount );
}

void PageSegmentLongStorage::incListEntryCount()
{
    setListEntryCount( listEntryCount() + 1 );
}

void PageSegmentLongStorage::decListEntryCount()
{
    setListEntryCount( listEntryCount() - 1 );
}

uint8_t PageSegmentLongStorage::findListSavePos(const page_segment_long_storage_list_item_t& dict_entry) const
{
    page_segment_long_storage_list_item_t de;
    for ( uint8_t i = 0; i < listEntryCount(); ++i )
    {
        de = listEntryAt( i );
        if (de.page_id >= dict_entry.page_id)
        {
            return i;
        }
    }
    
    return listEntryCount();
}

page_segment_long_storage_list_item_t PageSegmentLongStorage::listEntryAt(uint8_t pos) const
{
    page_segment_long_storage_list_item_t list_entry;
    list_entry.max_free_chunk_block = 0;
    list_entry.page_id = 0;
    
    if ( pos < listEntryCount() )
    {
        m_data->seek( OFFSET_PAGES_INFO_LIST + ( pos * PAGE_LONG_STORAGE_LIST_ENTRY_SIZE ) );
        
        //cout << m_data->pos() << endl;
        
        list_entry.page_id = m_data->readUInt32();
        list_entry.max_free_chunk_block = m_data->readUInt16();
    }
    
    return list_entry;
}

void PageSegmentLongStorage::removeListEntryByPageId(page_id_t page_id)
{
    page_segment_long_storage_list_item_t de;
    for ( uint8_t i = 0; i < listEntryCount(); ++i )
    {
        de = listEntryAt( i );
        if (de.page_id == page_id)
        {
            removeListEntryAt( i );
            
            updateMaxChunkCountAvail();
            return;
        }
    }
}

page_segment_long_storage_list_item_t PageSegmentLongStorage::removeListEntryAt(uint8_t pos)
{
    m_data->seek( OFFSET_PAGES_INFO_LIST + ( pos * PAGE_LONG_STORAGE_LIST_ENTRY_SIZE ) );
    
    page_segment_long_storage_list_item_t dict_entry;
    dict_entry.page_id = m_data->readUInt32();
    dict_entry.max_free_chunk_block = m_data->readUInt16();
    
    pos++;
    uint16_t count = listEntryCount() - pos;
    
    m_data->shiftLeft( ( count ) * PAGE_LONG_STORAGE_LIST_ENTRY_SIZE, count );
    decListEntryCount();
    
    return dict_entry;
}

uint8_t PageSegmentLongStorage::saveListEntry(const page_segment_long_storage_list_item_t& list_entry)
{

    uint8_t savePos = findListSavePos(list_entry);
    writeListEntrytAt( savePos, list_entry );
    
    updateMaxChunkCountAvail();
    
    return savePos;
}

void PageSegmentLongStorage::writeListEntrytAt(uint8_t pos, const page_segment_long_storage_list_item_t& list_entry)
{
    uint16_t ptr = OFFSET_PAGES_INFO_LIST + ( pos * PAGE_LONG_STORAGE_LIST_ENTRY_SIZE );
    m_data->seek( ptr );
    bool replace = false;
    
    if ( pos < listEntryCount() )
    {
        if (m_data->readUInt32At( ptr ) == list_entry.page_id)
        {
            replace = true;
        }
    }
    
    if ( ! replace )
    {
        uint16_t count = listEntryCount() - pos;
        
        m_data->shiftRight( count * PAGE_LONG_STORAGE_LIST_ENTRY_SIZE, count );
        
        m_data->writeUInt32(list_entry.page_id);
        m_data->writeUInt16(list_entry.max_free_chunk_block);
        
        incListEntryCount();
    }
    else
    {
        m_data->moveBy( 4 );
        m_data->writeUInt16(list_entry.max_free_chunk_block);
    }
    
}
