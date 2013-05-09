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


#ifndef VALUEINTEGER_H
#define VALUEINTEGER_H

#include "table/ValueIncrementable.h"
#include "utils/String.h"

class ValueInteger : public ValueIncrementable
{
public:
    ValueInteger();
    ValueInteger(int32_t value);

    virtual ~ValueInteger();

    virtual const uint8_t* rawData();

    virtual int8_t compare(Value* value) const;
    
    int32_t int32Value() const;
    void setInt32Value(int32_t value);
    virtual void setRawData(const uint8_t* data, uint16_t len);
    virtual uint16_t storeSize() const;

    virtual String toString() const
    {
        if ( ! isNull() )
        {
            return String::intToStr( int32Value() );
        }
        else
        {
            return "__null__";
        }
    }

    virtual void setAutoincValue(int32_t value);
    virtual int32_t autoincValue() const;
    virtual bool isAutoincReplace() const;

protected:
    virtual void deleteValue();

private:
    uint8_t* m_raw_data;
};

#endif // VALUEINTEGER_H
