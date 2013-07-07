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


#ifndef ROW_H
#define ROW_H

#include <vector>
#include <stdint.h>

class BlockStorageManager;
class KeyValue;
class PageTableInfo;
class String;
class Value;
class ValueLongBlock;

#include "table/BlockDefs.h"

class Row
{
public:
    Row( );
    Row( PageTableInfo* tableinfo );
    ~Row();

    void setPageTableInfo( PageTableInfo* page_tableinfo );

    Row* addValue( Value* val );
    inline uint16_t valuesCount() const { return m_values.size(); }
    Value* valueAt( uint16_t pos) const;
    void setValueAt( uint16_t pos, Value* value );

    uint16_t byteSize() const;

    KeyValue* createKeyValueByName( const String& keyName ) const;
    PageTableInfo* tableinfo() const { return m_tableinfo; }

    void setAutoincVal(int32_t value);
    int32_t autoincVal() const;
    
    void setBlockStorageManager( BlockStorageManager* blockStorageManager ) { m_block_storage_manager = blockStorageManager; }
    
    void saveLongValues();

    Row& operator<<( Value* value ) { addValue( value ); return *this; }
private:
    PageTableInfo* m_tableinfo;
    
    BlockStorageManager* m_block_storage_manager;

    uint16_t m_row_byteSize;

    typedef std::vector<Value*> values_vector_t;
    typedef std::vector<ValueLongBlock*> long_values_vector_t;

    values_vector_t m_values;
    long_values_vector_t m_long_values;
};

std::ostream& operator << (std::ostream& os, const Row& row);

#endif // ROW_H
