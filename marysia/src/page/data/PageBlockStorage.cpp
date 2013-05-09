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


#include "PageBlockStorage.h"

#include "utils/ByteData.h"

PageBlockStorage::PageBlockStorage(ByteData* data): Page(data)
{
    m_page_size = PAGE_SIZE;
    m_is_fixed_lenght_page = true;
}

void PageBlockStorage::initEmpty()
{
    setBlockCount( 0 );
    setMaxChunkCountAvail(MAX_BLOCK_COUNT);
}

uint8_t PageBlockStorage::saveBlock(const block_t& block)
{
    uint8_t chunkCount = chunksRequiredForSize( block.block_header.block_len );
    uint8_t chunkNr = findFreeChunkNr( chunkCount );
    
    if ( chunkNr != 0 ) 
    {
        block_dict_entry_t dict_entry;
        dict_entry.chunk_count = chunkCount;
        dict_entry.chunk_nr = chunkNr;
        
        uint8_t dictPos = saveDictEntry( dict_entry );
        writeBlockAt( dictPos, block );
        return chunkNr;
    }
    return 0;
}

void PageBlockStorage::updateHeaderAtNr(uint8_t nr, const block_header_t& header)
{
    uint16_t ptr = dataPtrByChunkNum( nr );
    m_data->seek( ptr );
    m_data->writeInt8( 0 );
    m_data->writeUInt32( header.next_page );
    m_data->writeUInt8( header.next_nr );
    m_data->writeUInt16( header.block_len );
    markDirty();
}

void PageBlockStorage::updateBlockPointerAtNr(uint8_t nr, uint8_t next_nr, page_id_t next_page)
{
    uint16_t ptr = dataPtrByChunkNum( nr );
    m_data->seek( ptr );
    m_data->writeInt8( 0 );
    m_data->writeUInt32( next_page );
    m_data->writeUInt8( next_nr );
    markDirty();
}

block_header_t PageBlockStorage::removeBlockAtNr(uint8_t nr)
{
    bool found = false;
    uint8_t dictPos = findDictEntryPosByNr( nr, found );
    block_header_t bh;
    bh.block_len = 0;
    bh.next_nr = 0;
    bh.next_page = 0;
    
    if (found == true)
    {
        removeDictEntryAt( dictPos );
        bh = blockHeaderAtNr( nr );
        removeBlockAtPtr( dataPtrByChunkNum( nr ) );
        
        return bh;
    }
    return bh;
}

void PageBlockStorage::removeBlockAtPtr(uint16_t posPtr)
{
    m_data->seek( posPtr );
    m_data->writeInt8( 0 );
    m_data->writeUInt32( 0 );
    m_data->writeUInt8( 0 );
    uint16_t len = m_data->readUInt16At( m_data->pos() );
    m_data->writeUInt16( 0 );
    m_data->dataSet(0, len);
    markDirty();
}

void PageBlockStorage::setBlockCount(uint8_t count)
{
    m_data->writeUInt8At(OFFSET_BLOCK_COUNT, count);
    markDirty();
}

uint8_t PageBlockStorage::blockCount() const
{
    return m_data->readUInt8At(OFFSET_BLOCK_COUNT);
}

void PageBlockStorage::decBlockCount()
{
    setBlockCount( blockCount() - 1 );
}

void PageBlockStorage::incBlockCount()
{
    setBlockCount( blockCount() + 1 );
}

block_dict_entry_t PageBlockStorage::removeDictEntryAt(uint8_t pos)
{
    m_data->seek( OFFSET_CHUNK_DICT + ( pos * DICT_ENTRY_SIZE ) );
    
    block_dict_entry_t dict_entry;
    dict_entry.chunk_nr = m_data->readUInt8();
    dict_entry.chunk_count = m_data->readUInt8();

    uint16_t count = blockCount() - pos;

    m_data->shiftLeft( ( count ) * DICT_ENTRY_SIZE, DICT_ENTRY_SIZE );
    decBlockCount();
    
    uint8_t newFreeChunkAv = 0;
    
    block_dict_entry_t dict_entry_rob2;
    
    if (pos > 0)
    {
        dict_entry_rob2 = dictEntryAt(pos-1);
    }
    else
    {
        dict_entry_rob2.chunk_nr = 1;
        dict_entry_rob2.chunk_count = 0;
    }
    
    if ( pos == blockCount() )
    {
        newFreeChunkAv = MAX_BLOCK_COUNT - ( dict_entry_rob2.chunk_nr - 1 );
    }
    else
    {
        block_dict_entry_t dict_entry_rob = dictEntryAt( pos );
        newFreeChunkAv = dict_entry_rob.chunk_nr - dict_entry_rob2.chunk_nr;
    }
    
    if (newFreeChunkAv > maxChunkCountAvail())
    {
        setMaxChunkCountAvail( newFreeChunkAv );
    }

    return dict_entry;
}

void PageBlockStorage::writeDictEntrytAt(uint8_t pos, const block_dict_entry_t& dict_entry)
{
    m_data->seek( OFFSET_CHUNK_DICT + ( pos * DICT_ENTRY_SIZE ) );

    uint16_t count = blockCount() - pos;

    m_data->shiftRight( count * DICT_ENTRY_SIZE, DICT_ENTRY_SIZE );

    m_data->writeUInt8(dict_entry.chunk_nr);
    m_data->writeUInt8(dict_entry.chunk_count);

    incBlockCount();
    
    uint8_t maxFreeChunkAv = maxChunkCountAvail();
    
    if ( pos == blockCount() - 1 )
    {
        maxFreeChunkAv = maxFreeChunkAv - dict_entry.chunk_count;
    }
    else
    {
        uint8_t freeChunk = 0;
        block_dict_entry_t dict_entry1 = dict_entry;
        block_dict_entry_t dict_entry2;
        for ( uint8_t i = pos+1; i < blockCount(); ++i )
        {
            dict_entry2 = dictEntryAt( i );
            freeChunk = dict_entry2.chunk_nr - (dict_entry1.chunk_nr + dict_entry1.chunk_count);
            dict_entry1 = dict_entry2;
            
            if (maxFreeChunkAv < freeChunk)
            {
                maxFreeChunkAv = freeChunk;
            }
        }
        
        freeChunk = MAX_BLOCK_COUNT - (dict_entry1.chunk_nr + dict_entry1.chunk_count);
        
        if (maxFreeChunkAv < freeChunk)
        {
            maxFreeChunkAv = freeChunk;
        }
    }
    
    if ( maxFreeChunkAv < maxChunkCountAvail() )
    {
        setMaxChunkCountAvail( maxFreeChunkAv );
    }
}

block_dict_entry_t PageBlockStorage::dictEntryAt(uint8_t pos) const
{
    block_dict_entry_t dict_entry;
    dict_entry.chunk_count = 0;
    dict_entry.chunk_nr = 0;
    
    if ( pos < blockCount() )
    {
        m_data->seek( OFFSET_CHUNK_DICT + ( pos * DICT_ENTRY_SIZE ) );
        
        dict_entry.chunk_nr = m_data->readUInt8();
        dict_entry.chunk_count = m_data->readUInt8();
    }

    return dict_entry;
}

uint16_t PageBlockStorage::dataPtrByChunkNum(uint8_t nr)
{
    return OFFSET_DATA + ( ( nr - 1) * BLOCK_SIZE);
}

void PageBlockStorage::writeBlockAt(uint8_t pos, const block_t& block)
{
    block_dict_entry_t dict_entry = dictEntryAt( pos );
    uint16_t dPtr = dataPtrByChunkNum( dict_entry.chunk_nr );

    writeBlockAtPtr(dPtr, block);
}

void PageBlockStorage::writeBlockAtPtr(uint16_t pos, const block_t& block)
{
    m_data->seek( pos );
    m_data->writeInt8( 0 );
    m_data->writeUInt32( block.block_header.next_page );
    m_data->writeUInt8( block.block_header.next_nr );
    m_data->writeUInt16( block.block_header.block_len );
    m_data->writeData( block.block_data, block.block_header.block_len );
    markDirty();
}

block_t PageBlockStorage::blockAt(uint8_t pos) const
{
    uint8_t nr = dictEntryAt( pos ).chunk_nr;
    if (nr > 0)
    {
        return blockAtNr( nr );
    }
    block_t block;
    block_init_empty(block);
    return block;
}

block_t PageBlockStorage::blockAtNr( uint8_t nr ) const
{
    uint16_t ptr =  dataPtrByChunkNum( nr );
    return blockAtPtr( ptr );
}

block_header_t PageBlockStorage::blockHeaderAtNr(uint8_t nr) const
{
    uint16_t ptr =  dataPtrByChunkNum( nr );
    m_data->seek( ptr + 1 );
    block_header_t bh;
    bh.next_page = m_data->readUInt32();
    bh.next_nr = m_data->readUInt8();
    bh.block_len = m_data->readUInt16();
    return bh;
}

block_t PageBlockStorage::blockAtPtr( uint16_t posPtr ) const
{
    m_data->seek( posPtr + 1 );
    block_t block;

    block.block_header.next_page = m_data->readUInt32();
    block.block_header.next_nr = m_data->readUInt8();
    block.block_header.block_len = m_data->readUInt16();
    block.block_data = m_data->readData( block.block_header.block_len );

    return block;
}

uint8_t PageBlockStorage::findDictEntryPosByNr(uint8_t nr, bool& found) const
{
    found = false;
    for (uint8_t i = 0; i < blockCount(); ++i)
    {
        if (dictEntryAt(i).chunk_nr == nr)
        {
            found = true;
            return i;
        }
    }
    
    return 0;
}

uint8_t PageBlockStorage::findDictSavePos(const block_dict_entry_t& dict_entry) const
{
    block_dict_entry_t de;
    for ( uint8_t i = 0; i < blockCount(); ++i )
    {
        de = dictEntryAt( i );
        if (de.chunk_nr > dict_entry.chunk_nr)
        {
            return i;
        }
        else if (de.chunk_nr == dict_entry.chunk_nr)
        {
            return 0;
        }
    }
    
    return blockCount();
}

uint8_t PageBlockStorage::saveDictEntry(const block_dict_entry_t& dict_entry)
{
    uint8_t savePos = findDictSavePos(dict_entry);
    writeDictEntrytAt( savePos, dict_entry );
    
    return savePos;
}

uint8_t PageBlockStorage::findFreeChunkNr(uint8_t chunkCount) const
{
    uint8_t nrCheck = 1;
    block_dict_entry_t dict_entry;
    
    for ( uint8_t i = 0; i < blockCount(); ++i )
    {
        dict_entry = dictEntryAt( i );
        uint8_t freeChunks = dict_entry.chunk_nr - nrCheck;
        
        if ( freeChunks > chunkCount )
        {
            return nrCheck;
        }
        else
        {
            nrCheck += dict_entry.chunk_count + freeChunks;
        }
    }
    
    if ( ( MAX_BLOCK_COUNT - nrCheck + 1 ) >= chunkCount )
    {
        return nrCheck;
    }
    else 
    {
        return 0;
    }
}

uint8_t PageBlockStorage::maxChunkCountAvail() const
{
    return m_data->readUInt8At(OFFSET_MAX_CHUNK_AVAIL);
}

void PageBlockStorage::setMaxChunkCountAvail(uint8_t count)
{
    m_data->writeUInt8At(OFFSET_MAX_CHUNK_AVAIL, count);
    markDirty();
}

uint8_t PageBlockStorage::chunksRequiredForSize(uint16_t size)
{
    size += BLOCK_HEADER_SIZE;
    uint8_t chunkNrInc = ((size % BLOCK_SIZE) == 0)?0:1;
    
    return (size / BLOCK_SIZE) + chunkNrInc;
}
