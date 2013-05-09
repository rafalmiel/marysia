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


#ifndef PAGESYSTEMSEGMENT_H
#define PAGESYSTEMSEGMENT_H

#include "page/Page.h"

class PageSegment;

typedef struct
{
    String keyName;
    segment_id_t nodes_id;
    segment_id_t data_id;
    page_id_t nodes_page_id;
    page_id_t data_page_id;
    int32_t autoinc_value;
    page_id_t long_storage_page_id;
} key_entry_info_st;

/**
 * PAGE STRUCTURE:
 *
 * x|   HEADER
 * 8|   segmentCount (int 2) | segmentIdCounter (int 2) | freePageId ( int 4)
 * 8|*  segmentId ( int 2 ) | extentCount (int 1) | SegmentStatus (int 1) | pageId ( int 4 )
 * 8|   keysNum ( int 1 ) | OFFSET_KEYS_INFO
 * 82|* len ( int 1 ), name ( string 64 + 1 ), node_segment_id ( int 2 ), data_segment_id ( int 2 ), nodes_page_id(int 4), data_page_id(int 4), autoinc_value(int 4) | OFFSET_KEYS_LIST
 */

class PageSystemSegment : public Page
{
public:
    PageSystemSegment(ByteData* data);

    enum SystemSegmentOffset {
        OFFSET_SEGMENT_INFO = PAGE_HEADER_SIZE,     //8 bytes
        OFFSET_SEGMENT_LIST = OFFSET_SEGMENT_INFO + 8, //1024 bytes
        OFFSET_KEYS_INFO = OFFSET_SEGMENT_LIST + 12288, //8 bytes
        OFFSET_KEYS_LIST = OFFSET_KEYS_INFO + 8  //640 bytes
    };

    enum SystemSegmentConst {
        KEY_ENTRY_SIZE = 86,
        KEY_INFO_SIZE = 16
    };

    enum SegmentStatus {
        SEGMENT_FREE = 0,
        SEGMENT_USED = 1
    };

    void initEmpty();

    uint16_t segmentCount() const;
    void setSegmentCount(uint16_t count);
    void increaseSegmentCount();

    segment_id_t segmentIdCounter() const;
    void setSegmentIdCounter(segment_id_t count);
    void increaseSegmentIdCounter();
    segment_id_t nextSegmentId() const;

    page_id_t freePageId() const;
    void setFreePageId(page_id_t page_id);

    page_id_t pageIdBySegmentId(segment_id_t segment_id) const;

    PageSegment* registerSegment(uint8_t extentCount, Page::PageType segmentType = Page::SEGMENT_INODE);
    void unregisterSegment( segment_id_t segment_id );

    uint8_t keysCount() const;
    void setKeysCount(uint8_t count);
    void incrementKeysCount();
    
    void addKeyEntry(const String& keyName, segment_id_t nodes_segment_id, segment_id_t data_segment_id, page_id_t nodes_page_id, page_id_t data_page_id, int32_t autoinc_value = 1, page_id_t long_storage_page_id = 0);
    key_entry_info_st* keyEntryByName(const String& keyName) const;

    void updateKeyEntry(const String& keyName, key_entry_info_st* key_entry);

    void debugRegisteredSegments();
    void debugSystemPage();
private:
    uint16_t keyPosByName(const String& keyName) const;
};

#endif // PAGESYSTEMSEGMENT_H
