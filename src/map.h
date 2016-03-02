/*
 * (c) 2002-2016 Hannes Krueger
 * This file is part of the GPLIGC/ogie package
 */

#ifndef MAP_H
#define MAP_H  1

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
