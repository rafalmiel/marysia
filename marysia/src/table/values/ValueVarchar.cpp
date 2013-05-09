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


#include "ValueVarchar.h"

#include <string.h>
#include <iostream>

using namespace std;

ValueVarchar::ValueVarchar(uint16_t type_len) : Value(type_len)
{
    setType(VARCHAR_TYPE);
    setValueLen( 0 );
}

ValueVarchar::ValueVarchar(uint16_t type_len, const char* string): Value(type_len)
{
    setType(VARCHAR_TYPE);
    setStringValue( string );
}

ValueVarchar::ValueVarchar() : Value( 0 )
{
    setType(VARCHAR_TYPE);
    setValueLen( 0 );
}

ValueVarchar::ValueVarchar(const char* string): Value( 0 )
{
    setType(VARCHAR_TYPE);
    setStringValue( string );
}

ValueVarchar::~ValueVarchar()
{
    deleteValue();
}

const uint8_t* ValueVarchar::rawData()
{
    if (! isNull() )
    {
        return (uint8_t*) m_value;
    }
    else
    {
        return NULL;
    }
}

void ValueVarchar::deleteValue()
{
    if (m_value)
    {
        delete [] (char*) m_value;
        setValueLen(0);
    }
}

void ValueVarchar::setStringValue(const char* string)
{
    deleteValue();

    uint16_t len = strlen(string);
    if ( m_type_len != 0 )
        if (len > m_type_len) len = m_type_len;
    m_value = new char[len+1];
    
    memcpy(m_value, string, len);

    static_cast<char*>(m_value)[len] = '\0';
    
    setValueLen(len+1);
}

uint16_t ValueVarchar::storeSize() const
{
    return m_type_len+1;
}

const char* ValueVarchar::stringValue() const
{
    if ( ! isNull() )
    {
        return (const char*) m_value;
    }
    else
    {
        return "";
    }
}

void ValueVarchar::adjustValueLen()
{
    uint8_t* newval = new uint8_t[m_type_len + 1];
    memcpy(newval, m_value, m_type_len + 1);
    newval[m_type_len] = '\0';

    setStringValue( (const char*)newval );
    delete [] newval;
    
}

int8_t ValueVarchar::compare(Value* value) const
{
    if (isNull() && !value->isNull()) return -1;
    else if (!isNull() && value->isNull()) return 1;
    else if (isNull() && value->isNull()) return 0;

    return strcmp( stringValue(), static_cast< ValueVarchar* >( value )->stringValue() );
}
