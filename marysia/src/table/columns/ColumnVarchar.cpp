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


#include "ColumnVarchar.h"

#include "table/values/ValueVarchar.h"

ColumnVarchar::ColumnVarchar(ByteData* data) : Column( data )
{
    initDefaultValue();
}

ColumnVarchar::ColumnVarchar(const String& column_name, uint16_t type_len, bool nullable, const String& defVal):
    Column(column_name, VARCHAR_TYPE, type_len, nullable, false, new ValueVarchar( type_len, defVal.c_str() ) )
{
    initDefaultValue();
}

ColumnVarchar::ColumnVarchar(const String& column_name, uint16_t type_len, bool nullable):
    Column(column_name, VARCHAR_TYPE, type_len, nullable, false)
{
    initDefaultValue();
}

Value* ColumnVarchar::createEmptyDefaultVal() const
{
    if ( isNullable() )
    {
        return new ValueVarchar( m_type_len );
    }
    else
    {
        return new ValueVarchar( m_type_len, "" );
    }
}

uint16_t ColumnVarchar::valueStoreSize() const
{
    return m_type_len + 1;
}
