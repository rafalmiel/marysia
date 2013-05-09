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


#ifndef COLUMN_H
#define COLUMN_H

#include <stdint.h>

#include "utils/String.h"

class ByteData;
class Value;

#include "table/ColumnTypeDef.h"

class Column
{
public:
    
    Column(const String& column_name, ColumnType type, uint16_t type_len, bool nullable, bool ai, Value* def_value = NULL );
    Column(ByteData* data);

    virtual ~Column();

    inline const String& columnName() const { return m_column_name; }
    inline void setColumnName(const String& name) { m_column_name = name; }

    inline uint16_t columnNr() const { return m_column_num; }
    inline void setColumnNr(uint16_t num) { m_column_num = num; }

    inline ColumnType type() const { return m_type; }
    inline void setType(ColumnType type) { m_type = type; }

    inline uint16_t typeLen() const { return m_type_len; }
    inline void setTypeLen(uint16_t len) { m_type_len = len; }

    inline bool isNullable() const { return m_nullable; }
    inline void setIsNullable(bool is) { m_nullable = is; }

    inline bool isAutoincrement() const { return m_autoincrement; }
    inline void setIsAutoincrement(bool is) { m_autoincrement = is; }

    Value* defaultValue() const;
    void setDefaultValue(Value* defVal);

    inline uint16_t defaultValuePtr() const { return m_default_value_ptr; }
    inline void setDefaultValuePtr(uint16_t defVal) { m_default_value_ptr = defVal; }

    inline bool hasDefaultVal() const { return m_default_value != NULL; }

    uint8_t flags() const;
    void applyFlags(uint8_t flags);

    uint8_t byteSize() const;
    
    inline bool isLongColumn() const { return m_is_long_column; }

    virtual uint16_t valueStoreSize() const { return m_type_len; };

    friend class Row;

protected:
    void initDefaultValue();
    virtual Value* createEmptyDefaultVal() const = 0;
    
    String m_column_name;
    uint16_t m_column_num;
    
    ColumnType m_type;
    uint16_t m_type_len;
    
    bool m_nullable;
    bool m_autoincrement;
    
    bool m_is_long_column;
    
    Value* m_default_value;

    uint16_t m_default_value_ptr;
};

#endif // COLUMN_H
