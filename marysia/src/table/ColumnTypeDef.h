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


#ifndef COLUMNTYPEDEF_H
#define COLUMNTYPEDEF_H

enum ColumnType {
    VARCHAR_TYPE = 1,
    TINYINT_TYPE = 2, //1 byte
    SMALLINT_TYPE = 3, //2 bytes
    MEDIUMINT_TYPE = 4, //3 bytes
    INT_TYPE = 5, //4 bytes
    DATE_TYPE = 6,
    DATETIME_TYPE = 7,
    DOUBLE_TYPE = 8,
    BLOB_TYPE = 12
};



#endif // COLUMNTYPEDEF_H
