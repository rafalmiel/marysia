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


#ifndef PAGE_H
#define PAGE_H

#include <stdint.h>

#include "utils/String.h"

class ByteData;

#define PAGE_SIZE           16384
#define EXTENT_PAGE_COUNT   64
#define EXTENT_SIZE         EXTENT_PAGE_COUNT*PAGE_SIZE

typedef uint32_t page_id_t;
typedef uint16_t segment_id_t;

class Page
{

public:
    enum PageType {
        SYSTEM_SEGMENT_PAGE = 1,
        TABLE_INFO_PAGE = 2,
        SEGMENT_INODE = 6,
        SEGMENT_LONG_STORAGE_INODE = 7,
        BLOCK_STORAGE_PAGE = 10,
        INDEX_INTERN_NODE = 16,
        INDEX_LEAF_NODE = 17,
        UNDEFINED = 255
    };
    
    enum PageHeaderOffset {
        OFFSET_PAGE_ID = 0,     // 4 bytes
        OFFSET_PAGE_TYPE = OFFSET_PAGE_ID + 4,    // 1 byte
        OFFSET_SEGMENT_ID = OFFSET_PAGE_TYPE + 1  //2 bytes
    };

    static const uint8_t PAGE_HEADER_SIZE = 8;

    
    Page(ByteData* data);
    virtual ~Page();

    virtual ByteData* data()
    {
        return m_data;
    }

    page_id_t pageId() const;
    PageType pageType() const;
    segment_id_t segmentId() const;

    bool isDeallocated() const { return m_is_deallocated; }
    void deallocate();
    void resetData();

    void setPageId(page_id_t id);
    void setPageType(PageType type);
    void setSegmentId(segment_id_t segment_id);

    void setIsFixedLenght( bool is ) { m_is_fixed_lenght_page = is; }

    inline const String& filename() const { return m_filename; }
    inline void setFilename(const String& filename) { m_filename = filename; }

    inline bool isFixedLenghtPage() const { return m_is_fixed_lenght_page; }
    inline uint16_t pageSize() const { return m_page_size; }

    inline void markDirty( bool is = true ) { m_is_dirty = is; }

    bool isDirty() const { return m_is_dirty; }

    void flushToFile( const String& filename );
    
protected:
    bool m_is_dirty;
    
    ByteData* m_data;

    String m_filename;

    uint16_t m_page_size;
    bool m_is_fixed_lenght_page;

    bool m_is_deallocated;
};

#endif // PAGE_H
