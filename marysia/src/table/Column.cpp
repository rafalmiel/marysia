/*
 *  Marysia database server
 *  Copyright (C) 2011  Rafal Mielniczuk rafalm3@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "Column.h"

#include <iostream>
#include <string.h>

#include "utils/ByteData.h"
#include "table/Value.h"

using namespace std;

Column::Column(const String& column_name, ColumnType type, uint16_t type_len, bool nullable, bool ai, Value* def_value):
    m_column_name(column_name),
    m_type(type),
    m_type_len(type_len),
    m_nullable(nullable),
    m_autoincrement(ai),
    m_default_value( def_value ),
    m_is_long_column( false )
{
    
}

Column::Column(ByteData* data)
{
    m_type_len = data->readUInt16();
    applyFlags(data->readUInt8());
    m_default_value_ptr = data->readUInt16();
    uint8_t collen = data->readUInt8();
    m_column_name = data->readString(collen);
    m_default_value = NULL;
    m_is_long_column = false;
}

Column::~Column()
{
    delete m_default_value;
}

Value* Column::createEmptyDefaultVal() const
{
    cout << "err" << endl;
}


void Column::setDefaultValue(Value* defVal)
{
    if (m_default_value)
        delete m_default_value;
    m_default_value = defVal;
}

void Column::initDefaultValue()
{
    if (m_default_value == NULL)
    {
        m_default_value = createEmptyDefaultVal();
    }
}

Value* Column::defaultValue() const
{
    return m_default_value;
}

uint8_t Column::byteSize() const
{
    return 1 + 1 + 2 + 1 + 2 + 1 + m_column_name.size() + 1;
}

uint8_t Column::flags() const
{
    //flags 000000 autoincrement null
    uint8_t flags = 0;
    if (m_autoincrement) flags |= 0b00000010;
    if (m_nullable) flags |= 0b00000001;

    return flags;
}

void Column::applyFlags(uint8_t flags)
{
    m_autoincrement = (flags & 0b00000010);
    m_nullable = (flags & 0b00000001);
}

