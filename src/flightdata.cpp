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

#include "flightdata.h"
#include "GLexplorer.h"
#include "oglexfunc.h"
#include "config.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <string>
#include <errno.h>
#include <new>

#ifndef __OSX__
#include <GL/glu.h>
#else
#include <OpenGL/glu.h>
#endif

#ifdef HAVE_LIBGPS
#include <libgpsmm.h>

#if GPSD_API_MAJOR_VERSION != 5
#undef HAVE_LIBGPS
#endif

#endif

//const std::string rcsid_flightdata_cpp =
//    "$Id: flightdata.cpp 3 2014-07-31 09:59:20Z kruegerh $";
//const std::string rcsid_flightdata_h = FLIGHTDATA_H;


Flightdata::Flightdata()
{
	n = 0;
	cycles = -1;
	consecutive_valid_fixes = 1;
	gps_interruptions = 0;
}

Flightdata::~Flightdata()
{
//        delete gps;
}


void Flightdata::initgps(const char* _serv, const char* _port)
{
#ifdef HAVE_LIBGPS


	if (conf_pointer->VERBOSE.get())
		cout << "Trying to connect to gpsd at "<< _serv << ":" << _port << endl;

	stringstream _servstring;
	_servstring << _serv << ":" << _port;
	serverstring = _servstring.str();

// #if GPSD_API_MAJOR_VERSION == 5
 	//good
// #else
//  	cerr << "unsupported GPSD_API_MAJOR_VERSION == " << GPSD_API_MAJOR_VERSION << endl;
// 	exit(0);
// #endif

// #if GPSD_API_MAJOR_VERSION == 5
	// create gpsmm object
	gps = new gpsmm (_serv, _port);


	if (gps->stream(WATCH_ENABLE|WATCH_JSON) == NULL) {
	  cerr << "cannot connect to gpsd" << endl;
	  exit (0);
	}

	// waiting is timeout in mu-secs // blocking!
	while (!gps->waiting(20000000)) { // 20 secs is ok at initialisation
	 cout << "no answer gps->waiting, after 20e6 mu-secs!" << endl;
	 exit (0);
	}

	// data is here, so poll it!
	if ( (gpsdata = gps->read()) == NULL) {
	    cerr << "read error" << endl;
	    exit(0);
	}

	if (!gpsdata) {
		cerr << "connection to gpsd failed! Error number " << errno << endl;
		exit(0);

	} else {

		if (conf_pointer->VERBOSE.get())
		  cout << "connected to gpsd!"<< endl;
		  //<< gpsdata->dev.ndevices << " devices connected to gpsd" <<endl;

		if (!gpsdata->set == DEVICEID_SET) {
			cerr << "No GPS device connected to gpsd" << endl;
			exit(0);
		} else {
			if (conf_pointer->DEBUG.get()) {
				//cout << "first device connected at: " << gpsdata->devicelist[0] << endl;
 				//cout << "Device: >" << gpsdata->dev.path << "< >" << gpsdata->dev.subtype << "<" << endl;
 				// there is no device info yet...
 				cout << "gpsd API: " << gpsdata->version.proto_major << "." << gpsdata->version.proto_minor << endl;
 				cout << "external version (release): " << gpsdata->version.release << endl;
 				cout << "internal revision    (rev): " << gpsdata->version.rev << endl;

				cout << "Version Info from headers:" << endl <<
				  "GPSD_API_MAJOR_VERSION: "<< GPSD_API_MAJOR_VERSION << endl <<
				  "GPSD_API_MINOR_VERSION: "<< GPSD_API_MINOR_VERSION <<  endl <<
				  "sizeof gps_data_t  (8240 on pc12): " << sizeof (gps_data_t) << endl;

			}
		}
	}

	// waiting for the first fix! (we need one before we can start)
	// otherwise we have no idea were we are!
	int wait=0;
	int i, recv;

	again:
	while (gpsdata->fix.mode < 2) {

		// try every second, till we get an 2D fix
		//sleep (1);

		// das parsen in gpsdata vertraegt nur 1 query-letter
		//gpsdata = gps->query("J=1\n");
		//gpsdata = gps->query("oyq\n");
		while( !gps->waiting(500000) ) {};
		gpsdata = gps->read();

		recv=0;
		if (gpsdata->satellites_visible) {
		  if (gpsdata->satellites_visible < MAXCHANNELS) { // filter bad/large satellites_visible!
			for (i = 0; i < gpsdata->satellites_visible; i++) {
				if (gpsdata->ss[i] > 20) recv++;
			}
		  } //else { cerr << "sat_visible >= MAXCHANNELS/" << MAXCHANNELS << " this would cause trouble!"<< endl;}
		}

		if (conf_pointer->VERBOSE.get())
		  cout << "Waiting ("<<wait++<<" polls) for first fix (mode="<<gpsdata->fix.mode<<"). Satellites view/recv/used: "
		    << gpsdata->satellites_visible << "/"
		    << recv << "/" << gpsdata->satellites_used << "       \r" << flush;

		int timeout = 180;
		if (wait > timeout) {
			cerr <<endl<< "timeout. No 2D fix after " << timeout << " polls" << endl;
			exit(0);
		}
	}


	// now we have a fix!
	if ( isnan(gpsdata->fix.latitude) || isnan(gpsdata->fix.longitude) || gpsdata->fix.longitude > 180.0 || gpsdata->fix.longitude < -180.0
		|| gpsdata->fix.latitude > 90.0 || gpsdata->fix.latitude < -90.0
	) {
		cerr << endl << "Initfix is fscked. I'll try again!" << flush << endl;
		goto again;
	}

	if (conf_pointer->VERBOSE.get()) {
	  cout << endl << "InitfiX: " << gpsdata->fix.latitude << "  " << gpsdata->fix.longitude << flush << endl ;
	  //if (gpsdata->set & DEVICELIST_SET) { // keine ahnung warum das nicht gesetzt ist?
	    int i;
	    cout << "DEVICELIST: number of devs: " << gpsdata->devices.ndevices << endl;
	    for (i = 0; i < gpsdata->devices.ndevices; i++) {
		cout << "no: " << gpsdata->devices.ndevices
                     << "  path="<<  gpsdata->devices.list[i].path
                     << "  driver=" << gpsdata->devices.list[i].driver << endl;
	    }
	  //}
	}


	proj_pointer->set_center_lat(gpsdata->fix.latitude);
	proj_pointer->set_center_lon(gpsdata->fix.longitude);

	center_lat = gpsdata->fix.latitude;
	center_lon = gpsdata->fix.longitude;
	center_alt = 0;

	float _alt=0;


	if ( isnan(gpsdata->fix.altitude) || gpsdata->fix.altitude < -9000.0 || gpsdata->fix.altitude > 20000.0 ) {
		_alt = 0;
		if (conf_pointer->VERBOSE.get())
		cout << "initalt not used = " << _alt << endl;
	} else {
		_alt = gpsdata->fix.altitude;
		if (conf_pointer->VERBOSE.get())
		cout << "initalt = " << _alt << endl;
	}


	center_gps = _alt;

	latmax = center_lat;
	latmin = center_lat;

	lonmin = center_lon;
	lonmax = center_lon;

	altmax = 0;
	altmin = 0;

	gpsmax = center_gps;
	gpsmin = center_gps;

	conf_pointer->GPSALT.on();


	xmin=0; xmax=0; ymin=0; ymax=0;
	zmax= center_gps / 1000.0;
	zmin= center_gps / 1000.0;
	speedmax=0; speedmin=100; variomax=0; variomin=0;

	while (querygps() != 0) {
	//	if (conf_pointer->VERBOSE.get())
	//		cout << "FIRSTFIX. read invalid...." << endl;
	} ; //at least one fix has to be in the list before calling setinitpos etc


	// here action after first fix
	// set centers, projection

	// was ist wenn --lat --lon gegeben sind?
	// was ist wenn igc-file gegeben ist?

	// function Flighdata::update gps  (auch drawing liste)
	// schreiben für logger...!?
	// _> flightdata in gpsaltmode

#endif
	cerr << "Sorry. Support for gpsd wasn't included" << endl;
	exit (1);

}


int Flightdata::querygps(void)
{
  // return 0, if a new fix is added
#ifdef HAVE_LIBGPS
	//gpsdata = gps->query("oqm\n");  //should be read in "o" as well  //old from gpsd 2.39

	// poll blocks! so check first if new data is available!
	if (gps->waiting(1)) {gpsdata=gps->read();} else {return (1);}


	// mode valid, and new time
	if (gpsdata->fix.mode > 1 && (int)(gpsdata->fix.time+0.5) != oldgpstime) {


		if ( isnan(gpsdata->fix.time) || isnan(gpsdata->fix.latitude) || isnan(gpsdata->fix.longitude) || gpsdata->fix.longitude > 180.0 || gpsdata->fix.longitude < -180.0
		|| gpsdata->fix.latitude > 90.0 || gpsdata->fix.latitude < -90.0) {
			if (conf_pointer->DEBUG.get())
				cerr << "This fix is fscked! time, lat or lon NaN! or out of range" << flush << endl;
			// no fix, nothing to do
			consecutive_valid_fixes = 0;
			return (1);
		}

		// I havent found this cases...yet
		// check .. if speed != 0, but position is the same...
		// this should not happen, otherwise position seems to be invalid (or speed)
		if (lat.size() > 1) {
		if (gpsdata->fix.speed != 0 && gpsdata->fix.longitude == lon[lon.size()-1] && gpsdata->fix.latitude == lat[lat.size()-1]) {
			if (conf_pointer->DEBUG.get())
				cerr << "Speed != 0, but same coordinates, skipping!" << flush << endl;
				consecutive_valid_fixes = 0;
				return (1);
		}
		//cout << "New coordinates: " << gpsdata->fix.latitude << ", " << gpsdata->fix.longitude <<
		//	" Old coordinates: " << lat[lat.size()-1] << ", " << lon[lon.size()-1] << flush << endl;
		}

		// a valid position fix with 2 or less satellites is not possible.
		// my Geko301 gives positions like that (track extrapolation) e.g. tunnel
		int sat_limit = 2;
		if (gpsdata->satellites_used <= sat_limit) {
			if (conf_pointer->DEBUG.get())
				cerr << "Satellites used <="<< sat_limit <<"!!!  The GPS device is kidding us! Ignoring this fix!" << flush << endl;
				consecutive_valid_fixes = 0;
				return (1);
		}

		//skip first x fixes after invalids...
		// int _skips = 1;
		if (consecutive_valid_fixes < 1) {
			if (conf_pointer->DEBUG.get())
				cerr << "Skipping first fix after a series of invalids!" << flush << endl;
			consecutive_valid_fixes++;
			// set oldtime otherwise we end up here with the same fix again!
			oldgpstime = (int)(gpsdata->fix.time+0.5);

			// carefull here. works only with _skips==1 !!!
			gps_interruptions++;

			return (1);
		}

		consecutive_valid_fixes ++;

		// Valid position fix !!!
		n++;
		lat.push_back(gpsdata->fix.latitude);
		lon.push_back(gpsdata->fix.longitude);

		// take the larger value of epx epy
		if (gpsdata->fix.epy > gpsdata->fix.epx) {eph.push_back(gpsdata->fix.epy);}
		  else {eph.push_back(gpsdata->fix.epx);}
//		eph.push_back(0);


		float _alt = 0;

		// with altitude!
		if (gpsdata->fix.mode == MODE_3D) {

			// invalid altitude in 3D fix
			if ( isnan(gpsdata->fix.altitude) || gpsdata->fix.altitude < -9000 || gpsdata->fix.altitude > 20000 ) {
				cerr << "Mode=3d, but altitude is fscked!!!! Skipping alt." << flush << endl;
				// _alt is = 0 already
				vario.push_back(0);
				epc.push_back(0);
				epv.push_back(0);
			} else {
				_alt = gpsdata->fix.altitude;
				vario.push_back(gpsdata->fix.climb);
				epc.push_back(gpsdata->fix.epc); // rubbish!
				epv.push_back(gpsdata->fix.epv);
			}
		// without alt
		} else {
			//
			vario.push_back(0);
			epc.push_back(0);
			epv.push_back(0);
		}


		alt.push_back(0); // baro; always = 0 as long as there is no baro item in gps_data

		gpsalt.push_back(_alt);
		if ( _alt > gpsmax) gpsmax = _alt;
		if ( _alt < gpsmin) gpsmin = _alt;

		satused.push_back(gpsdata->satellites_used);
		satvis.push_back(gpsdata->satellites_visible);
		gpsmode.push_back(gpsdata->fix.mode);

		float _x,_y;
		proj_pointer->get_xy(gpsdata->fix.latitude, gpsdata->fix.longitude, _x, _y);
		x.push_back(-_x);
		y.push_back(-_y);

		z.push_back( _alt / 1000.0);
		if ( (_alt / 1000.0) > zmax) zmax = (_alt / 1000.0);
		if ( (_alt / 1000.0) < zmin) zmin = (_alt / 1000.0);


		if (conf_pointer->GPSDGNDELEV.get() && conf_pointer->DEM.get()) {
			vector <int> _xalt;
			vector <float> _xlat;
			vector <float> _xlon;
			_xalt.clear();
			_xlon.clear();
			_xlat.clear();

			_xlat.push_back(gpsdata->fix.latitude);
			_xlon.push_back(gpsdata->fix.longitude);
			_xalt.push_back(0);

			get_gnd_elevations(&_xalt, &_xlat, &_xlon, conf_pointer);
			gnd_elevation.push_back(_xalt[0]);

		} else {
			gnd_elevation.push_back(0);
		}


		/*
		andere min/max xy und lat lon usw, vario...
		werden zZt nicht upgedated. ist vielleicht auch nicht noetig?!???
		*/

		speed.push_back(gpsdata->fix.speed * 3.6);
		eps.push_back( (gpsdata->fix.eps*3.6) ); // seems to be rubbish!
		if ( (gpsdata->fix.speed*3.6) > speedmax  ) speedmax = (gpsdata->fix.speed*3.6);
		if ( (gpsdata->fix.speed*3.6) < speedmin  ) speedmin = (gpsdata->fix.speed*3.6);

		//cout << "spee min/max " << speedmin << "  " << speedmax  << endl;

		struct tm *t;
		time_t tt = (int)(gpsdata->fix.time+0.5);
		oldgpstime = (int)(gpsdata->fix.time+0.5);
		t = gmtime(&tt);

		dectime.push_back((double) t->tm_hour +
					  ((double) t->tm_min / 60.0) +
					  ((double) t->tm_sec / 3600.0));
			char buf[80];
			float hh = t->tm_hour + conf_pointer->getTimeZone();
			if (hh < 0) hh += 24;
			if (hh >= 24  && (t->tm_min > 0 || t->tm_sec > 0)) hh -= 24;

			sprintf(buf, "%02d:%02d:%02d", (int) hh, (int) t->tm_min,
				(int) t->tm_sec);
			time.push_back(buf);

		if (conf_pointer->DEBUG.get())
		cout << "GPS: M=" << gpsdata->fix.mode << " su=" << gpsdata->satellites_used <<
		" " << buf << " alt=" << gpsdata->fix.altitude << "m  lat=" << gpsdata->fix.latitude <<
		" lon=" << gpsdata->fix.longitude << flush << endl;

		// hier noch centers und max/min updaten!
		// evtl verschiede return-werte für landscape update


		return (0); //success!
	} else {
		if (gpsdata->fix.mode <= 1)
			consecutive_valid_fixes=0;
		return (1);
	}
#else
return(0);
#endif
}


int Flightdata::readIGCFile(string _filename, bool USE_V)
{
	Timecheck readigc("ReadIGC",conf_pointer);
	ifstream igcfile;
	igcfile.open(_filename.c_str());

	if (!igcfile) { cerr << "error opening :" << _filename << endl; exit (1);}

	n = 0;

	// nicht noetig, oder?
	x.clear();
	y.clear();
	z.clear();
	time.clear();
	dectime.clear();
	gnd_elevation.clear();
	gpsalt.clear();
	alt.clear();
	lat.clear();
	lon.clear();
	gpsmode.clear();
	satused.clear();


	int hh,
	    mm,
	    ss,
	    latdeg,
	    latminute,
	    latmindec, londeg, lonminute, lonmindec, _alt, _gpsalt;
	char entryindex, northsouth, eastwest, valid;
	double ns = 1.0;
	double ew = 1.0;

	char zeile[180];

	double _latsum = 0;
	double _lonsum = 0;
	double _altsum = 0;
	double _gpssum = 0;

	int invalidcounter = 0;
	int b_line_counter = 0;
	int line_counter = 0;

	while (igcfile) {
		igcfile.getline(zeile, 180, '\n');

		line_counter++;

		//cout << zeile << endl;

		if (sscanf(zeile, "%1s", &entryindex) == 1
		    && entryindex == 'B') {

			b_line_counter++;


			if (sscanf(zeile,
			       "%*c%2d%2d%2d%2d%2d%3d%c%3d%2d%3d%c%c%5d%5d",
			       &hh, &mm, &ss, &latdeg, &latminute,
			       &latmindec, &northsouth, &londeg,
			       &lonminute, &lonmindec, &eastwest, &valid,
			       &_alt, &_gpsalt) != 14) {

				       if(conf_pointer->VERBOSE.get()){
					       cout << "error parsing B record: " << zeile << endl;
				       }


			}

			if (valid == 'V')
				invalidcounter++;

			if (!USE_V) {
			       if (valid == 'V')
				continue;
			}

			if (northsouth == 'N')
				ns = 1.0;
			else
				ns = -1.0;
			lat.push_back(ns *
				      ((double) latdeg +
				       ((double) latminute / 60.0) +
				       ((double) latmindec / 60000.0)));
			_latsum =
			    _latsum +
			    (ns *
			     ((double) latdeg +
			      ((double) latminute / 60.0) +
			      ((double) latmindec / 60000.0)));
			if (eastwest == 'E')
				ew = 1.0;
			else
				ew = -1.0;
			lon.push_back(ew *
				      ((double) londeg +
				       ((double) lonminute / 60.0) +
				       ((double) lonmindec / 60000.0)));
			_lonsum =
			    _lonsum +
			    (ew *
			     ((double) londeg +
			      ((double) lonminute / 60.0) +
			      ((double) lonmindec / 60000.0)));
			alt.push_back((double) _alt);
			_altsum = _altsum + (double) _alt;
			gpsalt.push_back((double) _gpsalt);
			_gpssum = _gpssum + (double) _gpsalt;

			gnd_elevation.push_back(0);

			// these ones are unused in igc-file mode
			//
			gpsmode.push_back(0);
			satused.push_back(0);
			epc.push_back(0);
			eps.push_back(0);
			epv.push_back(0);
			eph.push_back(0);

			dectime.push_back((double) hh +
					  ((double) mm / 60.0) +
					  ((double) ss / 3600.0));
			char buf[80];

			hh += conf_pointer->getTimeZone();
			if (hh < 0) hh += 24;
			if (hh >= 24  && (mm > 0 || ss > 0)) hh -= 24;

			sprintf(buf, "%02d:%02d:%02d", (int) hh, (int) mm,
				(int) ss);
			time.push_back(buf);

			n++;
			//cout << n << "debug---" << endl;
		}

	}
	//cout << "ENDE DES EINLES" << endl;

	igcfile.close();


	if (conf_pointer->VERBOSE.get()) {
		cout << line_counter << " lines of IGC-file read" << endl;
		cout << b_line_counter << " lines of B-records recognized" << endl;
	}

	if (conf_pointer->VERBOSE.get() && USE_V) {
		cout << "Invalid position fixes: " << invalidcounter << endl;
	}

	center_lat = _latsum / (double) n;	//startwerte
	center_lon = _lonsum / (double) n;

	//cout << "altsum  " << _altsum << endl;
	center_alt = _altsum / (double) n;
	//cout << "center alt " << center_alt << endl;
	center_gps = _gpssum / (double) n;


	//startwerte fuer maxima/minima
	latmax = center_lat;
	latmin = center_lat;
	lonmin = center_lon;
	lonmax = center_lon;
	altmax = center_alt;
	altmin = center_alt;
	gpsmax = center_gps;
	gpsmin = center_gps;

	// determination of maxima and minima in original data
	for (int _q = 0; _q < n; _q++) {
		if (lat[_q] < latmin)
			latmin = lat[_q];
		if (lat[_q] > latmax)
			latmax = lat[_q];
		if (lon[_q] < lonmin)
			lonmin = lon[_q];
		if (lon[_q] > lonmax)
			lonmax = lon[_q];
		if (alt[_q] < altmin)
			altmin = alt[_q];
		if (alt[_q] > altmax)
			altmax = alt[_q];
		if (gpsalt[_q] < gpsmin)
			gpsmin = gpsalt[_q];
		if (gpsalt[_q] > gpsmax)
			gpsmax = gpsalt[_q];
	}

	if (conf_pointer->VERBOSE.get()) {
	 cout << "IGC-File boundaries: " << latmax << "  " << latmin << "   " << lonmin << "  " <<lonmax << endl;
	}

	//real centers
	center_lat = (latmin + latmax) / 2;
	center_lon = (lonmin + lonmax) / 2;
	center_alt = (altmin + altmax) / 2;
	center_gps = (gpsmin + gpsmax) / 2;

	//set in projection object too
	proj_pointer->set_center_lat(center_lat);
	proj_pointer->set_center_lon(center_lon);


	//calculate x,y and z values    (now using the projection object)!
	for (int _q = 0; _q < n; _q++) {

		float _ykmdist, _xkmdist;
		proj_pointer->get_xy(lat[_q], lon[_q], _xkmdist, _ykmdist);

		x.push_back(-_xkmdist);
		y.push_back(-_ykmdist);
		//z is altitude in km
		z.push_back(alt[_q] / 1000.0);
		//cout << _q << "debuggy 3te schleife" << endl;
	}

	// startwerte fuer xyz max und minima
	xmin = 0;
	ymin = 0;
	zmin = center_alt / 1000.0;
	xmax = 0;
	ymax = 0;
	zmax = center_alt / 1000.0;

	for (int _i = 0; _i < n; _i++) {
		if (x[_i] < xmin) {xmin = x[_i];}
		if (x[_i] > xmax) {xmax = x[_i];}
		if (y[_i] < ymin) {ymin = y[_i];}
		if (y[_i] > ymax) {ymax = y[_i];}
		if (z[_i] < zmin) {zmin = z[_i];}
		if (z[_i] > zmax) {zmax = z[_i];}
	}

	xcenter = (xmin + xmax) / 2;
	ycenter = (ymin + ymax) / 2;
	zcenter = (zmin + zmax) / 2;

	// another cycle to calculate speed and vario array
	variomax=0;
	variomin=0;

	speedmax= -1;
	speedmin= 10000;

	for (unsigned int c=0; c<x.size() ; c++) {
		marker_pos = c;
		vario.push_back(getvario());
		speed.push_back(getspeed());

		// get min and max's, but skip first 10 datapoints
		if (c > 10 ) {
			if (vario[vario.size()-1] > variomax) variomax = vario[vario.size()-1];
			if (vario[vario.size()-1] < variomin) variomin = vario[vario.size()-1];
			if (speed[speed.size()-1] > speedmax) speedmax =speed[speed.size()-1];
			if (speed[speed.size()-1] < speedmin) speedmin = speed[speed.size()-1];
		}

	}

	//set marker_pos
	marker_pos = 0;



	// Emergency fallback to gps-altitude!
                if (_altsum == 0 && !n==0) {
		cerr << "no barometric altitude found... using gps-altitude!" << endl;
		usegps();
		conf_pointer->GPSALT.on();
	}


	readigc.checknow("readigc end");
	return 1;
}


void Flightdata::setglEvaluator(void)
{

	for (int _counter = 0; _counter < n - 3; _counter = _counter + 2) {
		/*
		   GLfloat**  flightarray = new GLfloat*[3];
		   for (int _j=0; _j < n-1 ; _j++)
		   {
		   flightarray[_j] = new GLfloat[3];
		   }
		 */
		GLfloat flightarray[3][3];

		for (int _j = 0; _j <= 2; _j++) {
			flightarray[_j][0] = (GLfloat) x[_j + _counter];
			flightarray[_j][1] = (GLfloat) y[_j + _counter];
			flightarray[_j][2] = (GLfloat) z[_j + _counter];

			cout << flightarray[_j][0] << " " << _j +
			    _counter << " " << flightarray[_j][1] << " " <<
			    _j +
			    _counter << " " << flightarray[_j][2] << " " <<
			    _j + _counter << " " << endl;

		}

		glMap1f(GL_MAP1_VERTEX_3, 0.0f, 20.0f, (GLint) 3,
			(GLint) 3, &flightarray[0][0]);
		cout << "von " << _counter *
		    2 << "  bis  " << ((_counter + 3) * 2) << endl;

		glEnable(GL_MAP1_VERTEX_3);

		glColor3d(1.0, 1.0, 1.0);
		glBegin(GL_LINE_STRIP);
		for (GLfloat _j = 0; _j <= 20; _j++) {
			glEvalCoord1f(_j);
			//cout << "eval---" << _j << endl;
		}
		glEnd();

	}
	cout << "errors after eval:  " << (GLenum) glGetError() << endl;



}


int Flightdata::getstartelevation(bool GPS)
{
	double _sum = 0;
	for (int _z = 3; _z <= 10; _z++) {

		if (!GPS) {
			_sum = _sum + alt[_z];
		} else {
			_sum += gpsalt[_z];
		}
		//cout << "alt: " << alt[_z] << endl;
	}
	_sum = _sum / 8;

	return (int) _sum;
}

void Flightdata::marker_head(void)
{
	marker_pos = int (x.size()-1) ;
}

void Flightdata::forward_marker(void)
{
	marker_pos++;
	if (marker_pos >= int (x.size())) {
		if (cycles != -1) cycles--;
		if (conf_pointer->DEBUG.get())
			cout << "Cycles: " << cycles << endl;

		marker_pos = 0;
	}
}

void Flightdata::fforward_marker(void)
{
	marker_pos += 10;
	if (marker_pos >= int (x.size())) {
		marker_pos = marker_pos - x.size();
	}
}

void Flightdata::backward_marker(void)
{
	marker_pos--;
	if (marker_pos < 0) {
		marker_pos = x.size() - 1;
	}
}

void Flightdata::fbackward_marker(void)
{
	marker_pos -= 10;
	if (marker_pos < 0) {
		marker_pos = x.size() - abs(int (marker_pos));
	}
}

void Flightdata::setmarkerpos(int _pos)
{
	if (_pos >= 0 && _pos <= n) {
		marker_pos = _pos;

	} else {
		cerr << "Marker position out of range and ignored!" <<
		    endl;
	}

}

void Flightdata::setmarkertime(string _time)
{
	int hh,mm,ss;

	if (sscanf(_time.c_str(),"%d:%d:%d", &hh, &mm, &ss) != 3) {
		cout << "wrong format in --marker-time argument" << endl;
		return;
	}

	float dtime = (double) hh + ((double) mm / 60.0) +  ((double) ss / 3600.0);

	float diff=0.3;
	int _inx=0;
	for (int _x=0; _x<(int)dectime.size(); _x++) {
		if ( abs(dectime[_x] - dtime) < diff) {
			diff = abs(dectime[_x] - dtime);
			_inx = _x;
		}
	}

	if (_inx != 0) {
		setmarkerpos(_inx);
	} else {
		cout << "given time in --marker-time seems to be wrong" << endl;
	}
}


void Flightdata::draw_marker_ortho(void)
{
	//glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_LIGHTING);
	glColor3f(conf_pointer->markercol_r.get(),conf_pointer->markercol_g.get(), conf_pointer->markercol_b.get());
	glLineWidth(3.0);

	glBegin(GL_LINES);

	glVertex3f(x[marker_pos] - 5, y[marker_pos], 10);
	glVertex3f(x[marker_pos] + 5, y[marker_pos], 10);
	glVertex3f(x[marker_pos], y[marker_pos] - 5, 10);
	glVertex3f(x[marker_pos], y[marker_pos] + 5, 10);

	glVertex3f(x[marker_pos] + 1, y[marker_pos] - 0.25, 10);
	glVertex3f(x[marker_pos] + 1, y[marker_pos] + 0.25, 10);
	glVertex3f(x[marker_pos] + 2, y[marker_pos] - 0.25, 10);
	glVertex3f(x[marker_pos] + 2, y[marker_pos] + 0.25, 10);
	glVertex3f(x[marker_pos] + 3, y[marker_pos] - 0.25, 10);
	glVertex3f(x[marker_pos] + 3, y[marker_pos] + 0.25, 10);
	glVertex3f(x[marker_pos] + 4, y[marker_pos] - 0.25, 10);
	glVertex3f(x[marker_pos] + 4, y[marker_pos] + 0.25, 10);
	glVertex3f(x[marker_pos] + 5, y[marker_pos] - 0.25, 10);
	glVertex3f(x[marker_pos] + 5, y[marker_pos] + 0.25, 10);

	glVertex3f(x[marker_pos] - 0.25, y[marker_pos] + 1, 10);
	glVertex3f(x[marker_pos] + 0.25, y[marker_pos] + 1, 10);
	glVertex3f(x[marker_pos] - 0.25, y[marker_pos] + 2, 10);
	glVertex3f(x[marker_pos] + 0.25, y[marker_pos] + 2, 10);
	glVertex3f(x[marker_pos] - 0.25, y[marker_pos] + 3, 10);
	glVertex3f(x[marker_pos] + 0.25, y[marker_pos] + 3, 10);
	glVertex3f(x[marker_pos] - 0.25, y[marker_pos] + 4, 10);
	glVertex3f(x[marker_pos] + 0.25, y[marker_pos] + 4, 10);
	glVertex3f(x[marker_pos] - 0.25, y[marker_pos] + 5, 10);
	glVertex3f(x[marker_pos] + 0.25, y[marker_pos] + 5, 10);

	glEnd();

	//glPopAttrib();
	//restore our default state
	glLineWidth(1.0);
	glEnable(GL_LIGHTING);

}


void Flightdata::draw_marker(int _offset, float _scale, bool BW)
{

//cout << "draw marker" << endl;

	//glPushAttrib(GL_ENABLE_BIT);
	//glPushAttrib(GL_POLYGON_BIT);
	//glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	//glEnable(GL_NORMALIZE);
	glPolygonMode(GL_FRONT, GL_FILL);

	float val05= 0.5 * conf_pointer->MARKER_SIZE.get();
	float val1= 1.0 * conf_pointer->MARKER_SIZE.get();
	float val3= 3.0 * conf_pointer->MARKER_SIZE.get();
	float val5= 5.0 * conf_pointer->MARKER_SIZE.get();

	float col[3] = { conf_pointer->markercol_r.get(),conf_pointer->markercol_g.get(), conf_pointer->markercol_b.get() };
	if (BW) {
		glColor3fv(graycolor(col));
	} else {
		glColor3fv(col);
	}

	glBegin(GL_TRIANGLES);

	glNormal3f(-1, 0, -1.0 / 3.0);
	glVertex3f(x[marker_pos], y[marker_pos],
		   z[marker_pos] + (GLfloat) (_offset / 1000.0));
	glVertex3f(x[marker_pos] - val1, y[marker_pos] - val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);
	glVertex3f(x[marker_pos] - val1, y[marker_pos] + val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);

	glNormal3f(0, 1, -1.0 / 3.0);
	glVertex3f(x[marker_pos], y[marker_pos],
		   z[marker_pos] + (GLfloat) (_offset / 1000.0));
	glVertex3f(x[marker_pos] - val1, y[marker_pos] + val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);
	glVertex3f(x[marker_pos] + val1, y[marker_pos] + val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);

	glNormal3f(1, 0, -1.0 / 3.0);
	glVertex3f(x[marker_pos], y[marker_pos],
		   z[marker_pos] + (GLfloat) (_offset / 1000.0));
	glVertex3f(x[marker_pos] + val1, y[marker_pos] + val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);
	glVertex3f(x[marker_pos] + val1, y[marker_pos] - val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);

	glNormal3f(0, -1, -1.0 / 3.0);
	glVertex3f(x[marker_pos], y[marker_pos],
		   z[marker_pos] + (GLfloat) (_offset / 1000.0));
	glVertex3f(x[marker_pos] + val1, y[marker_pos] - val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);
	glVertex3f(x[marker_pos] - val1, y[marker_pos] - val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);

	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glVertex3f(x[marker_pos] + val1, y[marker_pos] - val1,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glVertex3f(x[marker_pos] + val1, y[marker_pos] + val1,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glVertex3f(x[marker_pos] - val1, y[marker_pos] + val1,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glVertex3f(x[marker_pos] - val1, y[marker_pos] - val1,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);

	glVertex3f(x[marker_pos] + val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glEnd();

	glBegin(GL_QUADS);

	glNormal3f(1, 0, 0);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);

	glNormal3f(0, 1, 0);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);

	glNormal3f(-1, 0, 0);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);

	glNormal3f(0, -1, 0);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);

	glEnd();


	//diable lighting for black lines
	glDisable(GL_LIGHTING);

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);

	glVertex3f(x[marker_pos], y[marker_pos],
		   z[marker_pos] + (GLfloat) (_offset / 1000.0));
	glVertex3f(x[marker_pos] - val1, y[marker_pos] - val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);
	glVertex3f(x[marker_pos] - val1, y[marker_pos] + val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);

	glVertex3f(x[marker_pos], y[marker_pos],
		   z[marker_pos] + (GLfloat) (_offset / 1000.0));
	glVertex3f(x[marker_pos] - val1, y[marker_pos] + val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);
	glVertex3f(x[marker_pos] + val1, y[marker_pos] + val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);

	glVertex3f(x[marker_pos], y[marker_pos],
		   z[marker_pos] + (GLfloat) (_offset / 1000.0));
	glVertex3f(x[marker_pos] + val1, y[marker_pos] + val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);
	glVertex3f(x[marker_pos] + val1, y[marker_pos] - val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);

	glVertex3f(x[marker_pos], y[marker_pos],
		   z[marker_pos] + (GLfloat) (_offset / 1000.0));
	glVertex3f(x[marker_pos] + val1, y[marker_pos] - val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);
	glVertex3f(x[marker_pos] - val1, y[marker_pos] - val1,
		   z[marker_pos] + (GLfloat) (_offset / 1000.0) +
		   val3 / _scale);

	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glVertex3f(x[marker_pos] + val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] + val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val3 / _scale);
	glVertex3f(x[marker_pos] - val05, y[marker_pos] - val05,
		   z[marker_pos] + (_offset / 1000.0) + val5 / _scale);

	glVertex3f(x[marker_pos], y[marker_pos],
		   z[marker_pos] + (_offset / 1000.0));
	glVertex3f(x[marker_pos], y[marker_pos], -0.2);
	glEnd();

                //glPopAttrib();

                //turn an lighting again (our default-state)
	glEnable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

}



void Flightdata::draw_linestrip(int _offset, bool BW,
				float _r_up, float _g_up, float _b_up,
				float _r_down, float _g_down,
				float _b_down)
{

	//glEnable(GL_LIGHTING);
	//glPushAttrib(GL_ENABLE_BIT);

	//disable lighting for the linestrip
	glDisable(GL_LIGHTING);
	glLineWidth(conf_pointer->flightstrip_width.get());	//line width

	float col[3];

	int _first, _last;

	if (conf_pointer->MARKER_RANGE.get()) {
		_first = marker_pos - conf_pointer->get_marker_back();
		_last = marker_pos + conf_pointer->get_marker_ahead();
		if (_first < 0) {
			_first = 0;
		}
		if (_last >= n) {
			_last = n - 1;
		}
	} else {
		_first = 0;
		_last = n - 1;
	}


	glBegin(GL_LINE_STRIP);	//all datapoints as linestrip!
	for (int _z = _first; _z <= _last; _z++) {


		if (conf_pointer->flightstrip_mode.get() == 0) {

			//set color up or down
			if (z[_z - 1] < z[_z]) {

			col[0] = _r_up;
			col[1] = _g_up;
			col[2] = _b_up;

			} else {

			col[0] = _r_down;
			col[1] = _g_down;
			col[2] = _b_down;

                                                }
		}


		// vario mode
		if (conf_pointer->flightstrip_mode.get() == 3) {
			getcolor(col,conf_pointer->flightstrip_colmap.get(), vario[_z], -10.0, +10.0);
		}

		// altitude!
		if (conf_pointer->flightstrip_mode.get() == 1)
			getcolor(col,conf_pointer->flightstrip_colmap.get(), z[_z], zmin, zmax);

		//speed
		if (conf_pointer->flightstrip_mode.get() == 2)
			getcolor(col,conf_pointer->flightstrip_colmap.get(), speed[_z], speedmin, speedmax);


		if (!BW) {
				glColor3fv(col);
			} else {
				glColor3fv(graycolor(col));
			}

		if (conf_pointer->ORTHOVIEW.get() && conf_pointer->ORTHOONTOP.get()) {
			glVertex3f((GLfloat) x[_z], (GLfloat) y[_z],
				(GLfloat) z[_z] + 1 );
		} else {
			// just much higher!
			glVertex3f((GLfloat) x[_z], (GLfloat) y[_z],
				(GLfloat) z[_z] + (_offset / 1000.0));
		}
	}
	glEnd();

	//glPopAttrib();

	glLineWidth(1.0);
	glEnable(GL_LIGHTING);
}


void Flightdata::draw_curtain(int _offset, bool BW, float _r, float _g,
			      float _b, float _alpha)
{

	//glPushAttrib(GL_ENABLE_BIT);
	//glPushAttrib(GL_DEPTH_BUFFER_BIT);
	//glPushAttrib(GL_POLYGON_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDepthMask(GL_FALSE);

	float col[4];

	col[0] = _r;
	col[1] = _g;
	col[2] = _b;
	col[3] = _alpha;

	int _first, _last;

	if (conf_pointer->MARKER_RANGE.get()) {
		_first = marker_pos - conf_pointer->get_marker_back();
		_last = marker_pos + conf_pointer->get_marker_ahead();
		if (_first < 0) {
			_first = 0;
		}
		if (_last >= n) {
			_last = n - 1;
		}
	} else {
		_first = 0;
		_last = n - 1;
	}


	float _elev;

	for (int _z = _first; _z <= _last - 1; _z++) {
		glBegin(GL_POLYGON);

		if (!BW) {
			glColor4fv(col);
		} else {
			glColor4fv(graycolor(col));
		}

		if (gnd_elevation[_z] <= -16384) {_elev = gnd_elevation[_z]+16384.0-50.0;} // 50m below surface...
			else {_elev = gnd_elevation[_z]-50.0;}
		if (gnd_elevation[_z] <= -16384 && conf_pointer->get_sealevel2() == 0) {_elev = -50.0;}


		glVertex3f((GLfloat) x[_z], (GLfloat) y[_z], _elev / 1000.0);
		glVertex3f((GLfloat) x[_z], (GLfloat) y[_z],
			   (GLfloat) z[_z] + (_offset / 1000.0));
		glVertex3f((GLfloat) x[_z + 1], (GLfloat) y[_z + 1],
			   (GLfloat) z[_z + 1] + (_offset / 1000.0));
		glVertex3f((GLfloat) x[_z + 1], (GLfloat) y[_z + 1], _elev / 1000.0);
		glEnd();
	}

	//glPopAttrib();
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}


void Flightdata::draw_shadow(int _offset, bool BW, float _width, float _r,
			     float _g, float _b)
{

	//glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glLineWidth(_width);

	float col[3];

	col[0] = _r;
	col[1] = _g;
	col[2] = _b;

	int _first, _last;

	if (conf_pointer->MARKER_RANGE.get()) {
		_first = marker_pos - conf_pointer->get_marker_back();
		_last = marker_pos + conf_pointer->get_marker_ahead();
		if (_first < 0) {
			_first = 0;
		}
		if (_last >= n) {
			_last = n - 1;
		}
	} else {
		_first = 0;
		_last = n - 1;
	}

	glBegin(GL_LINE_STRIP);

	for (int _z = _first; _z <= _last; _z++) {


		if (!BW) {
			glColor3fv(col);
		} else {
			glColor3fv(graycolor(col));
		}

		glVertex3f((GLfloat) x[_z], (GLfloat) y[_z],
			   (GLfloat) zmin + (_offset / 1000.0));
	}
	glEnd();

	glLineWidth(1.0);
	glEnable(GL_LIGHTING);
	//glPopAttrib();

}


void Flightdata::draw_shadow_terrain(bool BW, float _width, float _r,
				     float _g, float _b)
{

	//glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glLineWidth(_width);

	float col[3];

	col[0] = _r;
	col[1] = _g;
	col[2] = _b;

	int _first, _last;

	if (conf_pointer->MARKER_RANGE.get()) {
		_first = marker_pos - conf_pointer->get_marker_back();
		_last = marker_pos + conf_pointer->get_marker_ahead();
		if (_first < 0) {
			_first = 0;
		}
		if (_last >= n) {
			_last = n - 1;
		}

	} else {
		_first = 0;
		_last = n - 1;
	}

	glBegin(GL_LINE_STRIP);

	float _elev;


	for (int _z = _first; _z <= _last; _z++) {

	if (gnd_elevation[_z] <= -16384) {_elev = gnd_elevation[_z]+16384.0+5.0;} // 50m below surface...
		else {_elev = gnd_elevation[_z]+5.0;}
	if (gnd_elevation[_z] <= -16384 && conf_pointer->get_sealevel2() == 0) {_elev = 5.0;}

		if (!BW) {
			glColor3fv(col);
		} else {
			glColor3fv(graycolor(col));
		}

		glVertex3f((GLfloat) x[_z], (GLfloat) y[_z],
			   (GLfloat) (_elev / 1000.0));
	}
	glEnd();


	// and another one on sealevel3
	if (conf_pointer->get_sealevel3() != -22222) {
                glBegin(GL_LINE_STRIP);

	for (int _z = _first; _z <= _last; _z++) {

		if (!BW) {
			glColor3fv(col);
		} else {
			glColor3fv(graycolor(col));
		}

		glVertex3f((GLfloat) x[_z], (GLfloat) y[_z],
			   (GLfloat) (conf_pointer->get_sealevel3() / 1000.0))  ;
	}
	glEnd();
	}


	glLineWidth(1.0);
	glEnable(GL_LIGHTING);
	//glPopAttrib();

}


double Flightdata::getvario(void)
{

	if (marker_pos >= 1) {	//< int(x.size())-1) {
		return ((alt[marker_pos] -
			 alt[marker_pos - 1]) / ((dectime[marker_pos] -
						  dectime[marker_pos -
							  1]) * 3600));
	} else {
		return 0.0;
	}
}

double Flightdata::getgpsvario(void)
{

	if (marker_pos >= 1) {	//< int(x.size())-1) {
		return ((gpsalt[marker_pos] -
			 gpsalt[marker_pos - 1]) / ((dectime[marker_pos] -
						  dectime[marker_pos -
							  1]) * 3600));
	} else {
		return 0.0;
	}
}

double Flightdata::getintvario(int _fixes)
{

	if (marker_pos > _fixes) {
		return ((alt[marker_pos] -
			 alt[marker_pos -
			     _fixes]) / ((dectime[marker_pos] -
					  dectime[marker_pos -
						  _fixes]) * 3600));
	} else {
		return 0.0;
	}
}


double Flightdata::getgpsintvario(int _fixes)
{

	if (marker_pos > _fixes) {
		return ((gpsalt[marker_pos] -
			 gpsalt[marker_pos -
			     _fixes]) / ((dectime[marker_pos] -
					  dectime[marker_pos -
						  _fixes]) * 3600));
	} else {
		return 0.0;
	}
}

double Flightdata::getspeed(void)
{

	if (marker_pos >= 1) {


		return (great_circle_dist
			(lat[marker_pos], lon[marker_pos],
			 lat[marker_pos - 1], lon[marker_pos - 1])
			/ (dectime[marker_pos] - dectime[marker_pos - 1]));

	} else {
		return 0.0;
	}

}


void Flightdata::usegps(void)
{
	z.clear();
	vario.clear();

	zmin = gpsmin / 1000.0;
	zmax = gpsmax / 1000.0;

	for (int _q = 0; _q < n; _q++) {
		z.push_back(gpsalt[_q] / 1000.0);
	}
                 // another cycle to calculate speed and vario array

                variomax=0;
                variomin=0;

                for (unsigned int c=0; c<x.size() ; c++) {
		marker_pos = c;
		vario.push_back(getgpsvario());
		//speed.push_back(getspeed());

		// get min and max's, but skip first 10 datapoints
                                if (c > 10 ) {
			if (vario[vario.size()-1] > variomax) variomax = vario[vario.size()-1];
			if (vario[vario.size()-1] < variomin) variomin = vario[vario.size()-1];
			//if (speed[speed.size()-1] > speedmax) speedmax =speed[speed.size()-1];
			//if (speed[speed.size()-1] < speedmin) speedmin = speed[speed.size()-1];
		}
                }

               //set marker_pos
	marker_pos = 0;
}
