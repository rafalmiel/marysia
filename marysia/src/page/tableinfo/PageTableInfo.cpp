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


#include "PageTableInfo.h"

#include <iostream>
#include <string.h>

#include "table/ColumnFactory.h"
#include "table/Column.h"
#include "table/ColumnTypeDef.h"
#include "table/KeyInfo.h"
#include "table/ValueFactory.h"
#include "table/values/ValueInteger.h"
#include "table/values/ValueVarchar.h"
#include "utils/ByteData.h"
#include "utils/StringTokens.h"

using namespace std;

PageTableInfo::PageTableInfo(ByteData* data, bool init) : Page(data), m_primary_key(0)
{
    m_is_fixed_lenght_page = false;
    m_page_size = m_data->size();
    m_was_structure_changed = ! init;
    if (init)
    {
        m_data->seek(TABLE_NAME_OFFSET);
        uint8_t tablename_len = m_data->readUInt8();

        m_tablename = m_data->readString( tablename_len );

        uint16_t column_count = m_data->readUInt16At(INFO_COLUMN_COUNT_OFFSET);
        uint16_t columndef_ptr = m_data->readUInt16At(INFO_COLUMN_COUNT_OFFSET+2);

        uint16_t key_count = m_data->readUInt16At(INFO_KEY_COUNT_OFFSET);
        uint16_t keydef_ptr = m_data->readUInt16At(INFO_KEY_COUNT_OFFSET+2);

        for (int i = 0; i < column_count; ++i)
        {
            m_data->seek(columndef_ptr);

            Column* column = ColumnFactory::createColumn( m_data );

            column->setColumnNr( i );

            uint16_t def_val_ptr = column->defaultValuePtr();

            columndef_ptr = m_data->pos();

            if ( def_val_ptr != 0 )
            {
                m_data->seek(def_val_ptr);
                
                uint16_t def_val_len = m_data->readUInt16();
                
                column->setDefaultValue(
                    ValueFactory::createValue(
                        column->type(),
                        column->typeLen(),
                        m_data->dataPtr( true ),
                        def_val_len
                    )
                );
            }
            else
            {
                column->setDefaultValue( NULL );
            }

            addColumn(column);
        }

        for (int i = 0; i < key_count; ++i)
        {
            m_data->seek(keydef_ptr);

            uint16_t keysize = m_data->readUInt16();

            uint8_t flags = m_data->readUInt8();
            
            uint8_t parts_count = m_data->readUInt8();

            uint8_t keyname_len = m_data->readUInt8();

            String keyname;
            keyname = m_data->readString( keyname_len );

            KeyInfo* key_info = new KeyInfo(keyname);
            key_info->applyFlags(flags);

            for (int j = 0; j < parts_count; ++j)
            {
                uint8_t column_name_len = m_data->readUInt8();
                String column_name;
                column_name = m_data->readString( column_name_len );

                key_info->addKeyPart( new KeyPart( columnByName( column_name ) ) );
            }

            addKey( key_info );
        }

        m_was_structure_changed = m_is_dirty = false;
    }

}

void PageTableInfo::setTableName(const String& tablename)
{
    if (tablename.size() <= 64)
    {
        m_tablename = tablename;
        m_was_structure_changed = m_is_dirty = true;
    }
}

const String& PageTableInfo::tablename() const
{
    return m_tablename;
}

void PageTableInfo::setRowsCount(uint32_t cnt)
{
    m_data->writeUInt32At(INFO_ROWS_COUNT_OFFSET, cnt);
    m_was_structure_changed = m_is_dirty = true;
}

uint32_t PageTableInfo::rowsCount() const
{
    return m_data->readUInt32At(INFO_ROWS_COUNT_OFFSET);
}

uint16_t PageTableInfo::columnCount() const
{
    return m_columns.size();
}

void PageTableInfo::addColumn(Column* column)
{
    if (m_columns_map.find(column->columnName().c_str()) == m_columns_map.end())
    {
        column->setColumnNr( m_columns.size() );
        m_columns.push_back(column);
        m_columns_map[column->columnName().c_str()] = column;
        
        if (column->isLongColumn())
        {
            m_long_columns.push_back( column );
        }
        
        m_was_structure_changed = m_is_dirty = true;
    }
    else
    {
        delete column;
    }
}

void PageTableInfo::addKey(KeyInfo* key)
{
    /*if (key->isAutoincrement())
        key->setIsPrimary(true);*/
    
    if (key->isPrimary() && m_primary_key)
    {
        delete key;
        return;
    }
        

    m_keys.push_back(key);
    m_keys_map[ key->keyName().c_str() ] = key;

    if (key->isPrimary())
        m_primary_key = key;

    m_was_structure_changed = m_is_dirty = true;
}

uint16_t PageTableInfo::countSize() const
{
    uint16_t requiredSize = DEFAULT_VALUES_OFFSET;

    for (columns_vector_t::const_iterator it = m_columns.begin(); it != m_columns.end(); it++)
    {
        Column *col = (*it);
        if (col->hasDefaultVal())
        {
            requiredSize += 2 + col->valueStoreSize();
        }
        requiredSize += col->byteSize();
    }

    for (key_vector_t::const_iterator it = m_keys.begin(); it != m_keys.end(); it++)
    {
        KeyInfo *key = (*it);
        requiredSize += key->byteSize();
    }

    return requiredSize;
}

ByteData* PageTableInfo::data()
{
    if ( m_was_structure_changed )
    {
        m_page_size = countSize();

        ByteData* data = new ByteData( m_page_size );

        data->seek( 0 );
        m_data->seek( 0 );
        
        data->writeDataAt( 0, m_data->dataPtr( true ), PAGE_HEADER_SIZE );

        data->seek( TABLE_NAME_OFFSET );
        uint8_t len = m_tablename.size();
        data->writeUInt8( len + 1 );
        data->writeString( m_tablename.c_str(), 65 );

        data->seek( INFO_COLUMN_COUNT_OFFSET );
        data->writeUInt16( m_columns.size() );

        data->seek( INFO_KEY_COUNT_OFFSET );
        data->writeUInt16( m_keys.size() );

        data->seek( DEFAULT_VALUES_OFFSET );
        uint16_t def_vals_pointers[ m_columns.size() ];

        data->writeUInt32At(INFO_ROWS_COUNT_OFFSET, m_data->readUInt32At(INFO_ROWS_COUNT_OFFSET));

        uint16_t i = 0;
        for ( columns_vector_t::const_iterator it = m_columns.begin(); it != m_columns.end(); ++it )
        {
            const Column *col = ( *it );

            if (col->hasDefaultVal())
            {
                def_vals_pointers[i] = data->pos();
                data->writeUInt16( col->defaultValue()->valueLen() );
                data->writeData( col->defaultValue()->rawData(), col->defaultValue()->valueLen() );
                
                data->seek( def_vals_pointers[i] );
                data->moveBy( 2 + col->defaultValue()->storeSize() );
            }
            else
            {
                def_vals_pointers[i] = 0;
            }
            i++;
        }

        data->writeUInt16At(INFO_COLUMN_COUNT_OFFSET + 2,data->pos());

        i = 0;
        for (columns_vector_t::const_iterator it = m_columns.begin(); it != m_columns.end(); ++it)
        {
            const Column *col = ( *it );
            data->writeUInt8( col->byteSize() );
            data->writeUInt8( ( uint8_t ) col->type() );
            data->writeUInt16( col->typeLen() );
            data->writeUInt8( col->flags() );
            data->writeUInt16( def_vals_pointers[i] );
            data->writeUInt8( col->columnName().size() + 1 );
            data->writeString( col->columnName().c_str() );
            i++;
        }

        data->writeUInt16At( INFO_KEY_COUNT_OFFSET + 2, data->pos() );

        for (key_vector_t::const_iterator it = m_keys.begin(); it != m_keys.end(); ++it)
        {
            const KeyInfo *key = ( *it );

            data->writeUInt16( key->byteSize() );
            data->writeUInt8( key->flags() );
            data->writeUInt8( key->keyColumnNum() );
            data->writeUInt8( key->keyName().size() + 1 );
            data->writeString( key->keyName().c_str() );
            for (int j = 0; j < key->keyColumnNum(); ++j)
            {
                KeyPart* part = key->keyPart(j);
                data->writeUInt8( part->columnName().size() + 1 );
                data->writeString( part->columnName().c_str() );
            }
        }
        delete m_data;
        m_data = data;
        m_was_structure_changed = false;
    }
    return m_data;
}

void PageTableInfo::debugIndex()
{
    for (key_vector_t::const_iterator it = m_keys.begin(); it != m_keys.end(); ++it)
    {
        KeyInfo* key = (*it);
        cout << "klucz " << key->keyName() << " sklada sie z " << (uint16_t) key->keyColumnNum() << " kolumn" << endl;
    }
}

PageTableInfo::~PageTableInfo()
{
    for (columns_vector_t::const_iterator it = m_columns.begin(); it != m_columns.end(); ++it)
    {
        delete (*it);
    }

    for (key_vector_t::const_iterator it = m_keys.begin(); it != m_keys.end(); ++it)
    {
        delete (*it);
    }

    m_columns.clear();
    m_columns_map.clear();
    m_keys.clear();
    m_keys_map.clear();
    m_long_columns.clear();
}

