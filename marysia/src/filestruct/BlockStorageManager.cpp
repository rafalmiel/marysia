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

#include "BlockStorageManager.h"

#include <vector>

#include "buffer/Buffer.h"
#include "filestruct/Tablespace.h"
#include "table/KeyInfo.h"
#include "page/data/PageBlockStorage.h"
#include "page/data/PageSegmentLongStorage.h"
#include "page/data/PageSystemSegment.h"
#include "page/PageFactory.h"
#include "page/tableinfo/PageTableInfo.h"
#include "table/BlockData.h"

BlockStorageManager::BlockStorageManager(Buffer* buffer): 
    m_buffer(buffer)
{

}

void BlockStorageManager::openTable( const String& data_filename, PageTableInfo* page_table_info, Tablespace* tablespace )
{
    m_data_filename = data_filename;
    m_page_tableinfo = page_table_info;
    m_tablespace = tablespace;
}


block_ptr_t BlockStorageManager::saveData( uint8_t* data, uint32_t len )
{
    PageSystemSegment* syspage = m_tablespace->pageSystemSegment();
    key_entry_info_st* key_entry = syspage->keyEntryByName( m_page_tableinfo->primaryKey()->keyName() );
    if ( key_entry->long_storage_page_id == 0 )
    {
        page_info_t page_info = m_tablespace->allocateSegment(2, Page::SEGMENT_LONG_STORAGE_INODE);
        key_entry->long_storage_page_id = page_info.page_id;
        syspage->updateKeyEntry(key_entry->keyName, key_entry);
    }
    
    uint16_t page_count = ( ( len % PageBlockStorage::MAX_BLOCK_ON_PAGE == 0 )?0:1 ) + ( len / PageBlockStorage::MAX_BLOCK_ON_PAGE );

    uint32_t lenLeft = len;
    std::vector<block_ptr_t> block_ptrs_vec;
    for (uint16_t i = 0; i < page_count; ++i)
    {
        block_t block;
        block_init_empty(block);
        block.block_data = data + ( i * PageBlockStorage::MAX_BLOCK_ON_PAGE );
        if ( i == page_count - 1 )
        {
            block.block_header.block_len = lenLeft;
        }
        else
        {
            block.block_header.block_len = PageBlockStorage::MAX_BLOCK_ON_PAGE;
            lenLeft -= PageBlockStorage::MAX_BLOCK_ON_PAGE;
        }
        
        block_ptrs_vec.push_back( saveBlock(block, key_entry) );
    }
    
    for ( uint16_t i = 1; i < block_ptrs_vec.size(); ++i )
    {
        PageBlockStorage* page1 = m_tablespace->pageBlockStorage( block_ptrs_vec[i-1].page_id );
        block_header_t bh;
        bh.next_nr = block_ptrs_vec[i].nr;
        bh.next_page = block_ptrs_vec[i].page_id;
        page1->updateBlockPointerAtNr( block_ptrs_vec[i-1].nr, block_ptrs_vec[i].nr, block_ptrs_vec[i].page_id );
        m_buffer->savePage( page1 );
    }
    
    m_buffer->savePage( syspage );
    delete key_entry;
    
    return block_ptrs_vec[0];
}

block_ptr_t BlockStorageManager::saveBlock( const block_t& block, key_entry_info_st* key_entry )
{
    PageSystemSegment* syspage = m_tablespace->pageSystemSegment();
    
    uint8_t chunksRequired = PageBlockStorage::chunksRequiredForSize( block.block_header.block_len );
    
    PageSegmentLongStorage* pageSegmentLongStorage = NULL;
    PageBlockStorage* pageBlockStorage = NULL;
    page_segment_long_storage_list_item_t list_entry;
    
    page_id_t next_segment_id = key_entry->long_storage_page_id;
    
    do 
    {
        if ( pageSegmentLongStorage != NULL )
        {
            m_buffer->savePage( pageSegmentLongStorage );
        }
        
        pageSegmentLongStorage = m_tablespace->pageSegmentLongStorage( next_segment_id );
        //cout << "max free " << (uint16_t) pageSegmentLongStorage->maxChunkCountAvail() << " " << endl;
        if ( pageSegmentLongStorage->maxChunkCountAvail() >= chunksRequired)
        {
            for (uint16_t i = 0; i < pageSegmentLongStorage->listEntryCount(); ++i)
            {
                if ( ( list_entry = pageSegmentLongStorage->listEntryAt( i ) ).max_free_chunk_block >= chunksRequired)
                {
                    pageBlockStorage = m_tablespace->pageBlockStorage( list_entry.page_id );
                    
                    break;
                }
            }
        }
        else if ( pageSegmentLongStorage->hasFreePage() )
        {
            break;
        }
        
        if ( pageBlockStorage == NULL )
        {
            next_segment_id = syspage->pageIdBySegmentId( pageSegmentLongStorage->successorSegmentId() );
        }
        else 
        {
            break;
        }
    }
    while ( next_segment_id != 0 );
    
    if ( pageBlockStorage == NULL )
    {
        page_info_t page_info = m_tablespace->allocatePage( pageSegmentLongStorage->segmentId(), Page::SEGMENT_LONG_STORAGE_INODE );
        if (page_info.segment_id != pageSegmentLongStorage->segmentId())
        {
            m_buffer->savePage( pageSegmentLongStorage );
            pageSegmentLongStorage = m_tablespace->pageSegmentLongStorage( syspage->pageIdBySegmentId( page_info.segment_id ) );
        }
        pageBlockStorage = PageFactory::createPageBlockStorage(m_data_filename, page_info.page_id, page_info.segment_id);
        
        //page_segment_long_storage_list_item_t list_entry;
        //list_entry.page_id = page_info.page_id;
    }
    uint8_t chunkNr = 0;
    block_ptr_t block_ptr;
    block_ptr.page_id = 0;
    if ( ( chunkNr = pageBlockStorage->saveBlock( block ) ) > 0 )
    {
        list_entry.max_free_chunk_block = pageBlockStorage->maxChunkCountAvail();
        list_entry.page_id = pageBlockStorage->pageId();
        pageSegmentLongStorage->saveListEntry( list_entry );
        
        block_ptr.page_id = pageBlockStorage->pageId();
        block_ptr.nr = chunkNr;
    }
    
    m_buffer->savePage( pageBlockStorage );
    m_buffer->savePage( pageSegmentLongStorage );
    m_buffer->savePage( syspage );
    
    return block_ptr;
}

BlockData* BlockStorageManager::readData(const block_ptr_t& block_ptr) const
{
    BlockData* blockData = new BlockData();
    readDataRecursive(block_ptr, blockData);
    return blockData;
}

void BlockStorageManager::readDataRecursive(const block_ptr_t& block_ptr, BlockData* blockData) const
{
    uint8_t nr = block_ptr.nr;
    page_id_t page_id = block_ptr.page_id;
    
    PageBlockStorage* pageBlockStorage = m_tablespace->pageBlockStorage( page_id );
    
    block_t block;
    block_init_empty(block);
    block = pageBlockStorage->blockAtNr( nr );
    blockData->addBlock( block );
    
    if (block.block_header.next_page != 0) {
        block_ptr_t bptr;
        bptr.nr = block.block_header.next_nr;
        bptr.page_id = block.block_header.next_page;
        
        readDataRecursive(bptr, blockData);
    }
    
    m_buffer->savePage( pageBlockStorage );
}

bool BlockStorageManager::deleteData( const block_ptr_t& block_ptr)
{
    page_id_t page_id = block_ptr.page_id;
    uint8_t nr = block_ptr.nr;
    PageBlockStorage* pageBlockStorage = m_tablespace->pageBlockStorage( page_id );
    bool result = false;
    block_header_t bh;
    bh = pageBlockStorage->removeBlockAtNr( nr );
    if ( ! is_block_header_empty( bh ) )
    {
        PageSystemSegment* syspage = m_tablespace->pageSystemSegment();
        PageSegmentLongStorage* segmentLongStorage = m_tablespace->pageSegmentLongStorage( syspage->pageIdBySegmentId( pageBlockStorage->segmentId() ) );
        page_segment_long_storage_list_item_t list_entry;
        list_entry.page_id = pageBlockStorage->pageId();
        list_entry.max_free_chunk_block = pageBlockStorage->maxChunkCountAvail();
        segmentLongStorage->saveListEntry( list_entry );
        
        if ( pageBlockStorage->blockCount() == 0 )
        {
            key_entry_info_st* key_entry = syspage->keyEntryByName( m_page_tableinfo->primaryKey()->keyName() );
            if (key_entry->long_storage_page_id == segmentLongStorage->pageId() )
            {
                key_entry->long_storage_page_id = 0;
                syspage->updateKeyEntry(key_entry->keyName, key_entry);
            }
            delete key_entry;
            segmentLongStorage->removeListEntryByPageId( pageBlockStorage->pageId() );
            m_tablespace->deallocatePage( pageBlockStorage );
        }

        m_buffer->savePage( segmentLongStorage );
        m_buffer->savePage( syspage );
        result = true;
    }
    m_buffer->savePage( pageBlockStorage );
    
    if ( bh.next_page != 0 )
    {
        block_ptr_t bptr;
        bptr.nr = bh.next_nr;
        bptr.page_id = bh.next_page;
        result = deleteData( bptr );
    }
    
    return result;
}

