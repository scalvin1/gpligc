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

hacked from etopo2merger to merge more general
example:
./merger --lonmin 5.63833333310781 --latmax 49.17166666637981 --grid 0.000833333 --file ../../../srtm3/20367254/20367254.bil --rows 7151 --columns 14254 --sgrid 0.008333333333 --slatmax 90 --slonmin -180 --scolumns 43200 --srows 21600 --source /usr/local/share/gpligc/data/dem/WORLD3_SRTM30_GTOPO30_ETOPO2.DEM --out test.dem

*/

#include <fstream>
#include <iostream>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

using namespace std;
//const string rcsid_etopo2merger_cpp =
//    "$Id: merger.cpp 3 2014-07-31 09:59:20Z kruegerh $";

float  smaxlat, sminlon, maxlat, minlon, sgrid, grid;
string srcfile;
string infile;
string outfile;
int rows, srows, columns, scolumns;

int icounter =0;

ifstream in;
ifstream src;
ofstream out;

bool BIGENDIAN=false;
bool SBIGENDIAN=true;

short int
getalt(double _lat,double _lon)
{


	int steps_lat, steps_lon, i1, i2, verschieber;
	short int h1, h2, h3, h4;
	short int watercounter;
	short int seafloorcounter;
	vector <short int> neighbours;
	float lat_frac, lon_frac;




		watercounter = 0;
		seafloorcounter = 0;

		//second_try:


		steps_lat = (int) (1.0 +
				   (smaxlat -
				    _lat) / sgrid
		    );


		steps_lon = (int) ((_lon -
				    sminlon) /
				   sgrid
		    );


		lat_frac =
		    fabs(-steps_lat * sgrid +
			 smaxlat - _lat)
		    / sgrid;


		lon_frac =
		    fabs(-steps_lon * sgrid -
			 sminlon + _lon)
		    / sgrid;


		//cout << "lat: " << lat_frac << "      lon: " << lon_frac << endl;

		verschieber =
		    (steps_lat * scolumns + steps_lon) * 2;


		src.seekg(verschieber, ios::beg);



		i1 = src.get();
		i2 = src.get();

		if (SBIGENDIAN) {
				h1 = 256 * i1 + i2;
			} else {
				h1 = 256 * i2 + i1;
			}



		neighbours.push_back(h1);
		if (h1 == -9999)// || h1 <= -16384)
			watercounter++; //h1 = 0;	// in this case this is OK warum nicht 0?
		if (h1 <= -16384)
			seafloorcounter++;


		steps_lat = (int) (1.0 +
				   (smaxlat -
				    _lat) /
				   sgrid);


		steps_lon = (int) (1.0 +
				   (_lon -
				    sminlon) /
				   sgrid);

		verschieber =
		    (steps_lat * scolumns + steps_lon) * 2;


		src.seekg(verschieber, ios::beg);


		i1 = src.get();
		i2 = src.get();

		if (SBIGENDIAN) {
				h2 = 256 * i1 + i2;
			} else {
				h2 = 256 * i2 + i1;
			}
		//h2 = 256 * i1 + i2;




		neighbours.push_back(h2);
		if (h2 == -9999)// || h2 <= -16384)
			watercounter++;//h2 = 0;
		if (h2 <= -16384)
			seafloorcounter++;


		steps_lat = (int) ((smaxlat -
				    _lat) /
				   sgrid);


		steps_lon = (int) ((_lon -
				    sminlon) /
				   sgrid);

		verschieber =
		    (steps_lat * scolumns + steps_lon) * 2;

		src.seekg(verschieber, ios::beg);


		i1 = src.get();
		i2 = src.get();
		if (SBIGENDIAN) {
				h3 = 256 * i1 + i2;
			} else {
				h3 = 256 * i2 + i1;
			}
		//h3 = 256 * i1 + i2;



		neighbours.push_back(h3);
		if (h3 == -9999)// || h3 <= -16384)
			watercounter++;//h3 = 0;
		if (h3 <= -16384)
			seafloorcounter++;

		steps_lat = (int) ((smaxlat -
				    _lat) /
				   sgrid);


		steps_lon = (int) (1.0 +
				   (_lon -
				    sminlon) /
				   sgrid);

		verschieber =
		    (steps_lat * scolumns + steps_lon) * 2;


		src.seekg(verschieber, ios::beg);


		i1 = src.get();
		i2 = src.get();
		if (SBIGENDIAN) {
				h4 = 256 * i1 + i2;
			} else {
				h4 = 256 * i2 + i1;
			}
		//h4 = 256 * i1 + i2;


		neighbours.push_back(h4);
		if (h4 == -9999) // || h4 <= -16384)
			watercounter++;//h4 = 0;
		if (h4 <= -16384)
			seafloorcounter++;


		// this implements bilinear-interpolation!
		short int alt;
		alt = (int) (
			((1.0 - lon_frac) * (1.0 - lat_frac) * (double) h1 +
			   lon_frac * (1.0 - lat_frac) * (double) h2 +
			   (1.0 - lon_frac) * lat_frac * (double) h3 +
			   lat_frac * lon_frac * (double) h4) +0.5);
			   // now rounded :-) rounding error?!

		// if there is water
		if (watercounter == 1 || watercounter == 2) {
			alt = 0;
		}

		if (watercounter > 2)
			alt = -22222;

		// if one or two of the neighbours are seafloor, then we set the elevation to 0 (coast)
		if (seafloorcounter > 0 ) {
			if (seafloorcounter == 1 || seafloorcounter == 2)
				alt = 0;

			//if three neighbours are seafloor, then we set elevation to -16384 (sea with 0)
			// if -16383  => workaround... rounding error in interpolation?
			if (seafloorcounter == 3 || alt == -16383 )

				alt = -16384;
		// if 4x seafloor, then its ok
		}



		// Add warning signs! if invalid elevations read
		// this only works for data in srtm-3 from seamless server, which uses 0 for invalid data (stupid!)
		/*int neg_counter=0;
		if (_conf->GETELEV_WARN.get()) {
			// count neighbours equal zero!
			for(unsigned int a=0; a<= neighbours.size()-1; a++) {
				if (neighbours[a] <= 0) {
					neg_counter--;
				}
			}
			// we return a negative integer, indicating how many points are invalid
			if (neg_counter < 0) {(*_gnd_elev)[i] = neg_counter;}
		}

		// simple preliminary error-guess
		if (_conf->GETELEV_ERROR.get()) {
			int min=10000;
			int max=0;
			for(unsigned int a=0; a<= neighbours.size()-1; a++) {
				if (neighbours[a] < min)
					min = neighbours[a];
				if (neighbours[a] > max)
					max = neighbours[a];
			}

			// an error is added to the elevation array
			if (neg_counter < 0) {
					(*_gnd_elev).push_back(max);
					if (max-min != max) {
						cerr << "AAAAAAAAAAAAAAAALLLLLLLLLLLAAAAAAAAAARRRRRRRMMMMMMMM!!"<<endl;
					}

				}else{
					(*_gnd_elev).push_back(max-min);
				}
		}

		*/



	//src.close();
	//if (_conf->DEBUG.get() ) {cout <<  "get_gnd_elevations finished " <<  endl << flush ;}
	return alt;
}


void failed(void)
{
	cout << "Some of the files are missing, "
	    << "or we are unable to write to the output file."
	    << endl;
	exit(1);
}

//---------------------------------------------

int main(int argc, char *argv[])
{



	// parsing of options...
	for (int i = 0; i < argc; i++) {

		if (strcmp(argv[i], "--slatmax") == 0)
			sscanf(argv[i + 1], "%f", &smaxlat);
		if (strcmp(argv[i], "--slonmin") == 0)
			sscanf(argv[i + 1], "%f", &sminlon);
		if (strcmp(argv[i], "--latmax") == 0)
			sscanf(argv[i + 1], "%f", &maxlat);
		if (strcmp(argv[i], "--lonmin") == 0)
			sscanf(argv[i + 1], "%f", &minlon);
		if (strcmp(argv[i], "--out") == 0)
			outfile = argv[i + 1];
		if (strcmp(argv[i], "--source") == 0)
			srcfile = argv[i+1];
		if (strcmp(argv[i], "--file") == 0)
			infile = argv[i+1];
		if (strcmp(argv[i], "--sgrid") == 0)
			sscanf(argv[i + 1], "%f", &sgrid);
			//sgrid = argv[i+1];
		if (strcmp(argv[i], "--grid") == 0)
			sscanf(argv[i + 1], "%f", &grid);
			//grid = argv[i+1];
		if (strcmp(argv[i], "--rows") == 0)
			sscanf(argv[i + 1], "%d", &rows);
			//rows = argv[i+1];
		if (strcmp(argv[i], "--srows") == 0)
			sscanf(argv[i + 1], "%d", &srows);
			//srows = argv[i+1];
		if (strcmp(argv[i], "--scolumns") == 0)
			sscanf(argv[i + 1], "%d", &scolumns);
			//scolumns = argv[i+1];
		if (strcmp(argv[i], "--columns") == 0)
			sscanf(argv[i + 1], "%d", &columns);
			//columns = argv[i+1];

	}

	if (sizeof(short int) != 2) {
		cerr << "sizeof(short int) " << sizeof(short int) << endl;
		cerr << "Sorry, can't help you here. Use the source, Luke."
		    << endl;
		exit(1);
	}

	bool FAILED = false;


	cout << "Trying to read: "<< infile <<".....";
		in.open(infile.c_str(), ios::binary);
		if (!in) {
			cout << "failed." << endl;
			FAILED = true;
		} else {
			cout << "ok." << endl;
		}


	cout << "trying to read " << srcfile << "  ...   ";
	src.open(srcfile.c_str(), ios::binary);
	if (!src) {
		cout << "failed" << endl;
		FAILED = true;
	} else {
		cout << "ok" << endl;
	}

	cout  << "Trying to open outputfile: " << outfile << " ...  ";
	out.open(outfile.c_str(), ios::binary);
	if (!out) {
		cout << "failed." << endl;
		FAILED = true;
	} else {
		cout << "ok." << endl;
	}

	if (FAILED)
		failed();

	cout << "File to be improved by interpolation: "<< infile << endl
		<< "rows: " <<rows<<"  columns: "<<columns<<endl<<"latmax :"<<maxlat<<"  lonmin: "<<minlon
		<< "  grid: "<<grid <<endl<<"output: "<< outfile<<endl;
	cout << "Interpolation source: "<< srcfile << endl
		<< "rows: " <<srows<<"  columns: "<<scolumns<<endl<<"latmax :"<<smaxlat<<"  lonmin: "<<sminlon
		<< "  grid: "<<sgrid <<endl;


// main part of etopo2merging -----------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------
// now read etopo into memory!

//		cout << "Reading etopo2 into RAM" << endl;
		for (int row = 0; row < rows; row++) {
			cout << (rows - row) << "     " << '\r' << flush;
			for (int col = 0; col < columns; col++) {
				int i1 = in.get();
				int i2 = in.get();
				short int salt;  //source alt

				//read in given (BIGENDIAN) endianess
				if (BIGENDIAN) {
					salt = 256 * i1 + i2;
				} else {
					salt = 256 * i2 + i1;
				}

				if (salt == 0) {	// undef interpolate from source
					double lat = maxlat - (grid * row);
					double lon = minlon + (grid * col);
					salt = getalt(lat, lon);

					//cout << salt << endl;
					icounter++;

				}


				i2 = (short int) (salt & 255);
				i1 = (short int) (salt & 65280) >> 8;
				out.put(i1);
				if (!out) {
					cerr <<
					"An error occured while writing the output file... (no disc space?)"
					<< endl;
					exit(1);
				}

				out.put(i2);
				if (!out) {
					cerr <<
					"An error occured while writing the output file... (no disc space?)"
					<< endl;
					exit(1);
				}
			}
		}

		cout <<  "Finished." << endl;

		cout << (rows * columns) << " DEM data points handled."<<endl << icounter << " interpolated" << endl;
		float per = ( (float)icounter / ((float)rows*(float)columns))  *100.0;
		cout << per << " %" << endl;
		cout << "Finished. Please check the output file." << endl;
}
