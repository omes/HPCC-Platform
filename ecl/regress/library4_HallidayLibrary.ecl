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

#option ('targetClusterType', 'hthor');
//Example of nested
namesRecord :=
            RECORD
string20        surname;
string10        forename;
integer2        age := 25;
            END;


filterLibrary(dataset(namesRecord) ds, string search, boolean onlyOldies) := interface
    export dataset(namesRecord) included;
    export dataset(namesRecord) excluded;
end;


//NOTE: This is an example of inheritance!

hallidayLibrary(dataset(namesRecord) ds) := interface
    export dataset(namesRecord) included;
    export dataset(namesRecord) excluded;
    export grouped dataset(namesRecord) both;
end;

namesTable := dataset('x',namesRecord,FLAT);

//Library definitions

impFilterLibrary(dataset(namesRecord) ds, string search, boolean onlyOldies) := module,library(FilterLibrary)
    f := ds;
    shared g := if (onlyOldies, f(age >= 65), f);
    export included := g(surname != search);
    export excluded := g(surname = search);
end;

baseLibrary(dataset(namesRecord) ds) := LIBRARY('FilterLibrary', filterLibrary(ds, 'Hawthorn', false));
impHallidayLibrary(dataset(namesRecord) ds) := module(baseLibrary(ds)),library(HallidayLibrary)
    export both := group(included,false) + group(excluded, false);
end;

#workunit('name','HallidayLibrary');
BUILD(impHallidayLibrary);
