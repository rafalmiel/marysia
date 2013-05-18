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


#include "Tableinfo.h"

#include <iostream>
#include <stdlib.h>

#include "buffer/Buffer.h"
#include "io/File.h"
#include "table/KeyInfo.h"
#include "page/PageFactory.h"
#include "utils/StringTokens.h"

using namespace std;

Tableinfo::Tableinfo(Buffer* buffer): m_buffer(buffer), m_page_tableinfo(NULL)
{

}

PageTableInfo* Tableinfo::getTableInfo()
{
    return static_cast<PageTableInfo*>( m_buffer->getPage( 0, m_current_filename, false) );
}

PageTableInfo* Tableinfo::createTableInfo(const String& tablename)
{
    if ( ! tableInfoExists( tablename ) )
    {
        m_page_tableinfo = PageFactory::createPageTableInfo( tablename );
        return m_page_tableinfo;
    }
}

void Tableinfo::openTable(const String& tablename)
{
    setCurrentTable( tablename );
    m_page_tableinfo = getTableInfo();
}

void Tableinfo::closeTable()
{
    m_buffer->savePage( m_page_tableinfo );
}

void Tableinfo::addColumn(Column* column)
{
    m_page_tableinfo->addColumn( column );
}

KeyInfo* Tableinfo::addKey(const String& key_name, const String& columns, bool is_primary/*, bool is_autoincrement*/, bool is_unique)
{
    if (! m_page_tableinfo->keyExists(key_name))
    {
        KeyInfo* key_info = new KeyInfo(key_name);
        key_info->setIsPrimary(is_primary);
        //key_info->setIsAutoincrement(is_autoincrement);
        key_info->setIsUnique(is_unique);

        //StringTokens tokens(columns, ";");
        std::vector<String> tokens = columns.split(";");

        for (int i = 0; i < tokens.size(); ++i)
        {
            key_info->addKeyPart( new KeyPart( m_page_tableinfo->columnByName( tokens[i] ) ) );
        }

        m_page_tableinfo->addKey( key_info );

        return key_info;
    }
    return NULL;
}

bool Tableinfo::tableInfoExists(const String& tablename)
{
    return File::file_exists( createTableFilename( tablename ) );
}

String Tableinfo::createTableFilename(const String& tablename)
{
    return tablename + "." + TABLE_DEF_EXT;
}
