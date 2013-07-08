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


#include "File.h"

#include <aio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

using namespace std;

File::File(): m_is_open(false), m_file_handle(0)
{

}

void File::mopen(const String& filename)
{
    if ( !m_is_open )
    {
        if ( file_exists( filename ) )
        {
            m_file = open( filename.c_str(), O_RDWR );

            if ( m_file < 0 )
            {
                m_file = open( filename.c_str(), O_RDWR | O_CREAT | O_TRUNC,
                               S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
            }
        }
        else
        {
            m_file = open( filename.c_str(), O_RDWR | O_CREAT | O_TRUNC,
                           S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
        }

        if ( m_file < 0 )
        {
            exit( 1 );
        }

        m_is_open = true;

        m_filename = filename;
    }
}

bool File::file_exists(const String& filename)
{
    bool res = access( filename.c_str(), F_OK ) == 0;
    return res;
}

void File::allocateSpace(const uint32_t psize) const
{
    truncate(m_filename.c_str(),msize() + psize);
}

void File::mwrite(const uint32_t offset, uint8_t* data, uint16_t size) const
{
    lseek( m_file, offset, SEEK_SET );
    uint8_t* buf = ( uint8_t* ) data;

    int32_t ret;
    uint32_t len = size;

    while ( len )
    {
        ret = write( m_file, buf, len );

        if ( ret < 1 )
            fprintf( stderr, "write error code %d\n", errno );

        if ( ret < 1 )
            exit( 1 );

        buf += ret;
        len -= ret;
    }
}

void File::mread(const uint32_t offset, uint8_t* rawdata, uint16_t size) const
{
    lseek( m_file, offset, SEEK_SET );
    uint8_t* data = ( uint8_t* ) rawdata;
    int32_t ret;
    uint32_t got;

    got = 0;

    while ( size )
    {
        ret = read( m_file, data, size );

        if ( ret < 0 )
            exit( 1 );

        if ( ret == 0 )
            break;

        data += ret;

        size -= ret;

        got += ret;
    }
}

int File::mflush() const
{
    //return fflush(m_file_handle);
    return 0;
}

void File::mclose()
{
    close(m_file);

    m_is_open = false;
}

uint32_t File::msize() const
{
    struct stat filestatus;
    stat( m_filename.c_str(), &filestatus );
    return (uint32_t) filestatus.st_size;
}

File::~File()
{
    if ( m_is_open )
    {
        mclose();
    }
}

