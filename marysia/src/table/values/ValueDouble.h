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


#ifndef VALUEDOUBLE_H
#define VALUEDOUBLE_H

#include "table/Value.h"

typedef union {
    double dbl_val;
    uint8_t rawArr[8];
} dblconv;


class ValueDouble : public Value
{
public:
    ValueDouble( );
    ValueDouble( double val );

    void setDoubleValue( double val );
    double doubleValue() const;

protected:
    virtual void deleteValue();

public:
    virtual String toString() const;
    virtual int8_t compare(Value* value) const;
    virtual uint16_t storeSize() const;
    virtual const uint8_t* rawData();
    virtual void setRawData(const uint8_t* data, uint16_t len);
    virtual ~ValueDouble();

private:
    uint8_t* m_raw_data;
};

#endif // VALUEDOUBLE_H
