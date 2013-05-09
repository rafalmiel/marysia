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


#include "KeyInfo.h"

KeyInfo::KeyInfo(): m_is_autoincrement( false ), m_ai_column_num( 0 )
{

}

KeyInfo::KeyInfo(const String& name): m_is_autoincrement( false ), m_ai_column_num( 0 )
{
    setKeyName(name);
}

void KeyInfo::addKeyPart(KeyPart* keyPart)
{
    m_key_parts.push_back(keyPart);
    keyPart->column()->setIsNullable( false );
    if ( keyPart->isAutoincrement() )
    {
        m_is_autoincrement = true;
        m_ai_column_num = keyPart->column()->columnNr();
    }
}

KeyInfo::~KeyInfo()
{
    for (key_parts_t::const_iterator it = m_key_parts.begin(); it != m_key_parts.end(); ++it)
    {
        delete (*it);
    }

    m_key_parts.clear();
}

uint16_t KeyInfo::byteSize() const
{
    uint16_t size = 2; //total len

    size += 1; //flags
    size += 1; //parts_count
    
    size += 1; //keyname len

    size += m_key_name.size() + 1;

    for (key_parts_t::const_iterator it = m_key_parts.begin(); it != m_key_parts.end(); ++it)
    {
        size += 1 + (*it)->columnName().size() + 1; //len + name
    }
    
    return size;
}

uint8_t KeyInfo::flags() const
{
    uint8_t flags = 0;
    if (m_is_primary) flags |= 0b00000001;
    //if (m_is_autoincrement) flags |= 0b00000010;
    if (m_is_unique) flags |= 0b00000100;

    return flags;
}

void KeyInfo::applyFlags(uint8_t flags)
{
    m_is_primary = (flags & 0b00000001);
    //m_is_autoincrement = (flags & 0b00000010);
    m_is_unique = (flags & 0b00000100);
}


