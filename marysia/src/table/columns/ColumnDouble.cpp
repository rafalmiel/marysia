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


#include "ColumnDouble.h"

#include "table/values/ValueDouble.h"

ColumnDouble::ColumnDouble(ByteData* data): Column(data)
{
    initDefaultValue();
}

ColumnDouble::ColumnDouble(const String& column_name, bool nullable, double defVal):
    Column( column_name, DOUBLE_TYPE, 8, nullable, false, new ValueDouble( defVal ) )
{
    initDefaultValue();
}

ColumnDouble::ColumnDouble(const String& column_name, bool nullable):
    Column( column_name, DOUBLE_TYPE, 8, nullable, false )
{
    initDefaultValue();
}


Value* ColumnDouble::createEmptyDefaultVal() const
{
    return new ValueDouble( 0 );
}



