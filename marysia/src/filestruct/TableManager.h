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


#ifndef TABLEMANAGER_H
#define TABLEMANAGER_H

class BlockStorageManager;
class BTree;
class Buffer;
class Column;
class KeyValue;
class Row;
class String;
class Tableinfo;
class Tablespace;

class TableManager
{
public:
    TableManager();
    virtual ~TableManager();

    void createTable( const String& tablename );
    void openTable( const String& tablename );
    void closeTable();

    void addColumn( Column* column );

    Tableinfo* meta() const { return m_tableinfo; }
    Tablespace* data() const { return m_tablespace; }

    void readInit(const String& key_name);
    Row* readNext();
    void readClose();
    
    //columns - column names separated by ";"
    void addKey(const String& key_name, const String& columns, bool is_primary, bool is_unique);


    bool insertRow( Row* row );
    void deleteRow( KeyValue* key_value );
    
    BlockStorageManager* blockStorageManager() const { return m_block_storage_manager; }

private:
    Tablespace* m_tablespace;
    Tableinfo* m_tableinfo;
    BTree* m_btree;
    BlockStorageManager* m_block_storage_manager;

    Buffer* m_buffer;
};

#endif // TABLEMANAGER_H