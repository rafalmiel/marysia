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

#include "PageFactory.h"

#include <iostream>

#include "cache/Cache.h"
#include "data/PageBlockStorage.h"
#include "data/PageIndexIntern.h"
#include "data/PageIndexLeaf.h"
#include "data/PageSegment.h"
#include "data/PageSystemSegment.h"
#include "data/PageUndefined.h"
#include "filestruct/Tableinfo.h"
#include "filestruct/Tablespace.h"
#include "page/data/PageSegmentLongStorage.h"
#include "tableinfo/PageTableInfo.h"
#include "tableinfo/PageTableInfo.h"
#include "utils/ByteData.h"

using namespace std;

Page* PageFactory::createPage(ByteData* data)
{
    Page* page = 0;

    if (data->size() >= Page::PAGE_HEADER_SIZE)
    {
        Page::PageType type = static_cast<Page::PageType>(data->readUInt8At(Page::OFFSET_PAGE_TYPE));
        
        switch (type)
        {
            case Page::SEGMENT_INODE:
                page = new PageSegment(data);
                break;
            case Page::SEGMENT_LONG_STORAGE_INODE:
                page = new PageSegmentLongStorage(data);
                break;
            case Page::SYSTEM_SEGMENT_PAGE:
                page = new PageSystemSegment(data);
                break;
            case Page::INDEX_INTERN_NODE:
                page = new PageIndexIntern(data);
                break;
            case Page::INDEX_LEAF_NODE:
                page = new PageIndexLeaf(data);
                break;
            case Page::TABLE_INFO_PAGE:
                page = new PageTableInfo(data);
                break;
            case Page::BLOCK_STORAGE_PAGE:
                page = new PageBlockStorage(data);
                break;
            default:

                break;
        }
    }
    return page;
}

Page* PageFactory::createPageUndefined(const String& filename, page_id_t page_id, segment_id_t segment_id)
{
    Page* page = new PageUndefined(new ByteData(PAGE_SIZE));
    page->setFilename(filename);
    page->setPageId(page_id);
    page->setSegmentId( segment_id );
    page->setPageType(Page::UNDEFINED);

    return page;
}


PageSegment* PageFactory::createPageSegment(const String& filename, page_id_t page_id, segment_id_t segment_id, uint8_t extentCount, Page::PageType sType)
{
    PageSegment* segment = new PageSegment(new ByteData(PAGE_SIZE));
    segment->setFilename(filename);
    segment->setPageId(page_id);
    segment->setPageType(sType);
    segment->setSegmentId(segment_id);
    segment->setExtentCount(extentCount);
    segment->clearPageBitfield();
    
    return segment;
}

PageSystemSegment* PageFactory::createPageSystemSegment(const String& filename)
{
    PageSystemSegment* syspage = new PageSystemSegment(new ByteData(PAGE_SIZE));
    syspage->setPageId( Tablespace::PAGE_SYSTEM_SEGMENTS_ID );
    syspage->setPageType( Page::SYSTEM_SEGMENT_PAGE );
    syspage->setSegmentId( 1 );
    syspage->setFilename( filename );
    syspage->initEmpty();

    return syspage;
}

PageTableInfo* PageFactory::createPageTableInfo(const String& tablename)
{
    PageTableInfo* tableinfo = new PageTableInfo(new ByteData(Page::PAGE_HEADER_SIZE),false);
    tableinfo->setPageId(0);
    tableinfo->setPageType(Page::TABLE_INFO_PAGE);
    tableinfo->setTableName(tablename);
    tableinfo->setSegmentId( 1 );

    tableinfo->setFilename( Tableinfo::createTableFilename(tablename) );

    return tableinfo;
}

PageIndexLeaf* PageFactory::createPageIndexLeaf(const String& filename, page_id_t page_id, segment_id_t segment_id, PageTableInfo* page_tableinfo)
{
    PageIndexLeaf* pageIndexLeaf = new PageIndexLeaf( new ByteData(PAGE_SIZE) );
    pageIndexLeaf->setPageId( page_id );
    pageIndexLeaf->setSegmentId( segment_id );
    pageIndexLeaf->setPageType( Page::INDEX_LEAF_NODE );
    pageIndexLeaf->setFilename( filename );
    pageIndexLeaf->setPageTableinfo( page_tableinfo );
    pageIndexLeaf->initEmpty();

    return pageIndexLeaf;
}

PageIndexIntern* PageFactory::createPageIndexIntern(const String& filename, page_id_t page_id, segment_id_t segment_id, PageTableInfo* page_tableinfo)
{
    PageIndexIntern* pageIndexIntern = new PageIndexIntern( new ByteData(PAGE_SIZE) );
    pageIndexIntern->setPageId( page_id );
    pageIndexIntern->setSegmentId( segment_id );
    pageIndexIntern->setPageType( Page::INDEX_INTERN_NODE );
    pageIndexIntern->setFilename( filename );
    pageIndexIntern->setPageTableinfo( page_tableinfo );
    pageIndexIntern->initEmpty();
    
    return pageIndexIntern;
}

PageBlockStorage* PageFactory::createPageBlockStorage(const String& filename, page_id_t page_id, segment_id_t segment_id)
{
    PageBlockStorage* pageBlock = new PageBlockStorage( new ByteData(PAGE_SIZE));
    pageBlock->setPageId(page_id);
    pageBlock->setSegmentId(segment_id);
    pageBlock->setPageType(Page::BLOCK_STORAGE_PAGE);
    pageBlock->setFilename(filename);
    pageBlock->initEmpty();
    
    return pageBlock;
}
