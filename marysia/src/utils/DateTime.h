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


#ifndef DATETIME_H
#define DATETIME_H

#include "time.h"

class String;

class DateTime
{
public:
    static time_t stringToDate(const String& string);
    static String dateToString( time_t time );

    static time_t stringToDatetime(const String& string);
    static String datetimeToString(time_t time);

private:
    static String prependZeroToSize(String str, size_t size);
};

#endif // DATETIME_H
