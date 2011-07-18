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

aaa := DATASET('aaa', {STRING1 fa}, hole);
bbb := DATASET('bbb', {STRING1 fb}, hole, aaa);

//sorted_bbb := SORTED(bbb, fb);

sorted_bbb := SORT(bbb,fb);

OUTPUT(SORT(aaa, fa, JOINED(sorted_bbb)));

//OUTPUT(SORT(aaa, fa, JOINED(SORTED(bbb, fb))));