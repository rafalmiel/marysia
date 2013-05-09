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


#ifndef TABLEINFO_H
#define TABLEINFO_H

#include "page/tableinfo/PageTableInfo.h"

class Buffer;
class Column;
class KeyInfo;

#define TABLE_DEF_EXT "inf"

class Tableinfo
{
public:
    Tableinfo(Buffer* buffer);

    PageTableInfo* createTableInfo(const String& tablename);
    PageTableInfo* getTableInfo();
    
    bool tableInfoExists(const String& tablename);
    
    static String createTableFilename(const String& tablename);

    inline void setCurrentTable(const String& tablename)
    {
        m_current_tablename = tablename;
        m_current_filename = createTableFilename( tablename );
    }

    void openTable( const String& tablename );

    void addColumn( Column* column );
    KeyInfo* addKey( const String& key_name, const String& columns, bool is_primary/*, bool is_autoincrement*/, bool is_unique );

    Column* columnByNum( int num ) const { return m_page_tableinfo->columnByNum( num ); }
    Column* columnByName( const String& name ) { return m_page_tableinfo->columnByName( name ); }

    void closeTable();

    PageTableInfo* page() const { return m_page_tableinfo; }
private:
    Buffer* m_buffer;

    String m_current_tablename;
    String m_current_filename;
    PageTableInfo* m_page_tableinfo;
};

#endif // TABLEINFO_H
