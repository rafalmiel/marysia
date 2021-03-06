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


#ifndef VALUEVARCHAR_H
#define VALUEVARCHAR_H

#include "table/Value.h"
#include "utils/String.h"

class ValueVarchar : public Value
{
public:
    ValueVarchar(uint16_t type_len);
    ValueVarchar(uint16_t type_len,  const char* string );
    ValueVarchar();
    ValueVarchar( const char* string );

    virtual ~ValueVarchar();

    const uint8_t* rawData();

    virtual int8_t compare(Value* value) const;
    
    const char* stringValue() const;
    void setStringValue(const char* string);

    virtual uint16_t storeSize() const;

    virtual String toString() const
    {
        if ( ! isNull() )
        {
            return stringValue();
        }
        else
        {
            return "__null__";
        }
    }
protected:
    virtual void adjustValueLen();
    virtual void deleteValue();
};

#endif // VALUEVARCHAR_H
