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


#ifndef PAGESEGMENT_H
#define PAGESEGMENT_H

#include "page/Page.h"

class PageSegment : public Page
{
public:
    enum PageSegmentOffset {
        OFFSET_EXTENT_COUNT = PAGE_HEADER_SIZE, //1 byte - max 16 extents
        OFFSET_PAGE_COUNT = OFFSET_EXTENT_COUNT + 1,
        OFFSET_SUCCESSOR_SEGMENT = OFFSET_PAGE_COUNT + 2,
        OFFSET_PREDECESOR_SEGMENT = OFFSET_SUCCESSOR_SEGMENT + 2,
        OFFSET_PAGES_BITFIELD = OFFSET_PREDECESOR_SEGMENT + 2 // 128 bytes
    };
    
    PageSegment(ByteData* data);
    PageSegment(ByteData* data, uint8_t extentCount);

    uint8_t extentCount() const;
    void setExtentCount(uint8_t count);

    uint16_t pageCount() const;
    void setPageCount(uint16_t count);

    void incPageCount();
    void decPageCount();

    bool hasFreePage() const;

    void clearPageBitfield();

    void setBitfieldBit(uint16_t nr, uint8_t bit);

    page_id_t findFirstFreePage() const;

    segment_id_t successorSegmentId() const;
    void setSuccessorSegmentId( segment_id_t segment_id );

    segment_id_t predecesorSegmentId() const;
    void setPredecesorSegmentId( segment_id_t segment_id );
};

#endif // PAGESEGMENT_H
