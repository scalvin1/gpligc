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

#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "oglexfunc.h"
#include "GLexplorer.h"
#include "map.h"
#include "cmdline.h"

//const std::string rcsid_oglexfunc_cpp =
//    "$Id: oglexfunc.cpp 3 2014-07-31 09:59:20Z kruegerh $";
//const std::string rcsid_oglexfunc_h = OGLEXFUNC_H;


#ifndef OPTIMIZER

void gltext(GLuint x, GLuint y, GLfloat scale, const char *txt, int _width,
	    int _height, float _r, float _g, float _b)
{
		/*
		cout << "gltext START: " << flush;
		glutReportErrors();
		cout << endl << flush;
		*/

	GLfloat font_scale = 119.05 + 33.33;	//see glutStrkeCharacter 10.3 GLUT-spec


	glMatrixMode(GL_PROJECTION);	//switch to projection matrix
	glPushMatrix();		//save projection matrix
	glLoadIdentity();	//load identity matrix

	//gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
	gluOrtho2D(0, _width, 0, _height);

	//Orthographic projection

	glMatrixMode(GL_MODELVIEW);	//switch to modelview matrix
	glPushMatrix();		//save modelview matrix
	glLoadIdentity();	//load identity matrix in modelview


	//glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);

	//was ist +1?
    	glTranslatef(x, y, +1);

	glColor3f(_r, _g, _b);
	glScalef(scale / font_scale, scale / font_scale,
		 scale / font_scale);

	//int sumwidth = 0;
	for (const char *p = txt; *p; p++)

		//#ifndef ONLY_OSMESA
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
//		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
		//sumwidth = sumwidth + glutStrokeWidth(GLUT_STROKE_MONO_ROMAN, *p);
		//#endif

	//glPopAttrib();

	//cout << "sum is = " << sumwidth << endl;

	glEnable(GL_LIGHTING);
	glDisable(GL_LINE_SMOOTH);


	glPopMatrix();		//restore modelview matrix
	glMatrixMode(GL_PROJECTION);	//switch to projrction matrix
	glPopMatrix();		// restore projection matrix
	glMatrixMode(GL_MODELVIEW);	//switch to modelview matrix again

		/*
		cout << "gltext END: " << flush;
		glutReportErrors();
		cout << endl << flush;
		*/

}

// This plots text on a transparent background...
void gltextvect(GLuint x, GLuint y, GLfloat scale, vector < string > lines, int _width, int _height, bool ortho)
{
		/*
		cout << "gltextvect START: " << flush;
		glutReportErrors();
		cout << endl << flush;
		*/

	//GLfloat font_scale = 119.05 + 33.33;	//see glutStrkeCharacter 10.3 GLUT-spec


	glMatrixMode(GL_PROJECTION);	//switch to projection matrix
	glPushMatrix();		//save projection matrix
	glLoadIdentity();	//load identity matrix

	//gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
	gluOrtho2D(0, _width, 0, _height);

	//Orthographic projection

	glMatrixMode(GL_MODELVIEW);	//switch to modelview matrix
	glPushMatrix();		//save modelview matrix
	glLoadIdentity();	//load identity matrix in modelview


  int font_height = (int) scale;

  int max_text_width = 0;
	for (int z = 0; z <= (int (lines.size() - 1)); z++) {

    int len = strlen(lines[z].c_str());
    if(len > max_text_width)
      max_text_width = len;
  }

  float x0 = x - 5.0 ;
  float y0 = (float) (_height - ((lines.size()+1)*font_height));
  float x1 = (float) (max_text_width*font_height/2.0); // 1.35 seems good as a font height/width ratio
  	// I changed it to 2.0 seems better, I have to check for possible differences between glut and freeglut
  float y1 = _height;

  // Draw an alpha blended background for the text
	//glPushAttrib(GL_ENABLE_BIT);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glEnable(GL_LINE_SMOOTH);
  glClearColor(0.0, 0.0, 0.0, 1.0);

  glBegin(GL_QUADS);
  glColor4f(0.0, 0.0, 0.0, 0.5);

  //was auch immer ich mir hier gedacht habe... 0.9 scheint OK zu sein
  float _alt=0.9 ; //=0.0; // was -0.9

//Das war wohl Zeitverschwendung
/*if (ortho)  {
	_alt = 0.9;
	cout << "orthomode..." << endl;
  } else {
	_alt= 0.9;
	cout << "nicht orthomode " << endl;
  }*/

  glVertex3f(x0, y0, _alt);  // at z=1, otherwise it is lower tham the surface in orthomode
  glVertex3f(x1, y0, _alt);
  glVertex3f(x1, y1, _alt);
  glVertex3f(x0, y1, _alt);
  glEnd();

  glDisable(GL_BLEND);

  //cerr << "lines: " << lines.size() << endl;
	for (int z = 0; z <= (int (lines.size() - 1)); z++) {

		gltext(x, _height - (font_height * (z+1)), font_height, lines[z].c_str(), _width, _height, 1.0,1.0,1.0);
	}

	//glPopAttrib();
	glEnable(GL_LIGHTING);
	glDisable(GL_LINE_SMOOTH);


	glPopMatrix();		//restore modelview matrix
	glMatrixMode(GL_PROJECTION);	//switch to projrction matrix
	glPopMatrix();		// restore projection matrix
	glMatrixMode(GL_MODELVIEW);	//switch to modelview matrix again

		/*
		cout << "gltextvect END: " << flush;
		glutReportErrors();
		cout << endl << flush;
		*/

}


//wrapper for single values
int get_1gnd_elevation(float _lat, float _lon, Config* _conf)
{
    vector<int> _alt;
    vector<float>_llat;
    vector<float>_llon;

    _llat.push_back(_lat);
    _llon.push_back(_lon);
    _alt.push_back(0);
    get_gnd_elevations(&_alt,&_llat,&_llon,_conf);
      return (_alt[0]);
}



// what should this function return in case of sea-surface ???
// altitudes above NN or above seafloor?
// for now it will return elevations above NN
// how to distinguish between voids? in srtm-3 from seamless server a void is 0!
// otherwise a void is -32...

void
get_gnd_elevations(vector < int >*_gnd_elev, vector < float >*_lat,
		   vector < float >*_lon, Config * _conf)
{

// vector _gnd_elev will be altered....

	if (_conf->DEBUG.get() ) {cout <<  "entering get_gnd_elevations  " << (*_gnd_elev).size()   << endl << flush ;}

	ifstream demfile;

	demfile.open(_conf->getDemFileName().c_str(), ios::binary);
	if (!demfile) {
		cerr <<
		    "Error opening DEM file: (in function get_gnd_elev) >"
		    << _conf->getDemFileName() << "<" << endl;
		exit(1);
	}


	int steps_lat, steps_lon, i1, i2, verschieber;
	short int h1, h2, h3, h4;
	short int watercounter;
	short int seafloorcounter;
	vector <short int> neighbours;
	float lat_frac, lon_frac;


	for (int i = 0; i < (int) _lat->size(); i++) {
		//init vars for each loop
		watercounter = 0;
		seafloorcounter = 0;

		second_try:

		if ((*_lat)[i] < _conf->getdem_lat_min() ||
			(*_lat)[i] > _conf->getdem_lat_max() ||
			(*_lon)[i] < _conf->getdem_lon_min() ||
			(*_lon)[i] > _conf->getdem_lon_max() ) {

				if ((*_lon)[i] > 180.0) {
					(*_lon)[i] -= 360.0;
					goto second_try;
				}
				if ((*_lon)[i] < -180.0) {
					(*_lon)[i] += 360.0;
					goto second_try;
				}


			cerr << "get_gnd_elevations(...): outside of DEM requested!" << endl;
			(*_gnd_elev)[i] = -9;
			(*_gnd_elev).push_back(9999);


			return;

		}



		steps_lat = (int) (1.0 +
				   (_conf->getdem_lat_max() -
				    (*_lat)[i]) / _conf->getdem_grid_lat()
		    );


		steps_lon = (int) (((*_lon)[i] -
				    _conf->getdem_lon_min()) /
				   _conf->getdem_grid_lon()
		    );


		lat_frac =
		    fabs(-steps_lat * _conf->getdem_grid_lat() +
			 _conf->getdem_lat_max() - (*_lat)[i])
		    / _conf->getdem_grid_lat();


		lon_frac =
		    fabs(-steps_lon * _conf->getdem_grid_lon() -
			 _conf->getdem_lon_min() + (*_lon)[i])
		    / _conf->getdem_grid_lon();


		//cout << "lat: " << lat_frac << "      lon: " << lon_frac << endl;

		verschieber =
		    (steps_lat * _conf->getdem_columns() + steps_lon) * 2;


		demfile.seekg(verschieber, ios::beg);



		i1 = demfile.get();
		i2 = demfile.get();

		if (_conf->BIGENDIAN.get()) {
				h1 = 256 * i1 + i2;
			} else {
				h1 = 256 * i2 + i1;
			}


		if (_conf->input_dem_factor.get() != 1.0) {
				h1 =  (int) (  (float)h1 * _conf->input_dem_factor.get()+0.5);
		}


		neighbours.push_back(h1);
		if (h1 == -9999)// || h1 <= -16384)
			watercounter++; //h1 = 0;	// in this case this is OK warum nicht 0?
		if (h1 <= -16384)
			seafloorcounter++;


		steps_lat = (int) (1.0 +
				   (_conf->getdem_lat_max() -
				    (*_lat)[i]) /
				   _conf->getdem_grid_lat());


		steps_lon = (int) (1.0 +
				   ((*_lon)[i] -
				    _conf->getdem_lon_min()) /
				   _conf->getdem_grid_lon());

		verschieber =
		    (steps_lat * _conf->getdem_columns() + steps_lon) * 2;


		demfile.seekg(verschieber, ios::beg);


		i1 = demfile.get();
		i2 = demfile.get();

		if (_conf->BIGENDIAN.get()) {
				h2 = 256 * i1 + i2;
			} else {
				h2 = 256 * i2 + i1;
			}
		//h2 = 256 * i1 + i2;

                                if (_conf->input_dem_factor.get() != 1.0) {
				h2 =  (int) (  (float)h2 * _conf->input_dem_factor.get()+0.5);
		}

		neighbours.push_back(h2);
		if (h2 == -9999)// || h2 <= -16384)
			watercounter++;//h2 = 0;
		if (h2 <= -16384)
			seafloorcounter++;


		steps_lat = (int) ((_conf->getdem_lat_max() -
				    (*_lat)[i]) /
				   _conf->getdem_grid_lat());


		steps_lon = (int) (((*_lon)[i] -
				    _conf->getdem_lon_min()) /
				   _conf->getdem_grid_lon());

		verschieber =
		    (steps_lat * _conf->getdem_columns() + steps_lon) * 2;

		demfile.seekg(verschieber, ios::beg);


		i1 = demfile.get();
		i2 = demfile.get();
		if (_conf->BIGENDIAN.get()) {
				h3 = 256 * i1 + i2;
			} else {
				h3 = 256 * i2 + i1;
			}
		//h3 = 256 * i1 + i2;

		if (_conf->input_dem_factor.get() != 1.0) {
				h3=  (int) (  (float)h3 * _conf->input_dem_factor.get()+0.5);
		}

		neighbours.push_back(h3);
		if (h3 == -9999)// || h3 <= -16384)
			watercounter++;//h3 = 0;
		if (h3 <= -16384)
			seafloorcounter++;

		steps_lat = (int) ((_conf->getdem_lat_max() -
				    (*_lat)[i]) /
				   _conf->getdem_grid_lat());


		steps_lon = (int) (1.0 +
				   ((*_lon)[i] -
				    _conf->getdem_lon_min()) /
				   _conf->getdem_grid_lon());

		verschieber =
		    (steps_lat * _conf->getdem_columns() + steps_lon) * 2;


		demfile.seekg(verschieber, ios::beg);


		i1 = demfile.get();
		i2 = demfile.get();
		if (_conf->BIGENDIAN.get()) {
				h4 = 256 * i1 + i2;
			} else {
				h4 = 256 * i2 + i1;
			}
		//h4 = 256 * i1 + i2;
                                if (_conf->input_dem_factor.get() != 1.0) {
				h4 =  (int) (  (float)h4 * _conf->input_dem_factor.get()+0.5);
		}

		neighbours.push_back(h4);
		if (h4 == -9999) // || h4 <= -16384)
			watercounter++;//h4 = 0;
		if (h4 <= -16384)
			seafloorcounter++;


		// this implements bilinear-interpolation!
		(*_gnd_elev)[i] = (int) (
			((1.0 - lon_frac) * (1.0 - lat_frac) * (double) h1 +
			   lon_frac * (1.0 - lat_frac) * (double) h2 +
			   (1.0 - lon_frac) * lat_frac * (double) h3 +
			   lat_frac * lon_frac * (double) h4) +0.5);
			   // now rounded :-) rounding error?!

		// if there is water
		if (watercounter == 1 || watercounter == 2) {
			(*_gnd_elev)[i] = 0;
		}

		if (watercounter > 2)
			(*_gnd_elev)[i] = 0; // this schould fix the seabug (David)
			// (*_gnd_elev)[i] = -22222;

		// if one or two of the neighbours are seafloor, then we set the elevation to 0 (coast)
		if (seafloorcounter > 0 ) {
			if (seafloorcounter == 1 || seafloorcounter == 2)
				(*_gnd_elev)[i] = 0;

			//if three neighbours are seafloor, then we set elevation to -16384 (sea with 0)
			// if -16383  => workaround... rounding error in interpolation?
			if (seafloorcounter == 3 || (*_gnd_elev)[i] == -16383 )

				(*_gnd_elev)[i] = -16384;
		// if 4x seafloor, then its ok
		}



		// Add warning signs! if invalid elevations read
		// this only works for data in srtm-3 from seamless server, which uses 0 for invalid data (stupid!)
		int neg_counter=0;
		if (_conf->GETELEV_WARN.get()) {
			// count neighbours equal zero!
			for(unsigned int a=0; a<= neighbours.size()-1; a++) {
				if (neighbours[a] <= 0) {
					neg_counter--;
				}
			}
			// we return a negative integer, indicating how many points are invalid
			if (neg_counter < 0) {(*_gnd_elev)[i] = neg_counter;}
		}

		// simple preliminary error-guess
		if (_conf->GETELEV_ERROR.get()) {
			int min=10000;
			int max=0;
			for(unsigned int a=0; a<= neighbours.size()-1; a++) {
				if (neighbours[a] < min)
					min = neighbours[a];
				if (neighbours[a] > max)
					max = neighbours[a];
			}

			// an error is added to the elevation array
			if (neg_counter < 0) {
					(*_gnd_elev).push_back(max);
					if (max-min != max) {
						cerr << "AAAAAAAAAAAAAAAALLLLLLLLLLLAAAAAAAAAARRRRRRRMMMMMMMM!!"<<endl;
					}

				}else{
					(*_gnd_elev).push_back(max-min);
				}
		}



	}

	demfile.close();
	if (_conf->DEBUG.get() ) {cout <<  "get_gnd_elevations finished " <<  endl << flush ;}
}


// this function turns any color into gray!
float *graycolor(float *_color)
{


	float red = _color[0];
	float green = _color[1];
	float blue = _color[2];

	for (int z = 0; z < 3; z++)
		_color[z] = 0.3 * red + 0.59 * green + 0.11 * blue;

	return _color;
}

#endif


float
great_circle_dist(float lat1, float lon1, float lat2, float lon2,
		  float radius)
{

	return (radius
		* (acos(cos(lat1 * (M_PI / 180.0)) *
			cos(lat2 * (M_PI / 180.0)) *
			cos(lon2 * (M_PI / 180.0) - lon1 * (M_PI / 180.0))
			+ sin(lat1 * (M_PI / 180.0))
			* sin(lat2 * (M_PI / 180.0)))
		)
	    );
}

#ifndef OPTIMIZER

//this is translated from GPLIGCfunctions.pm
void
go(float lat1, float lon1, float heading, float distance, float &lat2,
   float &lon2, float radius)
{

//distance needs to be in radians
	float c = ((2 * M_PI) / (radius * 2 * M_PI)) * distance;

//get a in radians
	float a = 90 - lat1;
	a *= M_PI / 180.0;

	float beta = heading * (M_PI / 180.0);

// berechnung nach Seitenkosinussatz
	float b = acos(cos(c) * cos(a)
		       + sin(c) * sin(a) * cos(beta)
	    );



	float gamma = asin((sin(c) / sin(b)) * sin(beta)
	    );

	b /= M_PI / 180.0;
	gamma /= M_PI / 180.0;

	lat2 = 90.0 - b;
	lon2 = lon1 + gamma;

	return;
}


// translation from GPLIGCfunctions.pm
float heading(float lat1, float lon1, float lat2, float lon2)
{



	float b = 90 - lat1;
	float a = 90 - lat2;


	float gamma = lon1 - lon2;
	if (gamma < 0) {
		gamma = gamma * -1.0;
	}

	float c = (acos(cos(a * (M_PI / 180.0)) * cos(b * (M_PI / 180.0))
			+
			sin(a * (M_PI / 180.0)) * sin(b * (M_PI / 180.0)) *
			cos(gamma * (M_PI / 180.0))
		   )) / (M_PI / 180.0);

	float alpha = 0;


	if (c != 0) {		// c == 0 if there is no movement ?
		float zwischenrechnung =
		    (sin(a * (M_PI / 180.0)) / sin(c * (M_PI / 180.0))) *
		    sin(gamma * (M_PI / 180.0));
		if (zwischenrechnung > 1) {
			zwischenrechnung = 1.0;
		}

		alpha = (asin(zwischenrechnung)) / (M_PI / 180.0);
	}
//if ($lon2 < $lon1) {$alpha = 360-$alpha};


//print "a= $a,   b= $b,  gamma = $gamma,   c = $c \n";

	if (lat1 >= lat2 && lon2 >= lon1) {
		alpha = 180 - alpha;
		goto endmarke;
	}			// quadrant 2

	if (lat1 >= lat2 && lon2 <= lon1) {
		alpha = 180 + alpha;
		goto endmarke;
	}			// quadrant 3

	if (lat1 <= lat2 && lon1 >= lon2) {
		alpha = 360 - alpha;
		goto endmarke;
	}			// quadrant 4


      endmarke:

	return alpha;

}

void getcolor(float *colorpointer, int colormap, float value, float min, float max)
{
// we use 2 colorscales. One beneath sealevel and one above.
// to use different colormaps (e.g. No.1 beneath, No.2 above)
// we need to choose before  getcolor is called.

// h value -22222 means: surface of ocean... (always blue)

// colormap 1
// For a start, this implements MATLABS eye-burning 'Jet'
// colormap with extension red->white for large values.
// Ripped out of GPEsim from Jan.Krueger@uni-konstanz.de

	int color = 0;
	int r = 0;
	int g = 0;
	int b = 0;

	int select = 1;
	float tmp = value - min;

	if (value == -22222.0) {
		b = 255;
		goto ende;
	}


	if (max - min == 0) {	// Avoid floating point exception (div by zero)
		//cerr << "Will be single colour. max == min in getcolor" << endl;
		//cerr << "Workaround max += 0.1";
		max += 0.1;
		//exit(1);
	}


// modified Jet colormap
	if (colormap == 1) {
		color = (int) (8 * 255 * tmp / (max - min));

		if (color < 0)
			color = 0;

		if (color >= 7 * 255) {	// Color map exception (i.e. off limits...)
			select = 10;
			goto switchlabel_1;
		}

		if (color > (6 * 255)) {	//red to white
			select = 7;
			goto switchlabel_1;
		}
		if (color > (5 * 255)) {	//magenta to red
			select = 6;
			goto switchlabel_1;
		}
		if (color > (4 * 255)) {	//blue to magenta
			select = 5;
			goto switchlabel_1;
		}
		if (color > (3 * 255)) {	//turquoise to blue
			select = 4;
			goto switchlabel_1;
		}
		if (color > (2 * 255)) {	//green to turquoise
			select = 3;
			goto switchlabel_1;
		}
		if (color > (255)) {	//yellow to green
			select = 2;
			goto switchlabel_1;
		}
		select = 1;	//red to yellow
		goto switchlabel_1;

	      switchlabel_1:
		switch (select) {
		case 10:
			{
				r = 255;	//exception to colormap jet for Psi^2<(minimum density)
				g = 255;	// white snowy peaks...
				b = 255;
				break;
			}
		case 9:	//sealevel
			{
				r = 0;
				g = 0;
				b = 255;
				break;
			}
		case 7:
			{
				r = 255;	// red to white
				g = color - 6 * 255;
				b = color - 6 * 255;
				break;
			}
		case 6:
			{
				r = 255;	//magenta to red
				g = 0;
				b = 6 * 255 - color;
				break;
			}
		case 5:
			{
				r = color - 4 * 255;	//blue to magenta
				g = 0;
				b = 255;
				break;
			}
		case 4:
			{
				r = 0;	//turquoise to blue
				g = 4 * 255 - color;
				b = 255;
				break;
			}
		case 3:
			{
				r = 0;	//green to turquoise
				g = 255;
				b = color - 2 * 255;
				break;
			}
		case 2:
			{
				r = 2 * 255 - color;	//yellow to green
				g = 255;
				b = 0;
				break;
			}
		case 1:
			{
				r = 255;	//red to yellow
				g = color;
				b = 0;
				break;
			}
		}		// switch
	}



	if (colormap == 2) {	// Atlas color map
		color = (int) (4 * 255 * tmp / (max - min));

		if (color < 0)
			color = 0;
		//      tmp = 0;

		if (color >= 3 * 255) {	// Color map exception (i.e. off limits...)
			select = 10;
			goto switchlabel_2;
		}
		if (color > (2 * 255)) {	//red to white
			select = 3;
			goto switchlabel_2;
		}
		if (color > (255)) {	//yellow to red
			select = 2;
			goto switchlabel_2;
		}
		select = 1;	//green to yellow
		goto switchlabel_2;

	      switchlabel_2:
		switch (select) {
		case 10:
			{
				r = 255;	//exception to colormap jet for Psi^2<(minimum density)
				g = 255;	// white snowy peaks...
				b = 255;
				break;
			}
		case 9:	//sealevel
			{
				r = 0;
				g = 0;
				b = 255;
				break;
			}
		case 3:
			{
				r = 255;	// red to white
				g = color - 2 * 255;
				b = color - 2 * 255;
				break;
			}
		case 2:
			{
				r = 240 + (int) (15.0 * (double) (color - 255) / 255.0);	//yellow to red
				g = 240 -
				    (int) (240.0 * (double) (color - 255) /
					   255.0);;
				b = 0;
				break;
			}
		case 1:
			{
				r = (int) (240.0 * (double) color / 255.0);	// green to yellow
				g = 180 +
				    (int) (60.0 * (double) color / 255.0);
				b = 0;
				break;
			}
		}		// switch
	}			// map2


	if (colormap == 3) {	//this is Hannes' Moon-(Color)map :)

		color = (int) ((255) * tmp / (max - min));

		if (color > (255)) {
			color = 255;
		}
		if (color < 0) {
			color = 0;
		}



		r = 255 - (255 - color);
		g = 255 - (255 - color);
		b = 255 - (255 - color);

	}


// Hannes - Colormap 4 - drk.green->yellow->red
	if (colormap == 4) {
		color =
		    (int) ((2 * 255 + 185) * tmp / (max - min));

		if (color < 0) {
			color = 0;
		}
		if (color > (2 * 255 + 185)) {
			color = 2 * 255 + 185;
		}

		if (color >= (185 + 255)) {
			select = 1;
			goto switchlabel_4;
		}

		if (color > (185)) {
			select = 2;
			goto switchlabel_4;
		}

		if (color >= 0) {
			select = 3;
			goto switchlabel_4;
		}

	      switchlabel_4:

		switch (select) {

			// yellow -> red
		case 1:
			{
				r = 255;
				g = 255 - (color - (185 + 255));
				b = 0;
				break;
			}

		case 2:	// green -> yellow
			{
				r = color - 185;
				g = 255;
				b = 0;
				break;
			}

		case 3:	// drk green -> green
			{
				r = 0;
				g = 70 + color;
				b = 0;
				break;
			}

		}


	}


// Hannes - Colormap 5 - violett -> blue -> l blue
	if (colormap == 5) {
		color = (int) ((2 * 255) * tmp / (max - min));

		if (color > (2 * 255)) {
			select = 3;
			goto switchlabel_5;
		}

		if (color < 0) {
			select = 4;
			goto switchlabel_5;
		}


		if (color >= (255)) {
			select = 1;
			goto switchlabel_5;
		}

		if (color >= 0) {
			select = 2;
			goto switchlabel_5;
		}

	      switchlabel_5:

		switch (select) {

			// d blue  -> l blue
		case 1:
			{
				r = 0;
				g = (color - 255);
				b = 255;
				break;
			}

		case 2:	// violett -> d blue
			{
				r = 255 - color;
				g = 0;
				b = 255;
				break;
			}

		case 3:	// out (upper end)
			{
				r = 0;
				g = 255;
				b = 255;
				break;
			}

		case 4:	// out lower end (magenta)
			{
				r = 255;
				g = 0;
				b = 255;
			}
		}


	}
// Hannes - Colormap 6 - killer rainbow
	if (colormap == 6) {
		color = (int) ((8 * 255) * tmp / (max - min));

		if (color > (8 * 255)) {
			color = 8 * 255;
		}
		if (color < 0) {
			color = 0;
		}


		if (color >= (7 * 255)) {
			select = 1;
			goto switchlabel_6;
		}

		if (color > (6 * 255)) {
			select = 2;
			goto switchlabel_6;
		}
		if (color > (5 * 255)) {
			select = 3;
			goto switchlabel_6;
		}
		if (color > (4 * 255)) {
			select = 4;
			goto switchlabel_6;
		}
		if (color > (3 * 255)) {
			select = 5;
			goto switchlabel_6;
		}
		if (color > (2 * 255)) {
			select = 6;
			goto switchlabel_6;
		}
		if (color > (1 * 255)) {
			select = 7;
			goto switchlabel_6;
		}
		if (color >= (0)) {
			select = 8;
			goto switchlabel_6;
		}



	      switchlabel_6:

		switch (select) {

			// red  -> white
		case 1:
			{
				r = 255;
				g = 255 - (8 * 255 - color);
				b = 255 - (8 * 255 - color);
				break;
			}

		case 2:	// magenta -> red
			{
				r = 255;
				g = 0;
				b = 7 * 255 - color;
				break;
			}

		case 3:	// blue -> magenta
			{
				r = 255 - (6 * 255 - color);
				g = 0;
				b = 255;
				break;
			}

		case 4:	// trk -> blue
			{
				r = 0;
				g = 5 * 255 - color;
				b = 255;
				break;
			}

		case 5:	// green -> trk
			{
				r = 0;
				g = 255;
				b = 255 - (4 * 255 - color);
				break;
			}


		case 6:	// yellow -> green
			{
				r = 3 * 255 - color;
				g = 255;
				b = 0;
				break;
			}

		case 7:	// red -> yellow
			{
				r = 255;
				g = 255 - (2 * 255 - color);
				b = 0;
				break;
			}

		case 8:	// black -> red
			{
				r = color;
				g = 0;
				b = 0;
				break;
			}


		}


	}


	if (colormap == 7) { // boaring all-white
		r = 255;
		g = 255;
		b = 255;
	}

// Hannes - Colormap 8 - black-red-yellow-white
	if (colormap == 8) {
		color = (int) ((3 * 255) * tmp / (max - min));

		if (color > (3 * 255)) {
			color = 3 * 255;
		}
		if (color < 0) {
			color = 0;
		}


		if (color >= (2 * 255)) {
			select = 1;
			goto switchlabel_8;
		}

		if (color > (1 * 255)) {
			select = 2;
			goto switchlabel_8;
		}
		if (color >= 0) {
			select = 3;
			goto switchlabel_8;
		}



	      switchlabel_8:

		switch (select) {

			// yellow  -> white
		case 1:
			{
				r = 255;
				g = 255; // - (4 * 255 - color);
				b = 255 - (3 * 255 - color);
				break;
			}

		case 2:	// red -> yellow
			{
				r = 255;
				g = 255 - (2*255 - color);
				b = 0;
				break;
			}

		case 3:	// black -> red
			{
				r = color;
				g = 0;
				b = 0;
				break;
			}

		}


	}



ende:
colorpointer[0] = (float) r / 255.0;
colorpointer[1] = (float) g / 255.0;
colorpointer[2] = (float) b / 255.0;
}


void HelpScreen(void) {
#ifndef ONLY_OSMESA
	vector<string> helptext;
	int font_height=  glutGet(GLUT_WINDOW_HEIGHT) / 30;

	// make shure that the text appears right and left in hw-stereo-mode
	glDrawBuffer(GL_BACK);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// the spaces are empirical...
	helptext.push_back("g,s           Move forward,backward");
	helptext.push_back("space,a        Move fast forward,backward");
	helptext.push_back("d,f           Move left,right");
	helptext.push_back("t,z           Move up,down");
	helptext.push_back("q,ESC         Exit");
	helptext.push_back("w            Fullscreen mode on/off ");
	helptext.push_back("p            Screenshot");
	helptext.push_back("m           Mouse control on/off ");
	helptext.push_back("o            toggle 2D/3D viewing mode");
	helptext.push_back("u,i           increase,decrease Flightstrip/Terrain offset ");
	helptext.push_back("+,-           increase,decrease z-axis scaling");
	helptext.push_back("F1-F4         Move Marker (fast back, back, forward, fast forward)");
	helptext.push_back("F5           Reset viewpoint");
	helptext.push_back("shift-F3,shift-F4   decrease,increase Flightstrip linewidth");
	helptext.push_back("shift-F1,shift-F2   decrease,increase Flying-Mode Framerepeat");
	helptext.push_back("shift-Q, shift-W   decrease,increase eye-distance in stereo-modes");

	// this should be clear ;-)
	//helptext.push_back("  ");
	//helptext.push_back("Right mouse button  =  Access menu  ");

	helptext.push_back("  ");
	helptext.push_back("for a full list read the manual (GPLIGC_Manual.pdf)");
	helptext.push_back(" ");
	helptext.push_back("To leave this menu press the \"ANY\"-key");

	glLineWidth(1.5);

  /*gltextvect(5, glutGet(GLUT_WINDOW_HEIGHT) - (font_height),
         font_height, helptext, glutGet(GLUT_WINDOW_WIDTH),
         glutGet(GLUT_WINDOW_HEIGHT));*/

	for (int z=0; z<=(int(helptext.size()-1)); z++) {
	gltext(5, glutGet(GLUT_WINDOW_HEIGHT) - (font_height*(z+1)),
         font_height, helptext[z].c_str(), glutGet(GLUT_WINDOW_WIDTH),
         glutGet(GLUT_WINDOW_HEIGHT));
	 }

	glLineWidth(1.0);
	glutSwapBuffers();
#endif
}



void InfoScreen(void) {
#ifndef ONLY_OSMESA
	vector<string> infotext;

                string logofile;
		vector <string> logofiles;
                bool LOGO = false;

		string homestring;

                // assemble path to logo (only in default install, or GPLIGCHOME (win32))

#ifdef __WIN32__
		char* home;
		home = getenv("GPLIGCHOME");
                //logofiles.push_back(home+"\\logo.jpg");
		//string homestring;

		if (home != NULL) {
			homestring=home;
			//logofiles.push_back(homestring + "logo.jpg");
			logofiles.push_back(homestring + "\\logo.jpg");
		}else{
			homestring = "";
		}

		logofiles.push_back("C:\\GPLIGC\\logo.jpg");
		logofiles.push_back("D:\\GPLIGC\\logo.jpg");
		logofiles.push_back("C:\\Programme\\GPLIGC\\logo.jpg");
		logofiles.push_back("C:\\Program files\\GPLIGC\\logo.jpg");
		logofiles.push_back("D:\\Programme\\GPLIGC\\logo.jpg");
		logofiles.push_back("D:\\Program files\\GPLIGC\\logo.jpg");

#else
		logofiles.push_back("/usr/local/share/gpligc/logo.jpg");
		logofiles.push_back("/usr/share/gpligc/logo.jpg");
		logofiles.push_back("/sw/usr/local/share/gpligc/logo.jpg");
		logofiles.push_back("/sw/local/share/gpligc/logo.jpg");
		logofiles.push_back("/sw/share/gpligc/logo.jpg");
		logofiles.push_back("/opt/share/gpligc/logo.jpg");
		logofiles.push_back("/opt/GPLIGC/logo.jpg");
		logofiles.push_back("/home/kruegerh/GPLIGC/GPLIGC/perl/logo.jpg");

#endif

	ifstream inlogo;

	for (unsigned i=0; i<logofiles.size();i++) {
		inlogo.clear();
		inlogo.open(logofiles[i].c_str());
		//cout << "trying to open: " << logofiles[i] << endl;
		if (!inlogo) {
			inlogo.close();
			//cfile.clear();
		} else {
			logofile = logofiles[i];
			LOGO=true;
			//if (DEBUG.get()) {
			//cout << "logofile Opened!!!: " << logofile << endl;
			//}
			break;
		}
	}


	// set font size
	int font_height=  glutGet(GLUT_WINDOW_HEIGHT) / 30;

	// make shure that the text appears right and left in hw-stereo-mode
	glDrawBuffer(GL_BACK);

	// clear to white and buffers
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// display logo if available
                if (LOGO) {
		Map logo;

		// we dont know this here... and otherwise -> segfault
		logo.set_conf_pointer(NULL);
		logo.readMap_jpeg((char*) logofile.c_str(), -1,-1);    // -1,-1 -> no rescaling is needed (not a texture!)

                                float logoratio = (float) logo.getHeight() / (float) logo.getWidth();
                                float windowratio = ((float) glutGet(GLUT_WINDOW_HEIGHT) / 2.0) / (float) glutGet(GLUT_WINDOW_WIDTH);

                                //cout << "logo read....fg" << endl << flush;
                                // logo is heigher than half window   (maximal height is windowheight/2)
                                if (logoratio > windowratio) {
			int targetheight =  (int)  (( (float) glutGet(GLUT_WINDOW_HEIGHT) / 2.0) +0.5) ;
			int targetwidth = (int) (((float)targetheight / logoratio)+0.5);
			//int xoffset =  (int) (((float)glutGet(GLUT_WINDOW_WIDTH) - (float) targetwidth) / 2.0);
			logo.map_to_colBuffer(0,0,targetwidth, targetheight);

		// logo is wider than window (maximal width is windowwidth)
		} else {
			int targetwidth =   glutGet(GLUT_WINDOW_WIDTH);
			int targetheight = (int)(((float)targetwidth * logoratio)+0.5);
			logo.map_to_colBuffer(0,0,targetwidth,targetheight);
		}
	}

	// display text
	string EX = "OGIE (openGLIGCexplorer) ";
	EX  += OGIE_VERSION;
	EX += DEVELOPEMENT;
	EX += " (build:";
	EX += __DATE__;
	EX += ")";
	infotext.push_back(EX);
                EX = COPYRIGHT;
                EX += " by ";
                EX += AUTHOR;
                EX += " ";
                EX += MAIL;
                infotext.push_back(EX);

//                 infotext.push_back(WEB2);
                infotext.push_back(WEB);
                infotext.push_back("  ");
                infotext.push_back("This program is free software: you can redistribute it and/or modify");
                infotext.push_back("it under the terms of the GNU General Public License as published by");
                infotext.push_back("the Free Software Foundation, either version 3 of the License, or");
                infotext.push_back("(at your option) any later version.");
                infotext.push_back(" ");
                infotext.push_back(" ");
                infotext.push_back("For details read the manual (GPLIGC_Manual.pdf)");

	infotext.push_back("To leave this information press the \"ANY\"-key");

	glLineWidth(1.5);
  /*gltextvect(5, glutGet(GLUT_WINDOW_HEIGHT) - (font_height),
        font_height, infotext, glutGet(GLUT_WINDOW_WIDTH),
         glutGet(GLUT_WINDOW_HEIGHT));
  */
  for (int z=0; z<=(int(infotext.size()-1)); z++) {
  gltext(5, glutGet(GLUT_WINDOW_HEIGHT) - (font_height*(z+1)),
        font_height, infotext[z].c_str(), glutGet(GLUT_WINDOW_WIDTH),
         glutGet(GLUT_WINDOW_HEIGHT));
	 }

	glLineWidth(1.0);
	glutSwapBuffers();
#endif
}



// this is taken from
// "Avoiding 16 Common OpenGL Pitfalls" by Mark J. Kilgard of Nvidia

void
glWindowPos4fMESAemulate(GLfloat x,GLfloat y,GLfloat z,GLfloat w)
{
  GLfloat fx, fy;

  /* Push current matrix mode and viewport attributes. */
  glPushAttrib(GL_TRANSFORM_BIT | GL_VIEWPORT_BIT);

    /* Setup projection parameters. */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
        glLoadIdentity();
        glDepthRange(z, z);
        glViewport((int) x - 1, (int) y - 1, 2, 2);
        /* Set the raster (window) position. */
        fx = x - (int) x;
        fy = y - (int) y;
        glRasterPos4f(fx, fy, 0.0, w);
      /* Restore matrices, viewport and matrix mode. */
      glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
  glPopAttrib();
}

void
glWindowPos2fMESAemulate(GLfloat x, GLfloat y)
{
  glWindowPos4fMESAemulate(x, y, 0, 1);
}


string
dec2humantime (float t, int n) {
	int hh = (int) t;
	float rest = t - hh;
	int mm = int(60 * rest);
	rest= 60 * rest - mm;
	int ss = int (60*rest);

	rest=60 * rest -ss;

	if (rest > 0.5) {ss++;}

	if (ss == 60) {ss=0;mm++;}

	char ret[8];

	if (n==1)
		sprintf(ret,"%02d",hh);

	if (n==2)
		sprintf(ret,"%02d:%02d",hh,mm);

	if (n==3)
		sprintf(ret,"%02d:%02d:%02d",hh,mm,ss);

	if (n==4)
		sprintf(ret,"%02d%02d%02d",hh,mm,ss);

	return ((string)ret);
}




Timecheck::Timecheck() {}

Timecheck::Timecheck(string _name, Config* _c) {

	name = _name;
	gettimeofday(&start,NULL);
	last = start;
	conf = _c;

	if (conf->DEBUG.get())
		cout << "########## " << name << " ##### started............" <<endl<<flush;

}


void Timecheck::checknow(string _here) {

	gettimeofday(&now,NULL);
	float _time = (((float) now.tv_sec - (float) start.tv_sec)
			       +
			       (((float) now.tv_usec -
				 (float) start.tv_usec) / 1000000.0));

	float _timelast = (((float) now.tv_sec - (float) last.tv_sec)
			       +
			       (((float) now.tv_usec -
				 (float) last.tv_usec) / 1000000.0));
	last = now;

	if (conf->DEBUG.get())
		cout <<"########## " << name << " ====> " <<_here << " ====> " << _time << "(" << _timelast << ")" << "secs" <<endl<<flush;



}


#endif
