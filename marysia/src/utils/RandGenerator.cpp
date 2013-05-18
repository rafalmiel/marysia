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

#include "RandGenerator.h"

#include <iostream>
#include <stdlib.h>
#include <limits>

#include <table/Column.h>
#include <table/values/ValueBlob.h>
#include <table/values/ValueDate.h>
#include <table/values/ValueDatetime.h>
#include <table/values/ValueDouble.h>
#include <table/values/ValueInteger.h>
#include <table/values/ValueSmallint.h>
#include <table/values/ValueTinyint.h>
#include <table/values/ValueVarchar.h>
#include "utils/String.h"
#include "utils/ByteData.h"

using namespace std;

String RandGenerator::randomString(uint8_t len)
{
    char str[len + 1];
    str[len] = '\0';

    for (int i = 0; i < len; ++i)
    {
        str[i] = (char) (rand() % ('z' - 'a' + 1) + 'a') ;
    }

    return str;
}

int32_t RandGenerator::randomInt(int32_t max)
{
    return rand() % max;
}

double RandGenerator::randomDouble()
{
    return drand48();
}

Value *RandGenerator::randomValue(Column *column)
{
    uint32_t size = 0;
    uint8_t* blob = NULL;
    //ByteData::dataMemSet( blob, 77, size );

    switch (column->type())
    {
    case VARCHAR_TYPE:
        return new ValueVarchar(randomString(column->typeLen()).c_str());
        break;
    case INT_TYPE:
        return new ValueInteger(randomInt(std::numeric_limits<int32_t>().max()));
        break;
    case SMALLINT_TYPE:
        return new ValueSmallint(randomInt(std::numeric_limits<int16_t>().max()));
        break;
    case TINYINT_TYPE:
        return new ValueTinyint(randomInt(std::numeric_limits<int8_t>().max()));
        break;
    case DATE_TYPE:
        return new ValueDate(randomInt(std::numeric_limits<int32_t>().max()));
        break;
    case DATETIME_TYPE:
        return new ValueDatetime(randomInt(std::numeric_limits<int32_t>().max()));
    break;
    case DOUBLE_TYPE:
        return new ValueDouble(randomDouble());
        break;
    case BLOB_TYPE:
        size = abs(randomInt(1024*32));
        blob = new uint8_t[size];
        ByteData::dataMemSet(blob, 77, size);
        return new ValueBlob(blob, size);
        break;
    }
}

