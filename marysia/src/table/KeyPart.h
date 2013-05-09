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


#ifndef KEYPART_H
#define KEYPART_H

#include "Column.h"

class KeyPart
{
public:
    KeyPart();
    KeyPart(Column* column);
    virtual ~KeyPart();

    const String& columnName() const { return m_column->columnName(); }
    void setColumn(Column* column) { m_column = column; }

    Column* column() const { return m_column; }

    bool isAutoincrement() const { return m_column->isAutoincrement(); }

    inline operator const String& () const {
        return m_column->columnName();
    }

private:
    Column* m_column;
};

#endif // KEYPART_H
