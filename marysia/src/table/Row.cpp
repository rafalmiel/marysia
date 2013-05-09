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


#include "Row.h"

#include <iostream>

#include "table/KeyInfo.h"

#include "filestruct/BlockStorageManager.h"
#include "page/tableinfo/PageTableInfo.h"
#include "table/KeyValue.h"
#include "table/ValueFactory.h"
#include "table/ValueIncrementable.h"
#include "table/ValueLongBlock.h"

using namespace std;

Row::Row(): m_tableinfo( 0 ), m_row_byteSize( 2/*row len value*/ )
{

}

Row::Row(PageTableInfo* tableinfo): m_row_byteSize( 2 )
{
    setPageTableInfo( tableinfo );
}

uint16_t Row::byteSize() const
{
    return m_row_byteSize;
}

Row::~Row()
{
    for ( values_vector_t::const_iterator it = m_values.begin(); it != m_values.end(); ++it )
    {
        delete (*it);
    }

    m_values.clear();
}

KeyValue* Row::createKeyValueByName(const String& keyName) const
{
    if ( m_tableinfo->keyExists( keyName ) )
    {
        KeyInfo* key_info = m_tableinfo->keyByName( keyName );
        KeyValue* key_value = new KeyValue( key_info );
        key_value->setOwnsValues( false );

        for ( int i = 0; i < key_info->keyColumnNum(); ++i )
        {
            key_value->addValue( valueAt( key_info->keyPart( i )->column()->columnNr() ) );
        }

        return key_value;
    }
    return NULL;
}

Value* Row::valueAt(uint16_t pos) const
{
    return m_values[ pos ];
}

void Row::setValueAt(uint16_t pos, Value* value)
{
    if ( m_values[ pos ] )
    {
        delete m_values[ pos ];
    }
    m_values[ pos ] = value;
}

void Row::setPageTableInfo(PageTableInfo* page_tableinfo)
{
    m_tableinfo = page_tableinfo;
    for ( int16_t i = 0; i < valuesCount(); ++i )
    {
        Value* val = valueAt( i );
        Column* column = m_tableinfo->columnByNum( i );
        if ( val != NULL )
        {
            
            valueAt( i )->setTypeLen( m_tableinfo->columnByNum( i )->typeLen() );
        }
        else
        {
            val = m_tableinfo->columnByNum( i )->defaultValue()->clone();
            m_values[ i ] = val;
        }
        
        m_row_byteSize += (2/*value ptr*/ + 2/*len value*/ + valueAt( i )->storeSize());
    }

}

void Row::saveLongValues()
{
    for ( long_values_vector_t::const_iterator it = m_long_values.begin(); it != m_long_values.end(); ++it ) {
        
        ValueLongBlock* valueLong = (*it);
        long_data_t long_data = valueLong->longData();
        
        block_ptr_t block_ptr;
        
        if ( long_data.total_len > 0 )
        {
            block_ptr = m_block_storage_manager->saveData( long_data.raw_data, long_data.total_len );
        } else 
        {
            block_ptr.nr = 0;
            block_ptr.page_id = 0;
        }
        
        long_data_info_t data_info;
        data_info.page_id = block_ptr.page_id;
        data_info.nr = block_ptr.nr;
        data_info.total_len = long_data.total_len;
        
        valueLong->setLongDataInfo( data_info );
    }
}

Row* Row::addValue(Value* val)
{
    if ( m_tableinfo != NULL )
    {
        Column* column = m_tableinfo->columnByNum( valuesCount() );
        if ( val != NULL )
        {
            val->setTypeLen( column->typeLen() );
        }
        else
        {
            val = column->defaultValue()->clone();
        }
        
        m_row_byteSize += (2/*value ptr*/ + 2/*len value*/ + m_tableinfo->columnByNum( valuesCount() )->valueStoreSize());
    }
    
    m_values.push_back( val );
    
    if ( val )
    {
        if ( val->isLongData() )
        {
            m_long_values.push_back( static_cast<ValueLongBlock*>( val ) );
        }
    }
    
    return this;
}

void Row::setAutoincVal(int32_t value)
{
    ValueIncrementable* incVal = static_cast<ValueIncrementable*>( valueAt( m_tableinfo->primaryKey()->aiColumnNum() ) );
    if ( incVal->isAutoincReplace() )
    {
        incVal->setAutoincValue( value );
    }
}

int32_t Row::autoincVal() const
{
    static_cast<ValueIncrementable*>( valueAt( m_tableinfo->primaryKey()->aiColumnNum() ) )->autoincValue();
}

std::ostream& operator<<(std::ostream& os, const Row& row)
{
    String out = "| ";
    for (int i = 0; i < row.valuesCount(); ++i)
    {
        out = (out + row.valueAt(i)->toString() + " | ");
    }
    os.write(out.c_str(),out.size());
    return os;
}
