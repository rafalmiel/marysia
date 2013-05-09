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


#include "StringTokens.h"

#include <iostream>

using namespace std;

StringTokens::StringTokens(const String& str, const String& delimeter)
{
    m_size = 1;
    const char* search = strpbrk( str.c_str(), delimeter.c_str() );
    while ( search != NULL )
    {
        m_size++;
        search = strpbrk( search + 1, delimeter.c_str() );
    }
    m_string_array = new String [ m_size ];
    m_char_arr = new char*[ m_size ];

    uint16_t len = 0;

    for (int i = 0; i < m_size; ++i)
    {
        uint16_t len2 = strcspn( str.c_str() + len, delimeter.c_str() ) + len;
        uint16_t size = len2 - len + 1;
        char* strrob = new char[ size ];
        strncpy( strrob, str.c_str() + len, size - 1 );
        strrob[ size - 1 ] = '\0';
        
        m_string_array[i] = strrob ;
        m_char_arr[i] = strrob;

        len = len2 + 1;
    }
}

StringTokens::~StringTokens()
{
    for (int i = 0; i < m_size; ++i)
    {
        delete [] m_char_arr[i];
    }
    delete [] m_char_arr;
    delete [] m_string_array;
}

