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

#ifndef BLOCKSTORAGEMANAGER_H
#define BLOCKSTORAGEMANAGER_H

#include "page/data/PageSystemSegment.h"
#include "page/Page.h"
#include "table/BlockDefs.h"

class Cache;
class BlockData;
class PageBlockStorage;
class PageSystemSegment;
class PageTableInfo;
class Tablespace;

class BlockStorageManager {
public:
    BlockStorageManager(Cache* buffer);
    
    void openTable(const String& data_filename, PageTableInfo* page_table_info, Tablespace* tablespace);
    
    block_ptr_t saveData(uint8_t* data, uint32_t len);
    bool deleteData( const block_ptr_t& block_ptr );
    BlockData* readData( const block_ptr_t& block_ptr ) const;
    
private:
    void readDataRecursive( const block_ptr_t& block_ptr, BlockData* blockData) const;
    block_ptr_t saveBlock(const block_t& block, key_entry_info_st* key_entry);
    
    Tablespace* m_tablespace;
    PageTableInfo* m_page_tableinfo;
    Cache* m_buffer;
    String m_data_filename;
};

#endif // BLOCKSTORAGEMANAGER_H
