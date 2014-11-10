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

#include "cache/Cache.h"
#include "btree/BTree.h"
#include "filestruct/BlockStorageManager.h"
#include "filestruct/Tableinfo.h"
#include "filestruct/Tablespace.h"
#include "table/KeyValue.h"
#include "table/Row.h"

TableManager::TableManager(): m_buffer(NULL), m_isTableOpen(false)
{
    m_buffer = new Cache();
    m_tableinfo = new Tableinfo(m_buffer);
    m_tablespace = new Tablespace(m_buffer);
    m_btree = new BTree( m_buffer );
    m_block_storage_manager = new BlockStorageManager( m_buffer );
}

bool TableManager::isOpened() const
{
    return m_isTableOpen;
}

bool TableManager::tableExists(const String &tablename) const
{
    return m_tableinfo->tableInfoExists( tablename );
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

        m_isTableOpen = true;
    }
    else
    {
        openTable( tablename );
    }
}

bool TableManager::openTable(const String& tablename)
{
    if ( m_tableinfo->tableInfoExists( tablename ) )
    {
        m_tableinfo->openTable( tablename );

        m_tablespace->openTable( tablename, m_tableinfo->page() );

        m_block_storage_manager->openTable( Tablespace::createTableDataFilename( tablename ), m_tableinfo->page(), m_tablespace );

        m_btree->openTable( Tablespace::createTableDataFilename( tablename ), m_tableinfo->page(), m_tablespace, m_block_storage_manager );

        m_isTableOpen = true;

        return true;
    } else {

        m_isTableOpen = false;
        return false;
    }
}

String TableManager::currentTableName() const
{
    return m_tableinfo->currentTableName();
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

Row* TableManager::read(const String& key_name, KeyValue* key_value)
{
    return m_btree->read(key_name, key_value);
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
    if (res) {
        m_tableinfo->incRowsCount();
    }
    delete row;
    return res;
}

bool TableManager::deleteRow(KeyValue* key_value)
{
    key_value->setKeyInfo( m_tableinfo->page()->primaryKey() );
    m_btree->deleteRow( key_value );
    if (m_btree->lastDeleteResult()) {
        m_tableinfo->decRowsCount();
    }
    delete key_value;
    return m_btree->lastDeleteResult();
}

uint32_t TableManager::rowsCount() const
{
    return m_tableinfo->rowsCount();
}

void TableManager::closeTable()
{
    m_tableinfo->closeTable();
    m_tablespace->closeTable();
    m_isTableOpen = false;
}

TableManager::~TableManager()
{
    delete m_tableinfo;
    delete m_tablespace;
    delete m_btree;
    delete m_block_storage_manager;
    delete m_buffer;
}
