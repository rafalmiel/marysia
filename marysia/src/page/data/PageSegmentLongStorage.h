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

#ifndef PAGESEGMENTLONGSTORAGE_H
#define PAGESEGMENTLONGSTORAGE_H

#include "page/data/PageSegment.h"

typedef struct {
    page_id_t page_id;
    uint16_t max_free_chunk_block;
} page_segment_long_storage_list_item_t;

/**
 * PAGE STRUCTURE:
 *
 * x|   PageSegment
 * 1024|  list_entry_count(int 2) | max_chunk_size_in_segment(int 1)
 * 1028|* page_id(int 4) max_free_chunk_block(int 2)
 **/
class PageSegmentLongStorage : public PageSegment
{
public:
    enum PageSegmentLongStorageOffset {
        OFFSET_LIST_ENTRY_COUNT = OFFSET_PAGES_BITFIELD + 1024,
        OFFSET_PAGES_INFO_LIST = OFFSET_LIST_ENTRY_COUNT + 4
    };
    
    enum PageSegmentLongStorageConst {
        PAGE_LONG_STORAGE_LIST_ENTRY_SIZE = 6,
    };
    
    PageSegmentLongStorage(ByteData* data);
    PageSegmentLongStorage(ByteData* data, uint8_t extentCount);
    
    uint8_t maxChunkCountAvail() const;

    uint16_t listEntryCount() const;
    uint8_t saveListEntry( const page_segment_long_storage_list_item_t& list_entry );
    page_segment_long_storage_list_item_t listEntryAt(uint8_t pos) const;
    void removeListEntryByPageId(page_id_t page_id);
    
private:
    void updateMaxChunkCountAvail();
    
    void setMaxChunkCountAvail(uint8_t count);
    
    void setListEntryCount(uint16_t count);
    
    void incListEntryCount();
    void decListEntryCount();
    
    uint8_t findListSavePos( const page_segment_long_storage_list_item_t& dict_entry ) const;
    void writeListEntrytAt(uint8_t pos, const page_segment_long_storage_list_item_t& list_entry);
    page_segment_long_storage_list_item_t removeListEntryAt(uint8_t pos);
    
};

#endif // PAGESEGMENTLONGSTORAGE_H
