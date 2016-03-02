/*
 * (c) 2002-2016 Hannes Krueger
 * This file is part of the GPLIGC/ogie package
 */

/*
DESCRIPTION:
class for points of interest (lifts, waypoints, etc)
*/

#ifndef  POINTS_H
#define  POINTS_H  1

#include <vector>
#include <string>

#ifndef __OSX__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif

#include "oconfig.h"
#include "oglexfunc.h"
#include "projection.h"
#include "ego.h"


class Points {

      public:
	Points();
	int readLiftsFile (string);
	int readWaypointsFile (string);

	void set_proj_pointer(Projection *p) { proj_pointer = p;}
	void set_conf_pointer(Config *c) { conf_pointer = c;}

	void drawLifts(bool BW);
	void drawLiftsTxt(bool BW, Ego *egopointer);
	void drawWaypoints(bool BW);
	void drawWaypointsTxt(bool BW, Ego *egopointer);
	void setDegRanges(float _latmax, float _latmin, float _lonmax, float _lonmin) {
		    latmax=_latmax;latmin=_latmin;lonmax=_lonmax;lonmin=_lonmin; }


      protected:
	int n;			//number of data points

	vector < float >x;	//km from center
	vector < float >y;
	vector < float >z;	//height in km
	vector < float >lat;
	vector < float >lon;
	vector < float >alt;	//height in m

	vector < float >var;
	vector < float >ivar;

	vector <float>  dectime;
	vector <string> fname;
	vector <string> date;

	vector <string> describtion;
	vector <string> name;
	vector < string > symbol;

	//double center_lat;
	//double center_lon, center_alt;
	double latmax, lonmax, altmax, altmin, latmin, lonmin;// zmax,zmin,xmax,xmin,ymax,ymin,xcenter,ycenter,zcenter;
        vector < float > vario;

	char *filename;		//name fdatafile

	Projection *proj_pointer;		//the projection-class pointer
	Config *conf_pointer;		// config-pointer

};

#endif
