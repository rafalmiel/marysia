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


#ifndef PAGETABLEINFO_H
#define PAGETABLEINFO_H

#include <map>
#include <string>
#include <vector>

#include "page/Page.h"

class Column;
class KeyInfo;

class PageTableInfo : public Page
{
public:
    
    PageTableInfo(ByteData* data, bool init = true);
    virtual ~PageTableInfo();

    void debugIndex();

    enum TableInfoOffset {
        TABLE_NAME_OFFSET = PAGE_HEADER_SIZE,
        INFO_COLUMN_COUNT_OFFSET = TABLE_NAME_OFFSET + 72,
        INFO_KEY_COUNT_OFFSET = INFO_COLUMN_COUNT_OFFSET + 4,
        DEFAULT_VALUES_OFFSET = INFO_KEY_COUNT_OFFSET + 4,
    };

    void setTableName(const String& tablename);
    const String& tablename() const;

    uint16_t columnCount() const;

    void addColumn(Column* column);
    void addKey(KeyInfo* key);

    Column* columnByNum(int num) const {return m_columns[num];}
    Column* columnByName(const String& name) {return m_columns_map[name.c_str()];}
    
    uint32_t longColumnsCount() const { return m_long_columns.size(); }
    Column* longColumn(uint32_t nr) const { return m_long_columns[nr]; }

    KeyInfo* keyByNum(int num) const {return m_keys[num];}
    KeyInfo* keyByName(const String& name) {return m_keys_map[name.c_str()];}

    KeyInfo* primaryKey() const { return m_primary_key; }

    bool keyExists( const String& keyName) { return m_keys_map.find( keyName.c_str() ) != m_keys_map.end(); }

    virtual ByteData* data();

private:
    uint16_t countSize() const;
    String m_tablename;

    typedef std::vector<Column*> columns_vector_t;
    typedef std::vector<KeyInfo*> key_vector_t;
    typedef std::map<std::string, Column*> columns_map_t;
    typedef std::map<std::string, KeyInfo*> keys_map_t;
    typedef std::vector<Column*> long_columns_vector_t;
    
    columns_vector_t m_columns;
    long_columns_vector_t m_long_columns;
    columns_map_t m_columns_map;
    
    key_vector_t m_keys;
    keys_map_t m_keys_map;
    
    KeyInfo* m_primary_key;

    bool m_was_structure_changed;
};

#endif // PAGETABLEINFO_H
