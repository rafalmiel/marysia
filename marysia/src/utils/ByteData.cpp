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


#include "ByteData.h"

#include <string.h>

#include "String.h"

ByteData::ByteData()
{

}

ByteData::ByteData(uint16_t size)
{
    init(size);
}

ByteData::ByteData(uint8_t* data, uint16_t size)
{
    init(data, size);
}

ByteData::~ByteData()
{
    delete [] m_data;
}

void ByteData::init ( uint16_t size )
{
    m_data = new uint8_t[size];
    memset(m_data,0,size);
    m_size = size;
    m_pos = 0;
}

void ByteData::init ( uint8_t* data, uint16_t size )
{
    m_data = data;
    m_size = size;
    m_pos = 0;
}

uint8_t ByteData::readUInt8At( uint16_t pos) const
{
    return m_data[pos];
}

int8_t ByteData::readInt8At(uint16_t pos) const
{
    return m_data[pos];
}

uint16_t ByteData::readUInt16At( uint16_t pos ) const
{
    return readUInt16FromArray( m_data + pos );
}

int16_t ByteData::readInt16At( uint16_t pos ) const
{
    return readInt16FromArray( m_data + pos );
}

uint32_t ByteData::readUInt32At( uint16_t pos ) const
{
    return readUInt32FromArray( m_data + pos );
}

int32_t ByteData::readInt32At( uint16_t pos ) const
{
    return readInt32FromArray( m_data + pos );
}

uint8_t ByteData::readUInt8()
{
    return readUInt8At(m_pos++);
}

int8_t ByteData::readInt8()
{
    return readInt8At(m_pos++);
}

uint16_t ByteData::readUInt16()
{
    uint16_t value = readUInt16At(m_pos);
    m_pos += 2;
    return value;
}

int16_t ByteData::readInt16()
{
    int16_t value = readInt16At(m_pos);
    m_pos += 2;
    return value;
}

uint32_t ByteData::readUInt32()
{
    uint32_t value = readUInt32At(m_pos);
    m_pos += 4;
    return value;
}

int32_t ByteData::readInt32()
{
    int32_t value = readInt32At(m_pos);
    m_pos += 4;
    return value;
}

void ByteData::seek ( uint16_t pos )
{
    m_pos = pos;
}

void ByteData::moveBy ( uint16_t pos )
{
    m_pos += pos;
}

void ByteData::writeUInt8At ( uint16_t pos, uint8_t value )
{
    m_data[pos] = value;
}

void ByteData::writeInt8At(uint16_t pos, int8_t value)
{
    m_data[pos] = value;
}

void ByteData::writeUInt16At ( uint16_t pos, uint16_t value )
{
    write2bytesToArray(m_data + pos, value);
}

void ByteData::writeInt16At ( uint16_t pos, int16_t value )
{
    write2bytesToArray(m_data + pos, value);
}

void ByteData::writeUInt32At ( uint16_t pos, uint32_t value )
{
    write4bytesToArray(m_data + pos, value);
}

void ByteData::writeInt32At ( uint16_t pos, int32_t value )
{
    write4bytesToArray(m_data + pos, value);
}

void ByteData::writeUInt8 ( uint8_t value )
{
    writeUInt8At(m_pos, value);
    m_pos++;
}

void ByteData::writeUInt16 ( uint16_t value )
{
    writeUInt16At(m_pos, value);
    m_pos += 2;
}

void ByteData::writeUInt32 ( uint32_t value )
{
    writeUInt32At(m_pos, value);
    m_pos += 4;
}

void ByteData::writeInt8 ( int8_t value )
{
    writeInt8At(m_pos, value);
    m_pos++;
}

void ByteData::writeInt16 ( int16_t value )
{
    writeInt16At(m_pos, value);
    m_pos += 2;
}

void ByteData::writeInt32 ( int32_t value )
{
    writeInt32At(m_pos, value);
    m_pos += 4;
}

const char* ByteData::readString(uint16_t len, uint16_t moveBy)
{
    const char* str = readStringAt(m_pos, len);
    m_pos += (moveBy == 0) ? len : moveBy;
    return str;
}

const char* ByteData::readStringAt(uint16_t pos, uint16_t len) const
{
    const char* str = (const char*) m_data + pos;
    return str;
}

uint8_t* ByteData::readData(uint16_t len, uint16_t moveBy)
{
    uint8_t* data = readDataAt(m_pos, len);
    m_pos += (moveBy == 0) ? len : moveBy;
    return data;
}

uint8_t* ByteData::readDataAt(uint16_t pos, uint16_t len)
{
    uint8_t* data = new uint8_t[len];
    memcpy(data, m_data + pos, len);
    return data;
}

void ByteData::writeString(const char* str, uint16_t fillLen)
{
    writeStringAt(m_pos, str, fillLen);
    m_pos += (fillLen == 0) ? strlen(str)+1 : fillLen;
}

void ByteData::writeStringAt( uint16_t pos, const char* str, uint16_t fillLen)
{
    uint16_t len = strlen(str);
    
    if (fillLen != 0)
    {
        dataSetAt(pos, 0, fillLen);
    }
    
    memcpy(m_data + pos, str, len);
    m_data[pos + len] = '\0';
}

void ByteData::writeData(const uint8_t* data, uint16_t len, uint16_t fillLen )
{
    writeDataAt(m_pos, data, len, fillLen);
    m_pos += (fillLen == 0) ? len : fillLen;
}

void ByteData::writeDataAt(uint16_t pos, const uint8_t* data, uint16_t len, uint16_t fillLen )
{
    memcpy(m_data + pos, data, len);
}

void ByteData::shrink(uint16_t len)
{
    uint8_t* newdata = new uint8_t[len];

    memcpy(newdata,m_data,len);

    delete[] m_data;

    m_data = newdata;
}

void ByteData::shiftLeft(uint16_t size, uint16_t count)
{
    shiftLeftAt(m_pos, size, count);
}

void ByteData::shiftRight(uint16_t size, uint16_t count)
{
    shiftRightAt(m_pos, size, count);
}

void ByteData::shiftLeftAt(uint16_t pos, uint16_t size, uint16_t count)
{
    memmove(m_data + pos - count, m_data + pos, size );
    dataSetAt(pos-count + size, 0, count);
}

void ByteData::shiftRightAt(uint16_t pos, uint16_t size, uint16_t count)
{
    memmove(m_data + pos + count, m_data + pos, size );
    dataSetAt(pos, 0, count);
}

void ByteData::dataSet(uint8_t value, uint16_t count)
{
    dataSetAt( m_pos, value, count);
    m_pos += count;
}

void ByteData::dataSetAt(uint16_t pos, uint8_t value, uint16_t count)
{
    dataMemSet( m_data + pos, value, count );
}

void ByteData::write2bytesToArray(uint8_t* table, uint16_t value)
{
    table[0] = ( uint8_t ) ( value >> 8 );
    table[1] = ( uint8_t ) ( value );
}

void ByteData::write4bytesToArray(uint8_t* table, uint32_t value)
{
    table[0] = ( uint8_t ) ( value >> 24 );
    table[1] = ( uint8_t ) ( value >> 16 );
    table[2] = ( uint8_t ) ( value >> 8 );
    table[3] = ( uint8_t ) ( value );
}

void ByteData::write2bytesToArray(uint8_t* table, int16_t value)
{
    table[0] = ( uint8_t ) ( value >> 8 );
    table[1] = ( uint8_t ) ( value );
}

void ByteData::write4bytesToArray(uint8_t* table, int32_t value)
{
    table[0] = ( uint8_t ) ( value >> 24 );
    table[1] = ( uint8_t ) ( value >> 16 );
    table[2] = ( uint8_t ) ( value >> 8 );
    table[3] = ( uint8_t ) ( value );
}

int32_t ByteData::readInt32FromArray(const uint8_t* table)
{
    return ( ( ( int32_t ) ( table[0] ) << 24 )
    | ( ( int32_t ) ( table[1] ) << 16 )
    | ( ( int32_t ) ( table[2] ) << 8 )
    | ( int32_t ) ( table[3] ) );
}

uint32_t ByteData::readUInt32FromArray(const uint8_t* table)
{
    return ( ( ( uint32_t ) ( table[0] ) << 24 )
    | ( ( uint32_t ) ( table[1] ) << 16 )
    | ( ( uint32_t ) ( table[2] ) << 8 )
    | ( uint32_t ) ( table[3] ) );
}

int16_t ByteData::readInt16FromArray(const uint8_t* table)
{
    return ( ( ( int16_t ) ( table[0] ) << 8 ) 
    | ( int16_t ) ( table[1] ) );
}

uint16_t ByteData::readUInt16FromArray(const uint8_t* table)
{
    return ( ( ( uint16_t ) ( table[0] ) << 8 ) 
    | ( uint16_t ) ( table[1] ) );
}

void ByteData::dataMemSet(uint8_t* table, uint8_t value, uint16_t count)
{
    memset( table, value, count );
}
