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


#ifndef RANDGENERATOR_H
#define RANDGENERATOR_H

#include <stdint.h>


class String;
class Value;
class Column;

class RandGenerator
{
public:
    static String randomString(uint8_t len);
    static int32_t randomInt(int32_t max);
    static double randomDouble();
    static Value *randomValue(Column *column);
};

#endif // RANDGENERATOR_H
