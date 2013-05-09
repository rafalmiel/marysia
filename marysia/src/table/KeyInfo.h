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


#ifndef KEYINFO_H
#define KEYINFO_H

#include <vector>

#include "KeyPart.h"

class KeyInfo
{
public:
    KeyInfo();
    KeyInfo(const String& name);
    virtual ~KeyInfo();

    const String& keyName() const { return m_key_name; }
    void setKeyName(const String& name) { m_key_name = name; }

    void addKeyPart(KeyPart* keyPart);

    uint8_t keyColumnNum() const { return m_key_parts.size(); }

    bool isPrimary() const { return m_is_primary; }
    bool isAutoincrement() const { return m_is_autoincrement; }
    bool isUnique() const { return m_is_unique; }
    uint16_t aiColumnNum() const { return m_ai_column_num; }

    void setIsPrimary( bool is = true ) { m_is_primary = is; }
    //void setIsAutoincrement( bool is = true ) { m_is_autoincrement = is; }
    void setIsUnique ( bool is = true ) { m_is_unique = is; }

    uint16_t byteSize() const;

    uint8_t flags() const;
    void applyFlags(uint8_t flags);

    KeyPart* keyPart(uint8_t num) const { return m_key_parts[num]; }

private:
    String m_key_name;
    uint16_t m_flags;

    bool m_is_primary;
    bool m_is_autoincrement;
    bool m_is_unique;
    uint16_t m_ai_column_num;

    typedef std::vector<KeyPart*> key_parts_t;

    key_parts_t m_key_parts;
};

#endif // KEYINFO_H
