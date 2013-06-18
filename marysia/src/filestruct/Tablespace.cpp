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

#include "cache/Cache.h"
#include "filestruct/Tableinfo.h"
#include "filestruct/Tablespace.h"
#include "io/File.h"
#include "table/KeyInfo.h"
#include "page/data/PageBlockStorage.h"
#include "page/data/PageIndexIntern.h"
#include "page/data/PageIndexLeaf.h"
#include "page/data/PageSegment.h"
#include "page/data/PageSegmentLongStorage.h"
#include "page/data/PageSystemSegment.h"
#include "page/PageFactory.h"

Tablespace::Tablespace(Cache* buffer):m_buffer(buffer)
{
}

File* Tablespace::dataFile() const
{
    return m_buffer->fileByFilename( m_current_filename );
}

void Tablespace::openTable(const String& tablename, PageTableInfo* page_table_info)
{
    setCurrentTable( tablename );
    m_page_tableinfo = page_table_info;
}

void Tablespace::initTablespace()
{
    PageSystemSegment* syspage = PageFactory::createPageSystemSegment( m_current_filename );

    m_buffer->savePage( syspage );

    PageSegment* pageSeg = pageSegment( allocateSegment( 1 ).page_id );

    pageSeg->setBitfieldBit( 1, 1 );

    m_buffer->savePage( pageSeg );
}

/**
 * dodaje pusty klucz
 */
void Tablespace::addKey(KeyInfo* keyInfo, Tableinfo* tableinfo)
{
    PageSystemSegment* syspage = pageSystemSegment();

    page_info_t page_info = allocatePage( allocateSegment( 1 ).segment_id );

    PageIndexLeaf* indexLeaf = PageFactory::createPageIndexLeaf(m_current_filename, page_info.page_id, page_info.segment_id, tableinfo->page() );

    syspage->addKeyEntry( keyInfo->keyName(), 0, page_info.segment_id, 0, page_info.page_id );

    m_buffer->savePage( syspage );
    m_buffer->savePage( indexLeaf );
}

void Tablespace::closeTable()
{

}

PageSystemSegment* Tablespace::pageSystemSegment() const
{
    return static_cast<PageSystemSegment*>(m_buffer->getPage( PAGE_SYSTEM_SEGMENTS_ID , m_current_filename));
}

PageSegment* Tablespace::pageSegment( page_id_t page_id ) const
{
    return static_cast<PageSegment*> ( m_buffer->getPage( page_id, m_current_filename ) );
}

PageSegmentLongStorage* Tablespace::pageSegmentLongStorage(page_id_t page_id) const
{
    return static_cast<PageSegmentLongStorage*> ( m_buffer->getPage( page_id, m_current_filename ) );
}

PageIndexIntern* Tablespace::pageIndexIntern(page_id_t page_id) const
{
    PageIndexIntern* ind = static_cast<PageIndexIntern*>( m_buffer->getPage( page_id, m_current_filename ) );
    ind->setPageTableinfo( m_page_tableinfo );
    return ind;
}

PageIndexLeaf* Tablespace::pageIndexLeaf(page_id_t page_id) const
{
    PageIndexLeaf* ind = static_cast<PageIndexLeaf*>( m_buffer->getPage( page_id, m_current_filename ) );
    ind->setPageTableinfo( m_page_tableinfo );
    return ind;
}

PageIndex* Tablespace::pageIndex(page_id_t page_id) const
{
    PageIndex* ind = static_cast<PageIndex*>( m_buffer->getPage( page_id, m_current_filename ) );
    ind->setPageTableinfo( m_page_tableinfo );
    return ind;
}

PageBlockStorage* Tablespace::pageBlockStorage(page_id_t page_id) const
{
    return static_cast<PageBlockStorage*>(m_buffer->getPage( page_id, m_current_filename ));
}

void Tablespace::provideEnoughSpace(uint32_t spaceRequired)
{
    File* vdataFile = dataFile();
    if (vdataFile->msize() < spaceRequired) {
        vdataFile->allocateSpace(spaceRequired - vdataFile->msize());
    }
}

page_info_t Tablespace::allocateSegment( uint8_t extentCount, Page::PageType sType )
{
    PageSystemSegment* syspage = pageSystemSegment();

    PageSegment* segment = syspage->registerSegment( extentCount, sType );

    provideEnoughSpace( segment->pageId() * PAGE_SIZE + ( extentCount * EXTENT_SIZE ) );

    segment->setBitfieldBit(0,1);

    segment->setPageCount( 1 );

    page_info_t page_info;
    page_info.page_id = segment->pageId();
    page_info.segment_id = segment->segmentId();

    m_buffer->savePage(syspage);
    m_buffer->savePage(segment);

    return page_info;
}

/**
 * @brief alokuje strone w podanym segmencie. Jesli segment jest pelny, alokuje kolejny polaczony wskaznikiem z poprzednim
 *
 * @param segment_id segment w ktorym alokujemy strone
 * @return page_id_t
 **/
page_info_t Tablespace::allocatePage( segment_id_t segment_id, Page::PageType sType )
{
    page_info_t page_info;

    PageSystemSegment* syspage = pageSystemSegment();

    page_id_t page_id = syspage->pageIdBySegmentId( segment_id );

    PageSegment* segment = pageSegment( page_id );

    if ( segment->hasFreePage() )
    {
        uint16_t first_page_nr = segment->findFirstFreePage();

        page_id += first_page_nr;

        Page* page = PageFactory::createPageUndefined( m_current_filename, page_id, segment->segmentId() );

        page->setSegmentId( segment_id );

        m_buffer->savePage(page);

        segment->setBitfieldBit( first_page_nr, 1 );

        segment->incPageCount();

        page_info.page_id = page_id;
        page_info.segment_id = segment_id;
    } else if ( segment->successorSegmentId() != 0 )
    {
        page_info = allocatePage( segment->successorSegmentId() );

    } else
    {
        page_info_t pi = allocateSegment( segment->extentCount(), sType );
        PageSegment* newSeg = pageSegment( pi.page_id );

        segment->setSuccessorSegmentId( newSeg->segmentId() );
        newSeg->setPredecesorSegmentId( segment->segmentId() );

        page_info = allocatePage( pi.segment_id );

        m_buffer->savePage( newSeg );
    }

    m_buffer->savePage( segment );
    m_buffer->savePage( syspage );

    return page_info;
}

void Tablespace::deallocatePage(Page* page)
{
    segment_id_t segment_id = page->segmentId();
    page_id_t page_id = page->pageId();

    PageSystemSegment* syspage = pageSystemSegment();
    page_id_t page_segment_id = syspage->pageIdBySegmentId( segment_id );

    PageSegment* segment = pageSegment( page_segment_id );
    uint16_t page_nr = page_id - segment->pageId();

    segment->setBitfieldBit( page_nr, 0 );

    segment->decPageCount();

    page->deallocate();

    if ( segment->pageCount() == 1 )
    {
        deallocateSegment( segment );
    }
    
    m_buffer->savePage( segment );
    m_buffer->savePage( syspage );
}

void Tablespace::deallocateSegment( PageSegment* segment )
{
    PageSystemSegment* syspage = pageSystemSegment();

    segment_id_t succ_seg_id = segment->successorSegmentId();
    segment_id_t pred_seg_id = segment->predecesorSegmentId();

    if ( succ_seg_id != 0 )
    {
        PageSegment* seg = pageSegment( syspage->pageIdBySegmentId( succ_seg_id ) );
        seg->setPredecesorSegmentId( segment->predecesorSegmentId() );
        m_buffer->savePage( seg );
    }

    if ( pred_seg_id != 0 )
    {
        PageSegment* seg = pageSegment( syspage->pageIdBySegmentId( pred_seg_id ) );
        seg->setSuccessorSegmentId( segment->successorSegmentId() );
        m_buffer->savePage( seg );
    }

    syspage->unregisterSegment( segment->segmentId() );

    segment->deallocate();

    m_buffer->savePage( syspage );
}

String Tablespace::createTableDataFilename(const String& tablename)
{
    return tablename + "." + TABLE_SPACE_EXT;
}

