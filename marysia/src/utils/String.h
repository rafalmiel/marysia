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


#ifndef STRING_H
#define STRING_H

#include <iostream>
#include <stdint.h>
#include <vector>

class String
{
public:
    String() { m_string = ""; }
    String(const std::string& str) { m_string = str; }
    String(const char* str) { m_string = str; }

    inline const char* c_str() const { return m_string.c_str(); }
    inline const std::string& std_str() const { return m_string; }
    inline size_t size() const { return m_string.size();}

    inline operator const std::string& () const { return m_string;}

    inline String& operator=(const std::string& str) { m_string = str; return *this; }
    inline String& operator=(const char* str) { m_string = str; return *this; }
    inline String operator+(const String& str) const { return m_string + str.m_string; }
    inline String operator+(int32_t num) const { return (m_string + String::intToStr(num).std_str()); }
    inline bool operator!=(const String& str) const { return m_string != str.m_string; }
    inline bool operator==(const std::string& str) const { return m_string == str; }

    std::vector<String> split(const String& delimeter) const;

    static String intToStr(int32_t);
    static String doubleToStr(double n);

private:
    std::string m_string;
};

std::ostream& operator << (std::ostream& os, const String& str);

#endif // STRING_H
