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


#ifndef STRINGTOKENS_H
#define STRINGTOKENS_H

#include <stdint.h>
#include <string.h>

#include "utils/String.h"

/**
 * przestarzałe, używać String::split
 */
class StringTokens
{
public:
    StringTokens(const String& str, const String& delimeter);
    virtual ~StringTokens();
    
    uint16_t size() const { return m_size; }

    String& operator[](uint16_t num) const { return m_string_array[num]; }
private:
    uint16_t m_size;
    String* m_string_array;
    char** m_char_arr;
};

#endif // STRINGTOKENS_H
