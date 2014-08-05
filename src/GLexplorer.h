/*
 * $Id: GLexplorer.h 3 2014-07-31 09:59:20Z kruegerh $
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

#ifndef GLEXPLORER_H
#define GLEXPLORER_H    "$Id: GLexplorer.h 3 2014-07-31 09:59:20Z kruegerh $"

#include <stdio.h>
#include <jpeglib.h>
#include "cmdline.h"


// CMDLINE_PARSER_PACKAGE and CMDLINE_PARSER_VERSION taken from gengetopts cmdline.h
#ifdef CMDLINE_PARSER_PACKAGE
const std::string OGIE_PACKAGE = CMDLINE_PARSER_PACKAGE ;
#else
const std::string OGIE_PACKAGE = "OGIE" ;
#endif

#ifdef CMDLINE_PARSER_VERSION
const std::string OGIE_VERSION = CMDLINE_PARSER_VERSION ;
#else
const std::string OGIE_VERSION = "?.?" ;
#endif


const std::string DEVELOPEMENT = "";
const std::string COPYRIGHT = "(c) 2002-2014";
const std::string AUTHOR = "Hannes Krueger";
const std::string MAIL =  "<Hannes.Krueger@gmail.com>";
const std::string WEB2 = ""; //"http://freshmeat.net/projects/gpligc";
const std::string WEB ="http://gpligc.sf.net";

const int ESCAPE = 27;			//ASCII ESCAPE
const double PI_180 = 0.017453292;		//Pi divided by 180
const double DEGDIST = 111.1949266;	//1 in km on surface of the earth



extern "C" {
void write_JPEG_file (int image_width, int image_height, int quality,
                      char *jpegFileName, JSAMPLE *image_buffer);
unsigned char* read_jpg(int* width, int* height, char *filename); //, unsigned char *map);
void copy_resample_image_buffer (unsigned char* destination, unsigned char* source,
	int destination_x, int destination_y, int source_x, int source_y,
	int destination_width, int destination_height, int source_width, int source_height,
	int source_dimw, int destination_dimw);
}

void setinitpos(void);
void DrawInfo(void);

void screenshot();
void screenshot_rgb();
void screenshot_jpeg();		//Max's cool jpeg screenshot function
void InitGL (int , int) ;

void IdleFunc(void);
void joystick(unsigned int, int, int, int);


void update_locked_pos(void);
void update_diff_pos(void);

void GenFlightList(void);
void GenLandscapeList(void);
void GenAirspaceList(void);
void GenPointsList(void);

void Ende(int);


#endif
