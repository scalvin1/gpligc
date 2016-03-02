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

#include "map.h"
#include "projection.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <string>
#include <jpeglib.h>

#ifndef __OSX__
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#endif

#include "GLexplorer.h"


//const std::string rcsid_map_cpp =
//    "$Id: map.cpp 3 2014-07-31 09:59:20Z kruegerh $";
//const std::string rcsid_map_h = MAP_H;

void
 Map::readMap(string filename, int _width, int _height)
{

        file = filename;

	width = _width;
	height = _height;

	if (width == 0 || height == 0) {
		cerr << "Width or Height for a map-tile is not set!" <<
		    endl;
		cerr << filename << endl;
		exit(1);
	}

	int _max;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_max);

	//cout << "max is: " << _max << endl;

	if (_width > _max || _height > _max) {
		cerr << "Map tile \"" << filename <<
		    "\" ist too big. Maximum size for your openGL implementation is "
		    << _max << endl;
		exit(1);
	}

	map = new GLubyte[_width * _height * 3];

	ifstream mapfile;
	mapfile.open(filename.c_str(), ios::binary);
	if (!mapfile) {
		cerr << "Error opening Map file: >" << filename << "<" <<
		    endl;
		exit(1);
	}

	for (int nr = 0; nr < _width * _height * 3; nr++) {
		map[nr] = mapfile.get();
		//cout << map[nr] << endl;
	}
	mapfile.close();
}

void Map::readMap_jpeg(string filename, int _width, int _height)
{

	// width and height should be 0, and will be read from jpeg image, and internally rescaled (if necessary)
	// if width and height != 0 -> warning message, read from image -> rescaling
	// if width or height = -1  no rescaling!

	width = _width;
	height = _height;
	file = filename;

	bool RESCALE = true;

	// in logo conf_pointer will not be defined....
	if (conf_pointer)
	if (conf_pointer->DEBUG.get())
		cout << "readMap_jpeg(" << file << ", " << _width << ", " << _height << ")" << endl << flush; ;


	if (width == -1 && height == -1) RESCALE = false;

	if ( (width != 0 || height != 0) && (width != -1 || height != -1) ) {

		cerr <<
		    "Width or Height is specified for a jpeg-texture map..."
		    << endl;
		cerr << "This is not needed and will be ignored." << endl;
		cerr << filename << endl;
		width = 0;
		height = 0;
	}


	int _max;

	GLubyte *tmpmap;

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_max);

	if (conf_pointer)
	if (conf_pointer->DEBUG.get())
		cout << "GL_MAX_TEXTURE_SIZE: " << _max << endl;


	map = read_jpg(&width, &height, (char*) filename.c_str());	//, (unsigned char *) map);

	if (conf_pointer)
	if (conf_pointer->DEBUG.get())
		cout << file << " is " << width << "x" << height << endl;



	// AND HERE WE NEED TO CALCULATE THE NEXT POWER OF 2
	// AND CHECK IF THE SIZE ISNT ALREADY A POWER OF 2
	// THE NEXT POWER OF 2 SHOULD BE SMALLER THAN _max

	// check if the width and height of image is a power of 2
	double lbw = log((double) width) / log(2.0);
	double lbh = log((double) height) / log(2.0);

	// log of max allowed size
	double lmax = log((double) _max) / log(2.0);

	int lbw_i = (int) (lbw + 0.5);
	int lbh_i = (int) (lbh + 0.5);

	//cout << lbw_i << " " << lbh_i << endl;

	//cout << "log_2 ("<<width<<") = "<<lbw<<endl;
	//cout << "log_2 ("<<height<<") = "<<lbh<<endl;

	if (RESCALE)
	if (lbw != (double) lbw_i || lbh != (double) lbh_i || lbw > lmax || lbh > lmax) {
		cerr << "Resizing is needed for " << filename << endl;

		// calculate new width and height
		// aufrunden (+1)  abrunden -1 (eine potenz kleiner)
		int power_w = (int) lbw;	// - 1;
		int power_h = (int) lbh;	// -1;

		// bis <= max erniedrigen
		while (pow(2.0, power_w) > _max) {
			power_w--;
		}
		while (pow(2.0, power_h) > _max) {
			power_h--;
		}

		int new_width = (int) pow(2.0, power_w);
		int new_height = (int) pow(2.0, power_h);

		cout << "New width x height: " << new_width << "x" <<
		    new_height << "  (old: " << width << "x" << height << ")" << endl;

		tmpmap = new GLubyte[new_width * new_height * 3];
		copy_resample_image_buffer((unsigned char *) tmpmap,
					   (unsigned char *) map, 0, 0, 0,
					   0, new_width, new_height, width,
					   height, width, new_width);
		/*
		   copy_resample_image_buffer( unsigned char* destination, unsigned char* source,
		   int destination_x, int destination_y, int source_x, int source_y,
		   int destination_width, int destination_height, int source_width, int source_height,
		   int source_dimw, int destination_dimw);
		 */
		delete map;
		map = tmpmap;
		width = new_width;
		height = new_height;

	}


}

// _MODULATE = MAPS_UNLIGHTED (whether maps should be lighted or not, in !(not)coloured map mode
void Map::setglmap(double _altitude_of_map, bool COMPRESSION,
		   bool _MAPS_UNLIGHTED, bool BW, bool TEXCOOR)
{

	if (conf_pointer)
	if (conf_pointer->DEBUG.get())
		cout << "setglmap: " << file << endl;

	// this works much better than the internal format LUMINANCE... because of the better col->gray model
	// but its slower :(
	if (BW) {
		this->grayscale_map();
	}


	// first of all: load the texture
	if (!COMPRESSION) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R3_G3_B2, width, height,
			     0, GL_RGB, GL_UNSIGNED_BYTE, map);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_ARB,
			     width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
			     map);
	}


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);



	// we use modulate in any case!
	// The unmodulated case needs lighting aswell, therefore colormap 7 (white)
	// is used with modulation!

	if (_MAPS_UNLIGHTED) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	} else {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
			  GL_MODULATE);
	}

//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
//			  GL_MODULATE);


	if (conf_pointer)
	if (conf_pointer->DEBUG.get())
		cout << "glGetError: " << gluErrorString(glGetError()) << endl;



                // the texture coordinates dont need to be set here (in landscape mode)
	// in landscape mode they will be set in landscape glsettrianglestrip_tex...
	if (TEXCOOR) {
		float top_km, bottom_km, right_top_km, left_top_km,
		right_bottom_km, left_bottom_km;

		proj_pointer->get_xy(top, right, right_top_km, top_km);
		proj_pointer->get_xy(top, left, left_top_km, top_km);
		proj_pointer->get_xy(bottom, right, right_bottom_km, bottom_km);
		proj_pointer->get_xy(bottom, left, left_bottom_km, bottom_km);

		right_top_km *= -1.0;
		left_top_km *= -1.0;
		right_bottom_km *= -1.0;
		left_bottom_km *= -1.0;
		top_km *= -1.0;
		bottom_km *= -1.0;

		glEnable(GL_TEXTURE_2D);
		//glEnable(GL_LIGHTING);
		glDisable(GL_LIGHTING);
		//glPolygonMode(GL_FRONT, GL_FILL);
		glBegin(GL_POLYGON);
		//glColor3f(1.0,0,0);

		// unten links
		glTexCoord2f(0.0, 1.0);
		glVertex3f(left_bottom_km, bottom_km, _altitude_of_map);

		// unten rechts
		glTexCoord2f(1.0, 1.0);
		glVertex3f(right_bottom_km, bottom_km, _altitude_of_map);

		// oben rechts
		glTexCoord2f(1.0, 0.0);
		glVertex3f(right_top_km, top_km, _altitude_of_map);

		// oben links
		glTexCoord2f(0.0, 0.0);
		glVertex3f(left_top_km, top_km, _altitude_of_map);

		glEnd();

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
	}
}


void Map::grayscale_map(void)
{

	for (int _x = 0; _x <= (width * height * 3) - 3; _x += 3) {

		GLubyte col =
		    (GLubyte) ((float) map[_x] * 0.3 +
			       (float) map[_x + 1] * 0.59 +
			       (float) map[_x + 2] * 0.11);

		map[_x] = col;
		map[_x + 1] = col;
		map[_x + 2] = col;

	}

}

void Map::map_to_colBuffer(int x, int y, int dx, int dy)
{
	// this function scales the map to dx, dy
	// the put it to screen coordinates x,y

	GLubyte *tmpmap;
	GLubyte *flipmap;

	tmpmap = new GLubyte[dx * dy * 3];
	flipmap = new GLubyte[dx*dy*3];

	//cout << "vorher" << endl << flush;
	copy_resample_image_buffer((unsigned char *) tmpmap,
		              (unsigned char *) map, 0, 0, 0,
		              0, dx, dy, width,
			  height, width, dx);

	// flip up-down
	for (int z=0; z< dy; z++) {
		for (int s=0; s < dx*3 ; s++) {
			flipmap[(((dy-1)-z)*dx*3)+s] = tmpmap[(z*dx*3)+s];
		}
	}

	if (conf_pointer)
	if (conf_pointer->DEBUG.get()) {
		cout << "writing logo_image to color buffer" << endl << flush;
	}


	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glWindowPos2fMESAemulate((GLfloat) x, (GLfloat)y );
	glDrawPixels(dx,dy,  GL_RGB, GL_UNSIGNED_BYTE, flipmap);

	//free array
	delete[]tmpmap;
	delete[]flipmap;

}


Map::~Map()
{
	delete[]map;
}
