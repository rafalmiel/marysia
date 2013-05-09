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


#include "ColumnFactory.h"
#include "ColumnTypeDef.h"
#include "Column.h"
#include "columns/ColumnBlob.h"
#include "columns/ColumnInteger.h"
#include "columns/ColumnVarchar.h"
#include "columns/ColumnTinyint.h"
#include "columns/ColumnSmallint.h"
#include "columns/ColumnDatetime.h"
#include "columns/ColumnDate.h"
#include "columns/ColumnDouble.h"
#include "table/Column.h"
#include "utils/ByteData.h"

Column* ColumnFactory::createColumn( ByteData* byteData )
{
    uint8_t colsize = byteData->readUInt8();

    ColumnType type = ( ColumnType ) byteData->readUInt8();

    Column* column = 0;

    switch ( type )
    {
        case INT_TYPE:
            column = new ColumnInteger( byteData );
            break;
        case VARCHAR_TYPE:
            column = new ColumnVarchar( byteData );
            break;
        case TINYINT_TYPE:
            column = new ColumnTinyint( byteData );
            break;
        case SMALLINT_TYPE:
            column = new ColumnSmallint( byteData );
            break;
        case DATETIME_TYPE:
            column = new ColumnDatetime( byteData );
            break;
        case DATE_TYPE:
            column = new ColumnDate( byteData );
            break;
        case DOUBLE_TYPE:
            column = new ColumnDouble( byteData );
            break;
        case BLOB_TYPE:
            column = new ColumnBlob( byteData );
            break;
    }

    column->setType( type );
    
    return column;
}
