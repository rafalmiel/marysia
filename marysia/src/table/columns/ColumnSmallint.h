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


#ifndef COLUMNSMALLINT_H
#define COLUMNSMALLINT_H

#include "table/Column.h"

class ColumnSmallint : public Column
{
public:
    ColumnSmallint(ByteData* data);
    ColumnSmallint(const String& column_name, bool nullable, bool ai, int16_t defVal);
    ColumnSmallint(const String& column_name, bool nullable, bool ai);

protected:
    virtual Value* createEmptyDefaultVal() const;
};

#endif // COLUMNSMALLINT_H
