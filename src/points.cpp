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

#include "points.h"
#include "GLexplorer.h"
#include "oglexfunc.h"
#include "ego.h"
#include "projection.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <string>


#ifndef __OSX__
#include <GL/glu.h>
#include <GL/glut.h>
#else
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#endif


//const std::string rcsid_points_cpp =
//    "$Id: points.cpp 3 2014-07-31 09:59:20Z kruegerh $";
//const std::string rcsid_points_h = POINTS_H;


Points::Points()
{
n = 0;
}



int
 Points::readLiftsFile(string _filename)
{
	Timecheck readigc("ReadLifts",conf_pointer);
	ifstream liftsfile;
	liftsfile.open(_filename.c_str());

	if (!liftsfile) { cerr << "error opening :" << _filename << endl; exit (1);}

	n = 0;
	x.clear();
	y.clear();
	z.clear();
	dectime.clear();
	date.clear();
	fname.clear();
	alt.clear();
	lat.clear();
	lon.clear();
	int line_counter=0;
	int line_valid=0;
	char zeile[180];
	char d[20];
	char f[100];

	int _alt;
	float _lat, _lon,_dectime,_var,_ivar;//_latsum=0,_lonsum=0;

	while (liftsfile) {
		liftsfile.getline(zeile, 180, '\n');
		line_counter++;
		if (sscanf(zeile,
			       "%f %f %d %f %f %f %s %s",
			       &_lat, &_lon, &_alt,  &_ivar, &_var,&_dectime, d, f) != 8) {

				       if(conf_pointer->VERBOSE.get()){
					       cout << "error parsing liftsfile line: " << zeile << endl;
				       }
		} else {

		if (conf_pointer->DEBUG.get()) {
			cout << "lat:" << _lat << " lon:" << _lon << " alt:" << _alt << " dect:" << _dectime << " ivar:" << _ivar << " var:" << _var << " date:" << d << " file:" << f << endl;
		}
		line_valid++;
		n++;
		dectime.push_back(_dectime);
		alt.push_back(_alt);
		var.push_back(_var);
		ivar.push_back(_ivar);
		lat.push_back(_lat);
		//_latsum += _lat;
		//_lonsum += _lon;
		lon.push_back(_lon);
		date.push_back((string)d);
		fname.push_back((string)f);
		}

	}

	liftsfile.close();

	if (conf_pointer->VERBOSE.get()) {
		cout << line_counter << " lines of lifts-file read" << endl
		<< " valid lines: " << line_valid << endl;
	}

/*
	center_lat = _latsum / (double) n;	//startwerte
	center_lon = _lonsum / (double) n;

	//startwerte fuer maxima/minima
	latmax = center_lat;
	latmin = center_lat;
	lonmin = center_lon;
	lonmax = center_lon;
	altmax = center_alt;
	altmin = center_alt;

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
	}

	//real centers
	center_lat = (latmin + latmax) / 2;
	center_lon = (lonmin + lonmax) / 2;
	center_alt = (altmin + altmax) / 2;
*/
	//proj_pointer->set_center

	//cout << "test" << endl << flush;
	//cout << proj_pointer->get_center_lat() << endl << flush;

	//calculate x,y and z values    (now using the projection object)!
	for (int _q = 0; _q < n; _q++) {
		//cout << "_q" << _q << endl <<flush;
		//cout << "lat:" <<lat[_q] <<" lon:"<< lon[_q] << endl << flush;

		float _ykmdist, _xkmdist;
		proj_pointer->get_xy(lat[_q], lon[_q], _xkmdist, _ykmdist);

		x.push_back(-_xkmdist);
		y.push_back(-_ykmdist);
		//z is altitude in km
		z.push_back(alt[_q] / 1000.0);

	}

	// WOFÜR
	/*
	// startwerte fuer xyz max und minima
	xmin = 0;
	ymin = 0;
	zmin = center_alt / 1000.0;
	xmax = 0;
	ymax = 0;
	zmax = center_alt / 1000.0;

	for (int _i = 0; _i < n; _i++) {
		if (x[_i] < xmin) {
			xmin = x[_i];
		}
		if (x[_i] > xmax) {
			xmax = x[_i];
		}
		if (y[_i] < ymin) {
			ymin = y[_i];
		}
		if (y[_i] > ymax) {
			ymax = y[_i];
		}
		if (z[_i] < zmin) {
			zmin = z[_i];
		}
		if (z[_i] > zmax) {
			zmax = z[_i];
		}
	}

	xcenter = (xmin + xmax) / 2;
	ycenter = (ymin + ymax) / 2;
	zcenter = (zmin + zmax) / 2;
	*/

	readigc.checknow("readigc end");
	return 1;
}


// stupidly cloned from lifts
int
 Points::readWaypointsFile(string _filename)
{
	Timecheck readigc("ReadWaypoints",conf_pointer);
	ifstream liftsfile;
	liftsfile.open(_filename.c_str());

	if (!liftsfile) { cerr << "error opening :" << _filename << endl; exit (1);}

	n = 0;
	x.clear();
	y.clear();
	z.clear();
	dectime.clear();
	date.clear();
	fname.clear();
	symbol.clear();
	name.clear();
	describtion.clear();
	alt.clear();
	lat.clear();
	lon.clear();
	int line_counter=0;
	int line_valid=0;
	char zeile[180];
	char d[100];
	char f[100];
	char g[100];

	int _alt;
	float _lat, _lon;//_latsum=0,_lonsum=0;

	while (liftsfile) {
		liftsfile.getline(zeile, 180, '\n');
		line_counter++;

		// this is changed (from lifts)
		// wpt format is:
		// lat lon alt descshort namelong symbol
		if (sscanf(zeile,
			       " %f %f %d %s %s %s",
			       &_lat, &_lon, &_alt,  d, f,g) != 6) {

				       if(conf_pointer->VERBOSE.get()){
					       cout << "error parsing waypoints line: " << zeile << endl;
				       }
		} else {

		if (conf_pointer->DEBUG.get()) {
			cout << "lat:" << _lat << " lon:" << _lon << " alt:" << _alt << " name:" << d << " name:" << f << " symbol:" << g << endl;
		}
		line_valid++;
		n++;
		//dectime.push_back(0);
		alt.push_back(_alt);
		//var.push_back(0);
		//ivar.push_back(0);
		lat.push_back(_lat);
		//_latsum += _lat;
		//_lonsum += _lon;
		lon.push_back(_lon);
		//date.push_back((string)d);
		name.push_back((string)d);
		describtion.push_back((string)f);
		symbol.push_back((string)g);
		}

	}

	liftsfile.close();

	if (conf_pointer->VERBOSE.get()) {
		cout << line_counter << " lines of wpts-file read" << endl
		<< " valid lines: " << line_valid << endl;
	}

/*
	center_lat = _latsum / (double) n;	//startwerte
	center_lon = _lonsum / (double) n;

	//startwerte fuer maxima/minima
	latmax = center_lat;
	latmin = center_lat;
	lonmin = center_lon;
	lonmax = center_lon;
	altmax = center_alt;
	altmin = center_alt;

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
	}

	//real centers
	center_lat = (latmin + latmax) / 2;
	center_lon = (lonmin + lonmax) / 2;
	center_alt = (altmin + altmax) / 2;
*/
	//proj_pointer->set_center

	//cout << "test" << endl << flush;
	//cout << proj_pointer->get_center_lat() << endl << flush;

	//calculate x,y and z values    (now using the projection object)!
	for (int _q = 0; _q < n; _q++) {
		//cout << "_q" << _q << endl <<flush;
		//cout << "lat:" <<lat[_q] <<" lon:"<< lon[_q] << endl << flush;

		float _ykmdist, _xkmdist;
		proj_pointer->get_xy(lat[_q], lon[_q], _xkmdist, _ykmdist);

		x.push_back(-_xkmdist);
		y.push_back(-_ykmdist);
		//z is altitude in km
		z.push_back(alt[_q] / 1000.0);

	}
/*
	// startwerte fuer xyz max und minima
	xmin = 0;
	ymin = 0;
	zmin = center_alt / 1000.0;
	xmax = 0;
	ymax = 0;
	zmax = center_alt / 1000.0;

	for (int _i = 0; _i < n; _i++) {
		if (x[_i] < xmin) {
			xmin = x[_i];
		}
		if (x[_i] > xmax) {
			xmax = x[_i];
		}
		if (y[_i] < ymin) {
			ymin = y[_i];
		}
		if (y[_i] > ymax) {
			ymax = y[_i];
		}
		if (z[_i] < zmin) {
			zmin = z[_i];
		}
		if (z[_i] > zmax) {
			zmax = z[_i];
		}
	}

	xcenter = (xmin + xmax) / 2;
	ycenter = (ymin + ymax) / 2;
	zcenter = (zmin + zmax) / 2;
*/
	readigc.checknow("readigc end");
	return 1;
}

void Points::drawLifts(bool BW)
{
        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);

	float col[3] = {conf_pointer->lifts_color_r.get(), conf_pointer->lifts_color_g.get(), conf_pointer->lifts_color_b.get()};
	if (BW) {
		glColor3fv(graycolor(col));
	} else {
		glColor3fv(col);
	}


	for (int _q = 0; _q < n; _q++) {

		if (lat[_q]>=latmin && lat[_q]<=latmax && lon[_q]>= lonmin && lon[_q] <=lonmax) {
		glPushMatrix();
		glTranslatef(x[_q],y[_q],z[_q]);
		glScalef(1, 1, 1 / conf_pointer->z_scale.get());
		glutSolidSphere(conf_pointer->MARKER_SIZE.get()*0.25, 10,10 );
		glPopMatrix();

		if (conf_pointer->CURTAIN.get()) {
			glBegin(GL_LINES);
			glVertex3f(x[_q],y[_q],z[_q]);
			glVertex3f(x[_q],y[_q],0);
			glEnd();
		}
		}

	}
	glPopAttrib();
}

void Points::drawWaypoints(bool BW)
{
        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);

	float col[3] = {conf_pointer->waypoints_color_r.get(), conf_pointer->waypoints_color_g.get(), conf_pointer->waypoints_color_b.get()};
	if (BW) {
		glColor3fv(graycolor(col));
	} else {
		glColor3fv(col);
	}

	float offset=0;
	if (!conf_pointer->WAYPOINTS_OFFSET_TEXT_ONLY.get()) {
	 offset=conf_pointer->waypoints_offset.get() / 1000.0;
	}


	for (int _q = 0; _q < n; _q++) {

		if (lat[_q]>=latmin && lat[_q]<=latmax && lon[_q]>= lonmin && lon[_q] <=lonmax) {
		glPushMatrix();
		glTranslatef(x[_q],y[_q],z[_q]+offset);
		glScalef(1, 1, 1 / conf_pointer->z_scale.get());
		glutSolidSphere(conf_pointer->MARKER_SIZE.get()*0.25, 10,10 );
		glPopMatrix();

		if (conf_pointer->CURTAIN.get()) {
			glBegin(GL_LINES);
			glVertex3f(x[_q],y[_q],z[_q]+offset);
			glVertex3f(x[_q],y[_q],0);
			glEnd();
		}
		}

	}
	glPopAttrib();
}



void Points::drawLiftsTxt(bool BW, Ego *egopointer)
{
        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);

	float col[3] = {conf_pointer->text_color_r.get(), conf_pointer->text_color_g.get(), conf_pointer->text_color_b.get()};
	if (BW) {
		glColor3fv(graycolor(col));
	} else {
		glColor3fv(col);
	}

	for (int _q = 0; _q < n; _q++) {

		if (lat[_q]>=latmin && lat[_q]<=latmax && lon[_q]>= lonmin && lon[_q] <=lonmax) {
		glPushMatrix();
		glTranslatef(x[_q],y[_q],z[_q]+((conf_pointer->MARKER_SIZE.get()*0.25)/conf_pointer->z_scale.get()));
		glScalef(conf_pointer->pTextSize.get(),conf_pointer->pTextSize.get(), conf_pointer->pTextSize.get() / conf_pointer->z_scale.get());

		// rotate fo face the viewpoint!
		if (!conf_pointer->ORTHOVIEW.get()) {
		glRotatef(-egopointer->getspinz(), 0.0, 0.0, 1.0);
		glRotatef(360.0-egopointer->getspinx(), 1.0, 0.0, 0.0);
		}

		char txt[100];

		// option for different textstrings
		if (conf_pointer->lifts_info_mode.get() == 1)
			sprintf(txt,"%.1f",ivar[_q]*conf_pointer->getvspeed_unit_fac());
		if (conf_pointer->lifts_info_mode.get() == 2)
			sprintf(txt,"%.1f",var[_q]*conf_pointer->getvspeed_unit_fac());
		if (conf_pointer->lifts_info_mode.get() == 3)
			sprintf(txt,"%.0f",alt[_q]*conf_pointer->getalt_unit_fac());
		if (conf_pointer->lifts_info_mode.get() == 4)
			sprintf(txt,"%s",dec2humantime(dectime[_q],2).c_str());
		if (conf_pointer->lifts_info_mode.get() == 5)
			sprintf(txt,"%s",dec2humantime(dectime[_q],3).c_str());
		if (conf_pointer->lifts_info_mode.get() == 6)
			sprintf(txt,"%s",date[_q].c_str());
		if (conf_pointer->lifts_info_mode.get() == 7)
			sprintf(txt,"%s",fname[_q].c_str());

		glLineWidth(conf_pointer->text_width.get());
		for (const char *p = txt; *p; p++)
			glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
			//glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
		glPopMatrix();
		}
	}

	glLineWidth(1.0);
	glPopAttrib();
}

void Points::drawWaypointsTxt(bool BW, Ego *egopointer)
{
        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);

	float col[3] = {conf_pointer->text_color_r.get(), conf_pointer->text_color_g.get(), conf_pointer->text_color_b.get()};
	if (BW) {
		glColor3fv(graycolor(col));
	} else {
		glColor3fv(col);
	}

	 float orthooffset=0;
	 if (conf_pointer->ORTHOVIEW.get()) orthooffset=2.0;

	for (int _q = 0; _q < n; _q++) {

		if (lat[_q]>=latmin && lat[_q]<=latmax && lon[_q]>= lonmin && lon[_q] <=lonmax) {
		glPushMatrix();
		glTranslatef(x[_q],y[_q],z[_q]+conf_pointer->waypoints_offset.get()/1000.0
		    +orthooffset+((conf_pointer->MARKER_SIZE.get()*0.25)/conf_pointer->z_scale.get()));
		glScalef(conf_pointer->pTextSize.get(),conf_pointer->pTextSize.get(), conf_pointer->pTextSize.get() / conf_pointer->z_scale.get());

		// rotate fo face the viewpoint!
		if (!conf_pointer->ORTHOVIEW.get()) {
		glRotatef(-egopointer->getspinz(), 0.0, 0.0, 1.0);
		glRotatef(360.0-egopointer->getspinx(), 1.0, 0.0, 0.0);
		}

		char txt[100];

		// option for different textstrings
		if (conf_pointer->waypoints_info_mode.get() == 1)
			sprintf(txt,"%s",describtion[_q].c_str());
		if (conf_pointer->waypoints_info_mode.get() == 2)
			sprintf(txt,"%s",name[_q].c_str());
		if (conf_pointer->waypoints_info_mode.get() == 3)
			sprintf(txt,"%.0f",alt[_q]*conf_pointer->getalt_unit_fac());
		if (conf_pointer->waypoints_info_mode.get() == 4)
			sprintf(txt,"%s",symbol[_q].c_str());
		/*if (conf_pointer->waypoints_info_mode.get() == 5)
			sprintf(txt,"%s",dec2humantime(dectime[_q],3).c_str());
		if (conf_pointer->waypoints_info_mode.get() == 6)

		if (conf_pointer->waypoints_info_mode.get() == 7)
			sprintf(txt,"%s",fname[_q].c_str());*/

		glLineWidth(conf_pointer->text_width.get());
		for (const char *p = txt; *p; p++)
			glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
			//glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
		glPopMatrix();
		}
	}

	glLineWidth(1.0);
	glPopAttrib();
}
