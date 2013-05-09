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


#include "PageSystemSegment.h"

#include <iostream>

#include "page/PageFactory.h"
#include "page/data/PageSegment.h"
#include "utils/ByteData.h"

using namespace std;

PageSystemSegment::PageSystemSegment(ByteData* data) : Page(data)
{
    m_page_size = PAGE_SIZE;
    m_is_fixed_lenght_page = true;
}

void PageSystemSegment::initEmpty()
{
    setSegmentCount( 0 );
    setSegmentIdCounter( 0 );
    setFreePageId( 0 );
    setKeysCount( 0 );
}

void PageSystemSegment::increaseSegmentCount()
{
    setSegmentCount( segmentCount() + 1 );
}

uint16_t PageSystemSegment::segmentCount() const
{
    return m_data->readUInt16At( OFFSET_SEGMENT_INFO );
}

void PageSystemSegment::setSegmentCount( uint16_t count )
{
    m_data->writeUInt16At( OFFSET_SEGMENT_INFO, count );
    m_is_dirty = true;
}

void PageSystemSegment::increaseSegmentIdCounter()
{
    setSegmentCount( segmentIdCounter() + 1 );
}

segment_id_t PageSystemSegment::segmentIdCounter() const
{
    return m_data->readUInt16At( OFFSET_SEGMENT_INFO + 2 );
}

segment_id_t PageSystemSegment::nextSegmentId() const
{
    return segmentIdCounter() + ( segment_id_t ) 1;
}

void PageSystemSegment::setSegmentIdCounter( segment_id_t count )
{
    m_data->writeUInt16At( OFFSET_SEGMENT_INFO + 2, count );
    m_is_dirty = true;
}

page_id_t PageSystemSegment::freePageId() const
{
    return m_data->readUInt32At( OFFSET_SEGMENT_INFO + 4 );
}

void PageSystemSegment::setFreePageId( page_id_t page_id )
{
    m_data->writeUInt32At( OFFSET_SEGMENT_INFO+4, page_id );
    m_is_dirty = true;
}

PageSegment* PageSystemSegment::registerSegment( uint8_t extentCount, Page::PageType segmentType )
{
    uint16_t cntr = OFFSET_SEGMENT_LIST;
    bool newSeg = true;
    page_id_t segment_page_id;
    segment_id_t segment_id;

    for ( uint16_t i = 0; i < segmentCount(); ++i )
    {
        uint8_t extCount = m_data->readUInt8At( cntr + 2 );
        SegmentStatus status = (SegmentStatus) m_data->readUInt8At( cntr + 3 );

        if ( ( status == SEGMENT_FREE) && ( extCount == extentCount ) )
        {
            newSeg = false;
            break;
        }

        cntr += 8;
    }

    if ( ! newSeg )
    {
        m_data->seek( cntr );
        segment_id = m_data->readUInt16();
        m_data->moveBy( 1 );
        m_data->writeUInt8( SEGMENT_USED );
        segment_page_id = m_data->readUInt32();
    } else
    {
        m_data->seek( cntr );

        segment_id = segmentIdCounter() + 1;
        segment_page_id = freePageId();

        m_data->writeUInt16( segment_id );
        m_data->writeUInt8( extentCount );
        m_data->writeUInt8( SEGMENT_USED );
        m_data->writeUInt32( segment_page_id );

        increaseSegmentCount();
        setFreePageId( segment_page_id + ( EXTENT_PAGE_COUNT * extentCount ) );
    }


    if ( segment_id > segmentIdCounter() )
    {
        setSegmentIdCounter( segment_id );
    }

    return PageFactory::createPageSegment( m_filename, segment_page_id, segment_id, extentCount, segmentType );
}

void PageSystemSegment::unregisterSegment(segment_id_t segment_id)
{
    uint16_t cntr = OFFSET_SEGMENT_LIST;
    uint8_t foundPos = 0;
    uint16_t segCount = segmentCount();
    for ( uint16_t i = 0; i < segCount; ++i, foundPos = i )
    {
        segment_id_t sid = m_data->readUInt16At( cntr );
        if ( sid == segment_id )
        {
            break;
        }
        
        cntr += 8;
    }

    uint16_t foundPtr = cntr;

    bool allUnused = true;

    cntr += 8;

    for ( uint16_t i = foundPos + 1; i < segCount; ++i )
    {
        SegmentStatus status = (SegmentStatus) m_data->readUInt8At( cntr + 3 );
        if ( status == SEGMENT_USED )
        {
            allUnused = false;
            break;
        }

        cntr += 8;
    }

    cntr = OFFSET_SEGMENT_LIST + ( 8 * ( foundPos - 1 ) );

    if ( allUnused )
    {
        for ( uint16_t i = foundPos - 1; i >= 0; --i )
        {
            SegmentStatus status = (SegmentStatus) m_data->readUInt8At( cntr + 3 );
            if ( status == SEGMENT_USED )
            {
                break;
            }

            foundPos--;
            foundPtr-=8;
            cntr -= 8;
        }
    }


    if ( ! allUnused )
    {
        m_data->writeUInt8At( foundPtr + 3, SEGMENT_FREE );
        markDirty();
    }
    else
    {
        page_id_t page_id = m_data->readUInt32At( foundPtr + 4 );
        m_data->dataSetAt( foundPtr, 0, 8 * ( segCount - foundPos ) );
        setSegmentCount( segCount - ( segCount - foundPos ) );
        setFreePageId( page_id );
    }

}

page_id_t PageSystemSegment::pageIdBySegmentId( segment_id_t segment_id ) const
{
    m_data->seek( OFFSET_SEGMENT_LIST );
    while ( m_data->readUInt16() != segment_id ) {
        m_data->moveBy( 6 );
    }

    m_data->moveBy( 2 );
    return m_data->readUInt32();
}

uint8_t PageSystemSegment::keysCount() const
{
    return m_data->readUInt8At( OFFSET_KEYS_INFO );
}

void PageSystemSegment::setKeysCount(uint8_t count)
{
    m_data->writeUInt8At( OFFSET_KEYS_INFO, count );
    m_is_dirty = true;
}

void PageSystemSegment::incrementKeysCount()
{
    setKeysCount( keysCount() + 1 );
}

void PageSystemSegment::addKeyEntry(const String& keyName, segment_id_t nodes_segment_id, segment_id_t data_segment_id, page_id_t nodes_page_id, page_id_t data_page_id, int32_t autoinc_value, page_id_t long_storage_page_id)
{
    m_data->seek( OFFSET_KEYS_LIST + ( KEY_ENTRY_SIZE * keysCount() ) );

    m_data->writeUInt8( keyName.size() + 1 );
    m_data->writeString( keyName.c_str(), 65 );
    m_data->writeUInt16( nodes_segment_id );
    m_data->writeUInt16( data_segment_id );
    m_data->writeUInt32( nodes_page_id );
    m_data->writeUInt32( data_page_id );
    m_data->writeInt32( autoinc_value );
    m_data->writeUInt32( long_storage_page_id );

    incrementKeysCount();
    markDirty();
}

uint16_t PageSystemSegment::keyPosByName(const String& keyName) const
{
    uint8_t keyNum = keysCount();
    uint8_t count = 0;
    String str;
    if ( keyNum > 0 )
    {
        m_data->seek( OFFSET_KEYS_LIST - KEY_INFO_SIZE );
        uint8_t strLen = 0;
        do
        {
            if ( ( ++count ) > keyNum ) return 0;

            m_data->moveBy( KEY_INFO_SIZE );
            strLen = m_data->readUInt8();
            str = m_data->readString( strLen, 65 );
            
        } while ( str != keyName );
    }
    return m_data->pos();
}

void PageSystemSegment::updateKeyEntry(const String& keyName, key_entry_info_st* key_entry)
{
    uint16_t pos = keyPosByName( keyName );
    if ( pos )
    {
        m_data->seek( pos );
        m_data->writeUInt16( key_entry->nodes_id );
        m_data->writeUInt16( key_entry->data_id );
        m_data->writeUInt32( key_entry->nodes_page_id );
        m_data->writeUInt32( key_entry->data_page_id );
        m_data->writeInt32( key_entry->autoinc_value );
        m_data->writeUInt32( key_entry->long_storage_page_id );
        markDirty();
    }
}

key_entry_info_st* PageSystemSegment::keyEntryByName(const String& keyName) const
{
    uint16_t pos = keyPosByName( keyName );
    if ( pos )
    {
        m_data->seek( pos );
        key_entry_info_st* entry = new key_entry_info_st;
        entry->keyName = keyName;
        entry->nodes_id = m_data->readUInt16();
        entry->data_id = m_data->readUInt16();
        entry->nodes_page_id = m_data->readUInt32();
        entry->data_page_id = m_data->readUInt32();
        entry->autoinc_value = m_data->readInt32();
        entry->long_storage_page_id = m_data->readUInt32();
        return entry;
    }
    return NULL;
}

void PageSystemSegment::debugSystemPage()
{
    cout << "==== DEBUG SYSTEM START ====" << endl;
    cout << "segment count " << segmentCount() << endl;
    cout << "segment id counter "<< segmentIdCounter() << endl;
    cout << "free page id " << freePageId() << endl;
    debugRegisteredSegments();
    cout << "keys count " << (uint16_t)keysCount() << endl;;
    m_data->seek(OFFSET_KEYS_LIST);
    for (int i = 0; i < keysCount(); ++i)
    {
        uint8_t len = m_data->readUInt8();
        cout << "key name " << m_data->readString(len, 65) << endl;;
        cout << "node segment id " << m_data->readUInt16() << endl;;
        cout << "data segment id " << m_data->readUInt16() << endl;;
        cout << "node page id " << m_data->readUInt32() << endl;;
        cout << "data page id " << m_data->readUInt32() << endl;;
    }
    cout << "==== DEBUG SYSTEM STOP ====" << endl;
}

void PageSystemSegment::debugRegisteredSegments()
{
    m_data->seek(OFFSET_SEGMENT_LIST);
    for (int i = 0; i < segmentCount(); ++i)
    {
        cout << "segment id " << (uint32_t) m_data->readUInt16() << endl;
        cout << "extent count " << (uint32_t) m_data->readUInt8() << endl;
        cout << "segment status " << (uint32_t) m_data->readUInt8() << endl;
        cout << "page id " << (uint32_t) m_data->readUInt32() << endl;
        cout << "----" << endl;
    }
}


