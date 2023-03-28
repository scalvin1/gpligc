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
//#define _FILE_OFFSET_BITS 64  //does not seem to be needed here-JMWK

#include "landscape.h"
#include "oglexfunc.h"
#include "GLexplorer.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <string>

#ifndef __OSX__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif


//const std::string rcsid_landscape_cpp =
//    "$Id: landscape.cpp 3 2014-07-31 09:59:20Z kruegerh $";
//const std::string rcsid_landscape_h = LANDSCAPE_H;


Landscape::Landscape()
{
	CLEARFLAG = true;
	CLEARFLAG_SURFACE = false;
}


// read in the dem data... be sure that max_lon, min_lon, max_lat, min_lat is set before...
// the arguments given to this function are the properties of the DEM-File!!!
int Landscape::readDEM(string filename, int rows_lat, int colums_lon,
			double demlatmin, double demlatmax,
			double demlonmin, double demlonmax, bool FAST)
{

	Timecheck landtime("Landscape::readDEM", conf_pointer);

	// in fast mode (only for getting max and min alt)
	// no arrays need to be cleared in destructor...
	if (FAST)
		CLEARFLAG = false;



	int _max = 90;
	if (proj_pointer->get_projection() == PROJ_CYL_MERCATOR)
		_max = MERCATOR_MAX_LAT;

	if (max_lat > _max) {
		cerr << "north border set to " << _max << " north" <<
		    endl;
		max_lat = _max;
	}

	if (min_lat < -_max) {
		cerr << "south border set to " << _max << " south" <<
		    endl;
		min_lat = -_max;
	}


	// fuer WORLDDEM circumvent 180 -180 limits!?????? NNNNNEEEEEEEEDDDDS TO BE DONE!!!!!!!!!!!!

	// check, if the requested borders partially outside of DEM, then set to DEMs limits...
	if (max_lat > demlatmax) {
		max_lat = demlatmax;
		cerr << "maximum latitude exceeds available DEM. set to DEMs max!" << endl;
	}

	if ( min_lat < demlatmin) {
		min_lat = demlatmin;
		cerr << "minimum latitude exceeds available DEM. set to DEMs min!" << endl;
	}

	if (  max_lon > demlonmax && demlonmax != 180) {
		max_lon = demlonmax;
		cerr << "maximum longitude exceeds available DEM. set to DEMs max!" << endl;
	}

	if  (min_lon < demlonmin && demlonmin != -180) {
		min_lon = demlonmin;
		cerr << "minimum longitude exceeds available DEM. set to DEMs min!" << endl;
	}

	// Check if the requested are completely outside of DEM, then exit
	if (max_lat <= demlatmin || min_lat >= demlatmax || max_lon <= demlonmin || min_lon >= demlonmax) {
		cerr << "Area outside of available DEM was requested..." << endl;
		exit (1);
	}


	ifstream demfile;
	demfile.open(filename.c_str(), ios::binary);
	if (!demfile) {
		cerr << "Error opening DEM file: >" << filename << "<" <<
		    endl;
		exit(1);
	}


	// check size of demfile:
	demfile.seekg(0, ios::end);
	uint64_t fsize = demfile.tellg();
	if (fsize != (uint64_t) rows_lat * colums_lon * 2 ) {
		cerr << "Size of " << filename << " " << fsize << " doesnt agree with definition in configuration file " << 
			(uint64_t) rows_lat * colums_lon * 2 << endl;
		exit(1);
	}
	demfile.seekg(0, ios::beg);


calc_start:
	short int h;
	int upsteps = int (floor((demlatmax - max_lat) / grid_lat));
	int downsteps = int (ceil((demlatmax - min_lat) / grid_lat));
	int leftsteps = int (floor((min_lon - demlonmin) / grid_lon));
	int rightsteps = int (ceil((max_lon - demlonmin) / grid_lon));


	// why 2* downscalefactor?
	// changed to 1*downscalefact on 20100826, to check limits for landscape rebuild in gpsd mode
	// seems to be good. check with maps needed (and with dsf) !
	steps_lat = downsteps - upsteps + 1 * downscalefactor;
	steps_lon = rightsteps - leftsteps + 1 * downscalefactor;

	if (conf_pointer->DEBUG.get())
		cerr << "stepslta / stepslon" << steps_lat << " / " << steps_lon << endl;

	// if upscalefactor is given ( >1), multiply steps with upscalefac
	if (conf_pointer->getupscalefactor() > 1) {
		steps_lat = steps_lat * conf_pointer->getupscalefactor();
		steps_lon = steps_lon * conf_pointer->getupscalefactor();
	}


	// now we calculate the number of triangles before reading data:
	// so that we can prevent DoS in online functions!
	int latsteps = (int) (((float)    (steps_lat -
						3 * downscalefactor)/
					       (float) downscalefactor) +
					      0.5);
	int lonsteps = (int) (((float)    (steps_lon -
						2 * downscalefactor - 1) /
					       (float) downscalefactor) +
					      0.5);

	int triangles = latsteps * lonsteps * 2;

	if (conf_pointer->VERBOSE.get()) {
		cout << "Triangles to be used: " << (double) triangles << "   Maximum allowed: " << conf_pointer->MAXTRIANGLES.get() << endl;
		if (conf_pointer->QUADS.get())
			cout << "Quadrilaterals are used instead of triangles. Above number is not correct!" <<endl;
	}

	if ((float) triangles > conf_pointer->MAXTRIANGLES.get()) {
		if (conf_pointer->VERBOSE.get())
		cerr << "Maximum allowed triangles exceeded !!!" << endl << flush;

		if (!conf_pointer->AUTOREDUCE.get()) {
			if (conf_pointer->VERBOSE.get())
			cerr << "Terrain will be switched off!" << endl;
			conf_pointer->LANDSCAPE.off();
			CLEARFLAG = false;
			demfile.close();
			return (99);
		} else {
			if (conf_pointer->VERBOSE.get())
			cerr << "Autoreduce DEM resolution..." << endl;
			if (conf_pointer->getupscalefactor() > 1) {
				conf_pointer->setupscalefactor(conf_pointer->getupscalefactor()-1);
				if (conf_pointer->VERBOSE.get())
				cerr << "Upscalefactor reduced to " << conf_pointer->getupscalefactor() << endl;
			} else {
				// stupidly I have 2 doenscalefactors!!! one in conf and one on landscape :-(
				conf_pointer->setdownscalefactor(conf_pointer->getdownscalefactor()+1);
				downscalefactor++;
				if (conf_pointer->VERBOSE.get())
				cerr << "Downscalefactor increased to " << conf_pointer->getdownscalefactor() << endl;
			}
			goto calc_start;
		}

	}


                // the surface altitude array is needed in fast mode too

		CLEARFLAG_SURFACE = true;

                surface_h = new short int *[steps_lat];
                for (int i = 0; i < steps_lat; i += downscalefactor) {
				surface_h[i] = new short int[steps_lon];
	}

	// these are not required in fast mode
                if (!FAST) {
			surface_x = new float *[steps_lat];
			surface_y = new float *[steps_lat];

			for (int i = 0; i < steps_lat; i += downscalefactor) {
				surface_x[i] = new float[steps_lon];
				surface_y[i] = new float[steps_lon];
			}
	}

	vector<int>altitudes;
	altitudes.reserve(steps_lat*steps_lon);
	vector<float>latitudes;
	latitudes.reserve(steps_lat*steps_lon);
	vector<float>longitudes;
	longitudes.reserve(steps_lat*steps_lon);

	landtime.checknow("beforereadingDEM");

	for (int zeile = 0; zeile < steps_lat; zeile += downscalefactor) {
		for (int spalte = 0; spalte < steps_lon;
		     spalte += downscalefactor) {



			if (conf_pointer->getupscalefactor() == 1) {
			uint64_t verschieber =
			    ((uint64_t) colums_lon * upsteps + (uint64_t) zeile * colums_lon +
			     leftsteps + spalte) * 2;

			demfile.seekg(verschieber, ios::beg);

			///*
			int i1 = demfile.get();
			//*/

			int i2 = demfile.get();

			if (conf_pointer->BIGENDIAN.get()) {
				h = 256 * i1 + i2;
			} else {
				h = 256 * i2 + i1;
			}

			if (conf_pointer->input_dem_factor.get() != 1.0) {
				h =  (int) (  (float)h * conf_pointer->input_dem_factor.get()+0.5);
			}

			// -32768 is the ocean flag in srtm3 v 4.1 from cgiar (http://www.cgiar-csi.org/data/srtm-90m-digital-elevation-database-v4-1)
			if (h == -32768) {
				h = -22222;
			} else // ELSE!!! otherwise fsckup!

			// this is the water flag in WORLD3.DEM
			if (h <= -16384) {
				h += 16384;
				if (sealevel2 == 0)
					h = -22222;
			}

			if (h == -9999)	// -9999 is the index for water in GTOPO30
				h = -22222;	//water index! (in openGLIGCexplorer)

			// if sealevel2 is set (!=-22222)
			// if exact equal zero, we will take the waterflags!
			if (sealevel2 != -22222 && sealevel2 != 0) {
				if (h <= sealevel2)
					h = -22222;
			}
			surface_h[zeile][spalte] = (int) h;
			} else {
			// hier

				altitudes.push_back(0);

				// This was wrong and caused a shift!
				//longitudes.push_back(min_lon+spalte*((max_lon-min_lon)/steps_lon));
				//latitudes.push_back(max_lat-zeile*((max_lat-min_lat)/steps_lat));

				latitudes.push_back(demlatmax - ((double) upsteps) * (grid_lat) -
					((double) zeile) * (grid_lat/(double)conf_pointer->getupscalefactor()));
				longitudes.push_back(demlonmin + ((double) leftsteps) * (grid_lon) +
					((double) spalte) * (grid_lon/(double)conf_pointer->getupscalefactor()));




				//cout << latitudes.back() << "  " << longitudes.back() << endl;


			}
		}
	}

	landtime.checknow("afterreadingDEM");
//	if (conf_pointer->getupscalefactor() == 1) {
		demfile.close();

	// for upscaling (???)
	if (conf_pointer->getupscalefactor() > 1) {
	  // get_gnd_elevation does interpolation (needed for upscaling)
	  get_gnd_elevations(&altitudes,&latitudes,&longitudes,conf_pointer);

	  landtime.checknow("afterget_gnd_elev");

	  int _x;
	  std::reverse(altitudes.begin(), altitudes.end());
	  for (int zeile = 0; zeile < steps_lat; zeile += downscalefactor) {
		for (int spalte = 0; spalte < steps_lon;
		     spalte += downscalefactor) {

			// das scheint sehr langsam zu sein.. deque besser?
			_x = altitudes.back();
			altitudes.pop_back();
			//altitudes.erase(altitudes.begin());

			// CHECK SPECIAL CASES also for srtm3 with -32768
			// also see get_gnd_elevtions in oglexfunc.cpp

			if (_x <= -16384) {
				_x += 16384;
				if (sealevel2 == 0)
					_x = -22222;

			}

			if (_x == -9999)	// -9999 is the index for water in GTOPO30
				_x = -22222;	//water index! (in openGLIGCexplorer)

			// if sealevel2 is set (!=-22222)
			// if exact equal zero, we will take the waterflags!
			if (sealevel2 != -22222 && sealevel2 != 0) {
				if (_x <= sealevel2)
					_x = -22222;
			}
			surface_h[zeile][spalte] = _x;

			//cout << _x << "  " ;

		}
			//cout << endl;
	  }

	  landtime.checknow("after remapping altitudes to surface_h");
/*	for (int zeile = 0; zeile < steps_lat; zeile += downscalefactor) {
		for (int spalte = 0; spalte < steps_lon;
		     spalte += downscalefactor) { }}
	landtime.checknow("afterEMPTYloop");*/

	}



// Calculation of the x and y distances from center (we plot in kilometers)
// and maximum and minimum height
	max_h = surface_h[0][0];
	if (max_h == -22222)
		max_h = (short int) sealevel;
	min_h = surface_h[0][0];
	if (min_h == -22222)
		min_h = (short int) sealevel;
	for (int zeile = 0; zeile < steps_lat; zeile += downscalefactor)
		for (int spalte = 0; spalte < steps_lon;
		     spalte += downscalefactor) {
			double pointlat =
			    demlatmax - ((double) upsteps) * (grid_lat) -
			    ((double) zeile) * (grid_lat/(double)conf_pointer->getupscalefactor());
			double pointlon =
			    demlonmin + ((double) leftsteps) * (grid_lon) +
			    ((double) spalte) * (grid_lon/(double)conf_pointer->getupscalefactor());


			float _xkmdist, _ykmdist;

                                                if (!FAST) {


				proj_pointer->get_xy(pointlat, pointlon, _xkmdist,
					     _ykmdist);

				   //cout << _xkmdist << " " << _ykmdist << " KMDISTA" << endl;
				   surface_x[zeile][spalte] = -_xkmdist;
				   surface_y[zeile][spalte] = -_ykmdist;
			}

			//min and max
			if (surface_h[zeile][spalte] != -22222) {

				if ( !(conf_pointer->IGNORE_ELEV_RANGE.get() && surface_h[zeile][spalte] >= conf_pointer->ignore_elev_min.get() && surface_h[zeile][spalte] <= conf_pointer->ignore_elev_max.get()  )) {

				if (surface_h[zeile][spalte] < min_h)
					min_h = surface_h[zeile][spalte];

				if (surface_h[zeile][spalte] > max_h)
					max_h = surface_h[zeile][spalte];
				}
			}


		}



if (FAST)
	return 0;

// remapping the edges to desired values:=========================================
// to avoid overhanging textures
if (conf_pointer->MAP.get()) {

  cout << "MAPS used, terrain edges remapping active!" << endl;
  for (int zeile = 0; zeile < steps_lat; zeile += downscalefactor) {
			float _xkmdist, _ykmdist;
			double pointlat =
			    demlatmax - ((double) upsteps) * (grid_lat) -
			   ((double) zeile) * (grid_lat/(double)conf_pointer->getupscalefactor());
			//double pointlon =
			  //  demlonmin + ((double) leftsteps) * (grid_lon) +
			    //((double) spalte) * (grid_lon/(double)conf_pointer->getupscalefactor());

			    proj_pointer->get_xy(pointlat, min_lon, _xkmdist,
					     _ykmdist);

    surface_x[zeile][0]=-_xkmdist;

			    proj_pointer->get_xy(pointlat, max_lon, _xkmdist,
					     _ykmdist);

    surface_x[zeile][steps_lon-1]=-_xkmdist;

	vector<int> _alt;
	vector<float>_lat;
	vector<float>_lon;

	_lat.push_back(pointlat);_lat.push_back(pointlat);
	_lon.push_back(min_lon);_lon.push_back(max_lon);
	_alt.push_back(0);_alt.push_back(0);

	get_gnd_elevations(&_alt,&_lat,&_lon,conf_pointer);
	surface_h[zeile][0]=_alt[0];
	surface_h[zeile][steps_lon-1]=_alt[1];

  }

  for (int spalte = 0; spalte < steps_lon; spalte += downscalefactor) {
			float _xkmdist, _ykmdist;
			//double pointlat =
			  //  demlatmax - ((double) upsteps) * (grid_lat) -
			   // ((double) zeile) * (grid_lat/(double)conf_pointer->getupscalefactor());
			double pointlon =
			    demlonmin + ((double) leftsteps) * (grid_lon) +
			    ((double) spalte) * (grid_lon/(double)conf_pointer->getupscalefactor());

			    proj_pointer->get_xy(max_lat, pointlon, _xkmdist,
					     _ykmdist);

    surface_y[0][spalte]=-_ykmdist;

			    proj_pointer->get_xy(min_lat, pointlon, _xkmdist,
					     _ykmdist);

    surface_y[steps_lat-1][spalte]=-_ykmdist;

    vector<int> _alt;
	vector<float>_lat;
	vector<float>_lon;

	_lat.push_back(max_lat);_lat.push_back(min_lat);
	_lon.push_back(pointlon);_lon.push_back(pointlon);
	_alt.push_back(0);_alt.push_back(0);

	get_gnd_elevations(&_alt,&_lat,&_lon,conf_pointer);
	surface_h[0][spalte]=_alt[0];
	surface_h[steps_lat-1][spalte]=_alt[1];

 }
}
//===================================================================================


//if (conf_pointer->DEBUG.get() ) {cout <<  "entering normal-vector" <<  endl << flush ;}
Timecheck normaltime("Calc Normalvectors",conf_pointer);
//Calculation of normalvectors
//resize the array for normal vectors
	normalvectors = new float **[steps_lat];
	for (int i = 0; i < steps_lat; i += downscalefactor)
		normalvectors[i] = new float *[steps_lon];

	for (int i = 0; i < steps_lat; i += downscalefactor)
		for (int j = 0; j < steps_lon; j += downscalefactor)
			normalvectors[i][j] = new float[3];

	// reset normalvektor at the edges
	for (int zeile = 0; zeile < steps_lat; zeile += downscalefactor) {

	      normalvectors[zeile][0][0] = 0;
	      normalvectors[zeile][0][1] = 0;
	      normalvectors[zeile][0][2] = 1.;
	      normalvectors[zeile][steps_lon-1][0] = 0;
	      normalvectors[zeile][steps_lon-1][1] = 0;
	      normalvectors[zeile][steps_lon-1][2] = 1.;
	}
	for (int spalte = 0; spalte < steps_lon; spalte += downscalefactor) {

	      normalvectors[0][spalte][0]=0;
	      normalvectors[0][spalte][1]=0;
	      normalvectors[0][spalte][2]=1.;
              normalvectors[steps_lat-1][spalte][0]=0;
	      normalvectors[steps_lat-1][spalte][1]=0;
	      normalvectors[steps_lat-1][spalte][2]=1.;
	}


//Do the calculation
//ZZZZ here we should basically go the full range of the array und then we need i-1 and i+1 interpolated...
//
	for (int i = downscalefactor; i < steps_lat - downscalefactor;
	     i += downscalefactor) {
		for (int j = downscalefactor;
		     j < steps_lon - downscalefactor;
		     j += downscalefactor) {
//every point (i,j) has six neighbours (with common planes)
//dv are the difference vectors to these neighbours
			double dv_W[3];
			double dv_N[3];
			double dv_NE[3];
			double dv_E[3];
			double dv_S[3];
			double dv_SW[3];

			dv_W[0] =
			    surface_x[i][j] - surface_x[i][j -
							   downscalefactor];
			if (dv_W[0] < 0)
				dv_W[0] = dv_W[0] * -1.0;
			dv_W[1] = 0;
			dv_W[2] =
			    ((double) surface_h[i][j] -
			     surface_h[i][j - downscalefactor]) / 1000;
			if (dv_W[2] < 0)
				dv_W[2] = dv_W[2] * -1.0;

			dv_N[0] = 0;
			dv_N[1] =
			    surface_y[i - downscalefactor][j] -
			    surface_y[i][j];
			if (dv_N[1] < 0)
				dv_N[1] = dv_N[1] * -1.0;
			dv_N[2] =
			    ((double) surface_h[i - downscalefactor][j] -
			     surface_h[i][j]) / 1000;
			if (dv_N[2] < 0)
				dv_N[2] = dv_N[2] * -1.0;

			dv_NE[0] =
			    surface_x[i - downscalefactor][j +
							   downscalefactor]
			    - surface_x[i][j];
			if (dv_NE[0] < 0)
				dv_NE[0] = dv_NE[0] * -1.0;
			dv_NE[1] =
			    surface_y[i - downscalefactor][j +
							   downscalefactor]
			    - surface_y[i][j];
			if (dv_NE[1] < 0)
				dv_NE[1] = dv_NE[1] * -1.0;
			dv_NE[2] = ((double)
				    surface_h[i + downscalefactor][j +
								   downscalefactor]
				    - surface_h[i][j]) / 1000;
			if (dv_NE[2] < 0)
				dv_NE[2] = dv_NE[2] * -1.0;

			dv_E[0] =
			    surface_x[i][j + downscalefactor] -
			    surface_x[i][j];
			if (dv_E[0] < 0)
				dv_E[0] = dv_E[0] * -1.0;
			dv_E[1] = 0;
			dv_E[2] =
			    ((double) surface_h[i][j + downscalefactor] -
			     surface_h[i][j]) / 1000;
			if (dv_E[2] < 0)
				dv_E[2] = dv_E[2] * -1.0;

			dv_S[0] = 0;
			dv_S[1] =
			    surface_y[i + downscalefactor][j] -
			    surface_y[i][j];
			if (dv_S[1] < 0)
				dv_S[1] = dv_S[1] * -1.0;
			dv_S[2] =
			    ((double) surface_h[i + downscalefactor][j] -
			     surface_h[i][j]) / 1000;
			if (dv_S[2] < 0)
				dv_S[2] = dv_S[2] * -1.0;

			dv_SW[0] =
			    surface_x[i + downscalefactor][j -
							   downscalefactor]
			    - surface_x[i][j];
			if (dv_SW[0] < 0)
				dv_SW[0] = dv_SW[0] * -1.0;
			dv_SW[1] =
			    surface_y[i + downscalefactor][j -
							   downscalefactor]
			    - surface_y[i][j];
			if (dv_SW[1] < 0)
				dv_SW[1] = dv_SW[1] * -1.0;
			dv_SW[2] = ((double)
				    surface_h[i + downscalefactor][j -
								   downscalefactor]
				    - surface_h[i][j]) / 1000;
			if (dv_SW[2] < 0)
				dv_SW[2] = dv_SW[2] * -1.0;

//the normalvectors for the 6 neighbour triangles are
//or 4 neighbour "triangles" in the case of quads

			double nv_NW[3];
			double nv_NNE[3];
			double nv_ENE[3];
			double nv_SE[3];
			double nv_SSW[3];
			double nv_WSW[3];

			double nv_NE[3];
			double nv_SW[3];

			double nv_sum[3];

			vectorproduct(dv_N[0], dv_N[1], dv_N[2],
				      dv_W[0], dv_W[1], dv_W[2], nv_NW[0],
				      nv_NW[1], nv_NW[2]);
			norm(nv_NW[0], nv_NW[1], nv_NW[2]);

			vectorproduct(dv_S[0], dv_S[1], dv_S[2],
				      dv_E[0], dv_E[1], dv_E[2], nv_SE[0],
				      nv_SE[1], nv_SE[2]);
			norm(nv_SE[0], nv_SE[1], nv_SE[2]);

			if (conf_pointer->QUADS.get()) {
			vectorproduct(dv_W[0], dv_W[1], dv_W[2],
				      dv_S[0], dv_S[1], dv_S[2], nv_SW[0],
				      nv_SW[1], nv_SW[2]);
			norm(nv_SW[0], nv_SW[1], nv_SW[2]);

			vectorproduct(dv_E[0], dv_E[1], dv_E[2],
				      dv_N[0], dv_N[1], dv_N[2], nv_NE[0],
				      nv_NE[1], nv_NE[2]);
			norm(nv_NE[0], nv_NE[1], nv_NE[2]);
			}

			if (!conf_pointer->QUADS.get()) {
			vectorproduct(dv_NE[0], dv_NE[1], dv_NE[2],
				      dv_N[0], dv_N[1], dv_N[2], nv_NNE[0],
				      nv_NNE[1], nv_NNE[2]);
			norm(nv_NNE[0], nv_NNE[1], nv_NNE[2]);

			vectorproduct(dv_E[0], dv_E[1], dv_E[2],
				      dv_NE[0], dv_NE[1], dv_NE[2],
				      nv_ENE[0], nv_ENE[1], nv_ENE[2]);
			norm(nv_ENE[0], nv_ENE[1], nv_ENE[2]);

			vectorproduct(dv_SW[0], dv_SW[1], dv_SW[2],
				      dv_S[0], dv_S[1], dv_S[2], nv_SSW[0],
				      nv_SSW[1], nv_SSW[2]);
			norm(nv_SSW[0], nv_SSW[1], nv_SSW[2]);

			vectorproduct(dv_W[0], dv_W[1], dv_W[2],
				      dv_SW[0], dv_SW[1], dv_SW[2],
				      nv_WSW[0], nv_WSW[1], nv_WSW[2]);
			norm(nv_WSW[0], nv_WSW[1], nv_WSW[2]);
			}


			for (int k = 0; k < 3; k++) {

				if (!conf_pointer->QUADS.get()) {
				nv_sum[k] =
				    nv_NW[k] + nv_NNE[k] + nv_ENE[k] +
				    nv_SE[k] + nv_SSW[k] + nv_WSW[k];
				} else {
				nv_sum[k] =
				    nv_NW[k] + nv_NE[k] +
				    nv_SE[k] + nv_SW[k];
				}
			}
			norm(nv_sum[0],nv_sum[1],nv_sum[2]);
			normalvectors[i][j][0] = nv_sum[0];
			normalvectors[i][j][1] = nv_sum[1];
			normalvectors[i][j][2] = nv_sum[2];


	}}
	normaltime.checknow("End of Normalvectors");
	landtime.checknow("End of LandscapeReadDEM");
	//if (conf_pointer->DEBUG.get() ) {cout <<  "leaving remapping vector" <<  endl << flush ;}
	return 0;

}



// this is a wrapper for the oglxfunc - function getcolor...
void Landscape::getlandcolor(float *colorpointer, short int height,
			 int colormap)
{
// we use 2 colorscales. One beneath sealevel and one above.
// to use different colormaps (e.g. No.1 beneath, No.2 above)
// we need to choose before  getcolor is called.

// h value -22222 means: surface of ocean... (always blue)

// colormap 1
// For a start, this implements MATLABS eye-burning 'Jet'
// colormap with extension red->white for large values.
// Ripped out of GPEsim from Jan.Krueger@uni-konstanz.de

	int minValue, maxValue;
	//int maxValue = max_h;
	//int sealevel = -22222;

	maxValue = max_h;
	minValue = min_h;

	// sealevel between max and min -> then use 2 ranges : (min-sealevel, and sealevel-max)
	if (sealevel < maxValue && sealevel > minValue) {
		if (height <= (short int)  sealevel) {
			minValue = min_h;
			maxValue =  (short int) sealevel;
		}
		if (height > (short int) sealevel) {
			minValue = (short int) sealevel;
			maxValue = max_h;
		}
	}

	getcolor(colorpointer, colormap, (float)height, (float)minValue, (float)maxValue);


}


void Landscape::setgltrianglestrip_tex(int _colmapnr, int _colmapnr2, bool BW)
{
	proj_pointer->get_xy(max_lat, max_lon, tex_right_top_km,
			     tex_top_km);
	proj_pointer->get_xy(max_lat, min_lon, tex_left_top_km,
			     tex_top_km);
	proj_pointer->get_xy(min_lat, max_lon, tex_right_bottom_km,
			     tex_bottom_km);
	proj_pointer->get_xy(min_lat, min_lon, tex_left_bottom_km,
			     tex_bottom_km);

	tex_right_top_km *= -1.0;
	tex_left_top_km *= -1.0;
	tex_right_bottom_km *= -1.0;
	tex_left_bottom_km *= -1.0;
	tex_top_km *= -1.0;
	tex_bottom_km *= -1.0;

	//setgltrianglestrip (false, 3, true);
	setgltrianglestrip(false, _colmapnr, _colmapnr2, BW, true);
}

void Landscape::shadecolor(float *color, int zeile, int spalte, int downscalefactor)
{

	int n,s,w,e,nw,ne,sw,se;

	if (conf_pointer->shadedirection.get() == 8) {  nw= 0.5; n= 1.0; ne= 0.0;
							 w= 1.0;          e=-1.0;
							sw= 0.0; s=-1.0; se=-0.5;}

	if (conf_pointer->shadedirection.get() == 7) {  nw= 0.75; n= 0.0; ne=-0.75;
							 w= 1.0;          e=-1.0;
							sw= 0.75; s= 0.0; se=-0.75;}

	if (conf_pointer->shadedirection.get() == 6) {  nw= 0.0; n=-1.0; ne=-0.5;
							 w= 1.0;          e=-1.0;
							sw= 0.5; s= 1.0; se=-0.0;}

	if (conf_pointer->shadedirection.get() == 5) {  nw=-0.75; n=-1.0; ne=-0.75;
							 w= 0.0;          e= 0.0;
							sw= 0.75; s= 1.0; se= 0.75;}

	if (conf_pointer->shadedirection.get() == 4) {  nw=-0.5; n=-1.0; ne= 0.0;
							 w=-1.0;          e= 1.0;
							sw= 0.0; s= 1.0; se= 0.5;}

	if (conf_pointer->shadedirection.get() == 3) {  nw=-0.75; n= 0.0; ne= 0.75;
							 w=-1.0;          e= 1.0;
							sw=-0.75; s= 0.0; se= 0.75;}

	if (conf_pointer->shadedirection.get() == 2) {  nw= 0.0; n= 1.0; ne= 0.5;
							 w=-1.0;          e= 1.0;
							sw=-0.5; s=-1.0; se= 0.0;}

	if (conf_pointer->shadedirection.get() == 1) {  nw= 0.75; n= 1.0; ne= 0.75;
							 w= 0.0;          e= 0.0;
							sw=-0.75; s=-1.0; se=-0.75;}


	float _diff;
	if (zeile-downscalefactor >= 0 && spalte-downscalefactor >=0
	    && zeile+downscalefactor < steps_lat - 1*downscalefactor
	    && spalte+downscalefactor < steps_lon -1 * downscalefactor
	  ) {
		 _diff = (
			nw*surface_h[zeile-downscalefactor][spalte-downscalefactor] +
			se*surface_h[zeile+downscalefactor][spalte+downscalefactor] +
			w*surface_h[zeile][spalte-downscalefactor] +
			s*surface_h[zeile+downscalefactor][spalte] +
			n*surface_h[zeile-downscalefactor][spalte] +
			e*surface_h[zeile][spalte+downscalefactor] +
			sw*surface_h[zeile+downscalefactor][spalte-downscalefactor]+
			ne*surface_h[zeile-downscalefactor][spalte+downscalefactor]
			) / 5.0 ;
	} else {
		_diff =0;
	}

//	if (_diff > shade_max) {shade_max = _diff;}
//	if (_diff < shade_min) {shade_min = _diff;}

	if (conf_pointer->getshadescale() == 0.0) {
		conf_pointer->setshadescale( (max_h - min_h) / 7.0 );
		if (conf_pointer->DEBUG.get()) cout << "shadescale: " << conf_pointer->getshadescale() << endl;
	}

	float shade = 1;
	if (_diff > 0) {shade= 1.0-(_diff / conf_pointer->getshadescale() * conf_pointer->getupscalefactor()); }


	//cout << "("<<color[0]<<";"<<color[1]<<";"<<color[2]<<")===>(";

	color[0] = color[0]*shade;
	color[1] = color[1]*shade;
	color[2] = color[2]*shade;

	//cout << "("<<color[0]<<";"<<color[1]<<";"<<color[2]<<")";
	//cout << shade << "     ";

}

void Landscape::setgltrianglestrip(bool _WIRE, int _colmapnumber, int _colmapnr2, bool BW, bool _TEXMAP)
{
	Timecheck tstime("setgltrianglestrip", conf_pointer);

	if (!conf_pointer->MODULATE.get() && conf_pointer->MAP.get() && !conf_pointer->MAPS_UNLIGHTED.get()) {
		if (conf_pointer->DEBUG.get())
			cout << "Map mode without modulation!!! -> using colormap7 (white) with modulation)" << endl << flush;
		_colmapnumber=7;
		_colmapnr2 = 7;

	}
	//glPushAttrib(GL_ENABLE_BIT);
	//glPushAttrib(GL_POLYGON_BIT);
	//glPushAttrib(GL_LINE_BIT);

//	float shade_max = 0;
//	float shade_min = 0;
//	float shade = 1;

	//float *color = new float[3];
	float color[3];
	if (!_WIRE) {
		glEnable(GL_COLOR_MATERIAL);

		if (conf_pointer->NOLIGHTING.get())
			glDisable(GL_LIGHTING);
		//glPolygonMode(GL_FRONT, GL_FILL);
	} else {
		glLineWidth(1.0);
		glPolygonMode(GL_FRONT, GL_LINE);
		glDisable(GL_LIGHTING);
		glColor3f(0.5, 0.5, 0.5);
		//cout << "color ist set !!!!!!"<< endl;
	}

	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	float _full_y, _x_left_delta, _x_delta, _full_x_bottom,
	    _full_x_top = 0;

	if (_TEXMAP) {

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);


	// width of the top-edge
	_full_x_top = tex_right_top_km - tex_left_top_km;

	// width of the bottom-edge
	_full_x_bottom = tex_right_bottom_km - tex_left_bottom_km;

	// difference between width of top and bottom edge
	_x_delta = _full_x_bottom - _full_x_top;

	// difference in x between left top and left bottom....
	// this may differ from _x_right_delta and from 1/2*_x_delta, because
	// the center may not be in the center of the texture...
	_x_left_delta = tex_left_bottom_km - tex_left_top_km;

	// height of texture in km (bottom<->top)
	_full_y = tex_top_km - tex_bottom_km;

	}


	//cout << _x_left_delta << "<-left delta .... x_delta->" << _x_delta << endl;

	// normal vector dummy
	float replacing_nv[] = { 0.0, 0.0, 1.0};

	// hier ist noch ARBEIT fr downscaling mit maps


	for (int zeile = 0;//downscalefactor;
	     zeile < steps_lat - 1 * downscalefactor; // 2*
	     zeile += downscalefactor) {

		if (conf_pointer->QUADS.get()) {
			glBegin(GL_QUAD_STRIP); }
			else {
			glBegin(GL_TRIANGLE_STRIP);
		}

		for (int spalte = 0;//downscalefactor;
		     spalte < (steps_lon); // - downscalefactor);
		     spalte += downscalefactor) {

			if (!_WIRE) {


				if (surface_h[zeile][spalte] > sealevel) {
					getlandcolor(color,
						 surface_h[zeile][spalte],
						 _colmapnumber);
				} else {
					getlandcolor(color,
						 surface_h[zeile][spalte],
						 _colmapnr2);
				}

				//getcolor(color, surface_h[zeile][spalte],_colmapnumber);
				//cout << color[0] <<", " << color[1] << ", " << color[2] << endl;

				if (surface_h[zeile][spalte] != -22222) {
					glNormal3fv(normalvectors[zeile][spalte]);
				} else {
					glNormal3fv(replacing_nv);
				}
//				glNormal3fv(replacing_nv);


				// use terrainshading
				if (conf_pointer->TERRAINSHADING.get()) {
					shadecolor(color,zeile,spalte,downscalefactor);
				}


				if (!BW) {
					glColor3f(color[0], color[1],
						  color[2]);
				} else {
					glColor3fv(graycolor(color));
				}
			}

			if (_TEXMAP) {



				float _y_tex_km =
				    surface_y[zeile][spalte] -
				    tex_bottom_km;
				float _tex_y = 1 - _y_tex_km / _full_y;

				float _x_tex_km =
				    surface_x[zeile][spalte] -
				    (tex_left_top_km +
				     _tex_y * _x_left_delta);

				float _tex_x =
				    _x_tex_km / (_full_x_top +
						 _tex_y * _x_delta);

				glTexCoord2f(_tex_x, _tex_y);	//}

				//cout << _tex_x << "   " << _tex_y << endl;
				if (_tex_x < -0.5 || _tex_x > 1.5
				    || _tex_y < -0.5 || _tex_y > 1.5) {
					cerr <<
					    "seriously wrong texture coordinates.... contact author: "
					    << MAIL << endl;
					cerr << _tex_x << "   " << _tex_y
					    << endl;
				}


			}
			//if (surface_x[zeile][spalte] < -190) cout << surface_x[zeile][spalte] << "  " << endl;

			float _alt;
			if (surface_h[zeile][spalte] == -22222) {
				_alt = sealevel2 / 1000.0;
			} else {
				_alt = surface_h[zeile][spalte] / 1000.0;
			}

			// this is a exception. no sealevel2 is set and h is watersurface....
			if (surface_h[zeile][spalte] == -22222
			    && sealevel2 == -22222) {
				_alt = 0;
			}


			glVertex3f(surface_x[zeile][spalte],
				   surface_y[zeile][spalte], _alt);

			//cout << surface_x[zeile][spalte] << " " <<surface_y[zeile][spalte] << " " << surface_h[zeile][spalte]/1000.0 << endl;
			if (!_WIRE) {

				if (surface_h[zeile + downscalefactor]
				    [spalte] > sealevel) {
					getlandcolor(color,
						 surface_h[zeile +
							   downscalefactor]
						 [spalte], _colmapnumber);
				} else {
					getlandcolor(color,
						 surface_h[zeile +
							   downscalefactor]
						 [spalte], _colmapnr2);
				}



				if (surface_h[zeile][spalte] != -22222) {
					glNormal3fv(normalvectors[zeile +downscalefactor][spalte]);
				} else {
					glNormal3fv(replacing_nv);
				}

				if (conf_pointer->TERRAINSHADING.get()) {
					shadecolor(color,zeile+downscalefactor,spalte,downscalefactor);
				}

				//glColor3f (color[0], color[1], color[2]);
				if (!BW) {
					glColor3f(color[0], color[1],
						  color[2]);
				} else {
					glColor3fv(graycolor(color));
				}
			}

			if (_TEXMAP) {


				float _y_tex_km =
				    surface_y[zeile +
					      downscalefactor][spalte] -
				    tex_bottom_km;
				float _tex_y = 1 - _y_tex_km / _full_y;

				float _x_tex_km =
				    surface_x[zeile +
					      downscalefactor][spalte] -
				    (tex_left_top_km +
				     _tex_y * _x_left_delta);

				float _tex_x =
				    _x_tex_km / (_full_x_top +
						 _tex_y * _x_delta);

				glTexCoord2f(_tex_x, _tex_y);	//}

				if (_tex_x < -0.5 || _tex_x > 1.5
				    || _tex_y < -0.5 || _tex_y > 1.5) {
					cerr <<
					    "seriously wrong texture coordinates.... contact author: "
					    << MAIL << endl;
					cerr << _tex_x << "   " << _tex_y
					    << endl;
				}
			}

			if (surface_h[zeile + downscalefactor][spalte] == -22222) {
				_alt = sealevel2 / 1000.0;
			} else {
				_alt =  surface_h[zeile + downscalefactor][spalte] /
				    1000.0;
			}

			// this is a exception. no sealevel2 is set and h is watersurface....
			if (surface_h[zeile + downscalefactor][spalte] ==
			    -22222 && sealevel2 == -22222) {
				_alt = 0;
			}

			glVertex3f(surface_x[zeile + downscalefactor]
				   [spalte],
				   surface_y[zeile +
					     downscalefactor][spalte],
				   _alt);

		}
		glEnd();

	}

	//glPopAttrib();
	glEnable(GL_LIGHTING);

	if (_TEXMAP) {
		glDisable(GL_TEXTURE_2D);
	}

	glPolygonMode(GL_FRONT, GL_FILL);
	glDisable(GL_COLOR_MATERIAL);


	// here comes sealvel3

	if (conf_pointer->DEBUG.get()) {
		cout << "Sealevel3:" << sealevel3 << endl << flush;

		//cout << "Terrainshading: Max:" << shade_max << "  Min: " << shade_min << endl << flush;
	}

	if (sealevel3 != -22222) {

		if (conf_pointer->DEBUG.get())
			cout << "Sealevel3 is given" << endl << flush;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDepthMask(GL_FALSE);

		float col[4];
		float xlim,ylim;

		col[0] = 0;
		col[1] = 0;
		col[2] = 1;
		col[3] = 0.6;

		glBegin(GL_POLYGON);

		if (!conf_pointer->BW.get()) {
			glColor4fv(col);
		} else {
			glColor4fv(graycolor(col));
		}

		double sloffset = 0.5;
		proj_pointer->get_xy(max_lat,min_lon, xlim,ylim);
		cout << xlim << "  " << ylim << "   " << endl;
		glVertex3f((GLfloat) -xlim, (GLfloat) -ylim, ((sealevel3+sloffset)/1000.0));

		proj_pointer->get_xy(max_lat,max_lon, xlim,ylim);
		cout << xlim << "  " << ylim << "   " << endl;
		glVertex3f((GLfloat) -xlim, (GLfloat) -ylim, ((sealevel3+sloffset)/1000.0));

		proj_pointer->get_xy(min_lat,max_lon, xlim,ylim);
		cout << xlim << "  " << ylim << "   " << endl;
		glVertex3f((GLfloat) -xlim, (GLfloat) -ylim, ((sealevel3+sloffset)/1000.0));

		proj_pointer->get_xy(min_lat,min_lon, xlim,ylim);
		cout << xlim << "  " << ylim << "   " << endl;
		glVertex3f((GLfloat) -xlim, (GLfloat) -ylim, ((sealevel3+sloffset)/1000.0));

		glEnd();

		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT, GL_FILL);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);

	}
	tstime.checknow("end trianglestrip");
}


void Landscape::vectorproduct(double a1, double a2, double a3, double b1,
			      double b2, double b3, double &c1, double &c2,
			      double &c3)
{
	c1 = a2 * b3 - a3 * b2;
	c2 = a3 * b1 - a1 * b3;
	c3 = a1 * b2 - a2 * b1;
}

void Landscape::norm(double &a1, double &a2, double &a3)
{
	double l = sqrt(a1 * a1 + a2 * a2 + a3 * a3);
	a1 = a1 / l;
	a2 = a2 / l;
	a3 = a3 / l;
}




Landscape::~Landscape()
{

	//cout << "entering destructor of Landscape class" << endl << flush;

	if (CLEARFLAG) {
		for (int _i = 0; _i < steps_lat; _i += downscalefactor) {
			delete[]surface_h[_i];

			//if (CLEARFLAG) {

				delete[]surface_x[_i];
				delete[]surface_y[_i];
			//}
	}
	}

	if (CLEARFLAG_SURFACE)
		delete[]surface_h;
	if (CLEARFLAG) {
		delete[]surface_x;
		delete[]surface_y;
	}
	//cout << "3 * 2d cleared .." << endl << flush;
	if (CLEARFLAG) {
		for (int i = 0; i < steps_lat; i += downscalefactor)
			for (int j = 0; j < steps_lon;
			     j += downscalefactor)
				delete[]normalvectors[i][j];

		for (int i = 0; i < steps_lat; i += downscalefactor)
			delete[]normalvectors[i];

		delete[]normalvectors;
	}
}
