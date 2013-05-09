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

#include "KeyValue.h"

#include "table/KeyInfo.h"
#include "table/ValueIncrementable.h"

KeyValue::KeyValue(): m_key_info( 0 ), m_key_byteSize( 2 ), m_is_infimum( false )
{
    m_values.clear();
    m_owns_values = true;
}

KeyValue::KeyValue(KeyInfo* keyInfo): m_key_info( keyInfo ), m_key_byteSize( 2 ), m_is_infimum( false )
{
    m_values.clear();
    //m_values.resize( keyInfo->keyColumnNum(), NULL );
    m_owns_values = true;
}

KeyValue::~KeyValue()
{
    if ( m_owns_values )
    {
        for ( values_vector_t::const_iterator it = m_values.begin(); it != m_values.end(); ++it )
        {
            delete (*it);
        }
    }

    m_values.clear();
}

void KeyValue::setKeyInfo(KeyInfo* key_info)
{
    m_key_info = key_info;
    for ( int16_t i = 0; i < valuesCount(); ++i )
    {
        valueAt( i )->setTypeLen( m_key_info->keyPart(i)->column()->typeLen() );
        m_key_byteSize += (2/*len value*/ + valueAt( i )->storeSize());
    }
}

KeyValue* KeyValue::addValue(Value* value)
{
    if ( m_key_info != NULL )
    {
        value->setTypeLen( m_key_info->keyPart( valuesCount() )->column()->typeLen() );
        m_key_byteSize += (2/*len value*/ + value->storeSize());
    }
    m_values.push_back( value );

    return this;
}

int8_t KeyValue::compare(KeyValue* key_value) const
{
    if ( isInfimum() )
    {
        if ( key_value->isInfimum() ) return 0;
        else return -1;
    } else if ( key_value->isInfimum() ) return 1;
    
    for ( int i = 0; i < m_key_info->keyColumnNum(); ++i )
    {
        int8_t comp = valueAt( i )->compare( key_value->valueAt( i ) );
        if ( comp == 0 ) continue; else return comp;
    }
    
    return 0;
}

std::ostream& operator<<(std::ostream& os, const KeyValue& row)
{
    if ( row.isInfimum() )
    {
        os.write("| _infimum_ |", 13);
        return os;
    }
    String out = "| ";
    for (int i = 0; i < row.keyInfo()->keyColumnNum(); ++i)
    {
        out = (out + row.valueAt(i)->toString() + " | ");
    }
    os.write(out.c_str(),out.size());
    return os;
}
