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


#include "ValueFactory.h"

#include <iostream>

#include "table/Value.h"
#include "table/values/ValueBlob.h"
#include "table/values/ValueInteger.h"
#include "table/values/ValueTinyint.h"
#include "table/values/ValueVarchar.h"
#include "table/values/ValueDate.h"
#include "table/values/ValueDatetime.h"
#include "table/values/ValueDouble.h"
#include "table/values/ValueSmallint.h"
#include "utils/ByteData.h"

using namespace std;

Value* ValueFactory::createValue(ColumnType type, uint16_t type_len, const uint8_t* data, uint16_t valueLen)
{
    Value* defval = 0;
    switch (type)
    {
        case INT_TYPE:
            defval = new ValueInteger( );
            break;
        case VARCHAR_TYPE:
            defval = new ValueVarchar( type_len );
            break;
        case TINYINT_TYPE:
            defval = new ValueTinyint( );
            break;
        case SMALLINT_TYPE:
            defval = new ValueSmallint( );
            break;
        case DATETIME_TYPE:
            defval = new ValueDatetime( );
            break;
        case DATE_TYPE:
            defval = new ValueDate( );
            break;
        case DOUBLE_TYPE:
            defval = new ValueDouble( );
            break;
        case BLOB_TYPE:
            defval = new ValueBlob( );
    }

    defval->setRawData(data, valueLen);
    
    return defval;
}
