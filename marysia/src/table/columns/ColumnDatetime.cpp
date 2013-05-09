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


#include "ColumnDatetime.h"

#include "table/values/ValueDatetime.h"

ColumnDatetime::ColumnDatetime(ByteData* data): Column(data)
{
    initDefaultValue();
}

ColumnDatetime::ColumnDatetime(const String& column_name, bool nullable, const String& defVal):
    Column(column_name, DATETIME_TYPE, 4, nullable, false, new ValueDatetime(defVal) )
{
    initDefaultValue();
}

ColumnDatetime::ColumnDatetime(const String& column_name, bool nullable, int defVal):
    Column(column_name, DATETIME_TYPE, 4, nullable, false, new ValueDatetime(defVal) )
{
    initDefaultValue();
}

ColumnDatetime::ColumnDatetime(const String& column_name, bool nullable):
    Column(column_name, DATETIME_TYPE, 4, nullable, false, new ValueDatetime() )
{
    initDefaultValue();
}

Value* ColumnDatetime::createEmptyDefaultVal() const
{
    if (isNullable())
    {
        return new ValueDatetime();
    }
    else
    {
        return new ValueDatetime("1970-01-01 00:00:00");
    }
}

