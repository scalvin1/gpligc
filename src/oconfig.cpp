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

#include "oconfig.h"
#include "projection.h"
#include "value.h"

#ifndef __OSX__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <string>
#include <deque>


//const std::string rcsid_config_cpp =
//    "$Id: oconfig.cpp 3 2014-07-31 09:59:20Z kruegerh $";
//const std::string rcsid_oconfig_h = OCONFIG_H;
//const std::string rcsid_value_h = VALUE_H;


Switch::Switch()
{
	state = false;
}


void Switch::toggle(void)
{
	if (state) {
		state = false;
	} else {
		state = true;
	}
}


Config::Config()
{
	SHADE.on();
	FLIGHT.on();
	INFO.on();
	FOLLOW.on();

	GPSDGNDELEV.on();

	AIRSPACE_P.on();
	AIRSPACE_Q.on();
	AIRSPACE_R.on();
	AIRSPACE_D.on();
	AIRSPACE_C.on();
	AIRSPACE_CTR.on();
	AIRSPACE_WIRE.off();
	AirspaceLimit.init(1000,0,1500);

	LIFTS.off();

	BIGENDIAN.on();
	IGNORE_ELEV_RANGE.off();
	ignore_elev_min.init(-11111,-11111,11111);
	ignore_elev_max.init(-11111,-11111,11111);

	NumberOfMaps = 0;
	NumberOfMapSets = 1;
	//ActiveMapSet = 1;

	// Value _ config_parameter limit and start
	aov.init(90,1,179);
	z_scale.init(3.0,1.0,20.0);
	input_dem_factor.init(1.0, -1000000.0, 1000000.0);
	eye_dist.init(0.2,0.0,100.0);
	movie_msecs.init(0,0,1000);
	movie_repeat_factor.init(5,1,10000);

	SPINNING.init(0,-20,20);

	idle_auto_repeat_limit.init(150.0,1.0,10000.0);

	//FLYING_REPEAT.on();
	FLYING_AUTO_REPEAT.on();

	flightstrip_width.init(2.0,1.0,5.0);
	flightstrip_mode.init(1,0,3); // 0 = classic, 1=alt, 2=speed, 3=vario
	flightstrip_colmap.init(1,1,7);
	flightstrip_col_rup.init(1.0,0.0,1.0);
	flightstrip_col_gup.init(0.0,0.0,1.0);
	flightstrip_col_bup.init(0.0,0.0,1.0);
	flightstrip_col_rdown.init(0.0,0.0,1.0);
	flightstrip_col_gdown.init(0.0,0.0,1.0);
	flightstrip_col_bdown.init(0.0,0.0,1.0);
	markercol_r.init(1.0,0.0,1.0);
	markercol_g.init(0.0,0.0,1.0);
	markercol_b.init(0.0,0.0,1.0);

	shadedirection.init(4,1,8);

	colmapnumber.init(2,1,8);
	colmap2number.init(5,1,8);

	style.init(1,1,2); // default style (new text function for on-screen display, nice background=1, old style =2)
	info_fontsize.init(40,20,100);
	info_fontwidth.init(1.0, 0.5, 3.0);
	background.init(2,1,3);
	background_color_1r.init(0.7, 0.0, 1.0);
	background_color_1g.init(0.7, 0.0, 1.0);
	background_color_1b.init(1.0, 0.0, 1.0);
	background_color_2r.init(0.07, 0.0, 1.0);
	background_color_2g.init(0.07, 0.0, 1.0);
	background_color_2b.init(0.4, 0.0, 1.0);

	airspace_wire_color_r.init(0.0,0.0,1.0);
	airspace_wire_color_g.init(0.0,0.0,1.0);
	airspace_wire_color_b.init(0.0,0.0,1.0);
	airspace_wire_width.init(1.0,1.0,20.0);

	text_color_r.init(0.0,0.0,1.0);
	text_color_g.init(0.0,0.0,1.0);
	text_color_b.init(0.0,0.0,1.0);
	text_width.init(1.5,1.0,20.0);

	lifts_color_r.init(0.9,0.0,1.0);
	lifts_color_g.init(0.1,0.0,1.0);
	lifts_color_b.init(0.1,0.0,1.0);
	lifts_info_mode.init(1,1,7);

	waypoints_color_r.init(0.0,0.0,0.0);
	waypoints_color_g.init(0.0,0.0,0.0);
	waypoints_color_b.init(0.0,0.0,0.0);
	waypoints_info_mode.init(1,1,4);
	waypoints_offset.init(300,-10000,10000);
	WAYPOINTS_OFFSET_TEXT_ONLY.on();

	MAXTRIANGLES.init(1.5e6,0,10e42);
	AUTOREDUCE.on();

	fogdensity.init(0.01,0.0,0.5);
	offset.init(0,-65000,65000);
	orthoclipping.init(100.0,0.01,100000.0);
	ORTHOONTOP.on();
	ActiveMapSet.init(1,1,1);
	jpeg_quality.init(75,0,100);
	MARKER_SIZE.init(1.0, 0.01, 10.0);
	pTextSize.init(0.01,0.001, 1.0);
	fglut_version.init(-1, -32700, 32700); // -1=unknown (glut 3 API)

	joyfac_x.init(0.01, -1.0, 1.0);
	joyfac_y.init(0.01, -1.0, 1.0);
	joyfac_z.init(0.01, -1.0, 1.0);

	image_format = "jpeg";
	save_path = "";
	basename = "frame";

	marker_ahead = 0;
	marker_back = 100;

	char *home;

#ifndef __WIN32__
	home = getenv("HOME");
	if (home == NULL)
		cerr << "Your HOME environment variable isnt set!!!  :-(" << endl;
#else
	// needs to be set if you are using windoof
	home = getenv("GPLIGCHOME");

	if (home == NULL)
		cerr << "the GPLIHCHOME variable isnt set... Ill try some std-locations" << endl;
#endif


	if (home != NULL) {
		string homestring = home;

/* for version 1.9 use old and new names
 from 1.10 on, only new names for conf-file:
 .ogierc
 ogie.ini (win)
 */

#ifndef __WIN32__
	configfiles.push_back (homestring+"/.openGLIGCexplorerrc");
	configfiles.push_back (homestring+"/.opengligcexplorerrc");
	configfiles.push_back (homestring+ "/openGLIGCexplorerrc");
	configfiles.push_back (homestring+ "/opengligcexplorerrc");
	configfiles.push_back (homestring+ "/.ogierc");
#else
	configfiles.push_back (homestring+"\\openGLIGCexplorer.ini");
	configfiles.push_back (homestring+"\\openGLIGCexplorerrc");
	configfiles.push_back (homestring+"\\ogie.ini");
#endif
	} else {
#ifndef __WIN32__
	configfiles.push_back (".openGLIGCexplorerrc");
	configfiles.push_back (".opengligcexplorerrc");
	configfiles.push_back ("openGLIGCexplorerrc");
	configfiles.push_back ("opengligcexplorerrc");
	configfiles.push_back (".ogierc");
#else
	configfiles.push_back ("C:\\GPLIGC\\openGLIGCexplorer.ini");
	configfiles.push_back ("C:\\GPLIGC\\openGLIGCexplorerrc");
	configfiles.push_back ("D:\\GPLIGC\\openGLIGCexplorer.ini");
	configfiles.push_back ("D:\\GPLIGC\\openGLIGCexplorerrc");
	configfiles.push_back ("D:\\GPLIGC\\ogie.ini");
	configfiles.push_back ("C:\\Programme\\GPLIGC\\openGLIGCexplorer.ini");
	configfiles.push_back ("C:\\Programme\\GPLIGC\\openGLIGCexplorerrc");
	configfiles.push_back ("D:\\Programme\\GPLIGC\\openGLIGCexplorer.ini");
	configfiles.push_back ("D:\\Programme\\GPLIGC\\openGLIGCexplorerrc");
	configfiles.push_back ("D:\\Programme\\GPLIGC\\ogie.ini");
	configfiles.push_back ("C:\\Program files\\GPLIGC\\openGLIGCexplorer.ini");
	configfiles.push_back ("C:\\Program files\\GPLIGC\\openGLIGCexplorerrc");
	configfiles.push_back ("D:\\Program files\\GPLIGC\\openGLIGCexplorer.ini");
	configfiles.push_back ("D:\\Program files\\GPLIGC\\openGLIGCexplorerrc");
	configfiles.push_back ("D:\\Program files\\GPLIGC\\ogie.ini");
#endif
	}

	airspacefile = "not available";
	demfile = "not available";
	waypointsfile ="not available";

	// Unit stuff
	alt_unit_name = "m";
	alt_unit_fac = 1.0;

	speed_unit_name = "km/h";
	speed_unit_fac = 1.0;

	vspeed_unit_name = "m/s";
	vspeed_unit_fac = 1.0;

	framewritercounter = 1000;
	airfield_elevation = -9999;

	downscalefactor = 1;
	upscalefactor = 1;

	shadescale=0.0;

	border = 500;		//in km for the green underground
	border_land_lat = 5;	//in km for landscape-mode
	border_land_lon = 5;
	width = 800;		//width and height for glut window
	height = 600;
	initwidth = width;
	initheight = height;

	max_h = -9999;
	min_h = -9999;

	sealevel = 0;
	sealevel2 = -22222;
	sealevel3 = -22222;

	map_shift_lat = 0.0;
	map_shift_lon = 0.0;

	projection = PROJ_PSEUDO_CYL_NO1;

	timeZone = 0;
	timeZoneName = "utc";
}



int Config::getMapIndexLow(void)
{
	int _index = ActiveMapSet.get() - 2;

	if (NumberOfMapSets == 1) {
		return 0;
	}

	if (_index < 0) {
		return 0;
	} else {
		return mapcuts[_index];
	}
}


int Config::getMapIndexHigh(void)
{
	int _index = ActiveMapSet.get() - 1;

	if (NumberOfMapSets == 1) {
		return NumberOfMaps;
	}

	if (_index >= NumberOfMapSets - 1) {
		return NumberOfMaps;
	} else {
		return mapcuts[_index];
	}
}


int Config::set_ActiveMapSet(string set_name)
{
	for (int n = 0; n < (int) MapSetName.size(); n++) {
		if (MapSetName[n] == set_name) {
			cout << "MapSet " << set_name << " found: " << n +
			    1 << endl;
			ActiveMapSet.set(n + 1);
			return (0);
		}
	}
	return (-1);
}


int Config::readConfigFile(void)
{
	// first run: MAP_FILEs counted to get number of maps
	ifstream cfile;

	for (unsigned i=0; i<configfiles.size();i++) {
		cfile.clear();
		cfile.open(configfiles[i].c_str());
		if (DEBUG.get())
			cout << "trying to open: " << configfiles[i] << endl;
		if (!cfile) {
			cfile.close();
			//cfile.clear();
		} else {
			configfile = configfiles[i];
			if (DEBUG.get()) {
				cout << "Configfile Opened!!!: " << configfile << endl;
			}
			break;
		}
	}
	if (!cfile) {
		return -1;
	}


	char zeile[1024];
	int mapcounter = 0;

	// counting map_file!
	while (cfile) {
		cfile.getline(zeile, 1024, '\n');
			char arg1[1024], arg2[1024];
		if (sscanf(zeile, "%s %s", arg1, arg2) == 2) {
			if (strcasecmp(arg1, "MAP_FILE") == 0) {
				if (DEBUG.get()) {
					cout << "Map file found: (" << mapcounter+1 << ") " << arg1 << " -- "
					<< arg2 << endl;
				}
				mapcounter++;
			}
		}

		// do not evaluate VERBOSE option in configfile, IF:
		// --quiet was given (QUIET is already set then)
		// --verbose was given (VERBOSE is already set then, otherwis its default=false)
		// --debug was given (then DEBUG, VERBOS are set, QUIET unset already)
		if ( !QUIET.get() && !VERBOSE.get()  && !DEBUG.get()) {
			//cout << "evaluating conigfile for VERBOSE" << endl;
			if (strcasecmp(arg1, "VERBOSE") == 0) {
				if (checkBOOL(arg2,arg1)) {
					VERBOSE.on();
				} else {
					VERBOSE.off();
				}
				continue;
			}
		}
	}

	NumberOfMaps = mapcounter;
	mapcounter = -1;	// old initial value...

	// dont know how to reset file stream to read again, ... yet
	cfile.close();
	ifstream c2file(configfile.c_str());

	while (c2file) {
		c2file.getline(zeile, 1024, '\n');

		if (zeile[0] == '#') continue;
		if (zeile[0] == ' ') continue;
		if (zeile[0] == 0) continue;
		if (zeile[0] == 13) continue;

		char arg1[1024], arg2[1024];

		sscanf(zeile, "%s %s", arg1, arg2);

		if (strcasecmp(arg1, "LANDSCAPE") == 0) {

			if (checkBOOL(arg2,arg1)) {
				LANDSCAPE.on();
			} else {
				LANDSCAPE.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "VERBOSE") == 0) {
			// we read that already but have to recognize it here too
			continue;
		}

		if (strcasecmp(arg1, "FGLUT_CHECK") == 0) {
			if (checkBOOL(arg2,arg1)) {
				FGLUT_CHECK.on();
			} else {
				FGLUT_CHECK.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "WIRE") == 0) {
			if (checkBOOL(arg2,arg1)) {
				WIRE.on();
			} else {
				WIRE.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "MODULATE") == 0) {
			if (checkBOOL(arg2,arg1)) {
				MODULATE.on();
			} else {
				MODULATE.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "MAPS_UNLIGHTED") == 0) {
			if (checkBOOL(arg2,arg1)) {
				MAPS_UNLIGHTED.on();
			} else {
				MAPS_UNLIGHTED.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "CURTAIN") == 0) {
			if (checkBOOL(arg2,arg1)) {
				CURTAIN.on();
			} else {
				CURTAIN.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "MOUSE") == 0) {
			if (checkBOOL(arg2,arg1)) {
				MOUSE.on();
			} else {
				MOUSE.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "MARKER") == 0) {
			if (checkBOOL(arg2,arg1)) {
				MARKER.on();
				//ROTATECENTER.off();
			} else {
				MARKER.off();
				//ROTATECENTER.on();
			}
			continue;
		}

		if (strcasecmp(arg1, "INFO") == 0) {
			if (checkBOOL(arg2,arg1)) {
				INFO.on();
			} else {
				INFO.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "JOYSTICK_FACTOR_X") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			joyfac_x.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "JOYSTICK_FACTOR_Y") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			joyfac_y.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "JOYSTICK_FACTOR_Z") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			joyfac_z.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "GPSALT") == 0) {
			if (checkBOOL(arg2,arg1)) {
				GPSALT.on();
			} else {
				GPSALT.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "MAP") == 0) {
			if (checkBOOL(arg2,arg1)) {
				MAP.on();
			} else {
				MAP.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "MOVIE") == 0) {
			if (checkBOOL(arg2,arg1)) {
				FLYING.on();
			} else {
				FLYING.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "MOVIE_TIMER") == 0) {
			checkINT(arg2,arg1);
			long unsigned int _tmp;
			sscanf(arg2, "%lu", &_tmp);
			movie_msecs.set ( _tmp);
			continue;
		}

		if (strcasecmp(arg1, "MOVIE_REPEAT") == 0) {
			if (checkBOOL(arg2,arg1)) {
				FLYING_REPEAT.on();
			} else {
				FLYING_REPEAT.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "MOVIE_REPEAT_FACTOR") == 0) {
			checkINT(arg2,arg1);
			long unsigned int _tmp;
			sscanf(arg2, "%lu", &_tmp);
			movie_repeat_factor.set ( _tmp);
			continue;
		}

		if (strcasecmp(arg1, "MOVIE_REPEAT_FPS_LIMIT") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			idle_auto_repeat_limit.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "SPINNING") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			SPINNING.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "MAXTRIANGLES") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			MAXTRIANGLES.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "AUTOREDUCE") == 0) {
			if (checkBOOL(arg2,arg1)) {
				AUTOREDUCE.on();
			} else {
				AUTOREDUCE.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "QUADS") == 0) {
			if (checkBOOL(arg2,arg1)) {
				QUADS.on();
			} else {
				QUADS.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "GRAYSCALE") == 0) {
			if (checkBOOL(arg2,arg1)) {
				BW.on();
			} else {
				BW.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "STEREO") == 0) {
			if (checkBOOL(arg2,arg1)) {
				STEREO.on();
			} else {
				STEREO.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "STEREO_RG") == 0) {
			if (checkBOOL(arg2,arg1)) {
				STEREO_RG.on();
			} else {
				STEREO_RG.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "STEREO_RB") == 0) {
			if (checkBOOL(arg2,arg1)) {
				STEREO_RB.on();
			} else {
				STEREO_RB.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "STEREO_HW") == 0) {
			if (checkBOOL(arg2,arg1)) {
				STEREO_HW.on();
			} else {
				STEREO_HW.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "INVERSE_STEREO") == 0) {
			if (checkBOOL(arg2,arg1)) {
				SWAP_STEREO.on();
			} else {
				SWAP_STEREO.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "EYE_DIST") == 0) {
			checkFLOAT(arg2,arg1);
			float eyetmp;
			sscanf(arg2, "%f", &eyetmp);
			eye_dist.set(eyetmp);
			continue;
		}

		if (strcasecmp(arg1, "HAZE") == 0) {
			if (checkBOOL(arg2,arg1)) {
				FOG.on();
			} else {
				FOG.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "HAZE_DENSITY") == 0) {
			checkFLOAT(arg2,arg1);
			float fog;
			sscanf(arg2, "%f", &fog);
			fogdensity.set(fog);
			continue;
		}

		if (strcasecmp(arg1, "MARKER_BACK") == 0) {
			checkINT(arg2,arg1);
			sscanf(arg2, "%d", &marker_back);
			continue;
		}

		if (strcasecmp(arg1, "MARKER_AHEAD") == 0) {
			checkINT(arg2,arg1);
			sscanf(arg2, "%d", &marker_ahead);
			continue;
		}

		if (strcasecmp(arg1, "MARKER_RANGE") == 0) {
			if (checkBOOL(arg2,arg1)) {
				MARKER_RANGE.on();
			} else {
				MARKER_RANGE.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "MARKER_SIZE") == 0) {
			checkFLOAT(arg2,arg1);
			float ms;
			sscanf(arg2, "%f", &ms);
			MARKER_SIZE.set(ms);
			continue;
		}

		if (strcasecmp(arg1, "MARKERCOL_R") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			markercol_r.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "MARKERCOL_G") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			markercol_g.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "MARKERCOL_B") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			markercol_b.set(_temp);
			continue;
		}


		if (strcasecmp(arg1, "FOLLOW") == 0) {
			if (checkBOOL(arg2,arg1)) {
				FOLLOW.on();
			} else {
				FOLLOW.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "ANGLE_OF_VIEW") == 0) {
			checkINT(arg2,arg1);
			int _aov;
			sscanf(arg2, "%d", &_aov);

			if (_aov >= 1 && _aov <= 179) {
				aov.set( _aov);
			} else {
				cerr <<
				    "angle of view given in configfile is out of range... using default"
				    << endl;

			}

			continue;
		}

		if (strcasecmp(arg1, "SHADE") == 0) {
			if (checkBOOL(arg2,arg1)) {
				SHADE.on();
			} else {
				SHADE.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "WAYPOINTS_FILE") == 0) {
			waypointsfile = arg2;
			continue;
		}

		if (strcasecmp(arg1, "WAYPOINTS") == 0) {
			if (checkBOOL(arg2,arg1)) {
				WAYPOINTS.on();
			} else {
				WAYPOINTS.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "WAYPOINTS_OFFSET_TEXT_ONLY") == 0) {
			if (checkBOOL(arg2,arg1)) {
				WAYPOINTS_OFFSET_TEXT_ONLY.on();
			} else {
				WAYPOINTS_OFFSET_TEXT_ONLY.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "WAYPOINTS_OFFSET") == 0) {
			checkINT(arg2,arg1);
			int _tmp;
			sscanf(arg2, "%d", &_tmp);
			waypoints_offset.set(_tmp);
			continue;
		}

		if (strcasecmp(arg1, "OPEN_AIR_FILE") == 0) {
			airspacefile = arg2;
			continue;
		}

		if (strcasecmp(arg1, "AIRSPACE") == 0) {
			if (checkBOOL(arg2,arg1)) {
				AIRSPACE.on();
			} else {
				AIRSPACE.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "AIRSPACE_LIMIT") == 0) {
			checkINT(arg2,arg1);
			int _temp;
			sscanf(arg2, "%d", &_temp);
			AirspaceLimit.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "AIRSPACE_WIRE") == 0) {
			if (checkBOOL(arg2,arg1)) {
				AIRSPACE_WIRE.on();
			} else {
				AIRSPACE_WIRE.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "AIRSPACE_Q") == 0) {
			if (checkBOOL(arg2,arg1)) {
				AIRSPACE_Q.on();
			} else {
				AIRSPACE_Q.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "AIRSPACE_P") == 0) {
			if (checkBOOL(arg2,arg1)) {
				AIRSPACE_P.on();
			} else {
				AIRSPACE_P.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "AIRSPACE_R") == 0) {
			if (checkBOOL(arg2,arg1)) {
				AIRSPACE_R.on();
			} else {
				AIRSPACE_R.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "AIRSPACE_D") == 0) {
			if (checkBOOL(arg2,arg1)) {
				AIRSPACE_D.on();
			} else {
				AIRSPACE_D.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "AIRSPACE_C") == 0) {
			if (checkBOOL(arg2,arg1)) {
				AIRSPACE_C.on();
			} else {
				AIRSPACE_C.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "AIRSPACE_CTR") == 0) {
			if (checkBOOL(arg2,arg1)) {
				AIRSPACE_CTR.on();
			} else {
				AIRSPACE_CTR.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "COMPRESSION") == 0) {
			if (checkBOOL(arg2,arg1)) {
				COMPRESSION.on();
			} else {
				COMPRESSION.off();
			}
			continue;
		}



		if (strcasecmp(arg1, "FULLSCREEN") == 0) {
			if (checkBOOL(arg2,arg1)) {
				FULLSCREEN.on();
			} else {
				FULLSCREEN.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "DEM_FILE") == 0) {
			DEM.on();
			//demfile = new char[80];
			//strcpy(demfile, arg2);
			demfile = arg2;
			continue;

		}

		if (strcasecmp(arg1, "BIGENDIAN") == 0) {
			if (checkBOOL(arg2,arg1)) {
				BIGENDIAN.on();
			} else {
				BIGENDIAN.off();
			}
			continue;
		}

		if (strcasecmp(arg1, "DEM_GRID_LAT") == 0) {
			checkFLOAT(arg2,arg1);
			sscanf(arg2, "%f", &dem_grid_lat);
			continue;
		}

		if (strcasecmp(arg1, "DEM_GRID_LON") == 0) {
			checkFLOAT(arg2,arg1);
			sscanf(arg2, "%f", &dem_grid_lon);
			continue;
		}

		if (strcasecmp(arg1, "DEM_ROWS") == 0) {
			checkINT(arg2,arg1);
			sscanf(arg2, "%d", &dem_rows);
			continue;
		}

		if (strcasecmp(arg1, "DEM_COLUMNS") == 0) {
			checkINT(arg2,arg1);
			sscanf(arg2, "%d", &dem_columns);
			continue;
		}

		if (strcasecmp(arg1, "DEM_LAT_MAX") == 0) {
			checkFLOAT(arg2,arg1);
			sscanf(arg2, "%f", &dem_lat_max);
			continue;
		}

		if (strcasecmp(arg1, "DEM_LAT_MIN") == 0) {
			checkFLOAT(arg2,arg1);
			sscanf(arg2, "%f", &dem_lat_min);
			continue;
		}

		if (strcasecmp(arg1, "DEM_LON_MAX") == 0) {
			checkFLOAT(arg2,arg1);
			sscanf(arg2, "%f", &dem_lon_max);
			continue;
		}

		if (strcasecmp(arg1, "DEM_LON_MIN") == 0) {
			checkFLOAT(arg2,arg1);
			sscanf(arg2, "%f", &dem_lon_min);
			continue;
		}

		if (strcasecmp(arg1, "SCALE_Z") == 0) {
			checkFLOAT(arg2,arg1);
			float _ztemp;
			sscanf(arg2, "%f", &_ztemp);
			z_scale.set(_ztemp);
			continue;
		}

		if (strcasecmp(arg1, "DEM_INPUT_FACTOR") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			input_dem_factor.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "SPEED_UNIT_FAC") == 0) {
			checkFLOAT(arg2,arg1);
			sscanf(arg2, "%f", &speed_unit_fac);
			continue;
		}

		if (strcasecmp(arg1, "SPEED_UNIT_NAME") == 0) {
			//speed_unit_name = new char[80];
			//strcpy(speed_unit_name, arg2);
			speed_unit_name = arg2;
			continue;
		}

		if (strcasecmp(arg1, "VSPEED_UNIT_FAC") == 0) {
			checkFLOAT(arg2,arg1);
			sscanf(arg2, "%f", &vspeed_unit_fac);
			continue;
		}

		if (strcasecmp(arg1, "VSPEED_UNIT_NAME") == 0) {
			//vspeed_unit_name = new char[80];
			//strcpy(vspeed_unit_name, arg2);
			vspeed_unit_name = arg2;
			continue;
		}

		if (strcasecmp(arg1, "ALT_UNIT_FAC") == 0) {
			checkFLOAT(arg2,arg1);
			sscanf(arg2, "%f", &alt_unit_fac);
			continue;
		}

		if (strcasecmp(arg1, "ALT_UNIT_NAME") == 0) {
			//alt_unit_name = new char[80];
			//strcpy(alt_unit_name, arg2);
			alt_unit_name = arg2;
			continue;
		}

		if (strcasecmp(arg1, "BORDER") == 0) {
			checkFLOAT(arg2,arg1);
			sscanf(arg2, "%f", &border_land_lat);
			border_land_lon = border_land_lat;
			continue;
		}

		if (strcasecmp(arg1, "COLORMAP") == 0) {
			checkINT(arg2,arg1);
			int cmn;
			sscanf(arg2, "%d", &cmn);
			colmapnumber.set(cmn);
			continue;
		}

		if (strcasecmp(arg1, "COLORMAP_SEA") == 0) {
			checkINT(arg2,arg1);
			int cmn;
			sscanf(arg2, "%d", &cmn);
			colmap2number.set(cmn);
			continue;
		}

		if (strcasecmp(arg1, "SEALEVEL") == 0) {
			checkINT(arg2,arg1);
			sscanf(arg2, "%d", &sealevel);
			continue;
		}

		if (strcasecmp(arg1, "SEALEVEL2") == 0) {
			checkINT(arg2,arg1);
			sscanf(arg2, "%d", &sealevel2);
			continue;
		}

		if (strcasecmp(arg1, "SEALEVEL3") == 0) {
			checkINT(arg2,arg1);
			sscanf(arg2, "%d", &sealevel3);
			continue;
		}

		if (strcasecmp(arg1, "TIME_ZONE") == 0) {
			checkINT(arg2,arg1);
			sscanf(arg2, "%d", &timeZone);
			continue;
		}

		if (strcasecmp(arg1, "TIME_ZONE_NAME") == 0) {
			timeZoneName=arg2;
			continue;
		}

		if (strcasecmp(arg1, "PROJECTION") == 0) {
			checkINT(arg2,arg1);
			sscanf(arg2, "%ud", &projection);
			continue;
		}

		if (strcasecmp(arg1, "JPEG_QUALITY") == 0) {
			checkINT(arg2,arg1);
			int jpgq;
			sscanf(arg2, "%d", &jpgq);
			jpeg_quality.set(jpgq);
			continue;
		}

		if (strcasecmp(arg1, "IMAGE_FORMAT") == 0) {
			image_format = arg2;
			continue;
		}

		if (strcasecmp(arg1, "SAVE_PATH") == 0) {
			save_path=arg2 + SEPERATOR;
			continue;
		}

		if (strcasecmp(arg1, "BASENAME")==0) {
			basename=arg2;
			continue;
		}


		if (strcasecmp(arg1, "WINDOW_WIDTH") == 0) {
			checkINT(arg2,arg1);
			sscanf(arg2, "%d", &width);
			initwidth = width;
			continue;
		}

		if (strcasecmp(arg1, "WINDOW_HEIGHT") == 0) {
			checkINT(arg2,arg1);
			sscanf(arg2, "%d", &height);
			initheight = height;
			continue;
		}


		if (strcasecmp(arg1, "NUMBER_OF_MAPS") == 0) {
			cerr << "NUMBER_OF_MAPS  is not needed anymore! Can be removed from configfile" << endl;
			continue;
		}

		if (strcasecmp(arg1, "MAP_SHIFT_LAT") == 0) {
			checkFLOAT(arg2,arg1);
			sscanf(arg2, "%f", &map_shift_lat);
			continue;
		}

		if (strcasecmp(arg1, "MAP_SHIFT_LON") == 0) {
			checkFLOAT(arg2,arg1);
			sscanf(arg2, "%f", &map_shift_lon);
			continue;
		}


		if (strcasecmp(arg1, "MAP_CUT") == 0) {

			NumberOfMapSets++;
			ActiveMapSet.setmax(NumberOfMapSets);
			mapcuts.push_back(mapcounter + 1);

			// if there was no MAP_SET_NAME before MAP_CUT
			// -> no name given to prevoius map set
			// if the last map set has no name identifier,
			// one has to be set later (after processing configfile)

			if ((int)MapSetName.size() < NumberOfMapSets-1) {
				MapSetName.push_back("unnamed map set");
			}
			continue;
		}


		if (strcasecmp(arg1, "MAP_SET_NAME") == 0) {
			MapSetName.push_back(arg2);
			continue;
		}


		if (strcasecmp(arg1, "MAP_FILE") == 0) {
			mapcounter++;
			MapFilename.push_back(arg2);
			map_width.push_back(0);
			map_height.push_back(0);
			continue;
		}


		if (strcasecmp(arg1, "MAP_LEFT") == 0) {
			checkFLOAT(arg2,arg1);
			float tmp;
			sscanf(arg2, "%f", &tmp);
			map_left.push_back(tmp+map_shift_lon);
			continue;
		}

		if (strcasecmp(arg1, "MAP_RIGHT") == 0) {
			checkFLOAT(arg2,arg1);
			float tmp;
			sscanf(arg2, "%f", &tmp);
			map_right.push_back(tmp+map_shift_lon);
			continue;
		}

		if (strcasecmp(arg1, "MAP_TOP") == 0) {
			checkFLOAT(arg2,arg1);
			float tmp;
			sscanf(arg2, "%f", &tmp);
			map_top.push_back(tmp+map_shift_lat);
			continue;
		}

		if (strcasecmp(arg1, "MAP_BOTTOM") == 0) {
			checkFLOAT(arg2,arg1);
			float tmp;
			sscanf(arg2, "%f", &tmp);
			map_bottom.push_back(tmp+map_shift_lat);
			continue;
		}


		if (strcasecmp(arg1, "MAP_HEIGHT") == 0) {
			checkINT(arg2,arg1);
			int tmp;
			sscanf(arg2, "%d", &tmp);
			map_height[mapcounter]=tmp;
			continue;
		}

		if (strcasecmp(arg1, "MAP_WIDTH") == 0) {
			checkINT(arg2,arg1);
			int tmp;
			sscanf(arg2, "%d", &tmp);
			map_width[mapcounter]=tmp;
			continue;
		}

		if (strcasecmp(arg1, "FLIGHTSTRIPCOLUP_R") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			flightstrip_col_rup.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "FLIGHTSTRIPCOLUP_G") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			flightstrip_col_gup.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "FLIGHTSTRIPCOLUP_B") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			flightstrip_col_bup.set(_temp);
			continue;
		}
		if (strcasecmp(arg1, "FLIGHTSTRIPCOLDOWN_R") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			flightstrip_col_rdown.set(_temp);
			continue;
		}
		if (strcasecmp(arg1, "FLIGHTSTRIPCOLDOWN_G") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			flightstrip_col_gdown.set(_temp);
			continue;
		}
		if (strcasecmp(arg1, "FLIGHTSTRIPCOLDOWN_B") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			flightstrip_col_bdown.set(_temp);
			continue;
		}
		if (strcasecmp(arg1, "FLIGHTSTRIP_LINEWIDTH") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			flightstrip_width.set(_temp);
			continue;
		}
		if (strcasecmp(arg1, "FLIGHTSTRIP_COLORMAP") == 0) {
			checkFLOAT(arg2,arg1);
			int _temp;
			sscanf(arg2, "%d", &_temp);
			flightstrip_colmap.set(_temp);
			continue;
		}
		if (strcasecmp(arg1, "BACKGROUND_COLOR_1_R") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			background_color_1r.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "BACKGROUND_COLOR_1_G") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			background_color_1g.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "BACKGROUND_COLOR_1_B") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			background_color_1b.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "BACKGROUND_COLOR_2_R") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			background_color_2r.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "BACKGROUND_COLOR_2_G") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			background_color_2g.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "BACKGROUND_COLOR_2_B") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			background_color_2b.set(_temp);
			continue;
		}
		if (strcasecmp(arg1, "INFOFONT_LINEWIDTH") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			info_fontwidth.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "INFOFONT_SIZE") == 0) {
			checkINT(arg2,arg1);
			int _tmp;
			sscanf(arg2, "%d", &_tmp);
			info_fontsize.set ( _tmp);

			continue;
		}

		if (strcasecmp(arg1, "INFO_STYLE") == 0) {
			checkINT(arg2,arg1);
			int _tmp;
			sscanf(arg2, "%d", &_tmp);
			style.set ( _tmp);

			continue;
		}

		if (strcasecmp(arg1, "BACKGROUND_STYLE") == 0) {
			checkINT(arg2,arg1);
			int _tmp;
			sscanf(arg2, "%d", &_tmp);
			background.set ( _tmp);

			continue;
		}


		if (strcasecmp(arg1, "TEXT_COLOR_R") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			text_color_r.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "TEXT_COLOR_G") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			text_color_g.set(_temp);
			continue;
		}
		if (strcasecmp(arg1, "TEXT_COLOR_B") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			text_color_b.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "TEXT_LINEWIDTH") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			text_width.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "LIFTS_COLOR_R") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			lifts_color_r.set(_temp);
			continue;
		}
		if (strcasecmp(arg1, "LIFTS_COLOR_G") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			lifts_color_g.set(_temp);
			continue;
		}
		if (strcasecmp(arg1, "LIFTS_COLOR_B") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			lifts_color_b.set(_temp);
			continue;
		}

		if (strcasecmp(arg1, "WP_COLOR_R") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			waypoints_color_r.set(_temp);
			continue;
		}
		if (strcasecmp(arg1, "WP_COLOR_G") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			waypoints_color_g.set(_temp);
			continue;
		}
		if (strcasecmp(arg1, "WP_COLOR_B") == 0) {
			checkFLOAT(arg2,arg1);
			float _temp;
			sscanf(arg2, "%f", &_temp);
			waypoints_color_b.set(_temp);
			continue;
		}



                                // if this point is reached the keyword was not recognized!

                                cerr << "The keyword \"" << arg1 << "\" is not recognized (spelling?!)" << endl;

	}

	// if the last map set hasnt an identifier!
	if ((int)MapSetName.size() == NumberOfMapSets-1) {
				MapSetName.push_back("unnamed map set");
	}

	// check, if all TOP;RIGHT;LEFT;BOTTOM keys given for each MAP_FILE
	if (  (MapFilename.size() != map_top.size())
		|| (MapFilename.size() != map_bottom.size())
		|| (MapFilename.size() != map_right.size())
		|| (MapFilename.size() != map_left.size()) ) {

			cerr << "Not all necessary MAP_TOP, MAP_BOTTOM, MAP_RIGHT, MAP_LEFT specified. Please check your configuration file" << endl;
			exit(1);
	}

	return 1;
}


void Config::setConfigFileName(string _name)
{
	configfiles.insert(configfiles.begin(), _name);
}



int Config::checkBOOL(char* val, const char* key) {
	if (strcasecmp(val, "true") == 0) return 1;
	if (strcasecmp(val, "on") == 0) return 1;
	if (strcasecmp(val, "yes") == 0) return 1;
	if (strcmp(val, "1") == 0) return 1;
	if (strcasecmp(val, "false") == 0) return 0;
	if (strcasecmp(val, "off") == 0) return 0;
	if (strcasecmp(val, "no") == 0) return 0;
	if (strcmp(val, "0") == 0) return 0;

	cerr << "The Value \"" << val << "\" for keyword \"" << key
		<< "\"  was not recognized. Should be one of: true,false,on,off,0,1,yes,no" << endl;
	exit(1);
}


int Config::checkINT(char* val, const char* key)
{
	int i;
	char test[80];
	if (sscanf(val, "%d", &i) == 1) {
		sprintf(test,"%d",i); // interger cannot longer then 80 chars ! ?
		if (strcmp(test,val) == 0) return 1;
	}

	//cout << val << "<->" << test << "<---" << endl;
	cerr << "The Value \"" << val << "\" for keyword \"" << key << "\"  is not an INTEGER" << endl;
	exit(1);
}


int Config::checkFLOAT(char* val, const char* key)
{
	float f;

	if (sscanf(val, "%f", &f) == 1) return 1;

	cerr << "The Value \"" << val << "\" for keyword \"" << key << "\"  is not a FLOAT" << endl;

	exit(1);
}

// sets the framerate (adding to the smooth-vector)
void Config::set_fps(float f)
{
	fps.push_back(f);
	if (fps.size() > 15)
		fps.pop_front();
}

// calcs smoothed framerate
float Config::get_fps(void)
{
	float _sum=0;
	//cout << fps.size() << " -- ";
	for (unsigned int i=0; i<fps.size();i++) {
		_sum += fps[i];
		//cout << fps[i] << "  ";
	}
	//cout << endl;
	return (_sum / fps.size());
}


Config::~Config()
{
	//cout << "destruktor von Config" << endl << flush;
//	delete demfile;
	//delete speed_unit_name;
	//delete vspeed_unit_name;
	//delete alt_unit_name;
}
