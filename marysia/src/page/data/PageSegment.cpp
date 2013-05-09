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


#include "PageSegment.h"

#include <iostream>

#include "utils/ByteData.h"

using namespace std;

PageSegment::PageSegment(ByteData* data): Page(data)
{
    m_page_size = PAGE_SIZE;
    m_is_fixed_lenght_page = true;
}

PageSegment::PageSegment(ByteData* data, uint8_t extentCount): Page(data)
{
    m_page_size = PAGE_SIZE;
    m_is_fixed_lenght_page = true;
}

uint8_t PageSegment::extentCount() const
{
    return m_data->readUInt8At(OFFSET_EXTENT_COUNT);
}

void PageSegment::setExtentCount(uint8_t count)
{
    m_data->writeUInt8At(OFFSET_EXTENT_COUNT,count);
    m_is_dirty = true;
}

uint16_t PageSegment::pageCount() const
{
    return m_data->readUInt16At(OFFSET_PAGE_COUNT);
}

void PageSegment::setPageCount(uint16_t count)
{
    m_data->writeUInt16At(OFFSET_PAGE_COUNT, count);
    markDirty();
}

void PageSegment::incPageCount()
{
    setPageCount( pageCount() + 1 );
}

void PageSegment::decPageCount()
{
    setPageCount( pageCount() - 1 );
}

bool PageSegment::hasFreePage() const
{
    //cout << pageCount() << " < " << (((uint16_t)extentCount())<<6) << endl;
    return pageCount() < (((uint16_t)extentCount())<<6);
}

void PageSegment::setSuccessorSegmentId(segment_id_t segment_id)
{
    m_data->writeUInt16At( OFFSET_SUCCESSOR_SEGMENT, segment_id );
    markDirty();
}

segment_id_t PageSegment::successorSegmentId() const
{
    return m_data->readUInt16At( OFFSET_SUCCESSOR_SEGMENT );
}

void PageSegment::setPredecesorSegmentId(segment_id_t segment_id)
{
    m_data->writeUInt16At( OFFSET_PREDECESOR_SEGMENT, segment_id );
    markDirty();
}

segment_id_t PageSegment::predecesorSegmentId() const
{
    return m_data->readUInt16At( OFFSET_PREDECESOR_SEGMENT );
}

void PageSegment::clearPageBitfield()
{
    m_data->seek(OFFSET_PAGES_BITFIELD);
    for (uint8_t i = 0; i < (extentCount() << 3); ++i)
    {
        m_data->writeUInt8(0);
    }
    markDirty();
}

void PageSegment::setBitfieldBit(uint16_t nr, uint8_t bit)
{
    uint16_t pos = (OFFSET_PAGES_BITFIELD + ( nr >> 3 ));

    uint8_t bitnr = (nr & 0b00000111);

    uint8_t bitfield = m_data->readUInt8At(pos);

    uint8_t mask = (0b10000000 >> bitnr);

    bitfield = (bit)?(bitfield | mask):(bitfield & (~mask));

    m_data->writeUInt8At(pos, bitfield);

    markDirty();
}

page_id_t PageSegment::findFirstFreePage() const
{
    m_data->seek(OFFSET_PAGES_BITFIELD);
    for (uint8_t i = 0; i < (extentCount() << 3); ++i)
    {
        uint8_t pos = m_data->pos();
        uint8_t bitfield = m_data->readUInt8();

        if (bitfield != 0b11111111)
        {
            uint8_t count = 0;

            while ( bitfield & 0b10000000 )
            {
                count++;
                bitfield <<= 1;
            }

            return (i << 3) | count;
        }

    }

    return 0;
}

