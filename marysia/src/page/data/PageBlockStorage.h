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


#ifndef PAGEBLOCKSTORAGE_H
#define PAGEBLOCKSTORAGE_H

#include "page/Page.h"
#include "table/BlockDefs.h"

/**
 * PAGE STRUCTURE:
 *
 *   x|   HEADER
 *  32|   block_count (int 1) max_chunk_size_available(int 1)
 * 256|   blocks dict
 *   *|   blocks
 */
class PageBlockStorage : public Page
{
public:
    enum LongStorageOffset {
        OFFSET_BLOCK_COUNT = PAGE_HEADER_SIZE,
        OFFSET_MAX_CHUNK_AVAIL = OFFSET_BLOCK_COUNT + 1,
        OFFSET_INFO = OFFSET_MAX_CHUNK_AVAIL + 1,
        OFFSET_CHUNK_DICT = OFFSET_INFO + 30,
        OFFSET_DATA = OFFSET_CHUNK_DICT + 128
    };
    
    enum LongStorageConsts {
        MAX_BLOCK_COUNT = 126,
        BLOCK_SIZE = 128,
        BLOCK_HEADER_SIZE = 8,
        DICT_ENTRY_SIZE = 2,
        MAX_BLOCK_ON_PAGE = MAX_BLOCK_COUNT*BLOCK_SIZE - BLOCK_HEADER_SIZE
    };
    
    void initEmpty();
    
    PageBlockStorage(ByteData* data);

    block_t blockAtNr(uint8_t nr) const;
    block_t blockAt(uint8_t pos) const;
    uint8_t saveBlock( const block_t& block );
    block_header_t removeBlockAtNr(uint8_t nr);
    
    uint8_t maxChunkCountAvail() const;
    static uint8_t chunksRequiredForSize(uint16_t size);
    
    uint8_t blockCount() const;
    
    block_header_t blockHeaderAtNr( uint8_t nr ) const;
    
    void updateHeaderAtNr(uint8_t nr, const block_header_t& header);
    void updateBlockPointerAtNr(uint8_t nr, uint8_t next_nr, page_id_t next_page);
private:
    void setMaxChunkCountAvail(uint8_t count);
    
    
    void setBlockCount(uint8_t count);
    
    void incBlockCount();
    void decBlockCount();
    
    uint8_t findFreeChunkNr( uint8_t chunkCount ) const;
    uint8_t findDictSavePos( const block_dict_entry_t& dict_entry ) const;
    uint8_t findDictEntryPosByNr( uint8_t nr, bool& found ) const;

    block_dict_entry_t dictEntryAt(uint8_t pos) const;
    static uint16_t dataPtrByChunkNum( uint8_t nr );

    block_t blockAtPtr(uint16_t posPtr) const;
    
    uint8_t saveDictEntry( const block_dict_entry_t& dict_entry );
    void writeDictEntrytAt(uint8_t pos, const block_dict_entry_t& dict_entry);
    block_dict_entry_t removeDictEntryAt(uint8_t pos);
    void removeBlockAtPtr(uint16_t posPtr);
    void writeBlockAt(uint8_t pos, const block_t& block);
    void writeBlockAtPtr(uint16_t pos, const block_t& block);
};

#endif // PAGEBLOCKSTORAGE_H
