/*
 * (c) 2002-2014 Hannes Krueger
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

#ifndef MAP_H
#define MAP_H  1
// "$Id: map.h 3 2014-07-31 09:59:20Z kruegerh $"

#ifndef __OSX__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif


#include <string>

#include "projection.h"
#include "oglexfunc.h"
#include "oconfig.h"

using namespace std;

class Map {

      public:
	void readMap (string  filename, int height, int width);
	void readMap_jpeg (string filename, int height, int width);



	void settop (float _top) { top = _top; }
	void setbottom (float _bottom) { bottom = _bottom; }

	void setright (float _right) { right = _right; }
	void setleft (float _left) { left = _left; }

	void setcenter_lat (double _lat) { center_lat = _lat; }
	void setcenter_lon (double _lon) { center_lon = _lon; }

	unsigned char *getMap (void) const { return map; }
	int getHeight (void) const { return height; }
	int getWidth (void) const { return width; }
	void set_proj_pointer (Projection * p) { proj_pointer = p; }
	void set_conf_pointer (Config * c) { conf_pointer = c;}

	void grayscale_map (void);

	void map_to_colBuffer(int x, int y, int dx, int dy);

	void setglmap (double _altitude_of_map, bool COMPRESSION, bool _MODULATE, bool BW, bool TEXCOOR=true);
	~Map ();

      protected:


	float top;
	float right;
	float left;
	float bottom;

	double center_lat;
	double center_lon;

	string file; 			// only used for information output...

	int width;
	int height;

	GLubyte *map;

	Projection *proj_pointer;	//the projection-class pointer
	Config *conf_pointer;		//pointer to config


};

#endif
