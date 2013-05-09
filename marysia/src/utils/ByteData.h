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


#ifndef BYTEDATA_H
#define BYTEDATA_H

#include <stdint.h>

class ByteData
{
public:
    ByteData();
    ByteData( uint16_t size );
    ByteData( uint8_t* data, uint16_t size );

    virtual ~ByteData();
    
    void init ( uint16_t size );
    void init ( uint8_t* data, uint16_t size );

    uint8_t readUInt8();
    uint16_t readUInt16();
    uint32_t readUInt32();

    int8_t readInt8();
    int16_t readInt16();
    int32_t readInt32();

    uint8_t readUInt8At( uint16_t pos ) const;
    uint16_t readUInt16At( uint16_t pos ) const;
    uint32_t readUInt32At( uint16_t pos ) const;

    int8_t readInt8At( uint16_t pos ) const;
    int16_t readInt16At( uint16_t pos ) const;
    int32_t readInt32At( uint16_t pos ) const;

    void writeUInt8 ( uint8_t value );
    void writeUInt16 ( uint16_t value );
    void writeUInt32 ( uint32_t value );

    void writeUInt8At( uint16_t pos, uint8_t value );
    void writeUInt16At( uint16_t pos, uint16_t value );
    void writeUInt32At( uint16_t pos, uint32_t value );

    void writeInt8 ( int8_t value );
    void writeInt16 ( int16_t value );
    void writeInt32 ( int32_t value );
    
    void writeInt8At( uint16_t pos, int8_t value );
    void writeInt16At( uint16_t pos, int16_t value );
    void writeInt32At( uint16_t pos, int32_t value );

    const char* readString( uint16_t len, uint16_t moveBy = 0 );
    const char* readStringAt( uint16_t pos, uint16_t len ) const;
    
    uint8_t* readData(uint16_t len, uint16_t moveBy = 0 );
    uint8_t* readDataAt(uint16_t pos, uint16_t len);

    void writeString(const char* str, uint16_t fillLen = 0);
    void writeStringAt(uint16_t pos, const char* str, uint16_t fillLen = 0);

    void writeData(const uint8_t* data, uint16_t len, uint16_t fillLen = 0);
    void writeDataAt( uint16_t pos, const uint8_t* data, uint16_t len, uint16_t fillLen = 0);

    void seek ( uint16_t pos );
    void moveBy( uint16_t pos );

    void shiftRight(uint16_t size, uint16_t count);
    void shiftLeft(uint16_t size, uint16_t count);

    void shiftRightAt(uint16_t pos, uint16_t size, uint16_t count);
    void shiftLeftAt(uint16_t pos, uint16_t size, uint16_t count);

    void dataSetAt( uint16_t pos, uint8_t value, uint16_t count );
    void dataSet( uint8_t value, uint16_t count );

    void shrink(uint16_t len);

    inline bool atEnd() const { return m_pos >= m_size; }
    inline uint16_t pos() const { return m_pos; }
    inline uint16_t size() const { return m_size; }

    inline uint8_t* dataPtr(bool currentPtr = false) const
    {
        if (currentPtr)
        {
            return m_data + m_pos;
        } else
        {
            return m_data;
        }
    }
    
    static void write4bytesToArray(uint8_t* table, uint32_t value);
    static void write2bytesToArray(uint8_t* table, uint16_t value);
    static void write4bytesToArray(uint8_t* table, int32_t value);
    static void write2bytesToArray(uint8_t* table, int16_t value);
    
    static uint32_t readUInt32FromArray(const uint8_t* table);
    static int32_t readInt32FromArray(const uint8_t* table);
    static uint16_t readUInt16FromArray(const uint8_t* table);
    static int16_t readInt16FromArray(const uint8_t* table);
    
    static void dataMemSet( uint8_t* table, uint8_t value, uint16_t count );

private:
    uint8_t* m_data;
    uint16_t m_size;
    uint16_t m_pos;
};

#endif // BYTEDATA_H
