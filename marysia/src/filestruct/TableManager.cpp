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


#include "TableManager.h"

#include "buffer/Buffer.h"
#include "btree/BTree.h"
#include "filestruct/BlockStorageManager.h"
#include "filestruct/Tableinfo.h"
#include "filestruct/Tablespace.h"
#include "table/KeyValue.h"
#include "table/Row.h"

TableManager::TableManager(): m_buffer(NULL)
{
    m_buffer = new Buffer();
    m_tableinfo = new Tableinfo(m_buffer);
    m_tablespace = new Tablespace(m_buffer);
    m_btree = new BTree( m_buffer );
    m_block_storage_manager = new BlockStorageManager( m_buffer );
}

void TableManager::createTable(const String& tablename)
{
    if ( ! m_tableinfo->tableInfoExists( tablename ) )
    {
        m_tableinfo->createTableInfo(tablename);

        m_tableinfo->setCurrentTable( tablename );

        m_tablespace->openTable( tablename, m_tableinfo->page() );

        m_tablespace->initTablespace();
        
        m_block_storage_manager->openTable( Tablespace::createTableDataFilename( tablename ), m_tableinfo->page(), m_tablespace );

        m_btree->openTable( Tablespace::createTableDataFilename( tablename ), m_tableinfo->page(), m_tablespace, m_block_storage_manager );
    }
    else
    {
        openTable( tablename );
    }
}

void TableManager::openTable(const String& tablename)
{
    m_tableinfo->openTable( tablename );
    
    m_tablespace->openTable( tablename, m_tableinfo->page() );
    
    m_block_storage_manager->openTable( Tablespace::createTableDataFilename( tablename ), m_tableinfo->page(), m_tablespace );

    m_btree->openTable( Tablespace::createTableDataFilename( tablename ), m_tableinfo->page(), m_tablespace, m_block_storage_manager );

    
}

void TableManager::addColumn(Column* column)
{
    m_tableinfo->addColumn( column );
}

void TableManager::addKey(const String& key_name, const String& columns, bool is_primary, bool is_unique)
{
    KeyInfo* key_info = m_tableinfo->addKey(key_name, columns, is_primary, is_unique);

    if (key_info)
    {
        m_tablespace->addKey( key_info, m_tableinfo );
    }
}

Row* TableManager::readNext()
{
    return m_btree->readNext();
}

void TableManager::readInit(const String& key_name)
{
    m_btree->readInit( key_name );
}

void TableManager::readClose()
{
    m_btree->readClose();
}

bool TableManager::insertRow(Row* row)
{
    row->setPageTableInfo( m_tableinfo->page() );
    row->setBlockStorageManager( m_block_storage_manager );
    bool res = m_btree->insertRow( row );
    delete row;
    return res;
}

void TableManager::deleteRow(KeyValue* key_value)
{
    key_value->setKeyInfo( m_tableinfo->page()->primaryKey() );
    m_btree->deleteRow( key_value );
    delete key_value;
}

void TableManager::closeTable()
{
    m_tableinfo->closeTable();
    m_tablespace->closeTable();
}

TableManager::~TableManager()
{
    delete m_tableinfo;
    delete m_tablespace;
    delete m_btree;
    delete m_block_storage_manager;
    delete m_buffer;
}
