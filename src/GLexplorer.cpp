/*
 * (c) 2002-2021 Hannes Krueger
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
if compiled with -DWITH_OSMESA, the mesa offscreen rendering is included
   -> changed to autoconf -> HAVE_LIBOSMESA
if compiled with -DONLY_OSMESA, only osmesa will work. all interactive
features will be disabled, and therefore no GLUT or X11 is needed.
 --> however for text some glut functions are used....
*/

// all platforms include gl.h (except OSX)
#ifndef __OSX__
#include <GL/gl.h>
// glut not needed for ONLY_OSMESA
// actually ONLY_OSMESA shouldnt be used any longer
// #ifndef ONLY_OSMESA
#include <GL/glut.h>
#include <GL/glu.h>
// #endif

#else

// OSX here....
// #ifndef ONLY_OSMESA
#include <GLUT/glut.h>
// #endif
#include <OpenGL/gl.h>
#define __MACOSX__
#endif


#ifdef FREEGLUT
#include <GL/freeglut_ext.h>
#endif

#ifndef GLUT_VERSION
// this is the value from GL/freeglut_ext.h. If we dont have freeglut, we need to define it here
#define GLUT_VERSION  0x01FC
#endif


#include "oconfig.h"
#include "config.h"

#ifdef HAVE_LIBOSMESA
//#include <GL/glu.h>
#include <GL/osmesa.h>
//#include "osmesa.h"
#endif

#ifndef __WIN32__
#ifndef __OSX__
// #ifndef ONLY_OSMESA
#include <GL/glx.h>
// #endif
#endif
#endif

// just for version info
#ifdef HAVE_LIBGPS
#include "libgpsmm.h"
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include <jpeglib.h>

// do we need this ?
#ifndef __WIN32__
#include <csignal>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#include <sys/time.h>

#include <string>
#include <vector>

#include "GLexplorer.h"
#include "flightdata.h"
#include "ego.h"
#include "landscape.h"

#include "cmdline.h"
#include "map.h"
#include "oglexfunc.h"
#include "airspc.h"
#include "projection.h"
//#include "sysinfo.h"
#include "points.h"


// windows hasn't GLX and Mesa
#ifdef __WIN32__
#define NOGLX
//#include <strings.h>
//#undef WITH_OSMESA
#endif

// #ifdef __OSX__
// #define NOGLX
// #undef WITH_OSMESA
// #endif

// configure hasn't found glXCreatePbuffer
#ifndef HAVE_GLXPBUFFER
#define NOGLX
#endif

// no unix, no GLX, Mesa
#ifndef __unix__
#define NOGLX
//#undef WITH_MESA
#endif

#ifndef M_PI
#define M_PI 3.141592654
#endif


using namespace std;

//const string rcsid_GLexplorer_cpp =
//    "$Id: GLexplorer.cpp 3 2014-07-31 09:59:20Z kruegerh $";
//const string rcsid_GLexplorer_h = GLEXPLORER_H;

//Global stuff
Flightdata fd;			//flightdata object
				//needs to be global! (see keyPressed callback function...)

Airspace air;			//airspaces....

Projection proj;			//projection

Points lifts;
Points waypoints;

gengetopt_args_info args_info;
gpligc_ipc_block ipc_block;

Ego ego(0, 0, 0, 0, 0, 0);		//initialize ego
GLuint flightdata_displ_list;		//the display_lists
GLuint landscape_displ_list;	//
GLuint airspace_displ_list;
//GLuint lifts_displ_list;

Config conf;			//configuration stuff (initialized with compiled-in defaults, see constructor)
				// defaults can be overridden by 1. configfile and 2.cmdline

int lastmx = 0;			//last mouseposition
int lastmy = 0;

int rightbuttonstate = GLUT_UP;
int middlebuttonstate = GLUT_UP;
int leftbuttonstate = GLUT_UP;
//bool cntrlstate = false;


bool MENUPOPPED = false;		//menu hasnt popped or disappeared soon.
bool MENUCHANGED = false;		//menu has to be changed!
bool MENUSTATE = false;

bool HELP = false;			//flag for help-display
bool INFO = false;			// flag for info-display

bool MOUSEWARPED = false;	// Mouse warped indicator. Will be set when the mouse pointer
				// is forcefully re-centered by the program.

/*
bool SHIFTKEY = false;
bool CNTRLKEY = false;
bool ALTKEY = false;
*/

int window = -3992;		// GLUT window number (3992 to check if its changed (=window created))

vector<float>fps(10);		// used to smooth framerate


int movie_repeat_counter=0;


#ifndef __WIN32__
#ifndef ONLY_OSMESA
// GPLIGC communication... if SIGUSR1 received, reread shared mem block
void SIGUSR1Handler(int sig)
{
	if (sig == SIGUSR1) {
		int shm_id;
		key_t key;
		key = ftok(conf.getIGCFileName().c_str(), 76);
		//cout << "key : " << key << endl;
		shm_id = shmget(key, sizeof(gpligc_ipc_block), 0);

		if (shm_id != -1) {
			char *data;
			char *data_dest;
			data_dest = (char *) &ipc_block;
			data = (char *) shmat(shm_id, (void *) 0, 0);

			//copy shm segment to the ipc_block struct... (low-level)
			for (int _z = 0;
			     _z < (int) sizeof(gpligc_ipc_block); _z++) {
				data_dest[_z] = data[_z];
			}
			shmdt(data);

			//cout << "Inhalt:  " << ipc_block.marker_pos << endl;
			fd.setmarkerpos(ipc_block.marker_pos);
			if (conf.FOLLOW.get()) {
				update_locked_pos();
			}
			if (conf.MARKER_RANGE.get()) {
				GenFlightList();
			}
		}

		/*
		   if (conf.VERBOSE.get()) {
		   cout << "SIGUSR1 received...  Size of shared mem block:"
		   << sizeof(gpligc_ipc_block) << "   shmid:"<< shm_id <<endl;
		   }
		 */
		glutPostRedisplay();
	}
}

void update_shmem(void) {

	ipc_block.marker_pos = fd.getmarker();

	//cout << "openGLIGCexplorer: " << fd.getmarker();

	int shm_id;
	key_t key;
	key = ftok(conf.getIGCFileName().c_str(), 76);
	//cout << "key : " << key << endl;
	shm_id = shmget(key, sizeof(gpligc_ipc_block), 0);

	if (shm_id != -1) {
		char *data;
		char *data_src;
		data_src = (char *) &ipc_block;
		data = (char *) shmat(shm_id, (void *) 0, 0);

		//copy the ipc_block struct to shm-segment... (low-level)
		for (int _z = 0;
		     _z < (int) sizeof(gpligc_ipc_block); _z++) {
			 data[_z] = data_src[_z];
		}
		shmdt(data);
	}

	//cout << "update_shmem called... sending SIGUSR2 to GPLIGC" << endl;

	// this is now deactivated....

	//if (args_info.parent_pid_given) {
	//	kill(args_info.parent_pid_arg, SIGUSR2);
	//}

}
#endif
#endif



#ifndef ONLY_OSMESA
int menIdMain, menIdMapSet, menIdStereo, menIdColormap, menIdColormap_sea,
    menIdFSColormap, menIdFSMode, menIdAirspaceType;

void menuMain(int val)
{

	// this is needed for glutChangeToMenuEntry (which is done in current menu),
	// if this is called from keypressed function and current menu is other than menuMain
// 	glutSetMenu(menIdMain); // we dont changeToMenuEntry any more, here

	MENUPOPPED=true;

	switch (val) {

	case 1:
		conf.MAP.toggle();
		GenLandscapeList();
		GenFlightList();	//?
		glutPostRedisplay();

		MENUCHANGED= true;
// 		if (conf.MAP.get()) {
// 			glutChangeToMenuEntry(1, "Maps off", 1);
// 		} else {
// 			glutChangeToMenuEntry(1, "Maps on", 1);
// 		}


		break;

	case 2:
		conf.MODULATE.toggle();

		if (conf.MAP.get()) {
			GenLandscapeList();
			glutPostRedisplay();
		}
		MENUCHANGED= true;

// 		if (conf.MODULATE.get()) {
// 			glutChangeToMenuEntry(2, "Colored maps off", 2);
// 		} else {
// 			glutChangeToMenuEntry(2, "Colored maps on", 2);
// 		}


		break;

	case 11:
		if (conf.DEM.get()) {
			conf.LANDSCAPE.toggle();
			GenFlightList();
			GenLandscapeList();
			glutPostRedisplay();
		}
		MENUCHANGED= true;

// 		if (conf.LANDSCAPE.get()) {
// 			glutChangeToMenuEntry(5, "Terrain off", 11);
// 		} else {
// 			glutChangeToMenuEntry(5, "Terrain on", 11);
// 		}

		break;

	case 12:
		conf.FOG.toggle();
		//GenFlightList();
		InitGL(conf.getwidth(), conf.getheight());
		glutPostRedisplay();
		MENUCHANGED= true;

// 		if (conf.FOG.get()) {
// 			glutChangeToMenuEntry(8, "Haze off", 12);
// 		} else {
// 			glutChangeToMenuEntry(8, "Haze on", 12);
// 		}


		break;

	case 13:

		if (conf.getAirspaceFileName() != "not available") {
			conf.AIRSPACE.toggle();
			GenAirspaceList();
			glutPostRedisplay();
		}
		MENUCHANGED= true;

// 		if (conf.AIRSPACE.get()) {
// 			glutChangeToMenuEntry(9, "Airspace off", 13);
// 		} else {
// 			glutChangeToMenuEntry(9, "Airspace on", 13);
// 		}


		break;


	case 21:
		if (conf.FLIGHT.get()) {
			conf.CURTAIN.toggle();
			GenFlightList();
			glutPostRedisplay();
		}
		MENUCHANGED= true;

// 		if (conf.CURTAIN.get()) {
// 			glutChangeToMenuEntry(14, "Curtain off", 21);
// 		} else {
// 			glutChangeToMenuEntry(14, "Curtain on", 21);
// 		}


		break;

	case 31:
		if (conf.FLIGHT.get()) {

			conf.MARKER.toggle();
			glutPostRedisplay();
		}
		MENUCHANGED= true;


// 		if (conf.MARKER.get()) {
// 			glutChangeToMenuEntry(16, "Marker off", 31);
// 		} else {
// 			glutChangeToMenuEntry(16, "Marker on", 31);
// 		}


		break;

	case 32:
		conf.INFO.toggle();


		glutPostRedisplay();
		MENUCHANGED= true;

// 		if (conf.INFO.get()) {
// 			glutChangeToMenuEntry(17, "Info off", 32);
// 		} else {
// 			glutChangeToMenuEntry(17, "Info on", 32);
// 		}


		break;


	case 33:
		if (conf.FLIGHT.get()) {
			if (conf.FOLLOW.get()) {
				ego.setdx(0);
				ego.setdy(0);
				ego.setdz(0);
			} else {
				update_diff_pos();
			}

			conf.FOLLOW.toggle();
		}
		MENUCHANGED= true;

// 		if (conf.FOLLOW.get()) {
// 			glutChangeToMenuEntry(18, "Follow mode off", 33);
// 		} else {
// 			glutChangeToMenuEntry(18, "Follow mode on", 33);
// 		}


		break;

	case 34:
		if (conf.FLIGHT.get()) {
			conf.MARKER_RANGE.toggle();
			GenFlightList();
			glutPostRedisplay();
		}
		MENUCHANGED= true;

// 		if (conf.MARKER_RANGE.get()) {
// 			glutChangeToMenuEntry(19, "Range off", 34);
// 		} else {
// // 			glutChangeToMenuEntry(19, "Range on", 34);
// 		}


		break;



	case 35:
		// makes no sense without flight
		//if (conf.FLIGHT.get()) {
			conf.FLYING.toggle();
		//}

		if (conf.FLYING.get() || conf.JOYSTICK.get() || conf.GPSD.get()) {
			glutIdleFunc(&IdleFunc);
		} else {
			glutIdleFunc(NULL);
		}

		glutPostRedisplay();
		MENUCHANGED= true;

// 		if (conf.FLYING.get()) {
// 			glutChangeToMenuEntry(20, "Movie off", 35);
// 		} else {
// 			glutChangeToMenuEntry(20, "Movie on", 35);
// 		}


		break;

	case 36:
		conf.JOYSTICK.toggle();

		if (conf.FLYING.get() || conf.JOYSTICK.get() || conf.GPSD.get() ) {
			glutIdleFunc(&IdleFunc);
		} else {
			glutIdleFunc(NULL);
		}
		MENUCHANGED= true;

		if (conf.JOYSTICK.get()) {
			glutJoystickFunc(joystick,0);
// 			glutChangeToMenuEntry(21,"Joystick off", 36);
		} else {
			glutJoystickFunc(NULL,0);
// 			glutChangeToMenuEntry(21,"Joystick on", 36);
		}

		break;





	case 41:
		conf.ORTHOVIEW.toggle();
		GenFlightList();
		InitGL(conf.getwidth(), conf.getheight());	//???
		glutPostRedisplay();
		MENUCHANGED= true;

// 		if (conf.ORTHOVIEW.get()) {
// 			glutChangeToMenuEntry(23, "3D perspectivic view",
// 					      41);
// 		} else {
// 			glutChangeToMenuEntry(23, "2D orthographic view",
// 					      41);
// 		}


		break;


	case 97:
		HELP = true; INFO=false;
		glutPostRedisplay();
		break;

	case 98:
		INFO = true; HELP=false;
		glutPostRedisplay();
		break;

	case 99:
		Ende(0);
		break;

	}

}

// recreate/update menus!
void menuRecreate()
{
	if (conf.DEBUG.get())
	    cout << " re-creating menus"<< endl;
	glutSetMenu(menIdAirspaceType);

        if (conf.AIRSPACE_P.get()) {
		glutChangeToMenuEntry(1, "Prohibited D off", 1);

	} else {
		glutChangeToMenuEntry(1, "Prohibited P on", 1);
	}

	if (conf.AIRSPACE_R.get()) {
		glutChangeToMenuEntry(2, "Restricted R off", 2);	//2
	} else {
		glutChangeToMenuEntry(2, "Restricted R on", 2);
	}

	if (conf.AIRSPACE_Q.get()) {
		glutChangeToMenuEntry(3, "Danger Q off", 3);	//3
	} else {
		glutChangeToMenuEntry(3, "Danger Q on", 3);
	}

	if (conf.AIRSPACE_C.get()) {
		glutChangeToMenuEntry(4, "Class C off", 4);	//4
	} else {
		glutChangeToMenuEntry(4, "Class C on", 4);
	}

	if (conf.AIRSPACE_D.get()) {
		glutChangeToMenuEntry(5, "Class D off", 5);	//5
	} else {
		glutChangeToMenuEntry(5, "Class D on", 5);
	}

	if (conf.AIRSPACE_CTR.get()) {
		glutChangeToMenuEntry(6, "CTR off", 6);	//6
	} else {
		glutChangeToMenuEntry(6, "CTR on", 6);
	}

	// recreate/change main menu!
	glutSetMenu(menIdMain);
	if (conf.MAP.get()) {
		glutChangeToMenuEntry(1, "Maps off", 1);
	} else {
		glutChangeToMenuEntry(1, "Maps on", 1);
	}
	if (conf.MODULATE.get()) {
		glutChangeToMenuEntry(2, "Colored maps off", 2);
	} else {
		glutChangeToMenuEntry(2, "Colored maps on", 2);
	}
	if (conf.LANDSCAPE.get()) {
		glutChangeToMenuEntry(5, "Terrain off", 11);
	} else {
		glutChangeToMenuEntry(5, "Terrain on", 11);
	}
	if (conf.FOG.get()) {
		glutChangeToMenuEntry(8, "Haze off", 12);
	} else {
		glutChangeToMenuEntry(8, "Haze on", 12);
	}
	if (conf.AIRSPACE.get()) {
		glutChangeToMenuEntry(9, "Airspace off", 13);
	} else {
		glutChangeToMenuEntry(9, "Airspace on", 13);
	}
	if (conf.CURTAIN.get()) {
		glutChangeToMenuEntry(14, "Curtain off", 21);
	} else {
		glutChangeToMenuEntry(14, "Curtain on", 21);
		}
	if (conf.MARKER.get()) {
		glutChangeToMenuEntry(16, "Marker off", 31);
	} else {
		glutChangeToMenuEntry(16, "Marker on", 31);
	}
	if (conf.INFO.get()) {
		glutChangeToMenuEntry(17, "Info off", 32);
	} else {
		glutChangeToMenuEntry(17, "Info on", 32);
	}
	if (conf.FOLLOW.get()) {
		glutChangeToMenuEntry(18, "Follow mode off", 33);
	} else {
		glutChangeToMenuEntry(18, "Follow mode on", 33);
	}
	if (conf.MARKER_RANGE.get()) {
		glutChangeToMenuEntry(19, "Range off", 34);
	} else {
		glutChangeToMenuEntry(19, "Range on", 34);
	}
	if (conf.FLYING.get()) {
		glutChangeToMenuEntry(20, "Movie off", 35);
	} else {
		glutChangeToMenuEntry(20, "Movie on", 35);
	}
	if (conf.JOYSTICK.get()) {
// 		glutJoystickFunc(joystick,0);
		glutChangeToMenuEntry(21,"Joystick off", 36);
	} else {
// 		glutJoystickFunc(NULL,0);
		glutChangeToMenuEntry(21,"Joystick on", 36);
	}
	if (conf.ORTHOVIEW.get()) {
		glutChangeToMenuEntry(23, "3D perspectivic view",
				      41);
	} else {
		glutChangeToMenuEntry(23, "2D orthographic view",
				      41);
	}

	MENUCHANGED=false;

}

void menuMapSet(int val)
{
	MENUPOPPED=true;
	if (val == 9999) return;

	conf.ActiveMapSet.set(val);

	if (conf.MAP.get()) {
		GenLandscapeList();
		glutPostRedisplay();

	}

}

void menuStereo(int val)
{
	MENUPOPPED=true;
	// nothing to do in Hardware stereo mode
	// HW stereo can be enabled at start time and cannot be changed then
	if (conf.STEREO_HW.get()) {
		return;
	}

	bool REBUILD = false;

	switch (val) {

		// turn off stereo modes
	case 1:

		// rebuilding terrain list is necessary if we had colored anaglyph before
		// because everything is drawn in B&W then
		if ((conf.STEREO_RG.get() || conf.STEREO_RB.get())
		    && !conf.BW.get()) {
			REBUILD = true;
		}
		conf.STEREO.off();
		conf.STEREO_RG.off();
		conf.STEREO_RB.off();
		InitGL(conf.getwidth(), conf.getheight());
		if (REBUILD) {
			GenLandscapeList();
			GenFlightList();
			GenAirspaceList();
		}
		glutPostRedisplay();
		break;

	case 2:
		if (conf.STEREO.get()) {
			return;
		}
		if ((conf.STEREO_RG.get() || conf.STEREO_RB.get())
		    && !conf.BW.get()) {
			REBUILD = true;
		}
		conf.STEREO.on();
		conf.STEREO_RG.off();
		conf.STEREO_RB.off();
		InitGL(conf.getwidth(), conf.getheight());
		if (REBUILD) {
			GenLandscapeList();
			GenFlightList();
			GenAirspaceList();
		}
		glutPostRedisplay();
		break;

	case 3:
		if (conf.STEREO_RG.get()) {
			return;
		}
		if (!(conf.STEREO_RB.get() || conf.BW.get())) {
			REBUILD = true;
		}
		conf.STEREO.off();
		conf.STEREO_RG.on();
		conf.STEREO_RB.off();
		InitGL(conf.getwidth(), conf.getheight());
		if (REBUILD) {
			GenLandscapeList();
			GenFlightList();
			GenAirspaceList();
		}
		glutPostRedisplay();
		break;

	case 4:
		if (conf.STEREO_RB.get()) {
			return;
		}
		if (!(conf.STEREO_RG.get() || conf.BW.get())) {
			REBUILD = true;
		}
		conf.STEREO.off();
		conf.STEREO_RG.off();
		conf.STEREO_RB.on();
		InitGL(conf.getwidth(), conf.getheight());
		if (REBUILD) {
			GenLandscapeList();
			GenFlightList();
			GenAirspaceList();
		}
		glutPostRedisplay();
		break;

	case 5:
		conf.SWAP_STEREO.toggle();
		glutPostRedisplay();
		break;
	}
}


void menuColormap(int val)
{
	MENUPOPPED=true;
	if (conf.colmapnumber.get() == val) {
		return;
	}
	conf.colmapnumber.set(val);

	if ((conf.LANDSCAPE.get() && conf.MAP.get() && conf.MODULATE.get())
	    || (conf.LANDSCAPE.get() && !conf.MAP.get()
		&& !conf.WIRE.get())) {

		GenLandscapeList();
		glutPostRedisplay();
	}
}

void menuColormap_sea(int val)
{
	MENUPOPPED=true;
	if (conf.colmap2number.get() == val) {
		return;
	}
	conf.colmap2number.set(val);

	if ((conf.LANDSCAPE.get() && conf.MAP.get() && conf.MODULATE.get())
	    || (conf.LANDSCAPE.get() && !conf.MAP.get()
		&& !conf.WIRE.get())) {

		GenLandscapeList();
		glutPostRedisplay();
	}
}

void menuFSColormap(int val)
{
	MENUPOPPED=true;
	if (conf.flightstrip_colmap.get() == val) {
		return;
	}
	conf.flightstrip_colmap.set(val);
	GenFlightList();
	glutPostRedisplay();
}

void menuFSMode(int val)
{
	MENUPOPPED=true;
	if (conf.flightstrip_mode.get() == val) {
		return;
	}
	conf.flightstrip_mode.set(val);
	GenFlightList();
	glutPostRedisplay();
}

void menuAirspaceType(int val)
{
	MENUPOPPED=true;
	switch (val) {

	case 1:
		conf.AIRSPACE_P.toggle();
		MENUCHANGED = true;
		break;

	case 2:
		conf.AIRSPACE_R.toggle();
		MENUCHANGED = true;
		break;

	case 3:
		conf.AIRSPACE_Q.toggle();
		MENUCHANGED = true;
		break;

	case 4:
		conf.AIRSPACE_C.toggle();
		MENUCHANGED = true;
		break;

	case 5:
		conf.AIRSPACE_D.toggle();
		MENUCHANGED = true;
		break;

	case 6:
		conf.AIRSPACE_CTR.toggle();
		MENUCHANGED = true;
		break;
	}

	GenAirspaceList();
	glutPostRedisplay();
}

void GenMenu(void)
{
	// create Map-Set-Submenu
	//if (conf.getMapSetName().size() > 0) {
	//cout << "XXX" << conf.getMapSetName().size() << endl;

    // freeglut 3.0!
// 	glutSetMenuFont(menIdMain, GLUT_BITMAP_TIMES_ROMAN_10);

	menIdMapSet = glutCreateMenu(menuMapSet);
	vector < string > MapSets;
	MapSets = conf.getMapSetName();

	if (MapSets.size() == 0) {
		glutAddMenuEntry("no map sets found", 9999);
	}

	for (int i = 0; i < (int) MapSets.size(); i++) {
		//cout << "set:" << i <<" "<< MapSets[i] << endl;
		glutAddMenuEntry(MapSets[i].c_str(), i + 1);
	}
	//}

	// create Stereo menu
	menIdStereo = glutCreateMenu(menuStereo);
	glutAddMenuEntry("Stereo off", 1);
	glutAddMenuEntry("Double image", 2);
	glutAddMenuEntry("Red Green", 3);
	glutAddMenuEntry("Red Blue", 4);
	glutAddMenuEntry("Swap Images", 5);

	//create Colormapmenu
	menIdColormap = glutCreateMenu(menuColormap);
	glutAddMenuEntry("Max's colormap", 1);
	glutAddMenuEntry("Atlas colormap", 2);
	glutAddMenuEntry("Graymap", 3);
	glutAddMenuEntry("Atlas 2 colormap", 4);
	glutAddMenuEntry("Violett - blue", 5);
	glutAddMenuEntry("Killer Rainbow map", 6);
	glutAddMenuEntry("White", 7);


	//create Colormapmenu  sea
	menIdColormap_sea = glutCreateMenu(menuColormap_sea);
	glutAddMenuEntry("Max's colormap", 1);
	glutAddMenuEntry("Atlas colormap", 2);
	glutAddMenuEntry("Graymap", 3);
	glutAddMenuEntry("Atlas 2 colormap", 4);
	glutAddMenuEntry("Violett - blue", 5);
	glutAddMenuEntry("Killer Rainbow map", 6);
	glutAddMenuEntry("White", 7);

	//create Colormapmenu   FS
	menIdFSColormap = glutCreateMenu(menuFSColormap);
	glutAddMenuEntry("Max's colormap", 1);
	glutAddMenuEntry("Atlas colormap", 2);
	glutAddMenuEntry("Graymap", 3);
	glutAddMenuEntry("Atlas 2 colormap", 4);
	glutAddMenuEntry("Violett - blue", 5);
	glutAddMenuEntry("Killer Rainbow map", 6);
	glutAddMenuEntry("White", 7);

	//create FS Mode menu
	menIdFSMode = glutCreateMenu(menuFSMode);
	glutAddMenuEntry("classic", 0);
	glutAddMenuEntry("altitude", 1);
	glutAddMenuEntry("speed", 2);
	glutAddMenuEntry("vario", 3);


	//create Airspace type menu
	menIdAirspaceType=glutCreateMenu(menuAirspaceType);
        if (conf.AIRSPACE_P.get()) {
		glutAddMenuEntry("Prohibited P off", 1);	//1
	} else {
		glutAddMenuEntry("Prohibited P on", 1);
	}

	if (conf.AIRSPACE_R.get()) {
		glutAddMenuEntry("Restricted R off", 2);	//2
	} else {
		glutAddMenuEntry("Restricted R on", 2);
	}

	if (conf.AIRSPACE_Q.get()) {
		glutAddMenuEntry("Danger Q off", 3);	//3
	} else {
		glutAddMenuEntry("Danger Q on", 3);
	}

	if (conf.AIRSPACE_C.get()) {
		glutAddMenuEntry("Class C off", 4);	//4
	} else {
		glutAddMenuEntry("Class C on", 4);
	}

	if (conf.AIRSPACE_D.get()) {
		glutAddMenuEntry("Class D off", 5);	//5
	} else {
		glutAddMenuEntry("Class D on", 5);
	}

	if (conf.AIRSPACE_CTR.get()) {
		glutAddMenuEntry("CTR off", 6);	//6
	} else {
		glutAddMenuEntry("CTR on", 6);
	}


	// create Main Menu
	menIdMain = glutCreateMenu(menuMain);

	if (conf.MAP.get()) {
		glutAddMenuEntry("Maps off", 1);	//1
	} else {
		glutAddMenuEntry("Maps on", 1);
	}

	if (conf.MODULATE.get()) {
		glutAddMenuEntry("Colored maps off", 2);	//2
	} else {
		glutAddMenuEntry("Colored maps on", 2);
	}

	glutAddSubMenu("Map Sets", menIdMapSet);	//3


	//-----------
	glutAddMenuEntry(" ", 0);	//4


	if (conf.LANDSCAPE.get()) {
		glutAddMenuEntry("Terrain off", 11);	//5
	} else {
		glutAddMenuEntry("Terrain on", 11);
	}

	glutAddSubMenu("Terrain Colorramps", menIdColormap);	//6
	glutAddSubMenu("Sea Colorramps", menIdColormap_sea);	//7

	if (conf.FOG.get()) {
		glutAddMenuEntry("Haze off", 12);	//8
	} else {
		glutAddMenuEntry("Haze on", 12);
	}

	if (conf.AIRSPACE.get()) {
		glutAddMenuEntry("Airspace off", 13);	//9
	} else {
		glutAddMenuEntry("Airspace on", 13);
	}

	glutAddSubMenu("Airspace types", menIdAirspaceType); // 10

	//------------
	glutAddMenuEntry(" ", 0);	//11

	glutAddSubMenu("Flight track Colorramp", menIdFSColormap);	//12
	glutAddSubMenu("Flight track Mode", menIdFSMode);	//13

	if (conf.CURTAIN.get()) {
		glutAddMenuEntry("Curtain off", 21);	//14
	} else {
		glutAddMenuEntry("Curtain on", 21);
	}


	//---------
	glutAddMenuEntry(" ", 0);	//15

	if (conf.MARKER.get()) {
		glutAddMenuEntry("Marker off", 31);	//16
	} else {
		glutAddMenuEntry("Marker on", 31);
	}

	if (conf.INFO.get()) {
		glutAddMenuEntry("Info off", 32);	//17
	} else {
		glutAddMenuEntry("Info on", 32);
	}

	if (conf.FOLLOW.get()) {
		glutAddMenuEntry("Follow off", 33);	//18
	} else {
		glutAddMenuEntry("Follow on", 33);
	}

	if (conf.MARKER_RANGE.get()) {
		glutAddMenuEntry("Range off", 34);	//19
	} else {
		glutAddMenuEntry("Range on", 34);
	}

	if (conf.FLYING.get()) {
		glutAddMenuEntry("Movie off", 35);	//20
	} else {
		glutAddMenuEntry("Movie on", 35);
	}

	if (conf.JOYSTICK.get()) {
		glutAddMenuEntry("Joystick off",36);  //21
	} else {
		glutAddMenuEntry("Joystick on",36);
	}

	//-------

	glutAddMenuEntry(" ", 0);	//22

	if (conf.ORTHOVIEW.get()) {
		glutAddMenuEntry("3D perspectivic view", 41);	//23
	} else {
		glutAddMenuEntry("2D orthographic view", 41);
	}

	glutAddSubMenu("Stereo Modes", menIdStereo);	//24

	glutAddMenuEntry(" ", 0);	//25

	glutAddMenuEntry("Help", 97);	//26
	glutAddMenuEntry("About OGIE",98);		//27
	glutAddMenuEntry("Quit", 99);	//28
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


void menuStatus(int val, int x, int y)
{
	MENUPOPPED = true;

	if (conf.DEBUG.get())
		cout << "menuStatus called: ";

	if (val == GLUT_MENU_IN_USE) {
		MENUSTATE = true;
		if (conf.DEBUG.get())
			cout << "ON" << endl;

	}

	if (val == GLUT_MENU_NOT_IN_USE) {
		MENUSTATE = false;
		MENUPOPPED = false;
		if (conf.DEBUG.get())
			cout << "OFF" << endl;
		if (MENUCHANGED)
		  menuRecreate();
	}
}

// all the menu stuff  #ifndef ONLY_OSMESA
#endif


//this function sets up the flight-display list
void GenFlightList(void)
{
	if (conf.DEBUG.get()) {
		cout << "GenFlightList START: " << flush;
		#ifndef ONLY_OSMESA
		glutReportErrors();
		#endif
		cout << endl << flush;
	}

	glDeleteLists(flightdata_displ_list, 1);	//delete list to free memory
	flightdata_displ_list = glGenLists(1);	//create new List
	glNewList(flightdata_displ_list, GL_COMPILE);

	/*
	if (conf.DEBUG.get()) {
		cout << "GenFlightList 1: " << flush;
		glutReportErrors();
		cout << endl << flush;
	}
	*/

	// draw flight linestrip
	if (!conf.LANDSCAPE.get() && !conf.MAP.get()) {
		fd.draw_linestrip(conf.offset.get(), conf.STEREO_RB.get()
				  || conf.STEREO_RG.get()
				  || conf.BW.get(), 0.8, 0.1, 0.1,
				  1.0, 1.0, 1.0);
	} else {
		fd.draw_linestrip(conf.offset.get(), conf.STEREO_RB.get()
				  || conf.STEREO_RG.get()
				  || conf.BW.get(), conf.flightstrip_col_rup.get(), conf.flightstrip_col_gup.get(), conf.flightstrip_col_bup.get(),
				  conf.flightstrip_col_rdown.get(), conf.flightstrip_col_gdown.get(), conf.flightstrip_col_bdown.get());
	}

	/*
	if (conf.DEBUG.get()) {
		cout << "GenFlightList 2: " << flush;
		glutReportErrors();
		cout << endl << flush;
	}
	*/

	// The "Shadow" for "Flat-Landscape"
	if (conf.FLIGHT.get()) {
		if (!conf.LANDSCAPE.get()) {
			fd.draw_shadow(conf.offset.get(),
				       conf.STEREO_RB.get()
				       || conf.STEREO_RG.get()
				       || conf.BW.get(), 2.0, 0.0, 0.0,
				       0.0);
		} else {
			fd.draw_shadow_terrain(conf.STEREO_RB.get()
					       || conf.STEREO_RG.get()
					       || conf.BW.get(), 1.0, 0.0,
					       0.0, 0.3);
		}
	}
	// we need to render transparent stuff at the end (thus put it at the end of the flight_list)

	/*
                if (conf.DEBUG.get()) {
		cout << "GenFlightList 3: " << flush;
		glutReportErrors();
		cout << endl << flush;
	}
	*/

	// Curtain
	if (conf.CURTAIN.get()) {
		fd.draw_curtain(conf.offset.get(), conf.STEREO_RB.get()
				|| conf.STEREO_RG.get()
				|| conf.BW.get(), 0.0, 0.0, 0.4, 0.4);
	}
	glEndList();
	// this used to be an experiment to smooth the linestrip:
	// fd.setglEvaluator();
	if (conf.DEBUG.get()) {
		cout << "GenFlightList END: " << flush;
		#ifndef ONLY_OSMESA
		glutReportErrors();
		#endif
		cout << endl << flush;
	}
}

void GenAirspaceList()
{

                if (conf.DEBUG.get()) {
		cout << "GenAirList START: " << flush;
		#ifndef ONLY_OSMESA
		glutReportErrors();
		#endif
		cout << endl << flush;
	}

	glDeleteLists(airspace_displ_list, 1);	// delete list to free memory
	airspace_displ_list = glGenLists(1);	// create new List
	glNewList(airspace_displ_list, GL_COMPILE);

	//Airspace
	if (conf.AIRSPACE.get()) {
		air.drawAirspace(conf.STEREO_RB.get()
				 || conf.STEREO_RG.get() || conf.BW.get());
	}
	glEndList();

	if (conf.DEBUG.get()) {
		cout << "GenAirList END: " << flush;
		#ifndef ONLY_OSMESA
		glutReportErrors();
		#endif
		cout << endl << flush;
	}
}

/*
void GenPointsList()
{
	if (conf.DEBUG.get()) {
	cout << "GenPointsList START: " << flush;
	#ifndef ONLY_OSMESA
	glutReportErrors();
	#endif
	cout << endl << flush;
	}

	// lifts. more sections are needed for WPs etc
	glDeleteLists(lifts_displ_list,1);
	lifts_displ_list = glGenLists(1);
	glNewList(lifts_displ_list, GL_COMPILE);

	if (conf.LIFTS.get()) {
		lifts.drawLifts(conf.STEREO_RB.get()
				 || conf.STEREO_RG.get() || conf.BW.get());
	}

	glEndList();

	if (conf.DEBUG.get()) {
		cout << "GenPointsList END: " << flush;
		#ifndef ONLY_OSMESA
		glutReportErrors();
		#endif
		cout << endl << flush;
	}

}
*/

// generate the terrain display list
void GenLandscapeList()
{

	landscape_jump:

	if (conf.DEBUG.get()) {
		cout << "GenLandList START: " << flush;
		#ifndef ONLY_OSMESA
		glutReportErrors();
		#endif
		cout << endl << flush;
	}


	// loading message
	#ifndef ONLY_OSMESA
	if (!( conf.OSMESA.get() || conf.OFFSCREEN.get()))
		glutSwapBuffers();	//nobody knows whats in the back buffer,
	// but there is a good changce that after swapping there is the previous displayed image

	// make shure that the text appears right and left in hw-stereo-mode
	// doesnt make sense in offscreen mode... and GL_BACK would segfault!
	if (! ( conf.OSMESA.get() || conf.OFFSCREEN.get() )) {
		if (conf.STEREO_HW.get()) {
			glDrawBuffer(GL_BACK);
		}

    //vector <string> message;
    //message.push_back("loading...");


		int font_height = conf.getheight() / 5;
		glLineWidth(5.0);
		gltext(conf.getwidth() / 4, (conf.getheight() / 2),
			font_height, "loading...", conf.getwidth(),
			conf.getheight());
		glutSwapBuffers();
		glLineWidth(1.0);
	}
	#endif



	glDeleteLists(landscape_displ_list, 1);	//free memory
	landscape_displ_list = glGenLists(1);	//create new List
	glNewList(landscape_displ_list, GL_COMPILE);





	//the huge green rectangle       (flat-landscape)
	if (!conf.LANDSCAPE.get() && !conf.MAP.get()) {
		//glPushAttrib(GL_ENABLE_BIT);
		//glPushAttrib(GL_POLYGON_BIT);

		//glPolygonMode(GL_FRONT, GL_FILL);   //should already be set (default)
		glDisable(GL_LIGHTING);

		glBegin(GL_POLYGON);

		float _col[3] = { 0.0, 0.3, 0.0 };

		if (conf.BW.get() || conf.STEREO_RB.get()
		    || conf.STEREO_RG.get()) {
			glColor3fv(graycolor(_col));
		} else {
			glColor3fv(_col);
		}

		// if we have texture maps the green floor should be deeper to avoid strange effects...
		// can be deeper in any case (why not?) fd.getzmin - 0.1 km (=-100m)
		GLfloat greenheight =
		    fd.getzmin() + ((float) conf.offset.get() / 1000.0) -
		    0.1;


		glVertex3f((GLfloat) fd.getxmax() + conf.getborder(),
			   (GLfloat) fd.getymin() - conf.getborder(),
			   greenheight);
		glVertex3f((GLfloat) fd.getxmax() + conf.getborder(),
			   (GLfloat) fd.getymax() + conf.getborder(),
			   greenheight);
		glVertex3f((GLfloat) fd.getxmin() - conf.getborder(),
			   (GLfloat) fd.getymax() + conf.getborder(),
			   greenheight);
		glVertex3f((GLfloat) fd.getxmin() - conf.getborder(),
			   (GLfloat) fd.getymin() - conf.getborder(),
			   greenheight);
		glEnd();

		//glPopAttrib();
		glEnable (GL_LIGHTING);   // reset our default
	}


	// TESTING
/*	float _x,_y;
	proj.get_xy(47.0,11.0,_x,_y);
	glRasterPos3f(_x,_y,3.5);
	glColor3f(0,0,0);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'X');
*/

	// textured maps...    on flat ground
	if (conf.MAP.get() && !conf.LANDSCAPE.get()) {

		for (int r = conf.getMapIndexLow();
		     r < conf.getMapIndexHigh(); r++) {

			Map map;
			map.set_proj_pointer(&proj);
			map.set_conf_pointer(&conf);

			if (conf.getMapFilename(r)[ conf.getMapFilename(r).size() - 3] == 'j'
			    || conf.getMapFilename(r)[ conf.getMapFilename(r).size() -  3] == 'J') {

				map.readMap_jpeg(conf.getMapFilename(r),
						 conf.getmap_width(r),
						 conf.getmap_height(r));
			} else {

				map.readMap(conf.getMapFilename(r),
					    conf.getmap_width(r),
					    conf.getmap_height(r));
			}

			map.setcenter_lat(fd.getcenter_lat());
			map.setcenter_lon(fd.getcenter_lon());

			map.settop(conf.getmap_top(r));
			map.setbottom(conf.getmap_bottom(r));
			map.setright(conf.getmap_right(r));
			map.setleft(conf.getmap_left(r));

			map.setglmap(fd.getzmin() +
				     ((float) conf.offset.get() / 1000.0) -
				     0.01, conf.COMPRESSION.get(), true,
				     conf.STEREO_RB.get()
				     || conf.STEREO_RG.get()
				     || conf.BW.get());
		}
	}




	//Terrain with textured map
	if (conf.LANDSCAPE.get() && conf.MAP.get()) {

		short int _max_h_bkp;
		short int _min_h_bkp;

		if (conf.MODULATE.get()) {

			short int _max_h = -8888;
			short int _min_h = -8888;

			_max_h_bkp = conf.getmax_h();
			_min_h_bkp = conf.getmin_h();

			// get min and max_h with dummy readDEM_fast
			// vielleicht kriegt man das noch mal schneller hin?

			//cout << conf.getmax_h() << " max  -- min: " << conf.getmin_h() << endl;

			if (conf.getmax_h() == -9999
			    || conf.getmin_h() == -9999) {

				for (int r = conf.getMapIndexLow();
				     r < conf.getMapIndexHigh(); r++) {

					Landscape land;
					land.set_conf_pointer(&conf);

					land.set_sealevel((double) conf.
							  get_sealevel());
					land.set_sealevel2((double) conf.
							   get_sealevel2
							   ());
					land.set_sealevel3((double) conf.
							get_sealevel3());

					land.set_proj_pointer(&proj);
					land.setdownscalefactor(conf.
								getdownscalefactor
								());
					land.setcenter_lat(fd.
							   getcenter_lat
							   ());
					land.setcenter_lon(fd.
							   getcenter_lon
							   ());
					land.setmax_lat(conf.
							getmap_top(r));
					land.setmin_lat(conf.
							getmap_bottom(r));
					land.setmax_lon(conf.
							getmap_right(r));
					land.setmin_lon(conf.
							getmap_left(r));
					land.setgrid_lat(conf.
							 getdem_grid_lat
							 ());
					land.setgrid_lon(conf.
							 getdem_grid_lon
							 ());

					int ret = land.readDEM(conf.
						     getDemFileName(),
						     conf.
						     getdem_rows(),
						     conf.
						     getdem_columns(),
						     conf.
						     getdem_lat_min(),
						     conf.
						     getdem_lat_max(),
						     conf.
						     getdem_lon_min(),
						     conf.
						     getdem_lon_max
						     (), true);

					if (ret == 99) {
						if (conf.DEBUG.get())
							cout << "readDEM returned 99 " << endl << flush;
						glEndList();
						goto landscape_jump;
					}


					if (_min_h == -8888)
						_min_h = land.getmin_h();
					if (_max_h == -8888)
						_max_h = land.getmax_h();

					if (land.getmin_h() < _min_h)
						_min_h = land.getmin_h();
					if (land.getmax_h() > _max_h)
						_max_h = land.getmax_h();
				}

				if (conf.getmax_h() == -9999)
					conf.setmax_h(_max_h);
				if (conf.getmin_h() == -9999)
					conf.setmin_h(_min_h);
			}
			if (conf.VERBOSE.get()) {
				cout <<
				    "Max and Min Alt used for modulated colors: (min, max): "
				    << conf.getmin_h() << ", " << conf.
				    getmax_h() << endl;
			}
		}




		//cout << "entering Terrain AND Map mode..." << endl;
		for (int r = conf.getMapIndexLow();
		     r < conf.getMapIndexHigh(); r++) {

			Map map;
			map.set_proj_pointer(&proj);
			map.set_conf_pointer(&conf);
			Landscape land;
			land.set_conf_pointer(&conf);

			land.set_sealevel((double) conf.get_sealevel());
			land.set_sealevel2((double) conf.get_sealevel2());
			land.set_sealevel3((double) conf.get_sealevel3());


			land.set_proj_pointer(&proj);

			land.setdownscalefactor(conf.getdownscalefactor());

			if (conf.getMapFilename(r)[conf.getMapFilename(r).size() - 3] == 'j') {

				map.readMap_jpeg(conf.getMapFilename(r),
						 conf.getmap_width(r),
						 conf.getmap_height(r));

			} else {

				map.readMap(conf.getMapFilename(r),
					    conf.getmap_width(r),
					    conf.getmap_height(r));
			}

			land.setcenter_lat(fd.getcenter_lat());
			land.setcenter_lon(fd.getcenter_lon());
			map.setcenter_lat(fd.getcenter_lat());
			map.setcenter_lon(fd.getcenter_lon());

			land.setmax_lat(conf.getmap_top(r));
			land.setmin_lat(conf.getmap_bottom(r));
			land.setmax_lon(conf.getmap_right(r));
			land.setmin_lon(conf.getmap_left(r));

			land.setgrid_lat(conf.getdem_grid_lat());
			land.setgrid_lon(conf.getdem_grid_lon());

			int ret = land.readDEM(conf.getDemFileName(),
				     conf.getdem_rows(),
				     conf.getdem_columns(),
				     conf.getdem_lat_min(),
				     conf.getdem_lat_max(),
				     conf.getdem_lon_min(),
				     conf.getdem_lon_max());

			if (ret == 99) {
				if (conf.DEBUG.get())
					cout << "readDEM returned 99 " << endl << flush;
				glEndList();
				goto landscape_jump;
			}

			// load map, but do not set texture coordinates
			// that will be done via land.setgltrainglestrip_tex
			map.setglmap(fd.getzmin() +
				     ((float) conf.offset.get() / 1000.0) -
				     0.01, conf.COMPRESSION.get(),
				     conf.MAPS_UNLIGHTED.get(),
				     conf.STEREO_RB.get()
				     || conf.STEREO_RG.get()
				     || conf.BW.get(), false);

			if (conf.getmax_h() != -9999) {
				land.setmax_h(conf.getmax_h());
				//cout << "max = "<< conf.getmax_h() << endl;
			}

			if (conf.getmin_h() != -9999) {
				land.setmin_h(conf.getmin_h());
				//cout << "min = "<<conf.getmin_h() << endl;
			}

			land.setgltrianglestrip_tex(conf.colmapnumber.
						    get(),
						    conf.colmap2number.
						    get(),
						    (conf.STEREO_RB.get()
						     || conf.STEREO_RG.
						     get()
						     || conf.BW.get()));

		}

		// restore values
		if (conf.MODULATE.get()) {
			conf.setmax_h(_max_h_bkp);
			conf.setmin_h(_min_h_bkp);
		}
	}
	// Terrain, only
	if (conf.LANDSCAPE.get() && !conf.MAP.get()) {

		Landscape land;
		land.set_conf_pointer(&conf);

		land.set_sealevel((double) conf.get_sealevel());
		land.set_sealevel2((double) conf.get_sealevel2());
		land.set_sealevel3((double) conf.get_sealevel3());


		land.set_proj_pointer(&proj);

		land.setdownscalefactor(conf.getdownscalefactor());

		land.setcenter_lat(fd.getcenter_lat());
		land.setcenter_lon(fd.getcenter_lon());
		//adding a border in km (border_land)
		float border_deg_lat = conf.getborder_land_lat() / DEGDIST;
		float border_deg_lon =
		    (conf.getborder_land_lon() / DEGDIST) /
		    cos(fd.getcenter_lat() * PI_180);
		land.setmin_lat(fd.getlatmin() - border_deg_lat);
		land.setmin_lon(fd.getlonmin() - border_deg_lon);
		land.setmax_lat(fd.getlatmax() + border_deg_lat);
		land.setmax_lon(fd.getlonmax() + border_deg_lon);

		if (conf.VERBOSE.get()) {
		 cout << "Terrain boundaries: " << land.getmax_lat() << "  " << land.getmin_lat() <<
		 "   " << land.getmin_lon() << "  " << land.getmax_lon() << endl;
		}

		land.setgrid_lat(conf.getdem_grid_lat());
		land.setgrid_lon(conf.getdem_grid_lon());

		int ret = land.readDEM(conf.getDemFileName(), conf.getdem_rows(),
			     conf.getdem_columns(), conf.getdem_lat_min(),
			     conf.getdem_lat_max(), conf.getdem_lon_min(),
			     conf.getdem_lon_max());


		if (ret == 99) {
			if (conf.DEBUG.get())
				cout << "readDEM returned 99 " << endl << flush;
			glEndList();
			goto landscape_jump;
		}

		if (conf.getmax_h() != -9999) {
			land.setmax_h(conf.getmax_h());
			//cout << "max = "<< conf.getmax_h() << endl;
		}

		if (conf.getmin_h() != -9999) {
			land.setmin_h(conf.getmin_h());
			//cout << "min = "<<conf.getmin_h() << endl;
		}

		if (conf.VERBOSE.get()) {
			cout <<
			    "Max and Min Alt used for colors: (min, max): "
			    << land.getmin_h() << ", " << land.
			    getmax_h() << endl;


/*
			// these are derived from landscape.cpp
			int latsteps = (int) (((float)
					       (land.getsteps_lat() -
						3 *
						conf.
						getdownscalefactor()) /
					       (float) conf.
					       getdownscalefactor()) +
					      0.5);
			int lonsteps = (int) (((float)
					       (land.getsteps_lon() -
						2 *
						conf.getdownscalefactor() -
						1) /
					       (float) conf.
					       getdownscalefactor()) +
					      0.5);

			cout << "The Surface is build with " <<
			    latsteps * lonsteps * 2
			    << " Triangles." << endl;
*/

			cout << "DEM data is read in byte order: ";
			if (conf.BIGENDIAN.get()) {
				cout << "big endian" << endl;
			} else {
				cout << "little endian" << endl;
			}

			if (conf.input_dem_factor.get() != 1.0)
				cout << "DEM input scaling factor: " << conf.input_dem_factor.get() << endl;


					}

		land.setgltrianglestrip(conf.WIRE.get(),
					conf.colmapnumber.get(),
					conf.colmap2number.get(),
					(conf.STEREO_RB.get()
					 || conf.STEREO_RG.get()
					 || conf.BW.get()), false);


	}
	glEndList();		//end of the list

	if (conf.DEBUG.get()) {
		cout << "GenLandList END: " << flush;
		#ifndef ONLY_OSMESA
		glutReportErrors();
		#endif
		cout << endl << flush;
	}

}

// mouse callback-function (when any button is clicked)
#ifndef ONLY_OSMESA
void mouseClickMove(int xmouse, int ymouse)
{

	if (HELP || INFO) {
		return;
	}

	if (!conf.MOUSE.get()) {
		if (MOUSEWARPED) {
			lastmx = conf.getwidth() / 2;
			lastmy = conf.getheight() / 2;
			MOUSEWARPED = false;
			return;
		}
	}


	if (lastmx == 0) {	//nothing to do in first call
		lastmx = xmouse;	//but set the x and y in global lastmx
		lastmy = ymouse;	//and lastmy
		return;
	}




	//double factor;
	if (lastmx == 0) {	//nothing to do in first call
		lastmx = xmouse;	//but set the x and y in global lastmx
		lastmy = ymouse;	//and lastmy
		return;
	}

	double _dx = xmouse - lastmx;	//put differentials into _dx and _dy
	double _dy = ymouse - lastmy;
	lastmx = xmouse;	//save present position
	lastmy = ymouse;

	//cout << "dx dy " << _dx << " " << _dy << endl;
	if (abs(_dx) > 20 || abs(_dy) > 20) {
		// we don't want the view to skip, so ignore what is most likely the first call
		return;
	}


	if (!conf.ORTHOVIEW.get()) {	// Only if we are not in ORTHO view
		float _mstep;


		// left button: move towards point (center)  and back

		   if (middlebuttonstate == GLUT_DOWN && leftbuttonstate != GLUT_DOWN) {
     		   double factor;
		     if(_dy < 0){
		     factor = pow(0.99, abs((int)_dy));
		   } else  {
		    factor = pow(1.01, abs((int)_dy));
		    }

		     if (!conf.MARKER.get()) {

		     	// move to central point
			ego.setx(ego.getx() * factor);
		     	ego.sety(ego.gety() * factor);
		     	ego.setz(ego.getz() * factor);

			//if (conf.DEBUG.get())
			//	cout << "wo marker ego.x: " << ego.getx() << "   ego.y: " << ego.gety() << "   ego.z: " << ego.getz() << endl;

			} else {
			//move towards markerpos

			// z-koordinates is fscked, even for  !MARKER!!!
			//cout << "egoz/3: " << ego.getz()/3<< "  markerz: "<<fd.getmarkerz()<< "  fac" << factor << endl;
			//cout << (ego.getz()/3) - fd.getmarkerz() << "diff " << endl;
			//if (conf.DEBUG.get())
			//	cout << "w marker ego.x: " << ego.getx() << "   ego.y: " << ego.gety() << "   ego.z: " << ego.getz() << endl;

			ego.setx(  - fd.getmarkerx() + ( ego.getx() + fd.getmarkerx()) * factor );
		     	ego.sety(  - fd.getmarkery() + ( ego.gety() + fd.getmarkery()) * factor );
		     	ego.setz( (  fd.getmarkerz() + ( ego.getz()/conf.z_scale.get() - fd.getmarkerz()) * factor ) * conf.z_scale.get() );
			}

		     // this is probably not needed...
		     //ego.movef(0.2 * _dy);

		   }

		if (middlebuttonstate == GLUT_DOWN && leftbuttonstate == GLUT_DOWN) {
			ego.moveu(0.2 * _dy);
			ego.moves(0.2 * _dx);
		} else if (leftbuttonstate == GLUT_DOWN && middlebuttonstate != GLUT_DOWN) {	// left button is down
			if (args_info.smooth_mouse_given) {
				_mstep = 10;
			} else {
				_mstep = 3;
			}
			if (!conf.MARKER.get()) {
				ego.spinmoveradial(_dx / _mstep);	//spin the ego around the view centre while adjusting view angle
				ego.spinmovevertical(_dy / _mstep);	// grosser Teiler = sanfte Bewegung
			} else {
				ego.spinmoveradialpoint(_dx / _mstep, fd.getmarkerx(), fd.getmarkery());	//spin the ego around the view centre while adjusting view angle
				ego.spinmoveverticalpoint(_dy / _mstep, fd.getmarkerx(), fd.getmarkery());	// grosser Teiler = sanfte Bewegung
			}
		}
	}


	if (conf.FOLLOW.get()) {
		update_diff_pos();
	}


	if (!conf.MOUSE.get()) {
		// warp pointer to center of window
		glutWarpPointer(conf.getwidth() / 2, conf.getheight() / 2);
		MOUSEWARPED = true;
	}

	glutPostRedisplay();	//update display
}

void mouseClickState(int button, int state, int xmouse, int ymouse)
{
	if (button == GLUT_RIGHT_BUTTON) {
		rightbuttonstate = state;
	}
	if (button == GLUT_MIDDLE_BUTTON) {
		middlebuttonstate = state;
	}

	if (button == GLUT_LEFT_BUTTON) {
		leftbuttonstate = state;
	}

}

// mouse callback-function
void mouseMove(int xmouse, int ymouse)
{

	if (HELP || INFO) {
		return;
	}



	// return imediatley if any menu is in use
	if (MENUSTATE) {
		//cout << "menu acitve" << endl;
		return;
	}

	// if menu was used right before avoid "jumping"
	if (MENUPOPPED) {
		//cout << "mouse move called after menu POPPPED" << endl;
		lastmx = 0;
		lastmy=0;
		MENUPOPPED = false;

		//glutWarpPointer(conf.getwidth() / 2, conf.getheight() / 2);
		//MOUSEWARPED = true;
		return;
	}

	//if pointer was warped before (callback is triggered by warping...)
	//
	if (MOUSEWARPED) {
		lastmx = conf.getwidth() / 2;
		lastmy = conf.getheight() / 2;
		MOUSEWARPED = false;
		return;
	}

	if (lastmx == 0) {	//nothing to do in first call
		lastmx = xmouse;	//but set the x and y in global lastmx
		lastmy = ymouse;	//and lastmy
		return;
	}

	double _dx = xmouse - lastmx;	//put differences to _dx and _dy
	double _dy = ymouse - lastmy;
	lastmx = xmouse;	//save actual position
	lastmy = ymouse;

	//cout << "dx dy " << _dx << " " << _dy << endl;


	// this should prevent "jumps" if the curser enters the window again!
	// seems to be effective
	if (abs(_dx)+abs(_dy) > 100) {
	return;
	}

	if (!conf.ORTHOVIEW.get()) {
		float _mstep;
		if (args_info.smooth_mouse_given) {
			_mstep = 10;
		} else {
			_mstep = 3;
		}
		ego.spinz(_dx / _mstep);	//spin the ego!
		ego.spinx(-_dy / _mstep);	// grosser Teiler = sanfte Bewegung
	} else {
		ego.shiftx(_dx * conf.getorthoshift());
		ego.shifty(_dy * conf.getorthoshift());

		if (conf.FOLLOW.get())
			update_diff_pos(); // ????
		InitGL(conf.getwidth(), conf.getheight());

	}

	// warp pointer to center of window
	glutWarpPointer(conf.getwidth() / 2, conf.getheight() / 2);
	MOUSEWARPED = true;


	glutPostRedisplay();	//update display
}
#endif
//mouse functions excluded for ONLY_OSMESA

//Draw only.
void Draw(void)
{

	//Timecheck drawtime("Draw", &conf);

	//call the lists
	glCallList(landscape_displ_list);


	if (conf.FLIGHT.get()) {
		if (conf.MARKER.get()) {
			if (!conf.ORTHOVIEW.get()) {
				fd.draw_marker(conf.offset.get(),
					       conf.z_scale.get(),
					       conf.STEREO_RB.get()
					       || conf.STEREO_RG.get()
					       || conf.BW.get());
			} else {
				fd.draw_marker_ortho();
			}
		}

	}


	if (conf.LIFTS.get()) {
		lifts.drawLifts(conf.STEREO_RB.get()
				 || conf.STEREO_RG.get() || conf.BW.get());
		lifts.drawLiftsTxt(conf.STEREO_RB.get()
				 || conf.STEREO_RG.get() || conf.BW.get(), &ego);
	}

	if (conf.WAYPOINTS.get()) {
		waypoints.drawWaypoints(conf.STEREO_RB.get()
				 || conf.STEREO_RG.get() || conf.BW.get());
		waypoints.drawWaypointsTxt(conf.STEREO_RB.get()
				 || conf.STEREO_RG.get() || conf.BW.get(), &ego);
	}

	//glCallList(lifts_displ_list);
	glCallList(flightdata_displ_list);
	glCallList(airspace_displ_list);

	//lifts.drawLifts(1);

	if (conf.INFO.get()) {
		DrawInfo();
	}

	//drawtime.checknow("end of Draw");

}


// set the position and view-direction...
void setpos(void)
{

	glLoadIdentity();
	glRotatef(ego.getspinx(), 1.0, 0.0, 0.0);	//do the viewpoint transformation
	glRotatef(ego.getspiny(), 0.0, 1.0, 0.0);
	glRotatef(ego.getspinz(), 0.0, 0.0, 1.0);

	glTranslatef(ego.getx(), ego.gety(), -ego.getz());
	glScalef(1.0, 1.0, conf.z_scale.get());	//do z-axis scaling


}

void DrawBackground(int bgType)
{
  // Use an orthographic view, and save all matrices
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(-1, 1, -1, 1, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // we also save the enables, as we plan to enable/disable a lot of things
  glPushAttrib(GL_ENABLE_BIT);

  // disable the expensive tests
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  glDisable(GL_LIGHTING);

  // vertical gradient
  //int bgType = 2;

  switch (bgType) {

    case 1:
        // solid background // like the old one

        glDisable(GL_TEXTURE_2D);
        glColor3f(conf.background_color_1r.get(), conf.background_color_1g.get(), conf.background_color_1b.get());
        glBegin(GL_QUADS);
            glVertex2i( 1, -1);
            glVertex2i( 1,  1);
            glVertex2i(-1,  1);
            glVertex2i(-1, -1);
        glEnd();
        break;

    case 2:
        // vertical gradient
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
            glColor3f(conf.background_color_1r.get(), conf.background_color_1g.get(), conf.background_color_1b.get()); // lighter!
            glVertex2i(-1,  1);
            glVertex2i( 1,  1);

            glColor3f(conf.background_color_2r.get(), conf.background_color_2g.get(), conf.background_color_2b.get());
            glVertex2i( 1, -1);
            glVertex2i(-1, -1);
        glEnd();
        break;

    case 3:
        // horizontal gradient
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
            glColor3f(conf.background_color_1r.get(), conf.background_color_1g.get(), conf.background_color_1b.get());
            glVertex2i(-1,  1);
            glVertex2i(-1, -1);

            glColor3f(conf.background_color_2r.get(), conf.background_color_2g.get(), conf.background_color_2b.get());
            glVertex2i( 1, -1);
            glVertex2i( 1,  1);
        glEnd();
        break;

  }

  //
  // restore our states
  // this is done so as to leave the modelview matrix as the last active
  // matrix
  //
  glPopAttrib();

  glMatrixMode (GL_PROJECTION);
  glPopMatrix();

  glMatrixMode (GL_MODELVIEW);
  glPopMatrix();

}

// rendering routine
void DrawGLScene(void)
{
	// temporary
/*	if (conf.DEBUG.get()) {
		cout << "entered DRAWGLSCENE" << endl;
	}
*/
	//Timecheck drawgl("DrawGLScene", &conf);
	if (HELP) {
		HelpScreen();
		return;
	}

	if (INFO) {
		InfoScreen();
		return;
	}

	if (conf.ORTHOVIEW.get())
		InitGL(conf.getwidth(), conf.getheight());

	if (conf.STEREO_RB.get() || conf.STEREO_RG.get() || conf.BW.get()) {
		glClearColor(0.144f, 0.144f, 0.144f, 0.0f);	//Background is set to nice blue sky (grayscaled :)
	} else {

		glClearColor(0.1f, 0.1f, 0.5f, 0.0f);	//Background is set to nice blue sky
	}


	if (!conf.OFFSCREEN.get() && !conf.OSMESA.get()) {

		glDrawBuffer(GL_BACK);

	} else {
		glDrawBuffer(GL_FRONT);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear screen and depth buffer

	glLoadIdentity();	//load identity matrix

  if(! (conf.BW.get() || conf.FOG.get() || conf.STEREO_RB.get() || conf.STEREO_RG.get())) {
    DrawBackground(conf.background.get());
  }

	if (!conf.ORTHOVIEW.get()) {
		setpos();
	}


	// light position was here.... maybe here again for dynamic light-pos moving?
	GLfloat light_position[4] = { 5.0, 5.0, 1.0, 0.0 };	//set the light position
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);


	// stereoscopic drawing with 2 images
	if (conf.STEREO.get() && !conf.ORTHOVIEW.get()) {

		float _ed = conf.eye_dist.get();
		if (conf.SWAP_STEREO.get())
			_ed *= -1.0;


		// render left image!
		glViewport(0, 0, conf.getwidth() / 2, conf.getheight());
		ego.moves(-_ed / 2);
		setpos();
		Draw();
		ego.moves(_ed / 2);

		//render right image!
		glViewport(conf.getwidth() / 2, 0, conf.getwidth() / 2,
			   conf.getheight());
		ego.moves(_ed / 2);
		setpos();
		Draw();
		ego.moves(-_ed / 2);

		glViewport(0, 0, conf.getwidth(), conf.getheight());


	}
	// stereoscopic drawing with hardware
	#ifndef ONLY_OSMESA
	if (conf.STEREO_HW.get() && !conf.ORTHOVIEW.get()) {

		float _ed = conf.eye_dist.get();
		if (conf.SWAP_STEREO.get())
			_ed *= -1.0;


		// render left image!
		glDrawBuffer(GL_BACK_LEFT);
		ego.moves(_ed / 2);
		setpos();
		Draw();
		ego.moves(-_ed / 2);

		glClear(GL_DEPTH_BUFFER_BIT);

		//render right image!
		glDrawBuffer(GL_BACK_RIGHT);
		ego.moves(-_ed / 2);
		setpos();
		Draw();
		ego.moves(_ed / 2);




	}
	#endif

	// stereoscopic red/green or red/blue
	if ((conf.STEREO_RB.get() || conf.STEREO_RG.get())
	    && !conf.ORTHOVIEW.get()) {

		float _ed = conf.eye_dist.get();
		if (conf.SWAP_STEREO.get())
			_ed *= -1.0;


		//render red image
		ego.moves(-_ed / 2);
		setpos();
		glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);


		Draw();
		ego.moves(_ed / 2);

		glClear(GL_DEPTH_BUFFER_BIT);

		//render green or blue image
		ego.moves(_ed / 2);
		setpos();

		if (conf.STEREO_RB.get()) {
			glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);
		}

		if (conf.STEREO_RG.get()) {
			glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE);
		}

		Draw();
		ego.moves(-_ed / 2);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}

	// normal rendering , if no stereo or ortho-mode
	if ((!conf.STEREO.get() && !conf.STEREO_RB.get()
	     && !conf.STEREO_RG.get() && !conf.STEREO_HW.get())
	    || conf.ORTHOVIEW.get()) {
		Draw();
		//cout << "normal" << endl;
	}





	if (!conf.OFFSCREEN.get() && !conf.OSMESA.get()) {
		#ifndef ONLY_OSMESA
		glutSwapBuffers();
		#endif
	}			//and swap the buffers


	// every frame will be written as jpeg
	if (conf.MOVIE.get()) {
		screenshot();
	}

	//drawgl.checknow("end");

}

#ifndef ONLY_OSMESA
void ReSizeGLScene(int nwidth, int nheight)
{
	if (nheight == 0)	// this shouldnt happen
		nheight = 1;

	conf.setwidth(nwidth);
	conf.setheight(nheight);

	//glViewport(0, 0, conf.getwidth(), conf.getheight());        // Reset The Current Viewport

	InitGL(conf.getwidth(), conf.getheight());
	glutPostRedisplay();
}
#endif


// initialize openGL
void InitGL(int Width, int Height)	//set GL-Window size
{

	if (conf.DEBUG.get()) {
            cout << "entered void InitGL(WxH)" << Width << "x"  << Height << endl << flush;
	    //cout << "going to call glPixelstorei(GL_PACK_ALIGNEMENT, 1) ..." << flush;
        }

//#ifndef __OSX__
	glPixelStorei(GL_PACK_ALIGNMENT, 1);	//Prevents some strange crashes in the glReadPixels in the screenshot-function
//#else
//	if (conf.DEBUG.get()) {
//            cout << "... (SKIPPED __OSX__) ..." << flush;
//        }
//#endif

	/*if (conf.DEBUG.get()) {
            cout << "... returned from glPixelstorei(GL...)" << endl << flush;
        }*/

	glDepthFunc(GL_LESS);	// The Type Of Depth Test To Do
	glEnable(GL_DEPTH_TEST);	// Enables Depth Testing
	glEnable(GL_NORMALIZE);	// Enables normalizing of normal vectors

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glDisable(GL_COLOR_MATERIAL);
	glLineWidth(1.0);

	if (conf.SHADE.get()) {
		glShadeModel(GL_SMOOTH);	// Enables Smooth Color Shading
	} else {
		glShadeModel(GL_FLAT);
	}

	if (conf.FOG.get()) {
		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, GL_EXP2);
		glFogf(GL_FOG_DENSITY, conf.fogdensity.get());
		GLfloat fogcolor[4] = { 0.1, 0.1, 0.5, 1.0 };

		if (!(conf.BW.get() || conf.STEREO_RG.get()
		      || conf.STEREO_RB.get())) {
			glFogfv(GL_FOG_COLOR, fogcolor);
		} else {
			glFogfv(GL_FOG_COLOR, graycolor(fogcolor));
		}


	} else {
		glDisable(GL_FOG);
	}



	GLfloat light_ambient[4] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_color[4] = { 0.8, 0.8, 0.8, 1.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//light-position is set in DrawGLScene, after setpos


	//set viewport and projection
	glViewport(0, 0, Width, Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	// Reset the projection Matrix

	if (!conf.ORTHOVIEW.get()) {

		int _q=0;
		double _far;
		glGetIntegerv(GL_DEPTH_BITS, (GLint*) &_q);

		float screenfactor;

		float _near;

		//maybe this can be exactly calculated!
		if (_q < 24) {
			_near = 1.0;
		} else {
			_near = 0.2;
		}

		if (_q < 24) {
			_far = 300.0;
		} else {
			//_far = 500.0;
			_far = 600.0;
		//	cout << "FAR: 600 " <<_q << endl;
		}
		if (_q > 24) {
			_far = 2000.0;
//			cout << "FAR: 2000 " << _q <<endl;
		}

		if (args_info.clipping_far_given) {
			_far = args_info.clipping_far_arg;
		}

		if (args_info.clipping_near_given) {
			_near = args_info.clipping_near_arg;
		}

		if (conf.DEBUG.get()){
			cout << "Depth buffer: " << _q << " bit." <<endl
			<< "Clipping near: " << _near << endl << "Clipping far: "<< _far <<endl;

			double _s = pow(2.0, _q);
			cout << "depth values: " << _s << endl;;
			double zf = _far * _near / ((1/(double)_s)*(_far-_near) -_far);
			double zb = _far * _near / ((((double)_s-1)/_s) * (_far-_near) -_far);
			cout << "next near: " << zf <<endl<<"next far: "<< zb<<endl;


		}

		float _front_clip_width =
		    2.0 * _near * tan(((float) conf.aov.get() / 2.0) *
				      (M_PI / 180.0));


		if (conf.STEREO.get()) {
			screenfactor =
			    ((_front_clip_width) *
			     ((float) conf.getheight() /
			      ((float) conf.getwidth() / 2.0))) / 2.0;
		} else {
			screenfactor =
			    ((_front_clip_width) *
			     ((float) conf.getheight() /
			      (float) conf.getwidth())) / 2.0;
		}

		glFrustum(-_front_clip_width / 2.0,
			  _front_clip_width / 2.0, -screenfactor,
			  screenfactor, _near,
			  _far * conf.getdownscalefactor());
	} else {
		//experimental for now

		float xclip = (conf.orthoclipping.get() / 2.0);	//*(conf.getwidth() / conf.getheight()) ;
		float yclip =
		    (conf.orthoclipping.get() / 2.0) *
		    ((float) conf.getheight() / (float) conf.getwidth());

		glOrtho(ego.getOx() - xclip, ego.getOx() + xclip,
			ego.getOy() - yclip, ego.getOy() + yclip, -20.0,
			20.0);

	}

	//reset MatrixModel to our default state policy
	glMatrixMode(GL_MODELVIEW);
}

void screenshot() {

	Timecheck sshottime("Screenshot",&conf);
	if (   (strcasecmp(conf.get_image_format().c_str(), "jpg") == 0)
		|| (strcasecmp(conf.get_image_format().c_str(), "jpeg") == 0) )
			screenshot_jpeg();

	if ( strcasecmp(conf.get_image_format().c_str(),"rgb") == 0)
			screenshot_rgb();

	sshottime.checknow("");


}


// ersetzt von screenshot_jpeg
//framebuffer    schreibt den framebuffer in ein *.rgb file
void screenshot_rgb() {

		short int* rgbmap = new short int[conf.getwidth()*conf.getheight()*3];


		glReadPixels(0,0,conf.getwidth(),conf.getheight(),GL_RGB,GL_UNSIGNED_SHORT,rgbmap);


		char name[1024];
		//char _buff[2];

		cout << "writing a rgb " << conf.getwidth() << "x" << conf.getheight() << " picture with 6 Byte per Pixel" << endl;

		std::string basename = conf.get_basename();
		std::string path = conf.get_save_path();
		if (args_info.basename_given) { basename = args_info.basename_arg; }
		if (args_info.save_path_given) { path = args_info.save_path_arg + SEPERATOR; }


		if (args_info.os_outfile_given) {
			sprintf(name, "%s", args_info.os_outfile_arg);
		} else {

			ifstream testfile;

			while (1) {
				sprintf(name, "%s%s%d.rgb", path.c_str(), basename.c_str(), conf.getnextframewriternumber());
				//cout << name << "  geht nixht" << endl;
				testfile.open(name, ios::binary);
				if (!testfile) {break;}
				testfile.close();
				//cout << name << "  NEIN gibt schon" << endl;
			}
			//cout << name << "  geht!!!!" << endl;
		}

		ofstream framefile;
		framefile.open(name, ios::binary);

//			for (int _z=0 ;_z < (width*height*3); _z++){
//				framefile.write((char*)&rgbmap[_z],2);
//				}

			for (int _z=(conf.getwidth()*(conf.getheight()-1)*3); _z>=0; _z=_z-conf.getwidth()*3) {
				for (int _s=0; _s<conf.getwidth()*3; _s++) {
					framefile.write((char*)&rgbmap[_z+_s],2);
				}
			}


		cout << name << " written." << endl <<flush;
		//cout << sizeof (GLushort) << endl;
		delete [] rgbmap;

}



void screenshot_jpeg()
{

	// rendering in the GL_BACK buffer and reading from it will only work on
	// software GL. If GL_BACK is a hardware buffer it wont work
	// maybe they dont render overlayed regions of the buffer.
	// we will read from (GL_FRONT)

	glFinish();

	unsigned char *tmpArray, *jpegArray;
	int quality = conf.jpeg_quality.get();
	char name[1024];
	int index = 0;

	tmpArray =
	    new unsigned char[conf.getwidth() * conf.getheight() * 3];
	jpegArray =
	    new unsigned char[conf.getwidth() * conf.getheight() * 3];

	if (conf.OSMESA.get())
		glReadBuffer(GL_FRONT);

	if (conf.OFFSCREEN.get())
		glReadBuffer(GL_FRONT);

	glFinish();



	glReadPixels(0, 0, conf.getwidth(), conf.getheight(), GL_RGB,
		     GL_UNSIGNED_BYTE, tmpArray);



	index = 0;
	for (int _z = (conf.getwidth() * (conf.getheight() - 1) * 3);
	     _z >= 0; _z = _z - conf.getwidth() * 3) {
		for (int _s = 0; _s < conf.getwidth() * 3; _s++) {
			jpegArray[index] = tmpArray[_z + _s];
			index++;
		}
	}


	if (conf.VERBOSE.get()) {
		cout << "writing a " << conf.getwidth() << "x" << conf.
		    getheight() << " jpg image" << endl;
		cout << "jpeg quality: " << conf.jpeg_quality.get() << endl;
	}

	std::string basename = conf.get_basename();
	std::string path = conf.get_save_path();
	if (args_info.basename_given) { basename = args_info.basename_arg; }
	if (args_info.save_path_given) { path = args_info.save_path_arg + SEPERATOR; }

	if (args_info.os_outfile_given) {
		sprintf(name, "%s", args_info.os_outfile_arg);
	} else {
		ifstream testfile;
		while (1) {
			sprintf(name, "%s%s%d.jpg", path.c_str(), basename.c_str(), conf.getnextframewriternumber());
			//cout << name << "  geht nixht" << endl;
			testfile.open(name, ios::binary);
			if (!testfile) {break;}
			testfile.close();
			//cout << name << "  NEIN gibt schon" << endl;
			}
		//cout << name << "  geht!!!!" << endl;
		}

	//sprintf(name, "%s%s%d.jpg", path.c_str(), basename.c_str(), conf.getnextframewriternumber());

	//sprintf(name, "frame%d.jpg", conf.getnextframewriternumber());

	//if (args_info.os_outfile_given) {

	//	sprintf(name, "%s", args_info.os_outfile_arg);

	//}

	write_JPEG_file(conf.getwidth(), conf.getheight(), quality, name,
			(JSAMPLE *) jpegArray);

	if (conf.VERBOSE.get())
		cout << "jpeg: " << name << "  written ! " << endl <<
		    flush;

	delete tmpArray;
	delete jpegArray;

}


#ifndef ONLY_OSMESA
void IdleFunc(void)
{

	//needed in movie mode, to decide whether to repeat frame, or to move marker forward
	bool FORWARD=false;
	bool REPEAT=false;

	if (conf.JOYSTICK.get() && !conf.ORTHOVIEW.get()) {
		glutForceJoystickFunc();
	}

	// determine framerate;

	timeval now;
	timeval last;
	//timezone zone;

	//zone.tz_minuteswest = 0;
	//zone.tz_dsttime = DST_NONE;

	gettimeofday(&now, NULL);

	last = conf.get_time_mem();
	conf.set_time_mem(now);

	//cout << "last :   " << last.tv_sec << "     " << last.tv_usec << endl;
	//cout << "now :   " << now.tv_sec << "     " << now.tv_usec << endl;

	float rate =
		1.0 / (((float) now.tv_sec - (float) last.tv_sec)
		+
		(((float) now.tv_usec -
		(float) last.tv_usec) / 1000000.0));

	//cout << "rate: " << rate << endl;
	conf.set_framerate(rate);
	conf.set_fps(rate);



	// conf.movi_msec is millisecs usleep is in microseconds!
	if (conf.FLYING.get() && !conf.GPSD.get() ) {
		usleep((unsigned long int) conf.movie_msecs.get() * 1000);

		if (conf.FLYING_REPEAT.get())
			REPEAT=true;
		if (conf.FLYING_AUTO_REPEAT.get()) {
			if (conf.get_fps() > conf.idle_auto_repeat_limit.get()) {
				//cout << "fps exceeeds limit" << endl;
				REPEAT=true;
			}
		}

		if (REPEAT) {
			movie_repeat_counter++;
			//cout << "repeater : "<< movie_repeat_counter << endl;
			if (movie_repeat_counter >= conf.movie_repeat_factor.get()) {
				movie_repeat_counter=0;
				FORWARD=true;
			}
		} else {
			FORWARD=true;
		}
	}
	/*
	// update date in GPSD mode
	if (conf.GPSD.get())
		fd.querygps();

	// FLYING with GPSD: marker is kept at recent position!
	if (conf.FLYING.get() && conf.GPSD.get() ) {
		fd.marker_head();
	}
	*/
	if (conf.SPINNING.get() != 0) {
		ego.spinmoveradialpoint(conf.SPINNING.get(), fd.getmarkerx(), fd.getmarkery() );  //spin the ego around the marker
		if (conf.FOLLOW.get())
                                update_diff_pos();
	}


	if (conf.GPSD.get()) {
		if (fd.querygps() == 0) { // new data;
			if (conf.FLYING.get())
				fd.marker_head();
			GenFlightList();

			// leaving area!

			//cout << fd.getlat() << "  " << fd.getlon() << endl;
			//cout <<"lim: "<< fd.getcenter_lat()+(conf.getborder_land_lat()/DEGDIST) <<
			//"  " << fd.getcenter_lat()-(conf.getborder_land_lat()/DEGDIST) << endl;
			//cout << "lim2: " << fd.getcenter_lon()+(conf.getborder_land_lon()/DEGDIST)
			//<< " " << fd.getcenter_lon()-(conf.getborder_land_lon()/DEGDIST) << endl;

			if (conf.LANDSCAPE.get() && !conf.MAP.get()) {
			  // hier noch die limits checken
			  // in lat gehts manchmal ueber die kante..? ZZZZZ
			  // immer noch? sieht ganz gut aus soweit
			if (fd.getlat() > fd.getcenter_lat()-conf.getdem_grid_lat()*conf.getdownscalefactor()+((conf.getborder_land_lat()-0.0)/DEGDIST)
			  || fd.getlat() < fd.getcenter_lat()+conf.getdem_grid_lat()*conf.getdownscalefactor()-((conf.getborder_land_lat()-0.0)/DEGDIST)
			  || fd.getlon() > fd.getcenter_lon()-conf.getdem_grid_lon()*conf.getdownscalefactor()+(((conf.getborder_land_lon()-0.0)/DEGDIST)/
				  cos(fd.getcenter_lat() * PI_180))
 			  || fd.getlon() < fd.getcenter_lon()+conf.getdem_grid_lon()*conf.getdownscalefactor()-(((conf.getborder_land_lon()-0.0)/DEGDIST)/
				  cos(fd.getcenter_lat() * PI_180))
			  ) {

			  //cout << "REBUILDING LANDSCAPE" << endl;
			   fd.setcenter_lat(fd.getlat());
			   fd.setcenter_lon(fd.getlon());
			   fd.setlatmin(fd.getlat());
			   fd.setlatmax(fd.getlat());
			   fd.setlonmax(fd.getlon());
			   fd.setlonmin(fd.getlon());
			  GenLandscapeList();

			}
			}

		}
	}


	// forward exits only in flying mode without gpsd
	if (FORWARD && conf.FLIGHT.get()) {
		fd.forward_marker();

		if (fd.getcycles() == 0)
			Ende(0);

		// wenn marker bewegt neue flightlist machen
		if (conf.MARKER_RANGE.get()) {
			GenFlightList();
		}

#ifndef __WIN32__
		update_shmem();
#endif
	}

	if (conf.FOLLOW.get())
		update_locked_pos();


	glutPostRedisplay();
}
#endif


#ifndef ONLY_OSMESA
//joystick function
void joystick(unsigned int buttonMask, int x, int y, int z)
{
	if (conf.DEBUG.get())
		cout << "Joystick: ButtonMask: " << buttonMask << "   X=" << x
		<< "  Y=" << y << "  Z=" << z << "                     " << '\r' << flush;

	 ego.movef(0.2 * conf.joyfac_y.get() * (float) y * conf.getdownscalefactor());
	 ego.moves(-0.2 * conf.joyfac_x.get() * (float) x * conf.getdownscalefactor());
	 ego.movez(-0.05 * conf.joyfac_z.get() * (float) z * conf.getdownscalefactor());

	 // here is something  wrong...
	 // "flying-mode, follow-mode... ?!
	 // update the diff-pos, only if we really have moved!
	 if (conf.FOLLOW.get() && (x != 0 || y!=0 || z!=0))
		 update_diff_pos();

}
#endif


#ifndef ONLY_OSMESA
// Keyboard callback
void keyPressed(unsigned char key, int x, int y)
{

	//not too fast
	usleep(100);

	/*if (conf.DEBUG.get())
		cout << "Key: \"" << key << "\" pressed." << endl << flush;*/

	if (HELP || INFO) {
		HELP = false;
		INFO = false;
		glutPostRedisplay();
		return;
	}

	// escape?
	if (key == ESCAPE || key == 'q') {
		Ende(0);
	}




	// MOVING
	// some things do not make sense in ORTHO-MODE
	if (!conf.ORTHOVIEW.get()) {
		if (key == 't') {
			ego.movez(0.05 * conf.getdownscalefactor());
			if (conf.FOLLOW.get()) {
				update_diff_pos();
			}
		}
		if (key == 'z') {
			ego.movez(-0.05 * conf.getdownscalefactor());
			if (conf.FOLLOW.get()) {
				update_diff_pos();
			}
		}
		if (key == 'd') {
			ego.moves(0.2 * conf.getdownscalefactor());
			if (conf.FOLLOW.get()) {
				update_diff_pos();
			}
		}
		if (key == 'f') {
			ego.moves(-0.2 * conf.getdownscalefactor());
			if (conf.FOLLOW.get()) {
				update_diff_pos();
			}
		}
	}

	if (key == 'g') {
		if (!conf.ORTHOVIEW.get()) {
			ego.movef(-0.2 * conf.getdownscalefactor());
			if (conf.FOLLOW.get()) {
				update_diff_pos();
			}
		} else {

			float zoomfac = 1;

			if (conf.orthoclipping.get() < 5) {
				zoomfac = conf.orthoclipping.get()/50.0;
			}

			conf.orthoclipping.dec(2.0 *
					       conf.getdownscalefactor()
					       * zoomfac
					       );
			//InitGL(conf.getwidth(), conf.getheight());
		}

	}


	if (key == 's') {
		if (!conf.ORTHOVIEW.get()) {
			ego.movef(0.2 * conf.getdownscalefactor());
			if (conf.FOLLOW.get()) {
				update_diff_pos();
			}
		} else {

			float zoomfac = 1;

			if (conf.orthoclipping.get() < 5) {
				zoomfac = conf.orthoclipping.get()/50.0;
			}

			conf.orthoclipping.inc(2.0 *
					       conf.getdownscalefactor()
					       * zoomfac
					       );
			//InitGL(conf.getwidth(), conf.getheight());
		}
	}

	if (key == ' ') {
		if (!conf.ORTHOVIEW.get()) {
			ego.movef(-1.0 * conf.getdownscalefactor());
			if (conf.FOLLOW.get()) {
				update_diff_pos();
			}
		} else {
			conf.orthoclipping.dec(10.0 *
					       conf.getdownscalefactor());
			//InitGL(conf.getwidth(), conf.getheight());
		}
	}

	if (key == 'a') {
		if (!conf.ORTHOVIEW.get()) {
			ego.movef(1.0 * conf.getdownscalefactor());
			if (conf.FOLLOW.get()) {
				update_diff_pos();
			}
		} else {
			conf.orthoclipping.inc(10.0 *
					       conf.getdownscalefactor());
			//InitGL(conf.getwidth(), conf.getheight());
		}
	}


	if (!conf.ORTHOVIEW.get()) {

		if (key == '+')
			conf.z_scale.inc(0.1);
//			GenPointsList();
		if (key == '-')
			conf.z_scale.dec(0.1);
//			GenPointsList();
	}
// You cant get lower than fd.getzmin()+0.02
	if (ego.getz() <
	    (fd.getzmin() + ((float) conf.offset.get() / 1000.0) +
	     0.05) * conf.z_scale.get()) {
		ego.setz((fd.getzmin() +
			  ((float) conf.offset.get() / 1000.0) +
			  0.05) * conf.z_scale.get());
	}


	if (key == 'P')
		conf.MOVIE.toggle();

	//Curtain
	if (key == 'h') {
		menuMain(21);
	}

	if (key == 'p') {
		screenshot();
	}

	if (key == 'm') {
		conf.MOUSE.toggle();

		if (conf.MOUSE.get()) {
			glutPassiveMotionFunc(NULL);
			//glutMotionFunc(&mouseClickMove);
			glutSetCursor(GLUT_CURSOR_INHERIT);
		} else {
			//lastmx =0; lastmy =0;
			glutWarpPointer(conf.getwidth() / 2,
					conf.getheight() / 2);
			MOUSEWARPED = true;
			glutPassiveMotionFunc(&mouseMove);
			//glutMotionFunc(NULL);
			glutSetCursor(GLUT_CURSOR_NONE);
		}
	}
	//adjusting offset
	if (key == 'u') {
		conf.offset.inc(1);
		GenFlightList();
	}

	if (key == 'i') {
		conf.offset.dec(1);
		GenFlightList();
	}


	if (key == 'w') {
		if (conf.FULLSCREEN.get()) {
			glutReshapeWindow(conf.getinitwidth(),
					  conf.getinitheight());
			conf.setwidth(conf.getinitwidth());
			conf.setheight(conf.getinitheight());
			conf.FULLSCREEN.toggle();
		} else {
			glutFullScreen();
			//glutEnterGameMode();
			conf.FULLSCREEN.toggle();
		}
	}

	//switch on/off textured maps
	if (key == 'b') {
		menuMain(1);
	}

	if (key == 'j') {
		menuMain(12);
	}

	if (key == '9') {
		conf.fogdensity.dec(0.001);
		InitGL(conf.getwidth(), conf.getheight());
		//cout << conf.fogdensity.get() << endl;
	}

	if (key == '0') {
		conf.fogdensity.inc(0.001);
		InitGL(conf.getwidth(), conf.getheight());
		//cout << conf.fogdensity.get() << endl;
	}


	if (key == '7') {
		conf.aov.inc(1);
		//cout << conf.getFrust()<< endl;
		InitGL(conf.getwidth(), conf.getheight());
	}

	if (key == '8') {
		conf.aov.dec(1);
		//cout << conf.getFrust()<< endl;
		InitGL(conf.getwidth(), conf.getheight());
	}

	if (key == 'l') {
		menuMain(11);
	}

	if (key == 'L') {
		conf.WIRE.toggle();

		if ((!conf.MAP.get() || conf.LANDSCAPE.get())
		    && !(conf.MAP.get() && conf.LANDSCAPE.get())) {
			GenFlightList();
			GenLandscapeList();
		}
	}

	if (key == 'o') {
		menuMain(41);
	}

	if (key == 'O') {
		conf.SHADE.toggle();
		InitGL(conf.getwidth(), conf.getheight());
	}

	if (key == '1' && conf.colmapnumber.get() != 1) {
		menuColormap(1);
	}

	if (key == '2' && conf.colmapnumber.get() != 2) {
		menuColormap(2);
	}

	if (key == '3' && conf.colmapnumber.get() != 3) {
		menuColormap(3);
	}

	if (key == '4' && conf.colmapnumber.get() != 4) {
		menuColormap(4);
	}

	if (key == '5' && conf.colmapnumber.get() != 5) {
		menuColormap(5);
	}

	if (key == '6' && conf.colmapnumber.get() != 6) {
		menuColormap(6);
	}


	if (key == 'x') {
		int tmp = conf.ActiveMapSet.get();
		conf.ActiveMapSet.dec(1);
		if ((tmp != conf.ActiveMapSet.get()) && conf.MAP.get()) {
			GenLandscapeList();
		}
	}

	if (key == 'X') {
		conf.NOLIGHTING.toggle();
			GenLandscapeList();

	}

	if (key == 'c') {
		int tmp = conf.ActiveMapSet.get();
		conf.ActiveMapSet.inc(1);
		if ((tmp != conf.ActiveMapSet.get()) && conf.MAP.get()) {
			GenLandscapeList();
		}
	}

	if (key == 'y') {
		conf.COMPRESSION.toggle();
		if (conf.MAP.get()) {
			GenLandscapeList();
		}
	}

	if (key == 'A') {
		conf.SWAP_STEREO.toggle();
	}

	if (key == 'S') {
		if (conf.STEREO.get()) {
			menuStereo(1);
		} else {
			menuStereo(2);
		}
	}
	// Black and white
	if (key == 'B') {
		conf.BW.toggle();

		if (!(conf.STEREO_RG.get() || conf.STEREO_RB.get())) {
			GenLandscapeList();
			GenFlightList();
			GenAirspaceList();
			InitGL(conf.getwidth(), conf.getheight());
		}
	}

	if (key == 'D') {
		if (conf.STEREO_RG.get()) {
			menuStereo(1);
		} else {
			menuStereo(3);
		}
	}

	if (key == 'F') {
		if (conf.STEREO_RB.get()) {
			menuStereo(1);
		} else {
			menuStereo(4);
		}
	}

	if (key == 'Q') {
		conf.eye_dist.dec(0.05);
	}

	if (key == 'W') {
		conf.eye_dist.inc(0.05);
	}
	//cout << int(key) << " key" << endl;

	if (key == 'U') {
		menuMain(34);
	}

	if (key == 'M') {
		menuMain(33);
	}

	if (key == 'I') {
		menuMain(35);
	}

	if (key == 'J')
		menuMain(36);


	glutPostRedisplay();
}
#endif






#ifndef ONLY_OSMESA
void SpecialKeyPressed(int key, int x, int y)
{

	if (HELP || INFO) {
		HELP = false;
		INFO = false;
		glutPostRedisplay();
		return;
	}
	// no modifiers!
	if (glutGetModifiers() != GLUT_ACTIVE_SHIFT &&
	    glutGetModifiers() != GLUT_ACTIVE_CTRL &&
	    glutGetModifiers() != GLUT_ACTIVE_ALT) {



		if (key == GLUT_KEY_F1) {
			fd.fbackward_marker();
			if (conf.FOLLOW.get()) {
				update_locked_pos();
			}
			if (conf.MARKER_RANGE.get()) {
				GenFlightList();
			}
			//cout << fd.getmarker() << endl;
#ifndef __WIN32__
			update_shmem();
#endif
		}

		if (key == GLUT_KEY_F2) {
			fd.backward_marker();
			if (conf.FOLLOW.get()) {
				update_locked_pos();
			}
			if (conf.MARKER_RANGE.get()) {
				GenFlightList();
			}
			//cout << fd.getmarker() << endl;
#ifndef __WIN32__
			update_shmem();
#endif
		}

		if (key == GLUT_KEY_F3) {
			fd.forward_marker();
			if (conf.FOLLOW.get()) {
				update_locked_pos();
			}
			if (conf.MARKER_RANGE.get()) {
				GenFlightList();
			}
			//cout << fd.getmarker() << endl;
#ifndef __WIN32__
			update_shmem();
#endif
		}
		if (key == GLUT_KEY_F4) {
			fd.fforward_marker();
			if (conf.FOLLOW.get()) {
				update_locked_pos();
			}
			if (conf.MARKER_RANGE.get()) {
				GenFlightList();
			}
			//cout << fd.getmarker() << endl;
#ifndef __WIN32__
			update_shmem();
#endif
		}

		if (key == GLUT_KEY_F5) {
			setinitpos();
		}

		if (key == GLUT_KEY_F6) {
			menuMain(32);
		}

		if (key == GLUT_KEY_F7) {
			menuMain(31);
			//cout << "toggel marker" << endl;
		}

		if (key == GLUT_KEY_F8) {
			menuMain(2);
		}

		if (key == GLUT_KEY_F9) {
			menuMain(13);
		}


		if (key == GLUT_KEY_F10) {
			if (conf.colmap2number.get() >=
			    conf.colmap2number.getmin() + 1) {
				//conf.colmap2number.dec(1);
				menuColormap_sea(conf.colmap2number.get() -
						 1);
				//GenLandscapeList();
			}

		}

		if (key == GLUT_KEY_F11) {
			if (conf.colmap2number.get() <=
			    conf.colmap2number.getmax() - 1) {
				//conf.colmap2number.inc(1);
				menuColormap_sea(conf.colmap2number.get() +
						 1);
				//GenLandscapeList();
			}

		}

		if (key == GLUT_KEY_F12) {
			conf.WAYPOINTS.toggle();
		}


		// Rotations around marker pos (by A. Somers)
		// arrow-keys without mofifiers
                if (key == GLUT_KEY_LEFT) {
			ego.spinmoveradialpoint(1, fd.getmarkerx(), fd.getmarkery() );  //spin the ego around the marker
			if (conf.FOLLOW.get())
				update_diff_pos();
                }

                if (key == GLUT_KEY_UP) {

                   ego.spinmoveverticalpoint(1, fd.getmarkerx(), fd.getmarkery() );        //spin the ego around the marker
                   if (conf.FOLLOW.get())
                   update_diff_pos();
                }

                if (key == GLUT_KEY_RIGHT) {
                    ego.spinmoveradialpoint(-1, fd.getmarkerx(), fd.getmarkery() ); //spin the ego around the marker
                    if (conf.FOLLOW.get())
                    update_diff_pos();
                }

                if (key == GLUT_KEY_DOWN) {
                    ego.spinmoveverticalpoint(-1, fd.getmarkerx(), fd.getmarkery() );       //spin the ego around the marker
                    if (conf.FOLLOW.get())
                    update_diff_pos();
                }




		if (key == GLUT_KEY_PAGE_UP) {
			conf.MARKER_SIZE.inc(0.02);
		}

		if (key == GLUT_KEY_PAGE_DOWN) {
			conf.MARKER_SIZE.dec(0.02);
		}


		if (key == GLUT_KEY_HOME) {
			conf.lifts_info_mode.inc(1);
		}
		if (key == GLUT_KEY_END) {
			conf.lifts_info_mode.dec(1);
		}
		if (key == GLUT_KEY_INSERT) {
			cout << "insert unused" << endl;
		}
	}			// end no modifiers


	//modifier shift
	if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {

		if (key == GLUT_KEY_F1) {


			conf.movie_repeat_factor.dec(2);

			// warum dies?
			//if (conf.FLYING.get()) {
			//	glutIdleFunc(&IdleFunc);
			//}


		}

		if (key == GLUT_KEY_F2) {
                                                conf.movie_repeat_factor.inc(2);

			//if (conf.FLYING.get()) {
			//	glutIdleFunc(&IdleFunc);
			//}

		}



		if (key == GLUT_KEY_F3) {
			conf.flightstrip_width.dec(1.0);
			GenFlightList();
		}

		if (key == GLUT_KEY_F4) {
			conf.flightstrip_width.inc(1.0);
			GenFlightList();

		}


		if (key == GLUT_KEY_F5) {
			conf.flightstrip_mode.dec(1);
			GenFlightList();
		}

		if (key == GLUT_KEY_F6) {
			conf.flightstrip_mode.inc(1);
			GenFlightList();

		}

		if (key == GLUT_KEY_F7) {
			conf.flightstrip_colmap.dec(1);
			GenFlightList();
		}

		if (key == GLUT_KEY_F8) {
			conf.flightstrip_colmap.inc(1);
			GenFlightList();

		}



		if (key == GLUT_KEY_F9) {
			conf.AIRSPACE_WIRE.toggle();
			GenAirspaceList();
		}


		if (key == GLUT_KEY_F10) {
			cout << "shift-f10 unused" << endl;
		}

		if (key == GLUT_KEY_F11) {
			cout << "shift-f11 unused" << endl;
		}

		if (key == GLUT_KEY_F12) {
			cout << "Shift F12 glGetError" << endl;
			cout << "glGetError: " << gluErrorString(glGetError()) << endl;
		}


		//rotations around the center of the scene
		// with shift
		if (key == GLUT_KEY_LEFT) {
			ego.spinmoveradial(1);	//spin the ego around the center
			 if (conf.FOLLOW.get())
				 update_diff_pos();

		}
		if (key == GLUT_KEY_UP) {
			ego.spinmovevertical(1);	//spin the ego around the center vertically
			 if (conf.FOLLOW.get())
				 update_diff_pos();
		}
		if (key == GLUT_KEY_RIGHT) {
			ego.spinmoveradial(-1);	//spin the ego around the center
			 if (conf.FOLLOW.get())
				 update_diff_pos();
		}
		if (key == GLUT_KEY_DOWN) {
			ego.spinmovevertical(-1);	//spin the ego around the center vertically
			 if (conf.FOLLOW.get())
				 update_diff_pos();
		}



		if (key == GLUT_KEY_PAGE_UP) {
			conf.pTextSize.inc(0.0005);
		}

		if (key == GLUT_KEY_PAGE_DOWN) {
			conf.pTextSize.dec(0.0005);
		}

		if (key == GLUT_KEY_HOME) {
			conf.waypoints_info_mode.inc(1);
		}
		if (key == GLUT_KEY_END) {
			conf.waypoints_info_mode.dec(1);
		}
		/*
		if (key == GLUT_KEY_HOME) {
			cout << "shift-home unused" << endl;
		}
		if (key == GLUT_KEY_END) {
			cout << "shift-end unused" << endl;
		}*/
		if (key == GLUT_KEY_INSERT) {
			cout << "shift-insert unused" << endl;
		}


	}			// end shift modifier


	glutPostRedisplay();

}
#endif


#ifndef ONLY_OSMESA
void update_diff_pos()
{

	if (!conf.FLIGHT.get())
		return;


	double _x = 0, _y = 0, _z = 0;

	for (int _i = fd.getmarker() - 10; _i <= fd.getmarker() + 10; _i++) {
		int _c;
		_c = _i;
		if (_i < 0) {
			_c = 0;
		}
		if (_i >= fd.getn()) {
			_c = fd.getn() - 1;
		}
		_x += -fd.getx(_c);
		_y += -fd.gety(_c);
		_z += fd.getz(_c) * conf.z_scale.get();
	}

	_x /= 21;
	_y /= 21;
	_z /= 21;

	ego.setdx(ego.getx() - _x); //fd.getx(fd.getmarker()));
	ego.setdy(ego.gety() - _y); //fd.gety(fd.getmarker()));
	ego.setdz(ego.getz() -  _z);
		//  (fd.getz(fd.getmarker()) * conf.z_scale.get()));

	ego.setdox(ego.getOx() + _x);
	ego.setdoy(ego.getOy() + _y);

	//cout << "diff updated" << endl;

}
#endif

#ifndef ONLY_OSMESA
void update_locked_pos()
{

	if (!conf.FLIGHT.get())
		return;

	double _x = 0, _y = 0, _z = 0;

	for (int _i = fd.getmarker() - 10; _i <= fd.getmarker() + 10; _i++) {
		int _c;
		_c = _i;
		if (_i < 0) {
			_c = 0;
		}
		if (_i >= fd.getn()) {
			_c = fd.getn() - 1;
		}
		_x += -fd.getx(_c);
		_y += -fd.gety(_c);
		_z += fd.getz(_c) * conf.z_scale.get();
	}

	_x /= 21;
	_y /= 21;
	_z /= 21;



	ego.setx(ego.getdx() + _x);
	ego.sety(ego.getdy() + _y);
	ego.setz(ego.getdz() + _z);

	ego.setOx(ego.getdox() - _x);
	ego.setOy(ego.getdoy() - _y);

	if (conf.ORTHOVIEW.get())
		InitGL(conf.getwidth(), conf.getheight());

}
#endif

void DrawInfo(void)
{

//int font_height = int(glutGet(GLUT_WINDOW_HEIGHT) / 30);
	int font_height = conf.getheight() / conf.info_fontsize.get();

//cout << font_height << "fh" << endl;


	vector < string > lines;
	char buf[256];

	if (args_info.text_given) {

		//if (args_info.text_arg.length() < 256) {
		//sprintf(buf, args_info.text_arg);
		//lines.push_back(buf);
		lines.push_back(args_info.text_arg);
		//} else {lines.push_back("too long");}

	}

	if (!args_info.no_position_info_given) {
	if (!conf.ORTHOVIEW.get()) {
		sprintf(buf, "Heading: %.0f", ego.getspinz());
		lines.push_back(buf);
		sprintf(buf, "Dive angle: %.0f", 270 - ego.getspinx());
		lines.push_back(buf);
		int alt =
		    int ((ego.getz() * 1000 / conf.z_scale.get()) *
			 conf.getalt_unit_fac());
		sprintf(buf, "Altitude: %d %s", alt,
			conf.getalt_unit_name().c_str());
		lines.push_back(buf);


		float lat, lon;
		proj.get_latlon(-ego.getx(), -ego.gety(), lat, lon);

		sprintf(buf, "Lat: %.4f", lat);
		lines.push_back(buf);
		sprintf(buf, "Lon: %.4f", lon);
		lines.push_back(buf);


		sprintf(buf, "Angle of view %d", conf.aov.get());
		lines.push_back(buf);

		if (conf.JOYSTICK.get()) {
			sprintf(buf, "Joystick mode on");
			lines.push_back(buf);
		}

		if (conf.STEREO_HW.get () || conf.STEREO.get () || conf.STEREO_RG.get () || conf.STEREO_RB.get ()) {

		    sprintf(buf, "Eye distance: %.2f",conf.eye_dist.get());
		    lines.push_back(buf);


		}


	} else {


		float lat, lon;
		proj.get_latlon(ego.getOx(), ego.getOy(), lat, lon);

		sprintf(buf, "Lat: %.4f", lat);
		lines.push_back(buf);
		sprintf(buf, "Lon: %.4f", lon);
		lines.push_back(buf);
	}
	}

	if (!args_info.no_marker_pos_info_given) {
	if (conf.MARKER.get()) {

		if (!args_info.no_position_info_given) {
		lines.push_back("----------"); }

		if (conf.GPSD.get()  ) {
		  lines.push_back(fd.serverstring.c_str());
		}

		if (conf.GPSD.get() && conf.FLYING.get() ) {
			sprintf(buf, "GPS live mode / Position:");
			lines.push_back(buf);


		}

		if (conf.GPSD.get() && !conf.FLYING.get() ) {
			sprintf(buf, "GPS live mode / Marker:");
			lines.push_back(buf);
		}

		sprintf(buf, "Lat: %.4f", fd.getlat());
		lines.push_back(buf);
		sprintf(buf, "Lon: %.4f", fd.getlon());
		lines.push_back(buf);

		float offsetgps, offsetbaro;

		if (conf.GPSALT.get()) {
			offsetgps = conf.offset.get();
			offsetbaro = 0;
		} else {
			offsetgps = 0;
			offsetbaro = conf.offset.get();
		}


		if (!conf.GPSALT.get()) {
			sprintf(buf, "Alt (baro, MSL): %.0f %s",
				(fd.getalt() +
				 offsetbaro) * conf.getalt_unit_fac(),
				conf.getalt_unit_name().c_str());
			lines.push_back(buf);

			sprintf(buf, "Alt (baro, GND): %.0f %s",
				((fd.getalt() + offsetbaro) -
				 fd.getgndelevation()) *
				conf.getalt_unit_fac(),
				conf.getalt_unit_name().c_str());
			lines.push_back(buf);
		} else {

			sprintf(buf, "Alt (GPS, MSL): %.0f %s",
				(fd.getgpsalt() +
				 offsetgps) * conf.getalt_unit_fac(),
				conf.getalt_unit_name().c_str());
			lines.push_back(buf);

			sprintf(buf, "Alt (GPS, GND): %.0f %s",
				((fd.getgpsalt() + offsetgps) -
				 fd.getgndelevation()) *
				conf.getalt_unit_fac(),
				conf.getalt_unit_name().c_str());
			lines.push_back(buf);
		}

		if (conf.offset.get() != 0) {
			sprintf(buf, "Calibration offset: %d m",
			conf.offset.get());
			lines.push_back(buf);
		}

		sprintf(buf, "Surface elevation (MSL): %.0f %s",
			fd.getgndelevation() * conf.getalt_unit_fac(),
			conf.getalt_unit_name().c_str());
		lines.push_back(buf);

		if (conf.GPSD.get()) {
			sprintf(buf, "Sat_vis/Sat_used/Mode: %.0f/%.0f/%.0fD",
				fd.getsatvis(), fd.getsatused(), fd.getgpsmode());
			lines.push_back(buf);

			sprintf(buf, "eph/epv: %.0f/%.0f m",
				fd.geteph(), fd.getepv());
			lines.push_back(buf);


			/*   // rubbish?
			sprintf(buf, "eps/epc: %.2fkm/h / %.2fm/s",
				fd.geteps(), fd.getepc());
			lines.push_back(buf);
			*/

			sprintf(buf,"GPS interruptions: %d",fd.getgpsinterruptions());
			lines.push_back(buf);
		}

		sprintf(buf, "Markerpos: %d",
			 fd.getmarker());
		lines.push_back(buf);

		sprintf(buf, "Time (%s): %s", conf.getTimeZoneName().c_str(), fd.gettime().c_str());
		lines.push_back(buf);

		double drawvario, drawintvario;
		string gpsadd = "(baro)";

		if (conf.GPSALT.get()) {
			drawvario = fd.getgpsvario();
			drawintvario = fd.getgpsintvario(10);
			gpsadd = "(gps)";
		} else {
			drawvario = fd.getvario();
			drawintvario = fd.getintvario(10);
		}



		sprintf(buf, "Vario %s: %.2f %s", gpsadd.c_str(),
			drawvario * conf.getvspeed_unit_fac(),
			conf.getvspeed_unit_name().c_str());
		lines.push_back(buf);
		sprintf(buf, "Int.Vario %s: %.2f %s", gpsadd.c_str(),
			drawintvario * conf.getvspeed_unit_fac(),
			conf.getvspeed_unit_name().c_str());
		lines.push_back(buf);
		sprintf(buf, "Groundspeed: %.0f %s",
			fd.getspeed() * conf.getspeed_unit_fac(),
			conf.getspeed_unit_name().c_str());
		lines.push_back(buf);

	}
	}

	if (conf.FLYING.get() || conf.JOYSTICK.get() || conf.GPSD.get()) {


		if (conf.VERBOSE.get()) {
			sprintf(buf, "FPS:  %.1f",  conf.get_fps());
			lines.push_back(buf);
		}

		if (conf.FLYING.get() && conf.VERBOSE.get() && !conf.GPSD.get()) {
			sprintf(buf, "Framerepeat: %dx", conf.movie_repeat_factor.get());
			lines.push_back(buf);
		}
	}

	if (conf.GPSD.get()) {
		if (fd.getvalidfix() == 0) {
			sprintf(buf,"NO GPS SIGNAL!");
			lines.push_back(buf);
		}
	}


  //glLineWidth(2.0);
	glLineWidth(conf.info_fontwidth.get());


	if (conf.style.get() == 1) {
	gltextvect(5, conf.getheight(),
		       font_height, lines, conf.getwidth(),
		       conf.getheight(),conf.ORTHOVIEW.get());
	}
	if (conf.style.get() == 2) {
		for (int z=0; z<=(int(lines.size()-1)); z++) {
			gltext(5, conf.getheight()-(font_height*(z+1)),
		       font_height, lines[z].c_str(), conf.getwidth(),
		       conf.getheight());

		}
	}

	glLineWidth(1.0);
}




// this function sets the initial position! (at start and at F5-key)
void setinitpos(void)
{

	if (conf.DEBUG.get()) {
		cout << "setinitpos() called..." << endl;
	}

	ego.setspinx(270);	//initial dive angle = 0 ( 270-spinx)

	// initial ortho-position is in center
	ego.setOx(0);
	ego.setOy(0);

	// these are the dimnsions in lon and lat direction. In openGL-coordinates (x,y)
	float _lon_dist, _lat_dist;

	// these are used since we need to respect the projection
	// calculate borders in lat/lon
	float _latmax, _lonmax, _latmin, _lonmin;



	//-------------------------------------------------------------------------------- !!!!
	// hier gehts weiter


	if (!conf.FLIGHT.get()) {
		_latmax =
		    fd.getcenter_lat() +
		    (conf.getborder_land_lat() / DEGDIST);
		_latmin =
		    fd.getcenter_lat() -
		    (conf.getborder_land_lat() / DEGDIST);

		_lonmax = fd.getcenter_lon()
		    + (conf.getborder_land_lon() /
		       (DEGDIST *
			cos(fd.getcenter_lat() * (M_PI / 180.0))));


		_lonmin =
		    fd.getcenter_lon() -
		    (conf.getborder_land_lon() /
		     (DEGDIST * cos(fd.getcenter_lat() * (M_PI / 180.0))));

	} else {

		proj.get_latlon(fd.getxmax(), fd.getymax(), _latmax,
				_lonmax);
		proj.get_latlon(fd.getxmin(), fd.getymin(), _latmin,
				_lonmin);

		_latmax += (conf.getborder_land_lat() / DEGDIST);
		_latmin -= (conf.getborder_land_lat() / DEGDIST);
		_lonmax +=
		    conf.getborder_land_lon() / (DEGDIST *
						 cos(fd.getcenter_lat() *
						     (M_PI / 180.0)));
		_lonmin -=
		    conf.getborder_land_lon() / (DEGDIST *
						 cos(fd.getcenter_lat() *
						     (M_PI / 180.0)));


	}

	if (_latmax > 90) {
		_latmax = 90;
	}
	if (_latmin < -90) {
		_latmin = -90;
	}

	if (proj.get_projection() == PROJ_CYL_MERCATOR) {

		if (_latmax > MERCATOR_MAX_LAT) {
			_latmax = MERCATOR_MAX_LAT;
		}
		if (_latmin < -MERCATOR_MAX_LAT) {
			_latmin = -MERCATOR_MAX_LAT;
		}

	}

	float _screenratio =
	    (float) conf.getwidth() / (float) conf.getheight();

	float _xmax, _xmin, _ymax, _ymin;
	proj.get_xy(_latmax, _lonmax, _xmax, _ymax);
	proj.get_xy(_latmin, _lonmin, _xmin, _ymin);

	_lon_dist = -(_xmax - _xmin);
	_lat_dist = -(_ymax - _ymin);

	//cout << _lat_dist << " latdist       londist->" << _lon_dist << endl;

	float _landratio = _lon_dist / _lat_dist;

	if (_screenratio < _landratio) {
		conf.orthoclipping.set(_lon_dist);

		//cout << "wide" << endl;
	} else {

		conf.orthoclipping.set(_lat_dist * _screenratio);
		//cout << "hi " <<  _lat_dist * _screenratio << endl;
		//cout << "screenra " << _screenratio << endl;
	}

	//cout << "otrhocliiping: " << conf.getorthoclipping() << endl;
	// set orthoclipping END


	// set position for 3D mode
	// this is aequivalent to "S", default
	if (conf.FLIGHT.get()) {
		ego.setz(fd.getzmin() + 10 * conf.z_scale.get());	//set position 10km above minimum
		ego.setx(0);
		ego.sety(_ymin);	//over south border

		if (conf.DEBUG.get())
		cout << "setinitpos: flight zmin: " << fd.getzmin() << endl;

	}

	if (!conf.FLIGHT.get()) {
		ego.setz(10 * conf.z_scale.get());	// altitude is 10 km
		ego.setx(0);
		ego.sety(_ymin);	// over south border

	}
	// check for user given position:

	//position "north"
	if (args_info.init_pos_N_given) {
		if (conf.FLIGHT.get()) {
			ego.setx(0);
			ego.sety(_ymax);	//over north border
		} else {
			ego.setx(0);
			ego.sety(_ymax);	// over north border
		}
	}
	//position "south"
	if (args_info.init_pos_S_given) {
		if (conf.FLIGHT.get()) {
			ego.setx(0);
			ego.sety(_ymin);
		} else {
			ego.setx(0);
			ego.sety(_ymin);
		}
	}
	//position "West"
	if (args_info.init_pos_W_given) {
		if (conf.FLIGHT.get()) {
			ego.setx(_xmin);
			ego.sety(0);	//-(conf.getborder_land_lat()+fd.getymax ()) );
		} else {
			ego.setx(_xmin);
			ego.sety(0);	//-conf.getborder_land_lat());
		}
	}
	//position "east"
	if (args_info.init_pos_E_given) {
		if (conf.FLIGHT.get()) {
			ego.setx(_xmax);
			ego.sety(0);	//-(conf.getborder_land_lat()+fd.getymax ()) );
		} else {
			ego.setx(_xmax);
			ego.sety(0);	//-conf.getborder_land_lat());
		}
	}
	//position "NW"
	if (args_info.init_pos_NW_given) {
		if (conf.FLIGHT.get()) {
			ego.setx(_xmin);
			ego.sety(_ymax);
		} else {
			ego.setx(_xmin);
			ego.sety(_ymax);
		}
	}
	//position "SW"
	if (args_info.init_pos_SW_given) {
		if (conf.FLIGHT.get()) {
			ego.setx(_xmin);
			ego.sety(_ymin);
		} else {
			ego.setx(_xmin);
			ego.sety(_ymin);
		}
	}
	//position "NE"
	if (args_info.init_pos_NE_given) {
		if (conf.FLIGHT.get()) {
			ego.setx(_xmax);
			ego.sety(_ymax);
		} else {
			ego.setx(_xmax);
			ego.sety(_ymax);
		}
	}
	//position "SE"
	if (args_info.init_pos_SE_given) {
		if (conf.FLIGHT.get()) {
			ego.setx(_xmax);
			ego.sety(_ymin);
		} else {
			ego.setx(_xmax);
			ego.sety(_ymin);
		}
	}


	if (args_info.init_lat_given && args_info.init_lon_given) {
		float _x, _y;
		proj.get_xy(args_info.init_lat_arg, args_info.init_lon_arg,
			    _x, _y);

		ego.sety(_y);
		ego.setx(_x);
	}

	if (args_info.init_ortho_lat_given
	    && args_info.init_ortho_lon_given) {
		float _x2, _y2;
		proj.get_xy(args_info.init_ortho_lat_arg,
			    args_info.init_ortho_lon_arg, _x2, _y2);

		ego.setOy(-_y2);
		ego.setOx(-_x2);
	}

	if (args_info.init_ortho_width_given)
		conf.orthoclipping.set(args_info.init_ortho_width_arg);

	if (args_info.init_alt_given)
		ego.setz(((double) args_info.init_alt_arg / 1000.0) *
			 conf.z_scale.get());


	//calculate dive
	float _dist =
	    sqrt(ego.getx() * ego.getx() + ego.gety() * ego.gety());
	float _alt;
	if (conf.FLIGHT.get()) {
		_alt = ego.getz() - (fd.getzmin() * conf.z_scale.get());
	} else {
		_alt = ego.getz() - (0.5 * conf.z_scale.get());
	}
	float _atanarg = _alt / _dist;

	ego.setspinx((int) (270.0 + (atan(_atanarg)) / PI_180));

	//calculate heading
	// SW quadrant
	if (ego.getx() >= 0 && ego.gety() >= 0) {
		//cout << "SW Quadrant" << endl;
		if (ego.gety() != 0) {
			//cout << "y != 0" << endl;
			ego.setspinz((int)
				     (atan(ego.getx() / ego.gety())
				      / PI_180)
			    );
		} else {
			ego.setspinz(90);
		}
	}
	// NW quadrant
	if (ego.getx() >= 0 && ego.gety() < 0) {
		//cout << "NW Quadrant" << endl;
		if (ego.getx() != 0) {
			//cout << "x != 0" << endl;
			ego.setspinz((int)
				     (90.0 - (atan(ego.gety() / ego.getx())
					      / PI_180)
				     )
			    );
		} else {
			//cout << "else?" << endl;
			ego.setspinz(180);
		}
	}
	// NE quadrant
	if (ego.getx() < 0 && ego.gety() <= 0) {
		//cout << "NE Quadrant" << endl;
		if (ego.gety() != 0) {
			//cout << "y != 0" << endl;
			ego.setspinz((int)
				     (180.0 +
				      (atan(ego.getx() / ego.gety())
				       / PI_180)
				     )
			    );
		} else {
			//cout << "else?" << endl;
			ego.setspinz(270);
		}
	}
	// SE quadrant
	if (ego.getx() < 0 && ego.gety() > 0) {
		//cout << "SE Quadrant" << endl;
		if (ego.gety() != 0) {
			//cout << "y != 0" << endl;
			ego.setspinz((int)
				     (360.0 +
				      (atan(ego.getx() / ego.gety())
				       / PI_180)
				     )
			    );
		} else {
			//cout << "else?" << endl;
			ego.setspinz(270);
		}
	}


	if (args_info.init_pos_center_given) {
		ego.setx(0);
		ego.sety(0);
		ego.setspinz(0);
		ego.setspinx(360);

	}

	if (args_info.init_heading_given) {
		ego.setspinz((double) args_info.init_heading_arg);
		ego.spinz(0.1);
		ego.spinz(-0.1);
	}

	if (args_info.init_dive_given) {
		ego.setspinx((double) 270 - args_info.init_dive_arg);
		ego.spinx(0.1);
		ego.spinx(-0.1);
	}



	if (conf.FOLLOW.get() && conf.FLIGHT.get()) {
		#ifndef ONLY_OSMESA
		update_diff_pos();
		#endif
	}


	// this caused crashes on Mac OS X
	// setinitpos is called before glutcreatewindow, therefore initGL
	// causes gl... calls before the context is established!
	// but this call is not necessary...

//	InitGL(conf.getwidth(), conf.getheight());
}

void mesaonly_info (void) {
		#ifdef ONLY_OSMESA
		cout << endl << " This version of openGLIGCexplorer (ogie) can be used" << endl
		     << " for offscreen-rendering with osmesa only!" << endl
		     << " No interactive controlling, no output to the display!" << endl << endl;
		#endif
}


//main startup!
//----------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{


// if no argument is given, print help and exit
	if (argc == 1) {
		cmdline_parser_print_help();

		#ifdef ONLY_OSMESA
		mesaonly_info();
		#endif

		Ende(0);
		//exit(0);
	}


/* let's call our cmdline parser */
	if (cmdline_parser(argc, argv, &args_info) != 0) {
		Ende(1);
	}

// check option (for GPLIGC)
	if (args_info.check_given) {
		Ende(0);
	}

// build info...
	if (args_info.compiler_given) {

		char compiler[80] = "unkown";
		char system[80] = "unknown";
		char build[80] = "unknown";


#ifdef __GNUC__

#ifndef __GNUC_MINOR__
#define __GNUC_MINOR__ 0
#endif

#ifndef __GNUC_PATCHLEVEL__
#define __GNUC_PATCHLEVEL__ 0
#endif

#ifndef __VERSION__
#define __VERSION__ 0
#endif

		sprintf(compiler, "gcc %d.%d.%d  (%s)", __GNUC__,
			__GNUC_MINOR__, __GNUC_PATCHLEVEL__, __VERSION__);
#endif

		sprintf(build, "%s %s", __DATE__, __TIME__);

#ifdef __WIN32__
		sprintf(system, "Windows (__WIN32__) is set");
#endif

#ifdef __linux__
		sprintf(system, "Linux (__linux__) is set");
#endif

#ifdef __MACOSX__
		sprintf(system, "Mac OS X (__MAC_OS_X__) is set");
#endif

		std::cout << "Compiler: " << compiler << std::endl;
#ifdef __CYGWIN__
		std::cout << "Cygwin (__CYGWIN__) is set" << std::endl;
#endif

		std::cout << "Build: " << build << std::endl;
		std::cout << "Platform: " << system << std::endl;

#ifdef __unix__
		std::cout << "Unix (__unix__) is set" << std::endl;
#endif

//		std::cout << "System: " << SYSINFO << endl;

		Ende(0);
}


//-------------- THIS WILL FAIL without X!
// however, for osmesa offscreen, freeglut will fail without glutInit
// first call glutStrokeCharacter will stop the runtime
// no real offscreen without Xserver for now!
// see freeglut bug #204
// --no-info hilft!
#ifndef ONLY_OSMESA
	glutInit(&argc, argv);
#endif

#ifndef __WIN32__
#ifndef ONLY_OSMESA
// register function for SIGUSR1
	signal(SIGUSR1, SIGUSR1Handler);
#endif
#endif


	// if verbose given turn it on...
	// in reading configfile we will know, that --verbose was given,
	// because otherwise the default verbose=off would be active...
	if (args_info.verbose_given)
		conf.VERBOSE.on();

	// to be evaluated while reading configfile
	// quiet should override --verbose !!!
	if (args_info.quiet_given) {
		conf.QUIET.on();
		conf.VERBOSE.off();
	}

	// check for debugging option
	// later if debug is given, but after reading configfile
	// debug should override --quiet and activates VERBOSE
	if (args_info.debug_given) {
		conf.DEBUG.on();
		conf.VERBOSE.on();
		conf.QUIET.off();
		cout << "Debugging output enabled..." << endl;
	}

	#ifdef ONLY_OSMESA
	if (!conf.QUIET.get()){
	mesaonly_info();
	}
	#endif




//openGL query
#ifndef ONLY_OSMESA
	if (args_info.query_gl_given) {


		// this needs to be created, to get the information
		window = glutCreateWindow("");

		cout << "Information on the openGL implementation:" <<
		    endl;

		int _q;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_q);
		cout << "GL_MAX_TEXTURE_SIZE: " << _q << endl;

		glGetIntegerv(GL_MAX_EVAL_ORDER, (GLint*)&_q);
		cout << "GL_MAX_EVAL_ORDER: " << _q << endl;

		glGetIntegerv(GL_MAX_LIGHTS, (GLint*)&_q);
		cout << "GL_MAX_LIGHTS: " << _q << endl;

		glGetIntegerv(GL_RED_BITS, (GLint*)&_q);
		cout << "GL_RED_BITS: " << _q << endl;

		glGetIntegerv(GL_GREEN_BITS, (GLint*)&_q);
		cout << "GL_GREEN_BITS: " << _q << endl;

		glGetIntegerv(GL_BLUE_BITS, (GLint*)&_q);
		cout << "GL_BLUE_BITS: " << _q << endl;

		glGetIntegerv(GL_ALPHA_BITS, (GLint*)&_q);
		cout << "GL_ALPHA_BITS: " << _q << endl;

		glGetIntegerv(GL_DEPTH_BITS, (GLint*)&_q);
		cout << "GL_DEPTH_BITS: " << _q << endl;

		glGetIntegerv(GL_MAX_VIEWPORT_DIMS, (GLint*)&_q);
		cout << "GL_MAX_VIEWPORT_DIMS: " << _q << endl;

		glGetIntegerv(GL_AUX_BUFFERS, (GLint*)&_q);
		cout << "GL_AUX_BUFFERS: " << _q << endl;


		const GLubyte *_string;
		_string = glGetString(GL_VENDOR);
		cout << "GL_VENDOR: " << _string << endl;
		_string = glGetString(GL_RENDERER);
		cout << "GL_RENDERER: " << _string << endl;
		_string = glGetString(GL_VERSION);
		cout << "GL_VERSION: " << _string << endl;
		_string = glGetString(GL_EXTENSIONS);
		cout << "GL_EXTENSIONS: " << _string << endl;


#ifndef NOGLX
		cout << endl << "Information on the GLX library" << endl;
		//Xlib Diplay
		Display *dpy;
		//Get the Diplay...
		dpy = XOpenDisplay(NULL);	//will use env DIPLAY on posix .. man XOpenDisplay
		int major, minor;

		glXQueryVersion(dpy, &major, &minor);
		cout << "glXQueryVersion:  " << major << "." << minor <<
		    endl;

		cout << "glXGetClientString(GLX_VENDOR): " <<
		    glXGetClientString(dpy, GLX_VENDOR) << endl;
		cout << "glXGetClientString(GLX_VERSION): " <<
		    glXGetClientString(dpy, GLX_VERSION) << endl;
		cout << "glXGetClientString(GLX_EXTENSIONS): " <<
		    glXGetClientString(dpy, GLX_EXTENSIONS) << endl;

		// int screen ?
		cout << "glXQueryExtensionsString(screen 0): " <<
		    glXQueryExtensionsString(dpy, 0) << endl;
		//cout << "glXQueryExtensionsString(): " << glXQueryExtensionsString(dpy,1) << endl;


		cout << "glXQueryServerString(screen 0, GLX_VENDOR): " <<
		    glXQueryServerString(dpy, 0, GLX_VENDOR) << endl;
		cout << "glXQueryServerString(screen 0, GLX_VERSION): " <<
		    glXQueryServerString(dpy, 0, GLX_VERSION) << endl;
		cout << "glXQueryServerString(screen 0, GLX_EXTENSIONS): "
		    << glXQueryServerString(dpy, 0,
					    GLX_EXTENSIONS) << endl;
#endif

		// and here some information from GLUT header files
		// can be 1-5 (5 is glut.h in mesa 5.0.2 for example)
// 		cout << endl << "Information about GLUT (compiled in from glut.h)" << endl;
// 		cout << "GLUT_API_VERSION: " << GLUT_API_VERSION << endl;
// #ifdef FREEGLUT
// 		cout << "FREEGLUT: " << FREEGLUT << endl;
// #endif
// #ifdef FREEGLUT_VERSION_2_0
// 		cout << "FREEGLUT_VERSION_2_0: " << FREEGLUT_VERSION_2_0 << endl;
// #endif

		// some information from GLUT at runtime....
		cout << endl << "Some runtime GLUT information:" << endl;

		// IN FREEGLUT 2.2.0  (GLUT_VERSION=20200) these are only lies (for X11) (see freeglut_state.c)
		//cout << "glutDeviceGet(GLUT_HAS_MOUSE):  " << glutDeviceGet(GLUT_HAS_MOUSE) << endl;
		//cout << "glutDeviceGet(GLUT_HAS_JOYSTICK):  " << glutDeviceGet(GLUT_HAS_JOYSTICK) << endl;
		//cout << "glutDeviceGet(GLUT_OWNS_JOYSTICK):  " << glutDeviceGet(GLUT_OWNS_JOYSTICK) << endl;
		//cout << "glutDeviceGet(GLUT_JOYSTICK_BUTTONS):  " << glutDeviceGet(GLUT_JOYSTICK_BUTTONS) << endl;
		//cout << "glutDeviceGet(GLUT_JOYSTICK_AXES):  " << glutDeviceGet(GLUT_JOYSTICK_AXES) << endl;
		//cout << "glutDeviceGet(GLUT_JOYSTICK_POLL_RATE):  " << glutDeviceGet(GLUT_JOYSTICK_POLL_RATE) << endl;


		// will cause a warning on old-glut (because GLUT_VERSION isnt known to old-glut)
		cout << "glutGet(GLUT_VERSION): " << glutGet(GLUT_VERSION) << endl;
#ifdef HAVE_LIBGPS
		cout << endl << "GPSD supported"<< endl << "gpsd header version:" << endl;
		cout << "GPSD_API_MAJOR_VERSION: " << GPSD_API_MAJOR_VERSION << endl;
		cout << "GPSD_API_MINOR_VERSION: " << GPSD_API_MINOR_VERSION << endl;
#else
		cout << endl << "GPSD _NOT_ supported"<< endl;
#endif

#ifdef HAVE_LIBOSMESA
		cout << endl << "OSMesa supported" << endl;
#else
		cout << endl << "OSMesa _NOT_ supported" << endl;
#endif

		Ende(0);
	}
#endif


// ----------------- READ CONFIGFILE ----  override compiled in defaults
// some things (projection for example) need to be known before loading any igc files

// check for config file first
	if (args_info.config_file_given) {
		//cout << "Configfile !!! ::  " << conf.getConfigFileName() << endl;
		conf.setConfigFileName(args_info.config_file_arg);
	}
// read the configuration file first, then evaluate the cmdline
// cmdl can override configuration-file!

	conf.readConfigFile();

//------------------------------------------------------------------------

	if (args_info.get_elevation_given) {
		if (args_info.lat_given && args_info.lon_given) {

			vector<int>_alt;
			vector<float>_lat;
			vector<float>_lon;

			// WARN on should be used for srtm-3 from seamless server
			// off for SRTM30
			// see get_gnd_elevation
			conf.GETELEV_WARN.on();
//			conf.GETELEV_WARN.off();
			conf.GETELEV_ERROR.on();

			_lat.push_back(args_info.lat_arg);
			_lon.push_back(args_info.lon_arg);
			_alt.push_back(0);

			get_gnd_elevations(&_alt,&_lat,&_lon,&conf);

			// smaller 0 is invalid
			if (_alt[0] > 0) {
				cout << _alt[0] << endl;

				if (conf.GETELEV_ERROR.get())
					cout << _alt[1] << endl;
			} else {

				if (conf.GETELEV_WARN.get()) {
					cout << _alt[1] << endl;
					cout << "INVALID" << (-1*_alt[0]) <<endl;
				} else {
					cout << _alt[0] << endl;
	                                if (conf.GETELEV_ERROR.get())
    	                                    cout << _alt[1] << endl;
				}
			}



			exit (0);



		} else {
			cerr << "--get-elevation without --lat and --lon!" << endl ;

		}
	}

//air class knows the configuration...
	air.setConfig(&conf);
	fd.set_proj_pointer(&proj);
	fd.set_conf_pointer(&conf);
	lifts.set_proj_pointer(&proj);
	lifts.set_conf_pointer(&conf);
	waypoints.set_proj_pointer(&proj);
	waypoints.set_conf_pointer(&conf);


	if (args_info.cycles_given)
		fd.setcycles(args_info.cycles_arg);

//-----cmd line options to be read before igc file is loaded-------

	proj.set_projection(conf.get_projection());

	if (args_info.projection_cyl_platt_given)
		proj.set_projection(PROJ_CYL_PLATT);

	if (args_info.projection_cyl_mercator_given)
		proj.set_projection(PROJ_CYL_MERCATOR);

	if (args_info.projection_cyl_no1_given)
		proj.set_projection(PROJ_CYL_NO1);

	if (args_info.projection_pseudo_cyl_no1_given)
		proj.set_projection(PROJ_PSEUDO_CYL_NO1);


	// handled at the beginning of main, and config-reading now...
	//if (args_info.verbose_given)
	//	conf.VERBOSE.on();

        //if (args_info.quiet_given)
	//	conf.VERBOSE.off();


//------------------------------------------------------------------------


// if only one argument is given, assume that it is an igc-file
	if (argc == 2 && !args_info.gpsd_given && !args_info.gpsd_port_given && !args_info.gpsd_server_given) {
		//cout << "nur 1 arg ? : " << argc << endl;
		fd.readIGCFile(argv[1], args_info.use_all_fixes_given);

		// no valid fixes? try invalid ones
		if (fd.getn() == 0) {
			cerr <<
			    "No valid position fixes found... looking for invalid ones"
			    << endl;
			fd.readIGCFile(argv[1], true);
		}
		//if no position fixes recognized....
		if (fd.getn() == 0) {
			cerr << "This seems not to be an igc-file!" <<
			    endl;
			Ende(1);
		}

		conf.setIGCFileName(argv[1]);
	}

	if ( ! (argc==2 ||  args_info.igc_file_given || (args_info.lat_given && args_info.lon_given))
		&& (! args_info.gpsd_given && !args_info.gpsd_port_given && !args_info.gpsd_server_given)
					) {
		cerr << "Not enough! Give -i igcfile   or   --lat and --lon" << endl;
		Ende(1);
	}

	if (args_info.gpsd_given || args_info.gpsd_port_given || args_info.gpsd_server_given) {

#ifdef HAVE_LIBGPS
#if GPSD_API_MAJOR_VERSION != 5
		cerr << "We need libgps with GPSD_API_MAJOR_VERSION == 5 !!!" << endl
		    << "Version " << GPSD_API_MAJOR_VERSION << " was found ;-(" << endl;
		    Ende(1);
#endif
#endif


		conf.GPSD.on();
		conf.MARKER.on();
		conf.FOLLOW.on();
		conf.FLYING.on();

		stringstream _sp;

		if (args_info.gpsd_port_given) {
		   _sp << args_info.gpsd_port_arg;
		}

		if (args_info.gpsd_server_given && !args_info.gpsd_port_given) {
		  fd.initgps(args_info.gpsd_server_arg);
		}
		if (args_info.gpsd_server_given && args_info.gpsd_port_given) {
		  fd.initgps(args_info.gpsd_server_arg, _sp.str().c_str());
		}
		if (!args_info.gpsd_server_given && args_info.gpsd_port_given) {
		  fd.initgps("localhost", _sp.str().c_str());
		}
		if (!args_info.gpsd_server_given && !args_info.gpsd_port_given) {
		  fd.initgps();
		}

	}



	// check for igc-file (-i option)
	if (args_info.igc_file_given) {
		fd.readIGCFile(args_info.igc_file_arg,
			       args_info.use_all_fixes_given);

		// no valid fixes? try invalid ones
		if (fd.getn() == 0) {
			cerr <<
			    "No valid position fixes found... looking for invalid ones"
			    << endl;
			fd.readIGCFile(args_info.igc_file_arg, true);
		}
		//if no position fixes recognized....
		if (fd.getn() == 0) {
			cerr << "This seems not to be an igc-file!" <<
			    endl;
			Ende(1);
		}

		conf.setIGCFileName(args_info.igc_file_arg);
	}
// ------------------- READ CMDLINE ----- override CONFIGFILE options...



	if (args_info.airspace_file_given)
		conf.setAirspaceFileName(args_info.airspace_file_arg);

	if (args_info.lifts_given) {
		conf.LIFTS.on();
		conf.setLiftsFileName(args_info.lifts_arg);
		if (conf.VERBOSE.get()) {
			cout << "Lifts: " << conf.getLiftsFileName() << endl;
		}
	}

	if (args_info.waypoints_file_given) {
		conf.WAYPOINTS.on();
		conf.setWaypointsFileName(args_info.waypoints_file_arg);
		if (conf.VERBOSE.get()) {
			cout << "Waypoints: " << conf.getWaypointsFileName() << endl;
		}
	}

	if (args_info.waypoints_given) { conf.WAYPOINTS.on();}
	if (args_info.no_waypoints_given) { conf.WAYPOINTS.off();}

	if (args_info.waypoints_offset_given) { conf.waypoints_offset.set(args_info.waypoints_offset_arg);}

	if (args_info.waypoints_offset_spheres_given) { conf.waypoints_offset.set(args_info.waypoints_offset_spheres_arg);
	  conf.WAYPOINTS_OFFSET_TEXT_ONLY.off();
	}

	if (args_info.map_set_name_given) {
		if (conf.set_ActiveMapSet(args_info.map_set_name_arg) != 0) {
			cout << args_info.
			    map_set_name_arg << " not found ...." << endl;
		}
	}
// some output (if VERBOSE)
	if (conf.VERBOSE.get()) {
		cout << "Configfile:  " << conf.
		    getConfigFileName() << endl;
		cout << "OpenAir: " << conf.getAirspaceFileName() << endl;
		cout << "DEM-File: " << conf.getDemFileName() << endl;

		if (conf.getNumberOfMaps() != 0) {
			cout << conf.getNumberOfMaps() << " Digital maps found, " << conf.getNumberOfMapSets() << " Map-Sets"
				<< endl;
		}
	}
// landscape or flat?
	if (args_info.landscape_given)
		conf.LANDSCAPE.on();

	if (args_info.flat_given) {
		conf.LANDSCAPE.off();
		//conf.toggleLANDSCAPE();
	}

	if (args_info.flat_given && args_info.landscape_given) {
		cerr << "specify \"--flat\"  _or_  \"--landscape\"" <<
		    endl;
		Ende(1);
	}
// ALTITUDE DATA used !
	if (args_info.gpsalt_given)
		conf.GPSALT.on();
	if (args_info.baroalt_given)
		conf.GPSALT.off();

	if (args_info.ignore_elev_min_given && args_info.ignore_elev_max_given) {
		conf.IGNORE_ELEV_RANGE.on();
		conf.ignore_elev_max.set(args_info.ignore_elev_max_arg);
		conf.ignore_elev_min.set(args_info.ignore_elev_min_arg);
	}


// modulate or not?
	if (args_info.modulate_given)
		conf.MODULATE.on();

	if (args_info.no_modulate_given)
		conf.MODULATE.off();

// lighting of unmodulated maps
	if (args_info.maps_unlighted_given)
		conf.MAPS_UNLIGHTED.on();

	if (args_info.maps_lighted_given)
		conf.MAPS_UNLIGHTED.off();


// Airspace or not?
	if (args_info.airspace_given)
		conf.AIRSPACE.on();

	if (args_info.no_airspace_given)
		conf.AIRSPACE.off();

	if (args_info.airspace_wire_given)
		conf.AIRSPACE_WIRE.on();

	if (args_info.airspace_transparent_given)
		conf.AIRSPACE_WIRE.off();

	if (args_info.airspace_limit_given)
		conf.AirspaceLimit.set(args_info.airspace_limit_arg);


// dont start in this condition....
	if (conf.AIRSPACE.get()
	    && (conf.getAirspaceFileName() == "not available")) {
		cerr << "No Airspace available..." << endl;
		Ende(1);
	}
// MAP
	if (args_info.map_given) {
		//conf.setLANDSCAPE();
		//conf.toggleLANDSCAPE();
		conf.MAP.on();
	}

	if (args_info.no_map_given) {
		conf.MAP.off();
	}
// Wireframe or filled polygons
	if (args_info.wire_given)
		conf.WIRE.on();
	if (args_info.filled_given)
		conf.WIRE.off();

// sealevel stuff
	if (args_info.sealevel_given)
		conf.set_sealevel(args_info.sealevel_arg);
	if (args_info.sealevel2_given)
		conf.set_sealevel2(args_info.sealevel2_arg);
	if (args_info.sealevel3_given)
		conf.set_sealevel3(args_info.sealevel3_arg);



// no --marker-pos without igc-file
	if (args_info.marker_pos_given
	    && !(args_info.igc_file_given || argc == 2)) {
		cerr << "Can't use --marker-pos without flightdata" <<
		    endl;
		Ende(1);
	}
// no --marker without igc-file
// marker will be turned off later, if no flightdata is available... (see below)
/*
if (args_info.marker_given && !(args_info.igc_file_given || argc == 2) )  {
   	cerr << "Can't use --marker without flightdata" << endl;
   	Ende (1);
   }
*/


// Marker on or off?
	if (args_info.marker_given)
		conf.MARKER.on();
	if (args_info.no_marker_given)
		conf.MARKER.off();


	if (args_info.marker_size_given)
		conf.MARKER_SIZE.set(args_info.marker_size_arg);

	if (args_info.text_width_given)
		conf.text_width.set(args_info.text_width_arg);

	if (args_info.lifts_info_mode_given)
		conf.lifts_info_mode.set(args_info.lifts_info_mode_arg);

	if (args_info.waypoints_info_mode_given)
		conf.waypoints_info_mode.set(args_info.waypoints_info_mode_arg);

	if (args_info.text_size_given)
		conf.pTextSize.set(args_info.text_size_arg);

//spinning for movie
	if (args_info.spinning_given) {
		conf.SPINNING.set(args_info.spinning_arg);
	}

// Info  Diplay on or off?
	if (args_info.info_given)
		conf.INFO.on();
	if (args_info.no_info_given)
		conf.INFO.off();


// avoid --lat --lon with igc-file
	if ((args_info.lat_given && args_info.lon_given)
	    && (args_info.igc_file_given || argc == 2)) {
		cerr << "Can't use --lat, --lon and flightdata" << endl;
		Ende(1);
	}
// set marker position
	if (args_info.marker_pos_given)
		fd.setmarkerpos(args_info.marker_pos_arg);
	if (args_info.marker_time_given)
		fd.setmarkertime(args_info.marker_time_arg);

// fullscreen
	if (args_info.fullscreen_given)
		conf.FULLSCREEN.on();
	if (args_info.window_given)
		conf.FULLSCREEN.off();


// window geometry
	if (args_info.width_given) {
		conf.setwidth(args_info.width_arg);
		conf.setinitwidth(args_info.width_arg);
	}
	if (args_info.height_given) {
		conf.setheight(args_info.height_arg);
		conf.setinitheight(args_info.height_arg);
	}
// 3D or 2D
	if (args_info.ortho_given)
		conf.ORTHOVIEW.on();
	if (args_info.perspective_given)
		conf.ORTHOVIEW.off();


//grayscale
	if (args_info.grayscale_given)
		conf.BW.on();
	if (args_info.color_given)
		conf.BW.off();

//stereoscopic
	if (args_info.stereo_given)
		conf.STEREO.on();
	if (args_info.no_stereo_given)
		conf.STEREO.off();

//stereo rg
	if (args_info.stereo_rg_given)
		conf.STEREO_RG.on();
	if (args_info.no_stereo_rg_given)
		conf.STEREO_RG.off();

//stereo rb
	if (args_info.stereo_rb_given)
		conf.STEREO_RB.on();
	if (args_info.no_stereo_rb_given)
		conf.STEREO_RB.off();

//stereo hardware
	if (args_info.stereo_hw_given)
		conf.STEREO_HW.on();
	if (args_info.no_stereo_hw_given)
		conf.STEREO_HW.off();

// nur 1 von 4 ... STEREO modes...
// ??

	int _nr = 0;
	_nr +=
	    (int) conf.STEREO_RB.get() + (int) conf.STEREO_RG.get() +
	    (int) conf.STEREO.get() + (int) conf.STEREO_HW.get();

	if (_nr >= 2) {
		cerr << "Only one stereo-option can be used!" << endl;
		Ende(1);
	}
// image swapping! left <-> right
	if (args_info.inverse_stereo_given)
		conf.SWAP_STEREO.on();
	if (args_info.no_inverse_stereo_given)
		conf.SWAP_STEREO.off();


// eye-distance
	if (args_info.eye_dist_given) {
		if (args_info.eye_dist_arg > 0)
			conf.eye_dist.set(args_info.eye_dist_arg);
	}
// Offscreen rendering
	if (args_info.offscreen_given)
		conf.OFFSCREEN.on();

// Offscreen with mesa
	if (args_info.osmesa_given)
		conf.OSMESA.on();

	if (conf.OSMESA.get() && conf.OFFSCREEN.get()) {
		cerr << "only one offscreen rendering mode possible!" <<
		    endl;
		Ende(1);
	}

	if (conf.STEREO_HW.get()
	    && (conf.OSMESA.get() || conf.OFFSCREEN.get())) {
		cerr <<
		    "Hardware stereo isn't possible in offscreen rendering mode"
		    << endl;
		Ende(1);
	}

// jpeg quality for screenshots
	if (args_info.jpeg_quality_given) {
		if ( (args_info.jpeg_quality_arg >= 0) &&  (args_info.jpeg_quality_arg <= 100))
			conf.jpeg_quality.set(args_info.jpeg_quality_arg);
	}

// image format for output
	if (args_info.image_format_given)
		conf.set_image_format(args_info.image_format_arg);

// offset
	if (args_info.offset_given)
		conf.offset.set(args_info.offset_arg);

// airfield elevation
	if (args_info.airfield_elevation_given) {
		conf.setairfield_elevation(args_info.
					   airfield_elevation_arg);
	}
// avoid --airfield-elevation and --offset
	if (args_info.airfield_elevation_given && args_info.offset_given) {
		cerr <<
		    "specify \"--offset\" _or_ \"--airfield-elevation\"" <<
		    endl;
		Ende(1);
	}
	// gpsalt?
	if (conf.GPSALT.get()) {

		if (conf.VERBOSE.get())
			cout << "gps-altitude is used." << endl;

		fd.usegps();

	}
// if we have an elevation and an igc-file. Calculate offset!
	if (args_info.airfield_elevation_given
	    && (args_info.igc_file_given || argc == 2)) {
		conf.offset.set(-1 *
				(fd.getstartelevation(conf.GPSALT.get()) -
				 args_info.airfield_elevation_arg));
		if (conf.VERBOSE.get())
			cout << "offset = " << (-1 *
						(fd.
						 getstartelevation(conf.
								   GPSALT.
								   get()) -
						 args_info.
						 airfield_elevation_arg))
			    << endl;
		//cout << "offset = " << conf.getoffset() << endl;
	}
// Z-Axis scaling!
	if (args_info.scalez_given)
		conf.z_scale.set(args_info.scalez_arg);

// Downscaling
	if (args_info.downscaling_given)
		conf.setdownscalefactor(args_info.downscaling_arg);
	if (args_info.upscaling_given)
		conf.setupscalefactor(args_info.upscaling_arg);

// don't allow Landscape, Map and downscaling!
// but it is possible to switch in this state later... (not very beautiful, but works)
/*	if (conf.LANDSCAPE.get() && conf.MAP.get()
	    && conf.getdownscalefactor() != 1) {
		cerr << "Can't use textured maps, DEM and downscaling!" <<
		    endl;
		Ende(1);
	}
*/

// texture map compression!
	if (args_info.compression_given) {
		conf.COMPRESSION.on();
	}
	if (args_info.no_compression_given) {
		conf.COMPRESSION.off();
		//conf.toggleCOMPRESSION();
	}

	if (args_info.airspace_wire_col_r_given) {
		conf.airspace_wire_color_r.set(args_info.airspace_wire_col_r_arg);
	}
	if (args_info.airspace_wire_col_g_given) {
		conf.airspace_wire_color_g.set(args_info.airspace_wire_col_g_arg);
	}
	if (args_info.airspace_wire_col_b_given) {
		conf.airspace_wire_color_b.set(args_info.airspace_wire_col_b_arg);
	}
	if (args_info.airspace_wire_width_given) {
		conf.airspace_wire_width.set(args_info.airspace_wire_width_arg);
	}

// set colormap
	if (args_info.colormap_given) {
		if (args_info.colormap_arg >= 1
		    && args_info.colormap_arg <= 8) {
			conf.colmapnumber.set(args_info.colormap_arg);
		} else {
			cerr << "Colormap " << args_info.
			    colormap_arg << " doesnt exist. using default."
			    << endl;
		}
	}

// set colormap
	if (args_info.colormap_sea_given) {
		if (args_info.colormap_sea_arg >= 1
		    && args_info.colormap_sea_arg <= 8) {
			conf.colmap2number.set(args_info.colormap_sea_arg);
		} else {
			cerr << "Colormap " << args_info.
			    colormap_sea_arg <<
			    " doesnt exist. using default." << endl;
		}
	}

// Set border .....
	if (args_info.border_given) {
		conf.setborder_land_lat(args_info.border_arg);
		conf.setborder_land_lon(args_info.border_arg);
	}

	if (args_info.border_lat_given)
		conf.setborder_land_lat(args_info.border_lat_arg);
	if (args_info.border_lon_given)
		conf.setborder_land_lon(args_info.border_lon_arg);


// flat or gouraud-shading !
	if (args_info.flat_shading_given) {
		conf.SHADE.off();
	}
	if (args_info.gouraud_shading_given) {
		conf.SHADE.on();
	}

// lighting workaround for upscaling
	if (args_info.no_lighting_given) {
		conf.NOLIGHTING.on();
	}
	if (args_info.terrain_shading_given) {
		conf.TERRAINSHADING.on();
		conf.NOLIGHTING.on();
	}
	if (args_info.light_direction_given) {
		conf.shadedirection.set(args_info.light_direction_arg);
	}
	if (args_info.shading_scale_given) {
		conf.setshadescale(args_info.shading_scale_arg);
	}

//quads
	if (args_info.quads_given) {
		conf.QUADS.on();
	}

// Fog related stuff
	if (args_info.haze_given)
		conf.FOG.on();
	if (args_info.no_haze_given)
		conf.FOG.off();
	if (args_info.haze_density_given) {
		conf.fogdensity.set(args_info.haze_density_arg);
	}
//curtain
	if (args_info.curtain_given)
		conf.CURTAIN.on();
	if (args_info.no_curtain_given)
		conf.CURTAIN.off();



// Angle of View
	if (args_info.aov_given) {

		if (args_info.aov_arg >= 1 && args_info.aov_arg <= 179) {


			conf.aov.set(args_info.aov_arg);


		} else {

			cerr <<
			    "Angle of view is out of Range. Using default."
			    << endl;

		}
	}
// no flightdata, but  lat lon!   (dummy fd needed!)

// avoid lat or lon!
	if (args_info.lon_given && !args_info.lat_given) {
		cerr << "specify --lon and --lat !" << endl;
		Ende(1);
	}

	if (args_info.lat_given) {
		if (!args_info.lon_given) {
			cerr << "specify --lat and --lon !" << endl;
			Ende(1);
		}
		conf.FLIGHT.off();

		// maybe someone want to start without terrain (make sense since airspace and maps))
		//conf.LANDSCAPE.on();

		conf.setlat(args_info.lat_arg);
		conf.setlon(args_info.lon_arg);

		///set dummy values in fd!
		proj.set_center_lat(conf.getlat());	// set center for projections....
		proj.set_center_lon(conf.getlon());
		fd.setcenter_lat(conf.getlat());
		fd.setcenter_lon(conf.getlon());
		fd.setxcenter(0);
		fd.setycenter(0);
		fd.setzcenter(0);
		fd.setlatmin(conf.getlat());
		fd.setlonmin(conf.getlon());
		fd.setlatmax(conf.getlat());
		fd.setlonmax(conf.getlon());
		fd.setmarkerpos(0);

	}
// if we have flightdata and DEM-data, we can calculate the ground-altitude!
	if (conf.FLIGHT.get()) {
		if (conf.DEM.get())
			fd.readGNDElevation(&conf);
	}

// initialize airspace class
	if (conf.getAirspaceFileName() != "not available") {

		float border_deg_lat = conf.getborder_land_lat() / DEGDIST;
		float border_deg_lon =
		    (conf.getborder_land_lon() / DEGDIST) /
		    cos(fd.getcenter_lat() * PI_180);
		float _latmin = (fd.getlatmin() - border_deg_lat);
		float _lonmin = (fd.getlonmin() - border_deg_lon);
		float _latmax = (fd.getlatmax() + border_deg_lat);
		float _lonmax = (fd.getlonmax() + border_deg_lon);

		air.initAirspace(fd.getcenter_lat(), fd.getcenter_lon(),
				 _latmax, _latmin, _lonmax, _lonmin);
		air.set_proj_pointer(&proj);
		air.readOpenAirFile(conf.getAirspaceFileName());
	}
// some Verbosity!
	if (conf.VERBOSE.get()) {
		cout << fd.getn() << " = Number of position fixes" << endl;

		/*cout << "some info from igc-file:" << endl << "center x; y; z   " << fd.getxcenter () << " " << fd.
		   getycenter () << " " << fd.getzcenter () << endl << "xmin xmax ymin ymax zmin zmax  " << fd.
		   getxmin () << " " << fd.getxmax () << " " << fd.getymin () << " " << fd.getymax () << " " << fd.
		   getzmin () << " " << fd.getzmax () << endl; */
	}
// if no DEM-data available, turn off LANDSCAPE
	if (!conf.DEM.get()) {
		//no DEM File
		cerr << "No Digital Elevation Model (DEM-File)" << endl
		    << "Read documention how to set up the config file" <<
		    endl << "starting without Terrain" << endl;
		conf.LANDSCAPE.off();
		//conf.toggleLANDSCAPE();
	}
// impossibel condition ?!
	if (!conf.FLIGHT.get() && conf.MARKER.get()) {
		cerr <<
		    "Can't use \"Marker\" with no flightdata. \"Marker\" turned off."
		    << endl;
		conf.MARKER.off();
	}
// set colormap limits !
	if (args_info.colormap_max_given) {
		conf.setmax_h(args_info.colormap_max_arg);
		//cout << "colormap max" << endl;
	}
	if (args_info.colormap_min_given) {
		conf.setmin_h(args_info.colormap_min_arg);
		//cout << "colormap min" << endl;
	}
// set marker-range and follow
	if (args_info.follow_given)
		conf.FOLLOW.on();
	if (args_info.no_follow_given)
		conf.FOLLOW.off();
	if (args_info.marker_range_given)
		conf.MARKER_RANGE.on();
	if (args_info.no_marker_range_given)
		conf.MARKER_RANGE.off();
	if (args_info.marker_ahead_given)
		conf.set_marker_ahead(args_info.marker_ahead_arg);
	if (args_info.marker_back_given)
		conf.set_marker_back(args_info.marker_back_arg);

	if (args_info.flighttrack_mode_given) {
		if (args_info.flighttrack_mode_arg >= 0
		    && args_info.flighttrack_mode_arg <= 3) {
			conf.flightstrip_mode.set(args_info.flighttrack_mode_arg);
		} else {
			cerr << "Flighttrack mode " << args_info.
			    flighttrack_mode_arg << " doesnt exist. (0=classic 2 color,1=alt,2=speed,3=vario). using default now."
			    << endl;
		}
	}

//flightstrip_linewidth
	if (args_info.flighttrack_linewidth_given) {
		conf.flightstrip_width.set(args_info.flighttrack_linewidth_arg);
	}

	if (args_info.flighttrack_colormap_given) {
		conf.flightstrip_colmap.set(args_info.flighttrack_colormap_arg);
	}
//set initial position...

	if (conf.DEBUG.get()) {
	    cout << "end of preliminary work... calling setinitpos" << endl << flush;
	}

	setinitpos();

	if (conf.DEBUG.get()) {
            cout << "returned from setinipos()" << endl << flush;
        }

	if (conf.LIFTS.get()) {
		lifts.readLiftsFile(conf.getLiftsFileName());
		float border_deg_lat = conf.getborder_land_lat() / DEGDIST;
		float border_deg_lon =
		    (conf.getborder_land_lon() / DEGDIST) /
		    cos(fd.getcenter_lat() * PI_180);
		float _latmin = (fd.getlatmin() - border_deg_lat);
		float _lonmin = (fd.getlonmin() - border_deg_lon);
		float _latmax = (fd.getlatmax() + border_deg_lat);
		float _lonmax = (fd.getlonmax() + border_deg_lon);
		lifts.setDegRanges(_latmax,_latmin,_lonmax,_lonmin);
	}

	if (conf.getWaypointsFileName() != "not available") {
		waypoints.readWaypointsFile(conf.getWaypointsFileName());
		float border_deg_lat = conf.getborder_land_lat() / DEGDIST;
		float border_deg_lon =
		    (conf.getborder_land_lon() / DEGDIST) /
		    cos(fd.getcenter_lat() * PI_180);
		float _latmin = (fd.getlatmin() - border_deg_lat);
		float _lonmin = (fd.getlonmin() - border_deg_lon);
		float _latmax = (fd.getlatmax() + border_deg_lat);
		float _lonmax = (fd.getlonmax() + border_deg_lon);
		waypoints.setDegRanges(_latmax,_latmin,_lonmax,_lonmin);
	}

// ------------------ END of setting options etc -------------------------------------------------------
// ------------------ rendering ---------------------------------------------------------------------------


#ifdef NOGLX
// if someone tries to offscreen via GLX on non-GLX platform...
	if (conf.OFFSCREEN.get()) {
		cerr << "GLX offscreen rendering is not available!" << endl;
		Ende(1);
	}
#endif



#ifndef NOGLX
#ifndef ONLY_OSMESA
	// OFFRSCEEN Rendering (with GLX)
	if (conf.OFFSCREEN.get()) {

		// measuring time
		//timeval start;
		//timeval stop;
		//gettimeofday(&start, NULL);
		Timecheck offscreentime("offscreen_rendering", &conf);

		//create GLX offscreen rendering context....
		//Xlib Diplay
		Display *dpy;

		//Get the Diplay...
		dpy = XOpenDisplay(NULL);	//will use env DIPLAY on posix .. man XOpenDisplay

		if (dpy == NULL) {
			cerr << "Cannot open Display!" << endl;
			Ende(1);
		}
		// check for GLX extensions
		if (!glXQueryExtension(dpy, NULL, NULL)) {
			cerr <<
			    "X-Server doesn't have an OpenGL GLX extension"
			    << endl;
			Ende(1);
		}
		// Check GLX Version (we need >=1.3)
		int major, minor;
		glXQueryVersion(dpy, &major, &minor);

		if (conf.VERBOSE.get())
			cout << "GLX Version " << major << " . " << minor
			    << endl;


		if ((major == 1 && minor < 3) || major < 1) {
			cerr <<
			    "at least GLX 1.3 is needed for offscreen rendering using glx-pbuffers!"
			    << endl;
			cerr << "your version is: " << major << "." <<
			    minor << endl;
			Ende(1);
		}
		// these are the attributes the GLXFBConfig should have
		int attrib_list[] = {
			GLX_MAX_PBUFFER_PIXELS,
			conf.getheight() * conf.getwidth(),
			GLX_DOUBLEBUFFER, GL_FALSE,
			//GLX_RGBA, GL_TRUE,
			GLX_RENDER_TYPE, GLX_RGBA_BIT,
			GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT,

			//GLX_DEPTH_SIZE, 8,
			//GLX_X_RENDERABLE, GL_TRUE,
			//GLX_X_VISUAL_TYPE, GLX_X_VISUAL_TYPE,

			GLX_RED_SIZE, 8,
			GLX_BLUE_SIZE, 8,
			GLX_GREEN_SIZE, 8,
			GLX_DEPTH_SIZE, 16,
			None
		};


		// these are the attributes the pbuffer should have
		int pbuffattrib_list[] = {
			GLX_PBUFFER_HEIGHT, conf.getheight(),
			GLX_PBUFFER_WIDTH, conf.getwidth(),
			GLX_PRESERVED_CONTENTS, GL_TRUE,
			GLX_LARGEST_PBUFFER, GL_FALSE,
			None
		};

		// number of GLXFBConfigs returned by glXChooseFBConfig
		int elements;
		GLXFBConfig *configs;


		configs =
		    glXChooseFBConfig(dpy, 0, attrib_list, &elements);


		if (elements < 1) {
			cerr << "No GLXFBConfigs returned ... (" << elements << ")" << endl;
			Ende(1);
		}
		// Pbuffer and GLXContext
		GLXPbuffer pbuff;
		GLXContext ctx;

		// Create Pbuffer
		pbuff =
		    glXCreatePbuffer(dpy, configs[0], pbuffattrib_list);

		// this seems not to work? what will be returned if glXCreatePbuffer fails?
		/*
		   if (pbuff == NULL) {
		   cerr << "Can't create pbuffer" << endl;
		   Ende(1);
		   }
		 */

		// Create Context ( NULL -> no share list, GL_TRUE -> try to get an direct rendering context

		ctx =
		    glXCreateNewContext(dpy, configs[0], GLX_RGBA_TYPE,
					NULL, GL_FALSE);

		if (ctx == NULL) {
			cerr << "context not created" << endl;
			Ende(1);
		}
		//make the context current
		//glXMakeContextCurrent(dpy, pbuff, pbuff, ctx);
		// hier gibts nen badAlloc wenn man auf der console ist :(
		glXMakeCurrent(dpy, pbuff, ctx);


		//now we are ready to openGL!

		InitGL(conf.getwidth(), conf.getheight());

		if (conf.FLIGHT.get())
			GenFlightList();	//call the list-creator // with igc filename

		GenLandscapeList();
		GenAirspaceList();
//		GenPointsList();

		DrawGLScene();

		screenshot();

		/*gettimeofday(&stop, NULL);

		float _time = (((float) stop.tv_sec - (float) start.tv_sec)
			       +
			       (((float) stop.tv_usec -
				 (float) start.tv_usec) / 1000000.0));

		if (conf.VERBOSE.get())
			cout << "Rendering time: " << _time << " secs" <<
			    endl;*/
		offscreentime.checknow("rendering finished");

		Ende(0);

	}
#endif
#endif


#ifdef HAVE_LIBOSMESA
	// OFFRSCEEN Rendering with mesa (osmesa)
	// MESAMESAMESAMESAMESAMESAMESAMESAMESAMESAMESAMESAMESAMESAMESA ###############################
	if (conf.OSMESA.get()) {
		Timecheck osmesatime("osmesa_offscreen_redering", &conf);

		//create mesa offscreen rendering context....
		OSMesaContext ctx;
		void *buffer;

		if (conf.VERBOSE.get())
			cout << "Rendering offscreen using OSMesa." << endl;

		// 32 depth buffer bits
		ctx = OSMesaCreateContextExt(OSMESA_RGBA, 32,0,0,NULL);
		if (!ctx) {
			cerr << "creating osmesa context failed!" << endl;
			Ende(1);
		}

		// check max viewport dimensions
		int _osmaxh, _osmaxw;//, _maj, _min, _pat;
		OSMesaGetIntegerv(OSMESA_MAX_HEIGHT, &_osmaxh);
		OSMesaGetIntegerv(OSMESA_MAX_WIDTH, &_osmaxw);
		//OSMesaGetIntegerv(OSMESA_MAJOR_VERSION, &_maj);
		//OSMesaGetIntegerv(OSMESA_MINOR_VERSION, &_min);
		//OSMesaGetIntegerv(OSMESA_PATCH_VERSION, &_pat);

		if (conf.VERBOSE.get()){
			//cout << "Mesa, Version " << _maj <<"."<<_min<<"."<<_pat<<endl;
			cout << "OSMESA_MAX_HEIGHT: " << _osmaxh << endl;
			cout << "OSMESA_MAX_WIDTH: " << _osmaxw << endl;

		}

		if (conf.getwidth() <= _osmaxw && conf.getheight() <= _osmaxh) {

			buffer =
			    malloc(conf.getwidth() * conf.getheight() * 4 *
				   sizeof(GLubyte));
			if (!buffer) {
				cerr <<
				    "allocating buffer for osmesa failed!"
				    << endl;
				Ende(1);
			} else {
				if (conf.VERBOSE.get())
					cout << "buffer created " << conf.
					    getwidth() * conf.getheight() *
					    4 * sizeof(GLubyte) << endl;

			}


		} else {

			cerr <<
			    "width and/or height too big for osmesa (max "<< _osmaxw << "x" <<_osmaxh<<")"  << endl;
			Ende(1);

		}

		if (!OSMesaMakeCurrent
		    (ctx, buffer, GL_UNSIGNED_BYTE, conf.getwidth(),
		     conf.getheight())) {
			cerr << "Make context current failed!" << endl;
			Ende(1);
		}


		//now we are ready to openGL!

		InitGL(conf.getwidth(), conf.getheight());




		if (conf.VERBOSE.get()){
		int _dpw, _dph, _dpbpp;
		void *bp;
		OSMesaGetDepthBuffer( ctx, &_dpw, &_dph, &_dpbpp, &bp );//, void **buffer );
		cout << "Depth buffer: "<< _dpw << " " << _dph << " " << _dpbpp << endl;

		int _q;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_q);
		cout << "GL_MAX_TEXTURE_SIZE: " << _q << endl;

		//glGetIntegerv(GL_MAX_EVAL_ORDER, &_q);
		//cout << "GL_MAX_EVAL_ORDER: " << _q << endl;

		//glGetIntegerv(GL_MAX_LIGHTS, &_q);
		//cout << "GL_MAX_LIGHTS: " << _q << endl;

		glGetIntegerv(GL_RED_BITS, &_q);
		cout << "GL_RED_BITS: " << _q << endl;

		glGetIntegerv(GL_GREEN_BITS, &_q);
		cout << "GL_GREEN_BITS: " << _q << endl;

		glGetIntegerv(GL_BLUE_BITS, &_q);
		cout << "GL_BLUE_BITS: " << _q << endl;

		glGetIntegerv(GL_ALPHA_BITS, &_q);
		cout << "GL_ALPHA_BITS: " << _q << endl;

		glGetIntegerv(GL_DEPTH_BITS, &_q);
		cout << "GL_DEPTH_BITS: " << _q << endl;

		glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &_q);
		cout << "GL_MAX_VIEWPORT_DIMS: " << _q << endl;

		glGetIntegerv(GL_AUX_BUFFERS, &_q);
		cout << "GL_AUX_BUFFERS: " << _q << endl;


		const GLubyte *_string;
		_string = glGetString(GL_VENDOR);
		cout << "GL_VENDOR: " << _string << endl;
		_string = glGetString(GL_RENDERER);
		cout << "GL_RENDERER: " << _string << endl;
		_string = glGetString(GL_VERSION);
		cout << "GL_VERSION: " << _string << endl;
		//_string = glGetString(GL_EXTENSIONS);
		//cout << "GL_EXTENSIONS: " << _string << endl;
		}

		if (conf.FLIGHT.get())
			GenFlightList();	//call the list-creator // with igc filename

		GenLandscapeList();
		GenAirspaceList();
		//GenPointsList();
		DrawGLScene();
		screenshot();

		/* free the image buffer */
		free(buffer);

		/* destroy the context */
		OSMesaDestroyContext(ctx);

		osmesatime.checknow("rendering finished");
		Ende(0);
	}
#endif

#ifndef HAVE_LIBOSMESA
	// OFFRSCEEN Rendering with mesa (osmesa)
	if (conf.OSMESA.get()) {
		cerr << "Osmesa support is not compiled into this binary..." << endl;
		Ende(1);
	}
#endif



#ifndef ONLY_OSMESA
	//entering interactive mode....

	if (conf.DEBUG.get()) {
            cout << "entering interactive mode (no offscreen rendering)" << endl << flush;
        }

	if (args_info.movie_given ) {
		conf.FLYING.on();
	}


	// do this check only if DEBUG or FGLUT_CHECK is given...
// 	if (conf.DEBUG.get() || conf.FGLUT_CHECK.get()) {
		// check GLUT_VERSION... (maybe this should be tested earlier?!
		// but checking needs glutInit called before...
		int _glut_version = glutGet(GLUT_VERSION);

		if (_glut_version > 10000 && _glut_version < 20801) {
		  cout <<"    ****************************************************** " <<endl<<
		         "    **   You are using an ancient version of Freeglut.  ** " <<endl<<
		         "    **   Most likely you'll encounter problems with     **" <<endl<<
		         "    **   the menus or worse. Consider updating          **" <<endl<<
		         "    **   Freeglut to at least 2.8.1                     **" <<endl<<
		         "    ******************************************************" << endl;

		}

		if (conf.DEBUG.get()) {
    		    cout << "glutGet(GLUT_VERSION) called:" << _glut_version << endl << flush;
    		}

                // in freeglut freeglut_state.c

		// this is the case, when old-glut is used (+warning... because GLUT_VERSION not known)
		if (_glut_version == -1) {
			cout << "above warning is harmless... I just tried to glutGet(GLUT_VERSION), which is available"
				<< " in freeglut only..." << endl;
		} else {
			// OK we got an GLUT_VERSION number, but is it sure that its freeglut then?
			// maybe some strange gluts around, which have glutGet(GLUT_VERSION) ???
			conf.FGLUT.on();
			conf.fglut_version.set(_glut_version);
		}
// 	} else {
// 		if (conf.VERBOSE.get())
// 			cout << "FGLUT_CHECK skipped. Neither --debug nor FGLUT_CHECK given. Assume old glut." << endl;
// 	}


	if ( conf.DEBUG.get() || conf.VERBOSE.get() ){
		cout << "glutGet(GLUT_VERSION): " << conf.fglut_version.get() << endl;
		cout << "conf.FGLUT.get(): " << conf.FGLUT.get() << endl;
	}


	// try GLUT_STEREO if requestet by user
	if (conf.DEBUG.get()) {
            cout << "calling glutInitDisplayMode..." << endl << flush;
        }

	if (!conf.STEREO_HW.get()) {
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	} else {
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH |
				    GLUT_STEREO);
	}
	if (conf.DEBUG.get()) {
            cout << "glutInitDisplayMode called..." << endl << flush;
        }

	// now with depth buffer !

	if (conf.DEBUG.get()) {
            cout << "calling glutInitWindowSize and InitWindowPosition" << endl << flush;
        }
	glutInitWindowSize(conf.getwidth(), conf.getheight());

	// initial window position
	glutInitWindowPosition(0, 0);

	if (conf.DEBUG.get()) {
            cout << "initwindowsize and initwindowposition finished" << endl << flush;
        }

	// open the GLUT window
	char namestring[80];
	sprintf(namestring, "%s %s%s %s by %s", OGIE_PACKAGE.c_str(), OGIE_VERSION.c_str(),
		DEVELOPEMENT.c_str(), COPYRIGHT.c_str(), AUTHOR.c_str());

	if (conf.DEBUG.get()) {
            cout << "calling glutCreateWindow! :" << namestring << endl << flush;
        }

	window = glutCreateWindow(namestring);

	if (conf.DEBUG.get()) {
            cout << "glutCreateWindow called:" << window << endl << flush;
        }

        if (conf.DEBUG.get()) {
		const GLubyte *_string;
		_string = glGetString(GL_VENDOR);
		cout << "GL_VENDOR: " << _string;
		_string = glGetString(GL_RENDERER);
		cout << "  GL_RENDERER: " << _string;
		_string = glGetString(GL_VERSION);
		cout << "  GL_VERSION: " << _string << endl;
	}


	if (conf.FULLSCREEN.get())
		glutFullScreen();

	// register the rendering routine
	glutDisplayFunc(&DrawGLScene);

	glutReshapeFunc(&ReSizeGLScene);

	if (conf.GPSD.get() || conf.FLYING.get()) {
		glutIdleFunc(&IdleFunc);
	}

	// Initialize the window.
	InitGL(conf.getwidth(), conf.getheight());

	// openGL commandos erst nach erzeugung des openGL contextes (hier Glutfenster)
	if (conf.FLIGHT.get())
		GenFlightList();	//call the list-creator // with igc filename

	GenLandscapeList();
	GenAirspaceList();
	//GenPointsList();

	DrawGLScene();

	//create menus
	glutMenuStatusFunc(&menuStatus);
	GenMenu();



	//mouse and keyboard callback registration

	if (!conf.MOUSE.get()) {
		glutPassiveMotionFunc(&mouseMove);
		//glutMotionFunc(NULL);
		glutSetCursor(GLUT_CURSOR_NONE);
	}

	glutMotionFunc(&mouseClickMove);

	glutMouseFunc(&mouseClickState);



	glutKeyboardFunc(&keyPressed);
	glutSpecialFunc(&SpecialKeyPressed);


	// start main loop
	glutMainLoop();

#endif

	return 1;

}


void Ende(int e)
{

#ifndef ONLY_OSMESA
	if (window != -3992) {
		glutDestroyWindow(window);
	}
#endif

#ifndef __WIN32__
	// this tells GPLIGC that OGIE exits
	if (args_info.parent_pid_given) {
		kill(args_info.parent_pid_arg, SIGUSR1);
	}
#endif

	exit(e);
}
