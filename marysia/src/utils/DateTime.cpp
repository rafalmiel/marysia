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


#include "DateTime.h"

#include <stdlib.h>
#include <time.h>

#include "String.h"

time_t DateTime::stringToDate(const String& string)
{
    std::vector<String> spl = string.split("-");
    time_t rawtime;
    time(&rawtime);
    struct tm *str = localtime( &rawtime );
    str->tm_sec = 0;
    str->tm_min = 0;
    str->tm_hour = 0;
    str->tm_year = atoi(spl[0].c_str()) - 1900;
    str->tm_mon = atoi(spl[1].c_str()) - 1;
    str->tm_mday = atoi(spl[2].c_str());

    return mktime(str);
}

String DateTime::datetimeToString(time_t time)
{
    struct tm *a = localtime(&time);
    
    String res =
        String::intToStr(a->tm_year+1900) +
        "-" +
        prependZeroToSize(String::intToStr(a->tm_mon+1),2) +
        "-" +
        prependZeroToSize(String::intToStr(a->tm_mday),2) +
        " " +
        prependZeroToSize(String::intToStr(a->tm_hour),2) +
        ":" +
        prependZeroToSize(String::intToStr(a->tm_min),2) +
        ":" +
        prependZeroToSize(String::intToStr(a->tm_sec),2);
    
    return res;
}

String DateTime::dateToString(time_t time)
{
    struct tm *a = localtime(&time);
    
    String res = String::intToStr(a->tm_year+1900) +
                 "-" +
                 prependZeroToSize(String::intToStr(a->tm_mon+1),2) +
                 "-" +
                 prependZeroToSize(String::intToStr(a->tm_mday),2);
    
    return res;
}

time_t DateTime::stringToDatetime(const String& string)
{
    std::vector<String> parts = string.split(" ");
    std::vector<String> date = parts[0].split("-");
    std::vector<String> timev = parts[1].split(":");
    time_t rawtime;
    time(&rawtime);
    struct tm *str = localtime( &rawtime );
    str->tm_hour = atoi(timev[0].c_str());
    str->tm_min = atoi(timev[1].c_str());
    if (timev.size() > 2)
        str->tm_sec = atoi(timev[2].c_str());
    else
        str->tm_sec = 0;
    
    str->tm_year = atoi(date[0].c_str()) - 1900;
    str->tm_mon = atoi(date[1].c_str()) - 1;
    str->tm_mday = atoi(date[2].c_str());
    
    return mktime(str);
}


String DateTime::prependZeroToSize(String str, size_t size)
{
    uint16_t len = str.size();
    while (len < size) {
        str = ((String)"0" + str);
        len++;
    }
    return str;
}

