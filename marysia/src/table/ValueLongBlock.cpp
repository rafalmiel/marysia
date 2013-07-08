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

#include "ValueLongBlock.h"

#include <string.h>
#include "utils/ByteData.h"

ValueLongBlock::ValueLongBlock( long_data_t long_data ): Value( 10 ), m_raw_data( 0 )
{
    setLongData( long_data );
    
    setValueLen( 0 );
    
    m_is_long_data = true;
}

ValueLongBlock::ValueLongBlock() : Value( 10 ), m_raw_data( 0 )
{
    setValueLen( 0 );
    
    m_is_long_data = true;
    
    m_long_data.raw_data = NULL;
    m_long_data.total_len = 0;
}

ValueLongBlock::~ValueLongBlock()
{
    if ( m_raw_data ) delete [] m_raw_data;
    m_raw_data = 0;
    
    deleteValue();
    
   /* if ( m_long_data.raw_data )
    {
        delete [] m_long_data.raw_data;
    }*/
}

void ValueLongBlock::deleteValue()
{

    if ( m_value )
    {
        delete ( long_data_info_t* ) m_value;
    }

    if (m_long_data.raw_data) {
        delete [] m_long_data.raw_data;

        m_long_data.raw_data = 0;
        m_long_data.total_len = 0;
    }
}

long_data_info_t* ValueLongBlock::longDataInfo() const
{
    if ( m_value )
    {
        return ( long_data_info_t* ) m_value;
    }
    else
    {
        return NULL;
    }
}

void ValueLongBlock::setLongDataInfo(const long_data_info_t& long_data_info)
{
    deleteValue();
    
    m_value = new long_data_info_t;
    
    ( ( long_data_info_t* ) m_value )->page_id = long_data_info.page_id;
    ( ( long_data_info_t* ) m_value )->nr = long_data_info.nr;
    ( ( long_data_info_t* ) m_value )->total_len = long_data_info.total_len;
    
    if (long_data_info.total_len > 0)
    {
        setValueLen( 10 );
    }
    else 
    {
        setValueLen( 0 );
    }
    
}

long_data_t ValueLongBlock::longData() const
{
    return m_long_data;
}

void ValueLongBlock::setLongData(const long_data_t& long_data)
{
    m_long_data = long_data;
}

String ValueLongBlock::toString() const
{
    long_data_info_t* ldi = longDataInfo();
    return (String)"BLOB PTR. " + String::intToStr(ldi->page_id) + " " + ldi->nr + " " + ldi->total_len;
}

int8_t ValueLongBlock::compare(Value* value) const
{
    if ( isNull() && !value->isNull() ) return -1;
    else if ( !isNull() && value->isNull() ) return 1;
    else if ( isNull() && value->isNull() ) return 0;
    
    return 0;
}

uint16_t ValueLongBlock::storeSize() const
{
    return 10;
}

long_data_info_t ValueLongBlock::longDataInfoFromRawData ( const uint8_t* raw_data )
{
    long_data_info_t long_data;
    
    long_data.page_id = ByteData::readUInt32FromArray( raw_data );
    long_data.nr = raw_data[5];
    long_data.total_len = ByteData::readUInt32FromArray( raw_data + 6 );
    
    return long_data;
}

void ValueLongBlock::setRawData(const uint8_t* data, uint16_t len)
{
    long_data_info_t long_data;
    
    if ( len == storeSize() )
    {
        long_data = longDataInfoFromRawData( data );
    }
    else
    {
        long_data.page_id = 0;
        long_data.nr = 0;
        long_data.total_len = 0;
    }
    
    setLongDataInfo( long_data );
}

const uint8_t* ValueLongBlock::rawData()
{
    if ( m_raw_data ) delete [] m_raw_data;
    
    uint8_t* rdata = new uint8_t[ storeSize() ];
    
    ByteData::dataMemSet( rdata, 0, storeSize() );
    
    long_data_info_t* long_data = longDataInfo();
    
    if ( long_data )
    {
        ByteData::write4bytesToArray( rdata, long_data->page_id );
        ByteData::write2bytesToArray( rdata + 4, (uint16_t) long_data->nr );
        ByteData::write4bytesToArray( rdata + 6, long_data->total_len );
    }
    
    m_raw_data = rdata;
    
    return rdata;
}

