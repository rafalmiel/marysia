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


#include "ValueDatetime.h"

#include <ctime>
#include <iostream>

#include "utils/DateTime.h"
#include "utils/String.h"

using namespace std;

ValueDatetime::ValueDatetime(): Value(4), m_raw_data(0)
{
    setType(DATETIME_TYPE);
    setValueLen(0);
}

ValueDatetime::ValueDatetime(const String& datetime): Value(4),m_raw_data(0)
{
    setType(DATETIME_TYPE);
    setDatetime( datetime );
}

ValueDatetime::ValueDatetime(int datetime): Value(4),m_raw_data(0)
{
    setType(DATETIME_TYPE);
    setUnixTime(datetime);
}

ValueDatetime::~ValueDatetime()
{
    deleteValue();
}

void ValueDatetime::setDatetime(const String& datetime)
{
    setUnixTime( DateTime::stringToDatetime( datetime ) );
}

String ValueDatetime::datetime() const
{
    if ( ! isNull() )
    {
        return DateTime::datetimeToString( unixTime() );
    }
    else
    {
        return "1970-01-01 00:00:00";
    }
}

void ValueDatetime::deleteValue()
{
    if (m_raw_data) delete [] m_raw_data;
    if (m_value)
    {
        delete (int32_t*) m_value;
        setValueLen(0);
    }
}

int32_t ValueDatetime::unixTime() const
{
    if ( ! isNull() )
    {
        return *((int32_t*) m_value);
    }
    else
    {
        return 0;
    }
}

void ValueDatetime::setUnixTime(int32_t unixTime)
{
    deleteValue();
    
    m_value = new int32_t( unixTime );
    
    setValueLen(4);
}

int8_t ValueDatetime::compare(Value* value) const
{
    if (isNull() && !value->isNull()) return -1;
    else if (!isNull() && value->isNull()) return 1;
    else if (isNull() && value->isNull()) return 0;
    
    int32_t val1 = unixTime();
    int32_t val2 = static_cast< ValueDatetime* >( value )->unixTime();
    
    if (val1 > val2) return 1;
    else if (val1 < val2) return -1;
    else return 0;
}

uint16_t ValueDatetime::storeSize() const
{
    return m_type_len;
}

const uint8_t* ValueDatetime::rawData()
{
    if ( isNull() ) return NULL;
    
    if (m_raw_data) delete [] m_raw_data;
    
    m_raw_data = new uint8_t[4];
    
    int32_t val = unixTime();
    
    m_raw_data[0] = (uint8_t) (val >> 24 );
    m_raw_data[1] = (uint8_t) (val >> 16 );
    m_raw_data[2] = (uint8_t) (val >> 8 );
    m_raw_data[3] = (uint8_t) (val );
    
    return m_raw_data;
}

void ValueDatetime::setRawData(const uint8_t* data, uint16_t len)
{
    deleteValue();
    
    if ( len == 4 )
    {
        int32_t val = ( ( ( int32_t ) ( data[0] ) << 24 )
        | ( ( int32_t ) ( data[0 + 1] ) << 16 )
        | ( ( int32_t ) ( data[0 + 2] ) << 8 )
        | ( int32_t ) ( data[0 + 3] ) );
        
        setUnixTime(val);
    }
    else
    {
        setValueLen( 0 );
    }
}

String ValueDatetime::toString() const
{
    return datetime();
}

