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


#include "ValueDate.h"

#include "utils/DateTime.h"
#include "utils/String.h"

ValueDate::ValueDate(): ValueDatetime()
{
    setType(DATE_TYPE);
    setValueLen( 0 );
}

ValueDate::ValueDate(const String& date): ValueDatetime( )
{
    setType(DATE_TYPE);
    setDate( date );
}

ValueDate::ValueDate(int date)
{
    setType(DATE_TYPE);
    setUnixTime(date);
}

String ValueDate::date() const
{
    if ( ! isNull() )
    {
        return DateTime::dateToString( unixTime() );
    }
    else
    {
        return "1970-01-01";
    }
}

void ValueDate::setDate(const String& datetime)
{
    setUnixTime( DateTime::stringToDate( datetime ) );
}

String ValueDate::toString() const
{
    return date();
}
