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


#ifndef VALUEDATETIME_H
#define VALUEDATETIME_H

#include "table/Value.h"

class ValueDatetime : public Value
{
public:
    ValueDatetime();
    ValueDatetime(const String& datetime);
    ValueDatetime(int datetime);

    ~ValueDatetime();

protected:
    virtual void deleteValue();

    int32_t unixTime() const;
    void setUnixTime(int32_t unixTime);

public:
    void setDatetime(const String& datetime);
    String datetime() const;
    
    virtual int8_t compare(Value* value) const;
    virtual uint16_t storeSize() const;
    virtual const uint8_t* rawData();
    virtual void setRawData(const uint8_t* data, uint16_t len);
    virtual String toString() const;
private:
    uint8_t* m_raw_data;
};

#endif // VALUEDATETIME_H
