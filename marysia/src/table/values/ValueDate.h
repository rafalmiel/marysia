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


#ifndef VALUEDATE_H
#define VALUEDATE_H

#include "ValueDatetime.h"

class ValueDate : public ValueDatetime
{
public:
    ValueDate();
    ValueDate(const String& date);
    ValueDate(int date);

    void setDate(const String& datetime);
    String date() const;

    virtual String toString() const;
};

#endif // VALUEDATE_H
