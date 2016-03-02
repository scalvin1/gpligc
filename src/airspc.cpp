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

#include "airspc.h"

#include<fstream>
#include<cmath>
#include<cstring>

#ifndef __OSX__
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include "oglexfunc.h"


const double RADSTEP = 0.06;	// step in rad for drawing arcs

//const std::string rcsid_airspc_cpp =
//    "$Id: airspc.cpp 3 2014-07-31 09:59:20Z kruegerh $";
//const std::string rcsid_airspc_h = AIRSPC_H;


//Callback function for the gluTesselation
void errorCallback(GLenum errorCode)
{
	const GLubyte *estring;
	estring = gluErrorString(errorCode);
	cerr <<
	    "Tessellation Error  (plotting tops and bottoms of airspaces) :"
	    << estring << endl;
	//exit(0);
}


Airspace::Airspace()
{
}

Airspace::Airspace(float _center_lat, float _center_lon, float _latmax,
		   float _latmin, float _lonmax, float _lonmin)
{
	initAirspace(_center_lat, _center_lon, _latmax, _latmin, _lonmax,
		     _lonmin);
}


void Airspace::initAirspace(float _center_lat, float _center_lon,
			float _latmax, float _latmin, float _lonmax,
			float _lonmin)
{
	center_lat = _center_lat;
	center_lon = _center_lon;

	latmax = _latmax;
	lonmax = _lonmax;
	latmin = _latmin;
	lonmin = _lonmin;
}



void Airspace::readOpenAirFile(string filename)
{

	ifstream airfile(filename.c_str());

	if (!airfile) {
		cerr << "Can not open OpenAirspace File: " << filename <<
		    endl;
		exit(1);
	}

	if (conf->DEBUG.get())
	cout << "start reading:" << filename << endl;

	int badcounter=0;

//char zeile[80];
	string zeile;

// Flag, set to true, while scanning first airspace
	bool FIRST = true;

	bool AIRDEBUG = false;

// temporary scanning variables (-9999, - = not yet read)
	int scantmp_ah = -9999;
	string scantmp_ah_unit = "-";
	int scantmp_al = -9999;
	string scantmp_al_unit = "-";
	string scantmp_an = "-";
	string scantmp_ac = "-";
	vector < float >scantmp_xcoor;
	vector < float >scantmp_ycoor;
	vector < float >scantmp_lat;
	vector < float >scantmp_lon;


// Direction (clockwise +, or counterclockwise -) for arcs
	int D = +1;

// center point coordinates for arcs
	float X_lat = 0;
	float X_lon = 0;



	while (airfile) {
		getline(airfile, zeile);

		//skip lines starting # , empty lines...
		if (zeile[0] == '#')
			continue;
		if (zeile[0] == '*')
			continue;
		if (zeile[0] == 0)
			continue;
		if (zeile[0] == 13)
			continue;
		if (zeile.size() == 0)
			continue;

		//cout << zeile << "  " <<zeile.size() << endl;

		char arg1[80], arg2[80], arg3[80], arg4[80];


		//scan 2 arguments to check kind of entry
		sscanf(zeile.c_str(), "%s %s", arg1, arg2);

		//Airspace Type (Airspace Class)
		// Here starts a new Airspace description!
		// the last scanned from tmp has to be saved!
		if (strcmp(arg1, "AC") == 0) {
			if (!FIRST) {	// means, that we scanned at least one airspace already

				// save last scanned airspace
				lower.push_back(scantmp_al);
				lower_unit.push_back(scantmp_al_unit);
				upper.push_back(scantmp_ah);
				upper_unit.push_back(scantmp_ah_unit);
				type.push_back(scantmp_ac);
				name.push_back(scantmp_an);
				xcoor.push_back(scantmp_xcoor);



				ycoor.push_back(scantmp_ycoor);
				lat.push_back(scantmp_lat);
				lon.push_back(scantmp_lon);

				// clear scantmp_ ...
				scantmp_ah = -9999;
				scantmp_ah_unit = "-";
				scantmp_al = -9999;
				scantmp_al_unit = "-";
				scantmp_an = "-";
				scantmp_ac = "-";
				scantmp_xcoor.clear();
				scantmp_ycoor.clear();
				scantmp_lat.clear();
				scantmp_lon.clear();

				//reset direction
				D = +1;


			} else {
				FIRST = false;
			}

			//Airspace Type
			scantmp_ac = arg2;

		}


		// Airspace Name
		if (strcmp(arg1, "AN") == 0) {
			int pos = zeile.find("AN");
			scantmp_an = zeile.substr(pos + 3);
		}

		//Polygon Point  DP
		if (strcmp(arg1, "DP") == 0) {
			char lat[15], NS[5], lon[15], EW[5];
			string zeile_tmp =
			    zeile.substr(zeile.find("DP") + 2);
			//cout << "____>" << zeile_tmp  << endl;
			if (sscanf
			    (zeile_tmp.c_str(), "%s %1s %s %1s", lat, NS,
			     lon, EW) != 4) {
				cerr <<
				    "Error parsing DP line (airspace-file)"
				    << endl;
				exit(1);
			}
			float _tmplat = coor2dec(lat, NS);
			float _tmplon = coor2dec(lon, EW);

			if (_tmplat == -9999 || _tmplon == -9999) {
				cerr <<
				    "Error parsing coordinates from DP line"
				    << endl;
				exit(1);
			}
			//save coordinates
			scantmp_lat.push_back(_tmplat);
			scantmp_lon.push_back(_tmplon);

			// convert coordinates to our x,y [km] system
			/*float _lat_ddist =  center_lat -   _tmplat;
			   float _lon_ddist = center_lon -   _tmplon;
			   float _xkmdist = - _lon_ddist * DEGDIST *    cos (PI_180 * _tmplat);
			   float _ykmdist = - _lat_ddist * DEGDIST; */
			float _xkmdist, _ykmdist;
			proj->get_xy(_tmplat, _tmplon, _xkmdist,
					     _ykmdist);

			//and save to tmp-variables
			scantmp_xcoor.push_back(-_xkmdist);
			scantmp_ycoor.push_back(-_ykmdist);

		}

		// Variable definition
		if (strcmp(arg1, "V") == 0) {

			//cout << zeile << "-------" << arg2 << endl;

			if (strcmp(arg2, "D=+") == 0) {
				D = +1;
				//cout << " D + FOUND --------" << endl;
			}
			if (strcmp(arg2, "D=-") == 0) {
				D = -1;
				//cout << "D - FOUND -------" << endl;
			}

			string arg2_temp = arg2;

			if (arg2_temp.substr(0, 1) == "X") {
				char lat[15], NS[5], lon[15], EW[5];
				string zeile_tmp =
				    zeile.substr(zeile.find("X=") + 2);
				//cout << "____>" << zeile_tmp  << endl;
				if (sscanf
				    (zeile_tmp.c_str(), "%s %1s %s %1s",
				     lat, NS, lon, EW) != 4) {
					cerr <<
					    "Error parsing V X= line (airspace-file)"
					    << endl;
					exit(1);
				}
				X_lat = coor2dec(lat, NS);
				X_lon = coor2dec(lon, EW);

				//cout << "to parse " << lat << " " << NS << "   " << lon << " " << EW << endl;
				//cout << "parsed X= coordinates: " << X_y << "  " << X_x << endl;

				if (X_lat == -9999 || X_lon == -9999) {
					cerr <<
					    "error parsing coordinates in V X=... line"
					    << endl;
					exit(1);
				}
				//save coordinates     // why ?? this is a center-point ?! dont need to be saved?
				//scantmp_lat.push_back(X_y);
				//scantmp_lon.push_back(X_x);

				// X_x X_y should be X_lon, X_lat...
				/*
				   float _lat_ddist =  center_lat -   X_y;
				   float _lon_ddist = center_lon -  X_x;
				   X_x = - _lon_ddist * DEGDIST *    cos (PI_180 * X_y);
				   X_y=  - _lat_ddist * DEGDIST;
				 */
			}

		}

		//Arc, 3 parameters       DA
		// this one isnt really tested yet
		if (strcmp(arg1, "DA") == 0) {
			float alpha;
			float bravo;
			float radius;
			if (sscanf
			    (zeile.c_str(), "%s %f %f %f", arg1, &radius,
			     &alpha, &bravo) == 4) {


				alpha *= (M_PI / 180.0);
				bravo *= (M_PI / 180.0);
				radius *= 1.852;


				//draw the arcs
				float _lat, _lon, _x, _y;
				//counterclockwise
				if (D == 1) {

					if (alpha < bravo) {
						for (float winkel = alpha;
						     winkel <= bravo;
						     winkel =
						     winkel + RADSTEP) {
							go(X_lat, X_lon,
							   (winkel /
							    (M_PI /
							     180.0)),
							   radius, _lat,
							   _lon);
							proj->
							    get_xy(_lat,
								   _lon,
								   _x, _y);
							scantmp_xcoor.
							    push_back(-_x);
							scantmp_ycoor.
							    push_back(-_y);
							//scantmp_xcoor.push_back(X_x +( radius * cos (winkel)) );
							//scantmp_ycoor.push_back(X_y +( radius * sin (winkel)) );
						}
					} else {	// alpha > bravo

						for (float winkel = alpha;
						     winkel <= 2 * M_PI;
						     winkel =
						     winkel + RADSTEP) {
							go(X_lat, X_lon,
							   (winkel /
							    (M_PI /
							     180.0)),
							   radius, _lat,
							   _lon);
							proj->
							    get_xy(_lat,
								   _lon,
								   _x, _y);
							scantmp_xcoor.
							    push_back(-_x);
							scantmp_ycoor.
							    push_back(-_y);
							//scantmp_xcoor.push_back(X_x +( radius * cos (winkel)) );
							//scantmp_ycoor.push_back(X_y +( radius * sin (winkel)) );
						}
						for (float winkel = 0;
						     winkel <= bravo;
						     winkel =
						     winkel + RADSTEP) {
							go(X_lat, X_lon,
							   (winkel /
							    (M_PI /
							     180.0)),
							   radius, _lat,
							   _lon);
							proj->
							    get_xy(_lat,
								   _lon,
								   _x, _y);
							scantmp_xcoor.
							    push_back(-_x);
							scantmp_ycoor.
							    push_back(-_y);
							//scantmp_xcoor.push_back(X_x +( radius * cos (winkel)) );
							//scantmp_ycoor.push_back(X_y +( radius * sin (winkel)) );
						}


					}
				}
				// clockwise
				if (D == +1) {
					//cout << "CLOCKWISE" << endl;
					if (alpha > bravo) {

						for (float winkel = alpha;
						     winkel >= bravo;
						     winkel =
						     winkel - RADSTEP) {
							go(X_lat, X_lon,
							   (winkel /
							    (M_PI /
							     180.0)),
							   radius, _lat,
							   _lon);
							proj->
							    get_xy(_lat,
								   _lon,
								   _x, _y);
							scantmp_xcoor.
							    push_back(-_x);
							scantmp_ycoor.
							    push_back(-_y);
							//scantmp_xcoor.push_back(X_x +( radius * cos (winkel)) );
							//scantmp_ycoor.push_back(X_y +( radius * sin (winkel)) );
						}

					} else {

						for (float winkel = alpha;
						     winkel >= 0;
						     winkel =
						     winkel - RADSTEP) {
							go(X_lat, X_lon,
							   (winkel /
							    (M_PI /
							     180.0)),
							   radius, _lat,
							   _lon);
							proj->
							    get_xy(_lat,
								   _lon,
								   _x, _y);
							scantmp_xcoor.
							    push_back(-_x);
							scantmp_ycoor.
							    push_back(-_y);
							//scantmp_xcoor.push_back(X_x +( radius * cos (winkel)) );
							//scantmp_ycoor.push_back(X_y +( radius * sin (winkel)) );
						}
						for (float winkel =
						     2 * M_PI;
						     winkel >= bravo;
						     winkel =
						     winkel - RADSTEP) {
							go(X_lat, X_lon,
							   (winkel /
							    (M_PI /
							     180.0)),
							   radius, _lat,
							   _lon);
							proj->
							    get_xy(_lat,
								   _lon,
								   _x, _y);
							scantmp_xcoor.
							    push_back(-_x);
							scantmp_ycoor.
							    push_back(-_y);
							//scantmp_xcoor.push_back(X_x +( radius * cos (winkel)) );
							//scantmp_ycoor.push_back(X_y +( radius * sin (winkel)) );
						}


					}
				}


			} else {

				cerr <<
				    "Error reading DA-line from OpenAir file.. "
				    << zeile << endl;
				exit(1);
			}

		}

		// DB
		//Arc, 2 parameters  // this is the more important...
		if (strcmp(arg1, "DB") == 0) {
			char lat1[15], NS1[5], lon1[15], EW1[5];
			char lat2[15], NS2[5], lon2[15], EW2[5];
			string zeile_tmp =
			    zeile.substr(zeile.find("DB") + 2);
			//cout << "____>" << zeile_tmp  << endl;
			if (sscanf
			    (zeile_tmp.c_str(),
			     "%s %1s %s %1s , %s %1s %s %1s", lat1, NS1,
			     lon1, EW1, lat2, NS2, lon2, EW2) != 8) {
				cerr <<
				    "Error parsing DB line (airspace-file)   "
				    << zeile << endl << scantmp_an << endl;
				exit(1);
			}
			// store coord in A_ B_
			float A_lat = coor2dec(lat1, NS1);
			float A_lon = coor2dec(lon1, EW1);
			float B_lat = coor2dec(lat2, NS2);
			float B_lon = coor2dec(lon2, EW2);

			if (A_lat == -9999 || A_lon == -9999
			    || B_lat == -9999 || B_lon == -9999) {
				cerr <<
				    "error parsing coordinates in DB line"
				    << endl;
				exit(1);
			}
			// why ??????????
			/*
			   //save coordinates
			   scantmp_lat.push_back(A_y);
			   scantmp_lon.push_back(A_x);
			   //save coordinates
			   scantmp_lat.push_back(B_y);
			   scantmp_lon.push_back(B_x);
			 */


			// dont convert to
			/*
			   float _lat_ddist =  center_lat -   A_y;
			   float _lon_ddist = center_lon -  A_x;
			   A_x = - _lon_ddist * DEGDIST *    cos (PI_180 * A_y);
			   A_y =  - _lat_ddist * DEGDIST;

			   _lat_ddist =  center_lat -   B_y;
			   _lon_ddist = center_lon -  B_x;
			   B_x = - _lon_ddist * DEGDIST *    cos (PI_180 * B_y);
			   B_y =  - _lat_ddist * DEGDIST;
			 */

			//calclulate radius (twice and check)
			//float r1 = sqrt ( pow((A_x - X_x),2) + pow((A_y - X_y),2) );
			//float r2 = sqrt ( pow((B_x - X_x),2) + pow((B_y - X_y),2) );
			float r1 =
			    great_circle_dist(A_lat, A_lon, X_lat, X_lon);
			float r2 =
			    great_circle_dist(B_lat, B_lon, X_lat, X_lon);


			//cout << "diff : " << abs(r1-r2) << "   " << r1 << endl;

			/*if (abs(r1 - r2) > 100) {
			   cerr << "something is seriously wrong!" << endl;
			   exit (1);
			   }
			 */

			//cout << "diff : " << r1-r2 << endl;

			float R_ = (r1 + r2) / 2.0;

			// dont needed anymore
			//float _dlat =  A_lat - X_lat;
			//float _dlon =  A_lon - X_lon;

			/*float _asinarg = (X_lat - A_lat) / R_;
			   if (_asinarg > 1) {_asinarg =1; //cerr << "correction 1 alpha" << endl;
			   }
			   if (_asinarg < -1) {_asinarg = -1; //cerr << "correction -1 alpha" << endl;
			   }

			   float alpha = asin (_asinarg);

			   if (_dx > 0 && _dy > 0) { alpha = abs(alpha);}
			   if (_dx < 0 && _dy > 0) { alpha = M_PI - abs(alpha);}
			   if (_dx < 0 && _dy < 0) { alpha = M_PI + abs(alpha);}
			   if (_dx > 0 && _dy < 0) { alpha = 2 * M_PI - abs(alpha);}
			 */

			float alpha = heading(X_lat, X_lon, A_lat,
					      A_lon) * (M_PI / 180.0);
			//cout << "alpha " << alpha << endl;

			//if (alpha < 0) alpha += 2 * M_PI;

			//cout << R_ << "   " << X_y - B_y << endl;
			//cout << (X_y - B_y) / R_ << endl;

			/*_asinarg = (X_y - B_y) / R_;
			if (_asinarg > 1) {
			           _asinarg = 1;
			           //cerr << "argument of asin corrected to 1.  bravo " << endl;
				}

			if (_asinarg < -1) { _asinarg = -1; //cerr << "correction bravo -1" << endl;
						}




			float bravo =  asin (_asinarg);

			//if (scantmp_an == "DDS: 128.95 MHz\n") {
			//cout << "DEBUG:  " << alpha << "<-alpha bravo->" << bravo << endl;

			//}


			_dx =  B_x - X_x;
			_dy =  B_y - X_y;
			if (_dx > 0 &&  _dy >0) { bravo = abs(bravo);}
			if (_dx < 0 && _dy > 0) { bravo = M_PI - abs(bravo);}
			if (_dx < 0 && _dy < 0) { bravo = M_PI + abs(bravo);}
			if (_dx > 0 && _dy < 0) { bravo = 2 * M_PI - abs(bravo);}
			*/
			float bravo = heading(X_lat, X_lon, B_lat,
					      B_lon) * (M_PI / 180.0);
			//cout << "bravo " << bravo << endl;


			//set point A
			float _x, _y;
			proj->get_xy(A_lat, A_lon, _x, _y);
			scantmp_xcoor.push_back(-_x);
			scantmp_ycoor.push_back(-_y);



			//if (bravo < 0) bravo += 2 * M_PI;

			//float winkeldelta = (M_PI / 50);

			//draw the arcs
			float _lat, _lon;

			//counterclockwise
			if (D == 1) {

				if (alpha < bravo) {
					for (float winkel =
					     alpha + RADSTEP;
					     winkel <= bravo - RADSTEP;
					     winkel = winkel + RADSTEP) {
						go(X_lat, X_lon,
						   (winkel /
						    (M_PI / 180.0)), R_,
						   _lat, _lon);
						proj->get_xy(_lat,
								     _lon,
								     _x,
								     _y);
						scantmp_xcoor.
						    push_back(-_x);
						scantmp_ycoor.
						    push_back(-_y);
					}
				} else {	// alpha > bravo

					for (float winkel =
					     alpha + RADSTEP;
					     winkel <= 2 * M_PI;
					     winkel = winkel + RADSTEP) {
						//scantmp_xcoor.push_back(X_x +( R_ * cos (winkel)) );
						//scantmp_ycoor.push_back(X_y +( R_ * sin (winkel)) );
						go(X_lat, X_lon,
						   (winkel /
						    (M_PI / 180.0)), R_,
						   _lat, _lon);
						proj->get_xy(_lat,
								     _lon,
								     _x,
								     _y);
						scantmp_xcoor.
						    push_back(-_x);
						scantmp_ycoor.
						    push_back(-_y);
					}
					for (float winkel = 0;
					     winkel <= bravo - RADSTEP;
					     winkel = winkel + RADSTEP) {
						//scantmp_xcoor.push_back(X_x +( R_ * cos (winkel)) );
						//scantmp_ycoor.push_back(X_y +( R_ * sin (winkel)) );
						go(X_lat, X_lon,
						   (winkel /
						    (M_PI / 180.0)), R_,
						   _lat, _lon);
						proj->get_xy(_lat,
								     _lon,
								     _x,
								     _y);
						scantmp_xcoor.
						    push_back(-_x);
						scantmp_ycoor.
						    push_back(-_y);
					}


				}
			}
			// clockwise
			if (D == -1) {
				//cout << "CLOCKWISE" << endl;
				if (alpha > bravo) {

					for (float winkel =
					     alpha - RADSTEP;
					     winkel >= bravo + RADSTEP;
					     winkel = winkel - RADSTEP) {
						//scantmp_xcoor.push_back(X_x +( R_ * cos (winkel)) );
						//scantmp_ycoor.push_back(X_y +( R_ * sin (winkel)) );
						go(X_lat, X_lon,
						   (winkel /
						    (M_PI / 180.0)), R_,
						   _lat, _lon);
						proj->get_xy(_lat,
								     _lon,
								     _x,
								     _y);
						scantmp_xcoor.
						    push_back(-_x);
						scantmp_ycoor.
						    push_back(-_y);
					}

				} else {

					for (float winkel =
					     alpha - RADSTEP; winkel >= 0;
					     winkel = winkel - RADSTEP) {
						//scantmp_xcoor.push_back(X_x +( R_ * cos (winkel)) );
						//scantmp_ycoor.push_back(X_y +( R_ * sin (winkel)) );
						go(X_lat, X_lon,
						   (winkel /
						    (M_PI / 180.0)), R_,
						   _lat, _lon);
						proj->get_xy(_lat,
								     _lon,
								     _x,
								     _y);
						scantmp_xcoor.
						    push_back(-_x);
						scantmp_ycoor.
						    push_back(-_y);
					}
					for (float winkel = 2 * M_PI;
					     winkel >= bravo + RADSTEP;
					     winkel = winkel - RADSTEP) {
						//scantmp_xcoor.push_back(X_x +( R_ * cos (winkel)) );
						//scantmp_ycoor.push_back(X_y +( R_ * sin (winkel)) );
						go(X_lat, X_lon,
						   (winkel /
						    (M_PI / 180.0)), R_,
						   _lat, _lon);
						proj->get_xy(_lat,
								     _lon,
								     _x,
								     _y);
						scantmp_xcoor.
						    push_back(-_x);
						scantmp_ycoor.
						    push_back(-_y);
					}


				}
			}
			//set point B
			//float _x, _y;
			proj->get_xy(B_lat, B_lon, _x, _y);
			scantmp_xcoor.push_back(-_x);
			scantmp_ycoor.push_back(-_y);

		}

		// DC
		//Circle
		if (strcmp(arg1, "DC") == 0) {

			float _radius;	//radius of airspace
			if (sscanf(zeile.c_str(), "%s %f", arg1, &_radius)
			    == 2) {
				_radius *= 1.852;	//convert to km.

				//if (conf->DEBUG.get())
                                //    cout << "Radius of circle: " << _radius << endl;


				float _lat, _lon, _x, _y;
				//float winkeldelta = M_PI / 50 ;
				int _dbg = 0;
				for (float winkel = 0; winkel < 2 * M_PI;
				     winkel = winkel + RADSTEP) {
					go(X_lat, X_lon,
					   (winkel / (M_PI / 180.0)),
					   _radius, _lat, _lon);
					proj->get_xy(_lat, _lon,
							     _x, _y);
					scantmp_xcoor.push_back(-_x);
					scantmp_ycoor.push_back(-_y);
					_dbg++;
					//scantmp_xcoor.push_back(X_x +( _radius * cos (winkel)) );
					//scantmp_ycoor.push_back(X_y +( _radius * sin (winkel)) );
				}
				//cout << "number of circle: "<<_dbg<< endl;
				//cout << "....: " << scantmp_xcoor.size() << endl;


			} else {
				cerr <<
				    "Error reading DC-Line (OpenAir file): "
				    << zeile << endl;
				exit(0);
			}

		}

		// Ceiling altitude and Floor altitude

		if (strcmp(arg1, "AH") == 0 || strcmp(arg1, "AL") == 0) {

			string _top_or_bottom = arg1;

			int _alt = -9999;
			string _unit = "no";


			// four args:   AH 50 00 MSL   (strange syntax)
			if ( (sscanf(zeile.c_str(), "%s %d %s %s", arg1, &_alt,
			     arg3, arg4) == 4)
			     && (strcmp(arg3, "00") == 0)  ) {
			     	_alt = _alt * 100;
				_unit = arg4;

				if (AIRDEBUG)
					cout << "4 args read (strange syntax) >>"<< zeile.c_str()
					 << "<<==>>" << _alt << "<==>" << _unit << endl;

				goto end;
			}

			// four args:   AH 5000 ft STD    or AH 5000 ft MSL
			if (sscanf
			    (zeile.c_str(), "%s %d %s %s", arg1, &_alt,
			     arg2, arg3) == 4) {

			     _unit = arg3;

				if (AIRDEBUG)
				 cout << "4 args read >>"<< zeile.c_str() << "<<==>>" <<
				   _alt << "<==>" << arg2 <<  "<==>" << arg3 << endl;

				if (strcasecmp(arg2, "ft") != 0) {
					cout << "UNRECOGNIZED string in Airspace >>" << arg2 << "<<" << endl;
					badcounter++;
					}

				goto end;

			}
			//  AH 5000 ft,  AH 5000 MSL, AH 5000 GND ...
			if (sscanf
			    (zeile.c_str(), "%s %d %s", arg1, &_alt,
			     arg2) == 3) {

			     if (AIRDEBUG)
			     cout << "3 args read >>" << zeile.c_str() << "<<==>>" <<
			     	 _alt << "<==>" << arg2 << "<=="<< endl;


				_unit = arg2;
				goto end;
			}
			// AH FL100
			if (sscanf
			    (zeile.c_str(), "%s %2s %d", arg1, arg2,
			     &_alt) == 3) {

			     if (AIRDEBUG)
			     cout << "Flightlevel read >>" << zeile.c_str() << "<<==>>"
			     << arg2 << "<==>" << _alt << endl;

				_unit = arg2;
				goto end;
			}
			// AH  5000     -> probably 0
			if (sscanf(zeile.c_str(), "%s %d", arg1, &_alt) ==
			    2) {
				_unit = "no";


				goto end;
			}
			// AL    GND,   AL SFC,   AH UNLIM, etc
			if (sscanf(zeile.c_str(), "%s %s", arg1, arg2) ==
			    2) {
				_unit = arg2;
				_alt = -9998;
				goto end;

			}

			cout << "FAILURE!!!!!!! UNRECOGNIZED ALTITUDE IN AIRSPACE" << endl;
			badcounter++;

		      end:


			if (_alt != -9999) {

				// Altitude QNH
				if (_unit == "AMSL" || _unit == "ft"
				    || _unit == "FT" || _unit == "ALT"
				    || _unit == "alt" || _unit == "MSL") {
					_alt =
					    int ((float (_alt) /
						  3.28) +0.5);
					_unit = "MSL";
					//cout << "QNH -------" << endl;
				} else
				// Altitude GND ? Was machen wir denn mal damit
				// Landscape.getSURFACE_elevation(?)
				// siehe auch in der drawAirspace routine
				if (_unit == "AGL" || _unit == "agl"
				    || _unit == "Agl" || (_unit == "GND"
							  && _alt !=
							  -9998)) {
					_alt =
					    int ((float (_alt) /
						  3.28) +0.5);
					_unit = "GND";
					//cout << "GND ---------" << endl;
				} else

				if (_unit == "SFC" || _unit == "sfc"
				    || _alt == 0 || (_unit == "GND"
						     && _alt == -9998)) {
					_alt = -200;	// plot down to -200...
					_unit = "MSL";
					//cout << "SURFACE ----------" << endl;
				} else

				if (_unit == "STD" || _unit == "std") {
					_alt =
					    int ((float (_alt) /
						  3.28) +0.5);
					_unit = "STD";

				} else
				// Flightlevels ;   FL = altitude  / 100     [ft above STD]
				if (_unit == "fl" || _unit == "FL") {
					_alt =
					    int ((float (_alt * 100) /
						  3.28) +0.5);
					_unit = "STD";
					//cout << "FLIGHTLEVEL ------------" << endl;

				} else

				if (_unit == "UNLIM" || _unit == "UNLTD"
				    || _unit == "UNLIMITED" || _unit == "UNL") {
					_alt = 30000;	//should be enough...
					_unit = "MSL";
					//cout << "UNLIMITED -----------" << endl;

				} else {

					if (conf->VERBOSE.get()) {
					cerr << "Elevation unit unknown!!! >>" << _unit <<
					"<< ==> in line: " << zeile << endl;
					}
					badcounter++;

				}


			} else {
				cerr << "AL or AH  Line not parsed :-( " <<
				     " Line ->" << zeile << "<-" <<
				    endl;

				 //   << "Please send this output to: " << MAIL << endl;

				//exit(1);
			}


			if (_top_or_bottom == "AH") {
				scantmp_ah = _alt;
				scantmp_ah_unit = _unit;
			}

			if (_top_or_bottom == "AL") {
				scantmp_al = _alt;
				scantmp_al_unit = _unit;
			}


		}



	}

	if (conf->VERBOSE.get()) {
		cout << "Errors reading Airspacefile: " << badcounter <<endl;
	}
}

float Airspace::coor2dec(char *coorstring, char *NESWstring)
{

	float dec_coor = -9999;

	int degres = 0;
	int minutes = 0;
	int seconds = 0;
	float decminutes = 0;

	if (sscanf(coorstring, "%d:%d:%d", &degres, &minutes, &seconds) ==
	    3) {
		dec_coor =
		    (float) degres + ((float) minutes / 60.0) +
		    ((float) seconds / 3600.0);
		if (strcmp(NESWstring, "S") == 0
		    || strcmp(NESWstring, "W") == 0)
			dec_coor *= -1;
		return dec_coor;
	}

	if (sscanf(coorstring, "%d:%f", &degres, &decminutes) == 2) {
		dec_coor = (float) degres + (decminutes / 60.0);
		if (strcmp(NESWstring, "S") == 0
		    || strcmp(NESWstring, "W") == 0)
			dec_coor *= -1;
		return dec_coor;


	}

	return dec_coor;

}


void Airspace::drawAirspace(bool BW)
{


	// use blending
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//depth buffer read-only         (for transparent objects with blending)
	glDepthMask(GL_FALSE);



	// _n is the number of Airspaces read from Airspace file
	int _n = (int) type.size();

	// _m will be the number of polygon_points in one Airspace
	int _m;

	//loop over all airspaces
	for (int _z = 0; _z < _n; _z++) {



		_m = (int) xcoor[_z].size();
		//if (_m == 2) continue;

		bool DRAW = false;

		//default color for non classified airspaces
		// red almost opaque
		float col[4] = {1.0, 0.0, 0.0, 0.7};


		if (lower[_z] > (  ((float)conf->AirspaceLimit.get()*100.0)/3.28)   ) {
			continue;
		}


		//set color for type of airspace     R restricted, P prohibited, Q danger
		// and check if we should draw this type of airspace
                                // we need to continue before anything is drawn!
		if (type[_z] == "R") {

			if (!conf->AIRSPACE_R.get()) {continue;}
			col[0] = 0.0;
			col[1] = 0.0;
			col[2] = 1.0;
			col[3] = 0.2;
		}

		if (type[_z] == "Q") {

			if (!conf->AIRSPACE_Q.get()) {continue;}
			col[0] = 0.0;
			col[1] = 0.0;
			col[2] = 1.0;
			col[3] = 0.2;
		}

		if (type[_z] == "P") {

			if (!conf->AIRSPACE_P.get()) {continue;}
			col[0] = 0.0;
			col[1] = 0.0;
			col[2] = 1.0;
			col[3] = 0.2;
		}



		if (type[_z] == "CTR") {
			if (!conf->AIRSPACE_CTR.get()) {continue;}
			col[0] = 0.7;
			col[1] = 0.0;
			col[2] = 0.6;
			col[3] = 0.2;
		}

		if (type[_z] == "C") {
			if (!conf->AIRSPACE_C.get()) {continue;}
			col[0] = 0.0;
			col[1] = 0.7;
			col[2] = 0.0;
			col[3] = 0.2;
		}

		// D is same as ctr
		if (type[_z] == "D") {
			if (!conf->AIRSPACE_D.get()) {continue;}
			col[0] = 0.7;
			col[1] = 0.0;
			col[2] = 0.6;
			col[3] = 0.2;
		}



		// loop over all polygon-points of airspce _z
		//to check, if we need to draw it
		for (int _p = 0; _p < _m; _p++) {

			// if one single point is in limits, set DRAW to true

			//float _lat_ddist =  center_lat -   latmax;
			//float _lon_ddist = center_lon -   lonmax;
			float xkmmax;	// = - _lon_ddist * DEGDIST *    cos (PI_180 * latmax);
			float ykmmax;	// = - _lat_ddist * DEGDIST;

			proj->get_xy(latmax, lonmax, xkmmax,
					     ykmmax);

			xkmmax *= -1.0;
			ykmmax *= -1.0;

			float xkmmin = xkmmax * -1.0;
			float ykmmin = ykmmax * -1.0;

			if (ycoor[_z][_p] < ykmmax
			    && ycoor[_z][_p] > ykmmin
			    && xcoor[_z][_p] < xkmmax
			    && xcoor[_z][_p] > xkmmin)
				DRAW = true;
		}



		if (DRAW) {



			float buttom = lower[_z] / 1000.0;
			float top = upper[_z] / 1000.0;

			if (conf->DEBUG.get())  {
			    cout << "-----------------------------" << endl;
			    cout << "Airspace no. " << _z << ", Type: " << type[_z] << " --> " << name[_z] << endl;
			    cout << buttom << " <-> "<< top << endl;

			    if (buttom < -0.2001)
			    	cout << "EEEERRRRROOOORRRR!!!!!! No altitude for airspace lower limit!" << endl;
			    if (top < -0.2001)
			    	cout << "EEEERRRRROOOORRRR!!!!!! No altitude for airspace upper limit!" << endl;


			}

			if (conf->DEBUG.get()) {
                                    cout << "Numbers of points defining the airspace (lat/lon): " << lat[_z].size() << endl;
                                    cout << "Numbers of points defining the airspace (x/y): " << xcoor[_z].size() << endl;
				}

			// if Altitude is above ground...
			if (upper_unit[_z] == "GND") {

                                if (conf->DEBUG.get())
				cout <<  "GND... detected  altering altitude (upper)" << endl;

				//create vec
				vector < int >alt_tmp;



				// check if we have lat/lon of airspace points
				// in case of a circle-only (cylindric) airspace, we have the generated x/y only!
				if ((int) lat[_z].size() > 0) {
				    alt_tmp.resize(lat[_z].size());

				    // braucht coordinaten....
				    get_gnd_elevations(&alt_tmp, &lat[_z],&lon[_z], conf);

				} else {	// airspaces with only  V X= and DC (no lat/lon DP)
				    alt_tmp.resize(xcoor[_z].size());

				    vector<float> tmp_lat;
				    vector<float> tmp_lon;
				    tmp_lat.resize(xcoor[_z].size());
				    tmp_lon.resize(xcoor[_z].size());

				    if (conf->DEBUG.get())
					cout << "Airspace "<<name[_z]<<" is circle only..." << endl;

				    for (int tmp=0; tmp < (int) xcoor[_z].size(); tmp++) {
					proj->get_latlon(xcoor[_z][tmp], ycoor[_z][tmp], tmp_lat[tmp], tmp_lon[tmp]);
					//cout << tmp_lat[tmp] << "  " << tmp_lon[tmp] << endl;
				    }

				    get_gnd_elevations(&alt_tmp, &tmp_lat,&tmp_lon, conf);
				}



				float _gnd_alt_tmp = 0;
				for (unsigned int _q = 0;
				     _q < alt_tmp.size(); _q++) {

					if (alt_tmp[_q] > -16384 && alt_tmp[_q] != -9999) {
					_gnd_alt_tmp += alt_tmp[_q];
					}

					if (conf->DEBUG.get())
					cout << alt_tmp[_q] << " " << endl;
				}
				_gnd_alt_tmp /= alt_tmp.size();

				if (conf->DEBUG.get())
				cout << "Orignal (GND)  altitude: " << top ;

				top += (_gnd_alt_tmp / 1000.0);

				if (conf->DEBUG.get())
				cout << " altered to MSL: " << top << endl <<
					" Sum of groundpoints is:" << _gnd_alt_tmp << "  no points: " << alt_tmp.size() << " added: " << (_gnd_alt_tmp / 1000.0) << endl;
			}
			// if Altitude is above ground...
			if (lower_unit[_z] == "GND") {

				if (conf->DEBUG.get())
					cout << "GND... detected  altering altitude (lower)" << endl;
				//create vec

				vector < int >alt_tmp;


				//cylindric airspace
				if ((int)lat[_z].size() > 0) {
				    alt_tmp.resize(lat[_z].size());

				    // braucht coordinaten....
				    get_gnd_elevations(&alt_tmp, &lat[_z],&lon[_z], conf);

				} else {	// airspaces with only  V X= and DC (no lat/lon DP)
				    alt_tmp.resize(xcoor[_z].size());

				    vector<float> tmp_lat;
				    vector<float> tmp_lon;
				    tmp_lat.resize(xcoor[_z].size());
				    tmp_lon.resize(xcoor[_z].size());

				    if (conf->DEBUG.get())
					cout << "Airspace "<<name[_z]<<" is circle only..." << endl;

				    for (int tmp=0; tmp < (int) xcoor[_z].size(); tmp++) {
					proj->get_latlon(xcoor[_z][tmp], ycoor[_z][tmp], tmp_lat[tmp], tmp_lon[tmp]);
				    }

				    get_gnd_elevations(&alt_tmp, &tmp_lat,&tmp_lon, conf);
				}


				float _gnd_alt_tmp = 0;
				for (unsigned int _q = 0;
				     _q < alt_tmp.size(); _q++) {
					_gnd_alt_tmp += alt_tmp[_q];

					if (conf->DEBUG.get())
						cout << alt_tmp[_q] << " " << endl;
				}
				_gnd_alt_tmp /= alt_tmp.size();

				if (conf->DEBUG.get())
				cout << "Orignal (GND) altitude: " << buttom ;

				buttom += (_gnd_alt_tmp / 1000.0);

				if (conf->DEBUG.get())
				cout << "altered to MSL: " << buttom << endl;
			}



			// at first we plot  black wireframes (but only vertical boundaries (outlines)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glColor3f(conf->airspace_wire_color_r.get(), conf->airspace_wire_color_g.get(), conf->airspace_wire_color_b.get());
			glLineWidth(conf->airspace_wire_width.get());
//			glColor3fv(conf->)
			glBegin(GL_QUAD_STRIP);
			//cout << "---------------" << endl;
			for (int _p = 0; _p < _m; _p++) {


				//cout << xcoor[_z][_p] << "    " << ycoor[_z][_p] << endl;

				glVertex3f(xcoor[_z][_p], ycoor[_z][_p],
					   buttom);
				glVertex3f(xcoor[_z][_p], ycoor[_z][_p],
					   top);

			}

			// first point .... to close

			glVertex3f(xcoor[_z][0], ycoor[_z][0], buttom);
			glVertex3f(xcoor[_z][0], ycoor[_z][0], top);

			glEnd();
			// END of black wireframes;

			//switch back to filled polygons
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (!conf->AIRSPACE_WIRE.get()) {

			if (!BW) {
				glColor4fv(col);
			} else {
				glColor4fv(graycolor(col));
			}


			// draw outlines (this time coloured and transparent)
			glBegin(GL_QUAD_STRIP);
			for (int _p = 0; _p < _m; _p++) {



				glVertex3f(xcoor[_z][_p], ycoor[_z][_p],
					   buttom);
				glVertex3f(xcoor[_z][_p], ycoor[_z][_p],
					   top);

			}

			// first point .... to close


			glVertex3f(xcoor[_z][0], ycoor[_z][0], buttom);
			glVertex3f(xcoor[_z][0], ycoor[_z][0], top);

			glEnd();



			// draw ceiling

			//this is needed, because the GLU_TESS_VERTEX callback will return pointers to this
			// no airspace will have more than thousand polygon-points. (I hope)
			GLdouble coor[1000][3];

			GLUtesselator *tess;
			tess = gluNewTess();


			// the (void(*)()) typecasts are needed, because of differences between c and c++ !!!
			// windows needs (void(__stdcall *)())

// why ifndes ONLY_OSMESA?!
//#define CALLBACK
//#ifndef ONLY_OSMESA
//#ifdef __WIN32__
//#define CALLBACK __stdcall
//#endif
//#endif

#ifdef __WIN32__
#define CALLBACK __stdcall
#else
#define CALLBACK
#endif

// on matthews ppc mac this seems to be needed (its a 10.4.11?)

#ifdef __tripledotfix__
#define MACTRICK ...
#else
#define MACTRICK
#endif
			gluTessCallback(tess, GLU_TESS_VERTEX,
					(void (CALLBACK *)(MACTRICK)) glVertex3dv);
			gluTessCallback(tess, GLU_TESS_BEGIN,
					(void (CALLBACK *) (MACTRICK)) glBegin);
			gluTessCallback(tess, GLU_TESS_END,
					(void (CALLBACK *)(MACTRICK))	glEnd);
			gluTessCallback(tess, GLU_TESS_ERROR,
					(void (CALLBACK *) (MACTRICK)) errorCallback);

			gluTessBeginPolygon(tess, NULL);
			gluTessBeginContour(tess);

			for (int _p = 0; _p < _m; _p++) {

				coor[_p][0] = xcoor[_z][_p];	//_xkmdist;
				coor[_p][1] = ycoor[_z][_p];	//_ykmdist;
				coor[_p][2] = top;

				gluTessVertex(tess, coor[_p], coor[_p]);

				//cout << "tess" << ": " << coor[_p][0] << " " << coor[_p][1] << " " << coor[_p][2] << endl;

			}

			//cout << "TESS" << endl;
			gluTessEndContour(tess);
			gluTessEndPolygon(tess);
			// we will delete this later (after we plotted to bottom)
			//gluDeleteTess(tess);



			//draw floor if alt != 0
			if (lower[_z] != 0) {
				gluTessBeginPolygon(tess, NULL);
				gluTessBeginContour(tess);

				for (int _p = 0; _p < _m; _p++) {


					coor[_p][0] = xcoor[_z][_p];	//_xkmdist;
					coor[_p][1] = ycoor[_z][_p];	//_ykmdist;
					coor[_p][2] = buttom;

					gluTessVertex(tess, coor[_p],
						      coor[_p]);

					//cout << "tess" << endl;
					//cout << "tess button" << ": " << coor[_p][0] << " " << coor[_p][1] << " " << coor[_p][2] << endl;

				}

				//cout << "TESS" << endl;
				gluTessEndContour(tess);
				gluTessEndPolygon(tess);
			}
			gluDeleteTess(tess);

		}

	}

	}


	//glPopAttrib();
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ZERO);

        glDisable(GL_COLOR_MATERIAL);
	glLineWidth(1.0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT, GL_FILL);

}
