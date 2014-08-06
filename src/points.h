/*
 * $Id: points.h 3 2014-07-31 09:59:20Z kruegerh $
 * (c) 2002-2013 Hannes Krueger
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
--------------------------------------------------------------------------------

DESCRIPTION:
class for points (lifts, waypoints, etc)

--------------------------------------------------------------------------------
*/

#ifndef  POINTS_H
#define  POINTS_H   "$Id: points.h 3 2014-07-31 09:59:20Z kruegerh $"

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
