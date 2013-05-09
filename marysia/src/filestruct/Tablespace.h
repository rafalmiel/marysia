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


#ifndef TABLESPACE_H
#define TABLESPACE_H
#include <stdio.h>

#define TABLE_SPACE_EXT "bin"

#include "page/Page.h"

class KeyInfo;
class PageSegment;
class PageSegmentLongStorage;
class PageSystemSegment;
class File;
class Tableinfo;
class PageBlockStorage;
class PageIndexLeaf;
class PageIndexIntern;
class PageIndex;
class Buffer;
class PageTableInfo;


struct page_info_t {
    page_id_t page_id;
    segment_id_t segment_id;
};

class Tablespace
{
public:
    Tablespace(Buffer* buffer);

    enum SystemPageIds
    {
        PAGE_SYSTEM_SEGMENTS_ID = 1,
    };

    void initTablespace();

    void openTable( const String& tablename, PageTableInfo* page_table_info );
    
    void addKey( KeyInfo* keyInfo, Tableinfo* tableinfo );
    
    page_info_t allocateSegment(uint8_t extentCount, Page::PageType sType = Page::SEGMENT_INODE);
    page_info_t allocatePage(segment_id_t segment_id, Page::PageType sType = Page::SEGMENT_INODE);

    void deallocatePage( Page* page );
    void deallocateSegment( PageSegment* segment );

    void setCurrentTable(const String& tablename)
    {
        m_current_filename = createTableDataFilename( tablename );
    }

    static String createTableDataFilename(const String& tablename);

    void closeTable();

    void provideEnoughSpace(uint32_t spaceRequired);
    
    File* dataFile() const;
    
    PageSystemSegment* pageSystemSegment() const;
    PageSegment* pageSegment( page_id_t page_id ) const;
    PageSegmentLongStorage* pageSegmentLongStorage( page_id_t page_id ) const;
    PageIndexLeaf* pageIndexLeaf( page_id_t page_id ) const;
    PageIndexIntern* pageIndexIntern( page_id_t page_id ) const;
    PageIndex* pageIndex( page_id_t page_id ) const;
    PageBlockStorage* pageBlockStorage(page_id_t page_id) const;
private:
    Buffer* m_buffer;
    PageTableInfo* m_page_tableinfo;


    String m_current_filename;
};

#endif // TABLESPACE_H
