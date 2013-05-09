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


#ifndef VALUE_H
#define VALUE_H

#include <stdint.h>

#include "ColumnTypeDef.h"

class String;

class Value
{
public:
    Value(uint16_t type_len);

    virtual ~Value();
    
    inline ColumnType type() const { return m_type; }
    inline uint16_t valueLen() const { return m_value_len; }

    virtual void setRawData(const uint8_t* data, uint16_t len);
    virtual const uint8_t* rawData() = 0;
    virtual uint16_t storeSize() const = 0;

    virtual int8_t compare(Value* value) const = 0;

    inline void setTypeLen(uint16_t len) { m_type_len = len; if (valueLen() > m_type_len) adjustValueLen(); }

    virtual String toString() const = 0;

    inline bool isNull() const { return valueLen() == 0; }

    Value* clone();
    
    bool isLongData() const { return m_is_long_data; }
protected:
    virtual void adjustValueLen() { setValueLen( m_type_len ); }
    inline void setType(ColumnType type) { m_type = type; }
    inline void setValueLen(uint16_t len) { m_value_len = len; }

    virtual void deleteValue() = 0;
    
    ColumnType m_type;
    void* m_value;
    uint16_t m_value_len;
    uint16_t m_type_len;
    
    bool m_is_long_data;
};

#endif // VALUE_H
