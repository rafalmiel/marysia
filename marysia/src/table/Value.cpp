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


#include "Value.h"

#include <string.h>

#include "table/ValueFactory.h"
#include "table/values/ValueVarchar.h"

Value::Value(uint16_t type_len): m_value(0), m_type_len( type_len ), m_is_long_data( false )
{
}

Value::~Value()
{
}

void Value::setRawData(const uint8_t* data, uint16_t len)
{
    deleteValue();

    if (len > 0)
    {
        m_value = new uint8_t[len];
        
        memcpy(m_value,data, len);
        
        setValueLen(len);
    }
    else
    {
        setValueLen( 0 );
    }
}

Value* Value::clone()
{
    return ValueFactory::createValue( m_type, m_type_len, rawData(), m_value_len );
}

