/*
 * (c) 2002-2016 Hannes Krueger
 * This file is part of the GPLIGC/ogie package
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>


//const std::string rcsid_createworld_cpp =
//    "$Id: createworlddem.cpp 3 2014-07-31 09:59:20Z kruegerh $";
using namespace std;

int main(void)
{
	int i, j;
	char tmp;
	bool FAILED = false;

	// set 1
	//6,000 rows and 4,800 columns
	ifstream w180n90;
	ifstream w140n90;
	ifstream w100n90;
	ifstream w060n90;
	ifstream w020n90;
	ifstream e020n90;
	ifstream e060n90;
	ifstream e100n90;
	ifstream e140n90;

	// set 2

	ifstream w180n40;
	ifstream w140n40;
	ifstream w100n40;
	ifstream w060n40;
	ifstream w020n40;
	ifstream e020n40;
	ifstream e060n40;
	ifstream e100n40;
	ifstream e140n40;

	// set 3

	ifstream w180s10;
	ifstream w140s10;
	ifstream w100s10;
	ifstream w060s10;
	ifstream w020s10;
	ifstream e020s10;
	ifstream e060s10;
	ifstream e100s10;
	ifstream e140s10;

	// set 4
	//3,600 rows and 7,200 columns
	ifstream w180s60;
	ifstream w120s60;
	ifstream w060s60;
	ifstream w000s60;
	ifstream e060s60;
	ifstream e120s60;

	// whole world compiled
	// 21,600 rows and 43,200 columns
	ofstream world;

	cout << "trying to open all needed files... " << endl;

	w180n90.open("W180N90.DEM", ios::binary);
	w140n90.open("W140N90.DEM", ios::binary);
	w100n90.open("W100N90.DEM", ios::binary);
	w060n90.open("W060N90.DEM", ios::binary);
	w020n90.open("W020N90.DEM", ios::binary);
	e020n90.open("E020N90.DEM", ios::binary);
	e060n90.open("E060N90.DEM", ios::binary);
	e100n90.open("E100N90.DEM", ios::binary);
	e140n90.open("E140N90.DEM", ios::binary);

	w180n40.open("W180N40.DEM", ios::binary);
	w140n40.open("W140N40.DEM", ios::binary);
	w100n40.open("W100N40.DEM", ios::binary);
	w060n40.open("W060N40.DEM", ios::binary);
	w020n40.open("W020N40.DEM", ios::binary);
	e020n40.open("E020N40.DEM", ios::binary);
	e060n40.open("E060N40.DEM", ios::binary);
	e100n40.open("E100N40.DEM", ios::binary);
	e140n40.open("E140N40.DEM", ios::binary);

	w180s10.open("W180S10.DEM", ios::binary);
	w140s10.open("W140S10.DEM", ios::binary);
	w100s10.open("W100S10.DEM", ios::binary);
	w060s10.open("W060S10.DEM", ios::binary);
	w020s10.open("W020S10.DEM", ios::binary);
	e020s10.open("E020S10.DEM", ios::binary);
	e060s10.open("E060S10.DEM", ios::binary);
	e100s10.open("E100S10.DEM", ios::binary);
	e140s10.open("E140S10.DEM", ios::binary);

	w180s60.open("W180S60.DEM", ios::binary);
	w120s60.open("W120S60.DEM", ios::binary);
	w060s60.open("W060S60.DEM", ios::binary);
	w000s60.open("W000S60.DEM", ios::binary);
	e060s60.open("E060S60.DEM", ios::binary);
	e120s60.open("E120S60.DEM", ios::binary);

	world.open("WORLD.DEM", ios::binary);

	if (!w180n90)
		FAILED = true;
	if (!w140n90)
		FAILED = true;
	if (!w100n90)
		FAILED = true;
	if (!w060n90)
		FAILED = true;
	if (!w020n90)
		FAILED = true;
	if (!e020n90)
		FAILED = true;
	if (!e060n90)
		FAILED = true;
	if (!e100n90)
		FAILED = true;
	if (!e140n90)
		FAILED = true;

	if (!w180n40)
		FAILED = true;
	if (!w140n40)
		FAILED = true;
	if (!w100n40)
		FAILED = true;
	if (!w060n40)
		FAILED = true;
	if (!w020n40)
		FAILED = true;
	if (!e020n40)
		FAILED = true;
	if (!e060n40)
		FAILED = true;
	if (!e100n40)
		FAILED = true;
	if (!e140n40)
		FAILED = true;

	if (!w180s10)
		FAILED = true;
	if (!w140s10)
		FAILED = true;
	if (!w100s10)
		FAILED = true;
	if (!w060s10)
		FAILED = true;
	if (!w020s10)
		FAILED = true;
	if (!e020s10)
		FAILED = true;
	if (!e060s10)
		FAILED = true;
	if (!e100s10)
		FAILED = true;
	if (!e140s10)
		FAILED = true;

	if (!w180s60)
		FAILED = true;
	if (!w120s60)
		FAILED = true;
	if (!w060s60)
		FAILED = true;
	if (!w000s60)
		FAILED = true;
	if (!e060s60)
		FAILED = true;
	if (!e120s60)
		FAILED = true;

	if (FAILED) {
		cerr << endl <<
		    "At least one of the GTOPO30 tiles is missing... merging failed"
		    << endl;
		exit(1);
	}

	if (!world) {
		cerr << endl <<
		    "failed to open output file... merging failed" << endl;
		exit(1);
	}

	cout << "OK" << endl;

	for (i = 0; i < 6000; i++) {	// all rows

		cout << "Writing part 1 (of 4) " << 6000 -
		    i << "    " << '\r' << flush;

		for (j = 0; j < 4800; j++) {	// all columns
			tmp = w180n90.get();
			world.put(tmp);
			tmp = w180n90.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = w140n90.get();
			world.put(tmp);
			tmp = w140n90.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = w100n90.get();
			world.put(tmp);
			tmp = w100n90.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = w060n90.get();
			world.put(tmp);
			tmp = w060n90.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = w020n90.get();
			world.put(tmp);
			tmp = w020n90.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = e020n90.get();
			world.put(tmp);
			tmp = e020n90.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = e060n90.get();
			world.put(tmp);
			tmp = e060n90.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = e100n90.get();
			world.put(tmp);
			tmp = e100n90.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = e140n90.get();
			world.put(tmp);
			tmp = e140n90.get();
			world.put(tmp);
		}

		if (!world) {
			cerr << endl << "writing error occured!" << endl;
			exit(1);
		}
	}

	cout << endl;

	for (i = 0; i < 6000; i++) {	// all rows

		cout << "Writing part 2 (of 4) " << 6000 -
		    i << "    " << '\r' << flush;

		for (j = 0; j < 4800; j++) {	// all columns
			tmp = w180n40.get();
			world.put(tmp);
			tmp = w180n40.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = w140n40.get();
			world.put(tmp);
			tmp = w140n40.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = w100n40.get();
			world.put(tmp);
			tmp = w100n40.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = w060n40.get();
			world.put(tmp);
			tmp = w060n40.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = w020n40.get();
			world.put(tmp);
			tmp = w020n40.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = e020n40.get();
			world.put(tmp);
			tmp = e020n40.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = e060n40.get();
			world.put(tmp);
			tmp = e060n40.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = e100n40.get();
			world.put(tmp);
			tmp = e100n40.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = e140n40.get();
			world.put(tmp);
			tmp = e140n40.get();
			world.put(tmp);
		}

		if (!world) {
			cerr << endl << "writing error occured!" << endl;
			exit(1);
		}
	}

	cout << endl;

	for (i = 0; i < 6000; i++) {	// all rows

		cout << "Writing part 3 (of 4) " << 6000 -
		    i << "    " << '\r' << flush;

		for (j = 0; j < 4800; j++) {	// all columns
			tmp = w180s10.get();
			world.put(tmp);
			tmp = w180s10.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = w140s10.get();
			world.put(tmp);
			tmp = w140s10.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = w100s10.get();
			world.put(tmp);
			tmp = w100s10.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = w060s10.get();
			world.put(tmp);
			tmp = w060s10.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = w020s10.get();
			world.put(tmp);
			tmp = w020s10.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = e020s10.get();
			world.put(tmp);
			tmp = e020s10.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = e060s10.get();
			world.put(tmp);
			tmp = e060s10.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = e100s10.get();
			world.put(tmp);
			tmp = e100s10.get();
			world.put(tmp);
		}
		for (j = 0; j < 4800; j++) {	// all columns
			tmp = e140s10.get();
			world.put(tmp);
			tmp = e140s10.get();
			world.put(tmp);
		}

		if (!world) {
			cerr << endl << "writing error occured!" << endl;
			exit(1);
		}

	}

	cout << endl;

	for (i = 0; i < 3600; i++) {	// all rows

		cout << "Writing part 4 (of 4) " << 3600 -
		    i << "    " << '\r' << flush;

		for (j = 0; j < 7200; j++) {	// all columns
			tmp = w180s60.get();
			world.put(tmp);
			tmp = w180s60.get();
			world.put(tmp);
		}
		for (j = 0; j < 7200; j++) {	// all columns
			tmp = w120s60.get();
			world.put(tmp);
			tmp = w120s60.get();
			world.put(tmp);
		}
		for (j = 0; j < 7200; j++) {	// all columns
			tmp = w060s60.get();
			world.put(tmp);
			tmp = w060s60.get();
			world.put(tmp);
		}
		for (j = 0; j < 7200; j++) {	// all columns
			tmp = w000s60.get();
			world.put(tmp);
			tmp = w000s60.get();
			world.put(tmp);
		}
		for (j = 0; j < 7200; j++) {	// all columns
			tmp = e060s60.get();
			world.put(tmp);
			tmp = e060s60.get();
			world.put(tmp);
		}
		for (j = 0; j < 7200; j++) {	// all columns
			tmp = e120s60.get();
			world.put(tmp);
			tmp = e120s60.get();
			world.put(tmp);
		}

		if (!world) {
			cerr << endl << "writing error occured!" << endl;
			exit(1);
		}
	}

	cout << endl << "finished." << endl;

}
