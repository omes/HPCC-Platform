/*##############################################################################

    Copyright (C) 2011 HPCC Systems.

    All rights reserved. This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
############################################################################## */


surnameRecord := { string       surname };
surnameField := surnameRecord.surname;

//Really want to come up with a way to allow defaults
filterByName(virtual dataset({ string surname }) ds, opt <?> string filterField = ds.surname, string search) := function
    //??Need some way to use a field, but use a default name if none is supplied e.g. <filterField | surname> yuk
    //An alternative would be to reparse the defaults for the formal arguments, which may be slightly tricky as well.
    //or reinterpret ds.surname as ds->lookupSurname
    return ds(<filterField> = search);
END;

namesRecord := 
            RECORD
string20        surname;
string10        forename;
integer2        age := 25;
            END;

namesTable2 := dataset([
        {'Halliday','Gavin',31},
        {'Halliday','Liz',30},
        {'Salter','Abi',10},
        {'X','Z'}], namesRecord);

//output(filterByName(namesTable2, namesTable2.forename, 'Liz'));
output(filterByName(namesTable2, 'Halliday'));