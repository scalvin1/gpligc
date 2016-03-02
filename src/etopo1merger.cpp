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

/*
This program merges a WORLD.DEM (created with createworld from the GTOPO30-tiles)
and an etopo1.i2 into a WORLD3.DEM. (world3 is the new version with water-flag!)
(WORLD2 was without waterflag)
All files have 2 byte integer-values in big-endian order.

Another option is to prepare subsets of GTOPO30 WORLDx.DEM file
--latmax --latmin --lonmax --lonmin --out file

*/

#include <fstream>
#include <iostream>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <cstdlib>

//const double GTOPO30GRID = 0.00833333333333333;
//const double ETOPO2GRID = 0.03333333333333333;
const double GTOPO30GRID = 1.0/120.0;
const double ETOPO1GRID = 1.0/60.0;

using namespace std;
//const string rcsid_etopo1merger_cpp =
//    "$Id: etopo1merger.cpp 3 2014-07-31 09:59:20Z kruegerh $";

short int etopo_array[10801][21601];

// This function uses bilinear interpolation to map values
// from etopo1 to gtopo30
short int getalt(double lat, double lon, int debug)
{
	int steps_lat, steps_lon;
	short int alt, h1, h2, h3, h4, max, min;
	double lat_frac, lon_frac, tmp;

	double dem_lon_min = -180.0;
	double dem_lat_max = 90.0;

	// Layout:
	// h3  h4
	// h1  h2
	// lat_frac is the remaining fraction  on the side h1 towards h3
	// lon_frac is the remaining fraction  on the side h1 towards h2

	steps_lat = (int) (1.0 + (dem_lat_max - lat) / ETOPO1GRID);
	steps_lon = (int) ((lon - dem_lon_min) / ETOPO1GRID);

	lat_frac = -1.0 * (-steps_lat * ETOPO1GRID + dem_lat_max - lat) / ETOPO1GRID;
	lon_frac = (-steps_lon * ETOPO1GRID - dem_lon_min + lon) / ETOPO1GRID;
	h1 = etopo_array[steps_lat][steps_lon];

	if(lat_frac < 0 || lon_frac < 0 || debug) printf("latfrac = %f, lonfrac = %f\n", lat_frac, lon_frac);

	steps_lat = (int) (1.0 + (dem_lat_max - lat) / ETOPO1GRID);
	steps_lon = (int) (1.0 + (lon - dem_lon_min) / ETOPO1GRID);
	h2 = etopo_array[steps_lat][steps_lon];

	steps_lat = (int) ((dem_lat_max - lat) / ETOPO1GRID);
	steps_lon = (int) ((lon - dem_lon_min) / ETOPO1GRID);
	h3 = etopo_array[steps_lat][steps_lon];

	steps_lat = (int) ((dem_lat_max - lat) / ETOPO1GRID);
	steps_lon = (int) (1.0 + (lon - dem_lon_min) / ETOPO1GRID);
	h4 = etopo_array[steps_lat][steps_lon];

	tmp = ((1.0 - lon_frac) * (1.0 - lat_frac) * (double) h1
			 + lon_frac * (1.0 - lat_frac) * (double) h2
			 + (1.0 - lon_frac) * lat_frac * (double) h3
			 + lat_frac * lon_frac * (double) h4);

	tmp = (double)((float) tmp); //This is in order to achieve identical results on x86_64 and i386
	// Looks like Voodoo, but it can be understood: We are always sitting right on the fence
	// rounding this way or that. If x86_64 works with different internal precision, then the
	// rounding outcome in the cast operation (below) can go different ways...

	if(tmp > 0.0){
		alt = (short int) (0.5 + tmp);
	} else {
		alt = (short int) (-0.5 + tmp);
	}

	// Sanity checks
	max = -10000;
	min = 16000;
	if(h1 > max) max = h1;
	if(h2 > max) max = h2;
	if(h3 > max) max = h3;
	if(h4 > max) max = h4;
	if(h1 < min) min = h1;
	if(h2 < min) min = h2;
	if(h3 < min) min = h3;
	if(h4 < min) min = h4;
	if(alt > max || alt < min || debug){
		printf("Interpolation: h1 = %d, h2 = %d, h3 = %d, h4 = %d, alt = %d (%f), lat_frac = %f, lon_frac = %f\n",
			h1, h2, h3, h4, alt, tmp,lat_frac, lon_frac);
//		printf("%lf\n", tmp);
	}
	// end sanity checks

	return alt;
}


void failed(void)
{
	cout << "Some of the input files are missing, "
	    << "or we are unable to write to the output file."
	    << endl
	    <<
	    "The files WORLD.DEM should be in this directory. And for etopo1merging etopo1.i2!"
	    << endl;
	exit(1);
}

//---------------------------------------------

int main(int argc, char *argv[])
{
	bool ETOPOMERGING = true;
	double minlat, maxlat, minlon, maxlon;
	string filename;
	string source = "WORLD.DEM";

	// parsing of options...
	for (int i = 0; i < argc; i++) {

		if (strcmp(argv[i], "--latmin") == 0)
			sscanf(argv[i + 1], "%lf", &minlat);
		if (strcmp(argv[i], "--lonmin") == 0)
			sscanf(argv[i + 1], "%lf", &minlon);
		if (strcmp(argv[i], "--latmax") == 0)
			sscanf(argv[i + 1], "%lf", &maxlat);
		if (strcmp(argv[i], "--lonmax") == 0)
                                                sscanf(argv[i + 1], "%lf", &maxlon);
		if (strcmp(argv[i], "--out") == 0) {
			filename = argv[i + 1];
			ETOPOMERGING=false;
		}
		if (strcmp(argv[i], "--source") == 0)
			source = argv[i+1];
	}

	if (sizeof(short int) != 2) {
		cerr << "sizeof(short int) " << sizeof(short int) << endl;
		cerr << "Sorry, can't help you here. Use the source, Luke."
		    << endl;
		exit(1);
	}

	bool FAILED = false;

	ifstream etopo1;
	ifstream gtopo30;
	ofstream world3;
	ofstream subset;

	if (ETOPOMERGING) {
		cout << "Trying to read etopo1.i2 ...   ";
		etopo1.open("etopo1_ice_g_i2.bin", ios::binary);
		if (!etopo1) {
			cout << "failed." << endl;
			FAILED = true;
		} else {
			cout << "ok." << endl;
		}
	}

	cout << "trying to read " << source << "  ...   ";
	gtopo30.open(source.c_str(), ios::binary);
	if (!gtopo30) {
		cout << "failed" << endl;
		FAILED = true;
	} else {
		cout << "ok" << endl;
	}

	if (!ETOPOMERGING) {
		cout  << "Trying to open outputfile: " << filename << " ...  ";
		subset.open(filename.c_str(), ios::binary);
		if (!subset) {
			cout << "failed." << endl;
			FAILED = true;
		} else {
			cout << "ok." << endl;
		}
	}

	if (ETOPOMERGING) {
		cout << "Trying to open WORLD3.DEM for writing...   ";
		world3.open("SRTM30_ETOPO1.DEM", ios::binary);
		if (!world3) {
			cout << "failed." << endl;
			FAILED = true;
		} else {
			cout << "ok." << endl;
		}
	}

	if (FAILED) failed();

	int row, col;
	short int alt;
	int i1, i2;
	int offset = 0;

// main part of etopo1merging -----------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------
// now read etopo into memory!
	if (ETOPOMERGING) {
		cout << "Reading etopo1 into RAM" << endl;
		for (row = 0; row < 10801; row++) {
			cout << (10801 - row) << "     " << '\r' << flush;
			for (col = 0; col < 21601; col++) {
				i1 = etopo1.get();
				i2 = etopo1.get();
				etopo_array[row][col] = 256 * i2 + i1; //LSB first in etopo1!

				}
		}

		cout <<  "Finished." << endl;

		cout <<
		"Merging and interpolating... (this may take a little time)" <<
		endl;

		for (row = 0; row < 21600; row++) {
			cout << (21600 - row) << "     " << '\r' << flush;
			for (col = 0; col < 43200; col++) {
				i1 = gtopo30.get();
				i2 = gtopo30.get();
				alt = 256 * i1 + i2;
				if (alt == -9999) {	// this is ocean surface, get bathymetry from etopo1
					double lat = 90.0 - (GTOPO30GRID * row);
					double lon = -180.0 + (GTOPO30GRID * col);
					alt = getalt(lat, lon, 0);

					// Sanity checks
					//if(50473 == offset) {
					//	getalt(lat, lon, 1);
					////	exit(1);
					//}

					// we should not loose the information, that this is covered by water!
					// low areas might have negative elevations!
					// the deepest place on earth is heigher than 16384 (2^14)
					// so we will set the 2^14- bit to indicate water coverage

					//I havent understand the bit-logic of negative short int..

					// due to interpolation in the worse resolutioned
					// model we might cat positive values :(
					// they will be forced to 0
					if (alt >= 0) alt =0;
					alt -= 16384;
				}

				// split alt in big-endian 2 byte and write..
				i2 = (short int) (alt & 255);
				i1 = (short int) (alt & 65280) >> 8;

				// Sanity checks
				//if(50473 == offset) {
				//	printf("i1 = %d, i2 = %d\n", i1, i2);
				//}
				world3.put(i1);
				if (!world3) {
					cerr <<
					"An error occured while writing the output file... (no disc space?)"
					<< endl;
					exit(1);
				}

				world3.put(i2);
				if (!world3) {
					cerr <<
					"An error occured while writing the output file... (no disc space?)"
					<< endl;
					exit(1);
				}
				offset++;
			}
		}
	}

// main part of subset creating .-------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------

	if (!ETOPOMERGING) {
		cout << "Subset of  " << source << " will be created" << endl;
		cout << "Selected limits:" << endl <<
		 "Max Lat: " << maxlat << endl <<
		 "Min Lat: " << minlat << endl <<
		 "Max Lon: " << maxlon << endl <<
		 "Min Lon: " << minlon << endl << endl;

                double grid_lat =  1.0 /120.0;
                double grid_lon =  1.0 / 120.0;
                int downscalefactor = 1;

		int upsteps = int (((90 - maxlat) / grid_lat)+0.5);

		int downsteps = int (((90 - minlat) / grid_lat)+0.5);

		int leftsteps = int (((minlon - (-180)) / grid_lon)+0.5);

		int rightsteps = int (((maxlon - (-180)) / grid_lon)+0.5);

		int colums_lon = 43200;

                double DEM_LAT_MAX = 90.0 -(upsteps * grid_lat);
                double DEM_LAT_MIN = 90.0 - (downsteps * grid_lat);
                double DEM_LON_MAX = -180.0 + (rightsteps * grid_lon);
                double DEM_LON_MIN = -180.0 + (leftsteps * grid_lon);

                int steps_lat = downsteps - upsteps ; //+ 2 *downscalefactor;
		int steps_lon = rightsteps - leftsteps; // + 2 * downscalefactor;

                cout << filename << "  will be: " << (steps_lat) * (steps_lon  )*2 << " bytes." << endl << endl;

                cout << "DEM_FILE    " << filename << endl <<
                  "DEM_LAT_MAX    " << DEM_LAT_MAX << endl <<
                  "DEM_LAT_MIN    " << DEM_LAT_MIN << endl <<
		  "DEM_LON_MAX    " << DEM_LON_MAX << endl <<
		  "DEM_LON_MIN    " << DEM_LON_MIN << endl <<
		  "DEM_ROWS       " << steps_lat   <<  endl <<
		  "DEM_COLUMNS    " << steps_lon  << endl;

		int counter = steps_lat;

		for (int zeile = 0; zeile < steps_lat; zeile += downscalefactor) {
			cout << counter-- << "         " << '\r' << flush;
			for (int spalte = 0; spalte < steps_lon; spalte += downscalefactor) {

				// fuer 1 byte daten kommentiert ///* */ !?

				///*
				int verschieber =
				    (colums_lon * upsteps + zeile * colums_lon +
				     leftsteps + spalte) * 2;
				//*/

				/*
				   int
				   verschieber = (colums_lon * upsteps + zeile * colums_lon + leftsteps + spalte);
				 */

				//cout << "vershieber :  " << verschieber << endl;
				gtopo30.seekg(verschieber, ios::beg);

				///*
				int i1 = gtopo30.get();
				//*/

				int i2 = gtopo30.get();

				subset.put(i1);
				subset.put(i2);
			}
		}
	}
	cout << "Finished. Please check the output file." << endl;
}
