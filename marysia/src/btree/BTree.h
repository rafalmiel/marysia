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

#ifndef BTREE_H
#define BTREE_H

#include "page/Page.h"

class BlockStorageManager;
class Cache;
class KeyInfo;
class KeyValue;
class Page;
class PageIndex;
class PageIndexIntern;
class PageIndexLeaf;
class PageTableInfo;
class Row;
class String;
class Tablespace;

class BTree
{
public:
    BTree(Cache* buf);

    void openTable(const String& data_filename, PageTableInfo* page_table_info, Tablespace* tablespace, BlockStorageManager* block_storage_manager );
    const String& filename() const { return m_data_filename; }

    bool insertRow( Row* row );
    void deleteRow( KeyValue* key_value );

    void readInit(const String& key_name);
    Row* read(const String& key_name, KeyValue* key_value);
    Row* readNext();
    void readClose();
    void checkLeafCorrectness() const;
    bool lastDeleteResult() const
    {
        return m_lastDeleteRes;
    }

private:
    void merge(PageIndex* leftNode, PageIndex* rightNode, PageIndexIntern* anchorNode, KeyInfo* key_info, bool atEnd);
    PageIndex* rootNode(const String& key_name) const;

    KeyValue* insertRebalance(page_id_t thisNodeId, Row* row, page_id_t& resPage, bool& insertResult);
    bool removeRebalance(page_id_t thisNodeId, page_id_t parentNodeId, page_id_t lAnchor, page_id_t rAnchor, KeyValue* key);
    
    void createNewRoot(PageIndex* indexIntern1, PageIndex* indexIntern2, KeyValue* key);
        
    Cache* m_buffer;
    BlockStorageManager* m_block_storage_manager;
    Tablespace* m_tablespace;
    String m_data_filename;
    PageTableInfo* m_page_tableinfo;

    PageIndexLeaf* m_read_leaf_page;
    uint16_t m_read_pos;

    bool m_lastDeleteRes;

    //void insertRowToLeaf(page_id_t leaf_page_id, Row* row);
};

#endif // BTREE_H
