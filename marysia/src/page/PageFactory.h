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


#ifndef PAGEFACTORY_H
#define PAGEFACTORY_H

#include "Page.h"

class PageSegment;
class PageSystemSegment;
class ByteData;
class PageTableInfo;
class PageIndexLeaf;
class PageIndexIntern;
class PageBlockStorage;

class PageFactory
{
public:
    static Page* createPage(ByteData* data);
    static PageSegment* createPageSegment(const String& filename, page_id_t page_id, segment_id_t segment_id, uint8_t extentCount, Page::PageType sType = Page::SEGMENT_INODE);
    static Page* createPageUndefined(const String& filename, page_id_t page_id, segment_id_t segment_id);
    static PageSystemSegment* createPageSystemSegment(const String& filename );
    static PageTableInfo* createPageTableInfo(const String& tablename);
    static PageIndexLeaf* createPageIndexLeaf(const String& filename, page_id_t page_id, segment_id_t segment_id, PageTableInfo* page_tableinfo);
    static PageIndexIntern* createPageIndexIntern(const String& filename, page_id_t page_id, segment_id_t segment_id, PageTableInfo* page_tableinfo);
    static PageBlockStorage* createPageBlockStorage(const String& filename, page_id_t page_id, segment_id_t segment_id);
};

#endif // PAGEFACTORY_H
