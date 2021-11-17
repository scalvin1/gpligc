/*
 * (c) 2002-2021 Hannes Krueger
 * This file is part of the GPLIGC/ogie package
 */

#ifndef GLEXPLORER_H
#define GLEXPLORER_H  1

#include <stdio.h>
#include <jpeglib.h>
#include "cmdline.h"
#include "../config.h"


// CMDLINE_PARSER_PACKAGE is set to OGIE in gengetopts cmdline.h (via cmdline.ggo)
// uswed because PACKAGE from autotools is gpligc
#ifdef CMDLINE_PARSER_PACKAGE
const std::string OGIE_PACKAGE = CMDLINE_PARSER_PACKAGE ;
#else
const std::string OGIE_PACKAGE = "OGIE" ;
#endif

// this comes from ..config.h (autoconf)
// the package version
#ifdef VERSION
const std::string OGIE_VERSION = VERSION ;
#endif

// DEVELOPEMENT and WEB2 can be erased, once all occurances checked and removed
const std::string DEVELOPEMENT = "";
const std::string COPYRIGHT = "(c) 2002-2021";
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
