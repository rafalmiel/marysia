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


#include "String.h"
#include <sstream>

std::ostream& operator << (std::ostream& os, const String& str)
{
    os.write(str.c_str(), str.size());
    return os;
}

String String::intToStr(int32_t n)
{
    std::ostringstream result;
    result << n;
    return result.str();
}

String String::doubleToStr(double n)
{
    std::ostringstream result;
    result << n;
    return result.str();
}

std::vector< String, std::allocator< String > > String::split(const String& delimeter) const
{
    std::size_t found = std::string::npos;
    std::vector<String> result;
    std::size_t lastPos = 0;
    do
    {
        found = m_string.find(delimeter,lastPos);
        result.push_back(m_string.substr(lastPos, found - lastPos));
        lastPos = found + 1;
        
    } while (found != std::string::npos );

    return result;
}
