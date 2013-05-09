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


#include "ValueDouble.h"

#include <iostream>

#include "utils/String.h"

using namespace std;


ValueDouble::ValueDouble(): Value(8), m_raw_data( 0 )
{
    setType( DOUBLE_TYPE );
    setValueLen( 0 );
}

ValueDouble::ValueDouble(double val): Value(8), m_raw_data( 0 )
{
    setType( DOUBLE_TYPE );
    setDoubleValue( val );
}

void ValueDouble::deleteValue()
{
    if (m_raw_data) delete [] m_raw_data;
    if (m_value)
    {
        delete (double*) m_value;
    }
}

void ValueDouble::setDoubleValue(double val)
{
    deleteValue();

    m_value = new double( val );

    setValueLen( 8 );
}

double ValueDouble::doubleValue() const
{
    if ( ! isNull() )
    {
        return *((double*) m_value);
    }
    else
    {
        return 0;
    }
}

String ValueDouble::toString() const
{
    return String::doubleToStr( doubleValue() );
}

int8_t ValueDouble::compare(Value* value) const
{
    if (isNull() && !value->isNull()) return -1;
    else if (!isNull() && value->isNull()) return 1;
    else if (isNull() && value->isNull()) return 0;
    
    double val1 = doubleValue();
    double val2 = static_cast< ValueDouble* >( value )->doubleValue();
    
    if (val1 > val2) return 1;
    else if (val1 < val2) return -1;
    else return 0;
}

uint16_t ValueDouble::storeSize() const
{
    return m_type_len;
}

const uint8_t* ValueDouble::rawData()
{
    if ( isNull() ) return NULL;
    
    if (m_raw_data) delete [] m_raw_data;
    
    m_raw_data = new uint8_t[8];
    
    double val = doubleValue();

    dblconv conv;
    conv.dbl_val = val;
    
    for (uint8_t i = 0; i < 8; ++i)
    {
        m_raw_data[i] = conv.rawArr[i];
    }
    
    return m_raw_data;
}

void ValueDouble::setRawData(const uint8_t* data, uint16_t len)
{
    deleteValue();
    
    if ( len == 8 )
    {
        dblconv conv;

        for (uint8_t i = 0; i < 8; ++i)
        {
            conv.rawArr[i] = data[i];
        }
        
        double val = conv.dbl_val;
        
        setDoubleValue(val);
    }
    else
    {
        setValueLen( 0 );
    }
}

ValueDouble::~ValueDouble()
{
    deleteValue();
}

