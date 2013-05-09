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


#ifndef VALUESMALLINT_H
#define VALUESMALLINT_H

#include "table/ValueIncrementable.h"

class ValueSmallint : public ValueIncrementable
{

public:
    ValueSmallint();
    ValueSmallint(int16_t value);
    
    virtual ~ValueSmallint();

    int16_t int16Value() const;
    void setInt16Value(int16_t value);
    
    virtual int32_t autoincValue() const;
    virtual void setAutoincValue(int32_t value);
    virtual bool isAutoincReplace() const;

    virtual void setRawData(const uint8_t* data, uint16_t len);

protected:
    virtual void deleteValue();

public:
    virtual String toString() const;
    virtual int8_t compare(Value* value) const;
    virtual uint16_t storeSize() const;
    virtual const uint8_t* rawData();

private:
    uint8_t* m_raw_data;
};

#endif // VALUESMALLINT_H
