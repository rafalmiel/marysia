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


#ifndef KEYVALUE_H
#define KEYVALUE_H

#include <iostream>
#include <stdint.h>
#include <vector>

class KeyInfo;
class Value;

class KeyValue
{
public:
    KeyValue();
    KeyValue(KeyInfo* keyInfo);

    void setKeyInfo( KeyInfo* key_info );

    void setValue( uint8_t pos, Value* value) { m_values[ pos ] = value; }
    Value* valueAt( uint8_t pos ) const { return m_values[ pos ]; }
    KeyInfo* keyInfo() const { return m_key_info; }

    bool isInfimum() const { return m_is_infimum; }
    void setIsInfimum( bool is ) { m_is_infimum = is; }

    void setOwnsValues( bool own ) { m_owns_values = own; }

    int8_t compare( KeyValue* key_value ) const;

    inline uint16_t byteSize() const { return m_key_byteSize; }

    KeyValue* addValue( Value* value );

    uint16_t valuesCount() const { return m_values.size(); }

    KeyValue& operator<<( Value* value ) { addValue( value ); return *this; };

    ~KeyValue();

private:    
    typedef std::vector<Value*> values_vector_t;
    values_vector_t m_values;

    uint16_t m_key_byteSize;
    
    KeyInfo* m_key_info;

    bool m_owns_values;
    bool m_is_infimum;
};

std::ostream& operator << (std::ostream& os, const KeyValue& row);

#endif // KEYVALUE_H
