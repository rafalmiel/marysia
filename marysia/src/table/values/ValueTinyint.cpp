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


#include "ValueTinyint.h"

#include "utils/String.h"

ValueTinyint::ValueTinyint() : ValueIncrementable( 1 ), m_raw_data( 0 )
{
    setType( TINYINT_TYPE );
    setValueLen( 0 );
}

ValueTinyint::ValueTinyint(int8_t value): ValueIncrementable( 1 ), m_raw_data( 0 )
{
    setType( TINYINT_TYPE );
    setInt8Value(value);
}

ValueTinyint::~ValueTinyint()
{
    deleteValue();
}

int8_t ValueTinyint::int8Value() const
{
    if ( ! isNull() )
    {
        return *((int8_t*) m_value);
    }
    else
    {
        return 0;
    }
}

void ValueTinyint::setInt8Value(int8_t value)
{
    deleteValue();
    
    m_value = new int8_t( value );
    
    setValueLen(1);
}

int32_t ValueTinyint::autoincValue() const
{
    if ( ! isNull() )
    {
        return int8Value();
    }
    else
    {
        return 0;
    }
}

void ValueTinyint::setAutoincValue(int32_t value)
{
    setInt8Value( value );
}

bool ValueTinyint::isAutoincReplace() const
{
    return (isNull()) || (int8Value() == 0);
}

void ValueTinyint::deleteValue()
{
    if (m_raw_data) delete [] m_raw_data;
    
    if (m_value)
    {
        delete (int8_t*) m_value;
        setValueLen(0);
    }
}

String ValueTinyint::toString() const
{
    if ( ! isNull() )
    {
        return String::intToStr( int8Value() );
    }
    else
    {
        return "__null__";
    }
}

int8_t ValueTinyint::compare(Value* value) const
{
    if (isNull() && !value->isNull()) return -1;
    else if (!isNull() && value->isNull()) return 1;
    else if (isNull() && value->isNull()) return 0;
    
    int32_t val1 = int8Value();
    int32_t val2 = static_cast< ValueTinyint* >( value )->int8Value();
    
    if (val1 > val2) return 1;
    else if (val1 < val2) return -1;
    else return 0;
}

uint16_t ValueTinyint::storeSize() const
{
    return m_type_len;
}

void ValueTinyint::setRawData(const uint8_t* data, uint16_t len)
{
    deleteValue();
    
    if ( len == 1 )
    {
        int8_t val = ( ( ( int32_t ) ( data[0] ) ) );
        
        setInt8Value(val);
    }
    else
    {
        setValueLen( 0 );
    }
}


const uint8_t* ValueTinyint::rawData()
{
    if ( isNull() ) return NULL;
    
    if (m_raw_data) delete [] m_raw_data;
    
    m_raw_data = new uint8_t[1];
    
    int8_t val = int8Value();
    
    m_raw_data[0] = (uint8_t) (val);

    
    return m_raw_data;
}

