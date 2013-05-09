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


#include "ValueSmallint.h"

#include "utils/String.h"

ValueSmallint::ValueSmallint() : ValueIncrementable( 2 ), m_raw_data(0)
{
    setType(SMALLINT_TYPE);
    setValueLen( 0 );
}

ValueSmallint::ValueSmallint(int16_t value): ValueIncrementable( 2 ), m_raw_data(0)
{
    setType( SMALLINT_TYPE );
    setInt16Value( value );
}

ValueSmallint::~ValueSmallint()
{
    deleteValue();
}

int16_t ValueSmallint::int16Value() const
{
    if ( ! isNull() )
    {
        return *((int16_t*) m_value);
    }
    else
    {
        return 0;
    }
}

void ValueSmallint::setInt16Value(int16_t value)
{
    deleteValue();
    
    m_value = new int16_t( value );
    
    setValueLen(2);
}


int32_t ValueSmallint::autoincValue() const
{
    if ( ! isNull() )
    {
        return int16Value();
    }
    else
    {
        return 0;
    }
}

void ValueSmallint::setAutoincValue(int32_t value)
{
    setInt16Value( value );
}

bool ValueSmallint::isAutoincReplace() const
{
    return (isNull()) || (int16Value() == 0);
}

void ValueSmallint::deleteValue()
{
    if (m_raw_data) delete [] m_raw_data;
    
    if (m_value)
    {
        delete (int16_t*) m_value;
        setValueLen(0);
    }
}

String ValueSmallint::toString() const
{
    if ( ! isNull() )
    {
        return String::intToStr( int16Value() );
    }
    else
    {
        return "__null__";
    }
}

int8_t ValueSmallint::compare(Value* value) const
{
    if (isNull() && !value->isNull()) return -1;
    else if (!isNull() && value->isNull()) return 1;
    else if (isNull() && value->isNull()) return 0;
    
    int16_t val1 = int16Value();
    int16_t val2 = static_cast< ValueSmallint* >( value )->int16Value();
    
    if (val1 > val2) return 1;
    else if (val1 < val2) return -1;
    else return 0;
}

uint16_t ValueSmallint::storeSize() const
{
    return m_type_len;
}

const uint8_t* ValueSmallint::rawData()
{
    if ( isNull() ) return NULL;
    
    if (m_raw_data) delete [] m_raw_data;
    
    m_raw_data = new uint8_t[2];
    
    int32_t val = int16Value();;
    
    m_raw_data[0] = (uint8_t) (val >> 8 );
    m_raw_data[1] = (uint8_t) (val );

    
    return m_raw_data;
}

void ValueSmallint::setRawData(const uint8_t* data, uint16_t len)
{
    deleteValue();
    
    if ( len == 2 )
    {
        int16_t val = ( ( ( int16_t ) ( data[0] ) << 8 )
        | ( ( int16_t ) ( data[0 + 1] ) ) );
        
        setInt16Value(val);
    }
    else
    {
        setValueLen( 0 );
    }
}

