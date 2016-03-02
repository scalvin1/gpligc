/*
 * (c) 2002-2016 Hannes Krueger
 * This file is part of the GPLIGC/ogie package
 */

#ifndef LANDSCAPE_H
#define LANDSCAPE_H  1

#include <string>
#include <vector>

#include "projection.h"
#include "oconfig.h"


inline int runden(double a){return ((int)(a + 0.5));}

class Landscape {
      public:
	int readDEM (string filename, int rows_lat, int colums_lon, double demlatmin,
		      double demlatmax, double demlonmin, double demlonmax, bool FAST = false);

	Landscape();
	~Landscape ();
	void setgltrianglestrip (bool _WIRE, int _colmapnumber, int _colmapnr2, bool BW, bool _TEXMAP= false);
	void setgltrianglestrip_tex (int _colmapnr, int _colmapnr2, bool BW);
	void shadecolor (float*, int, int, int);
	void setcenter_lat (double _lat) {center_lat = _lat;}
	void setcenter_lon (double _lon) {center_lon = _lon;}
	void setmin_lon (double _lon) {min_lon = _lon;	}
	void setmax_lon (double _lon) {max_lon = _lon;}
	void setmin_lat (double _lat) {min_lat = _lat;}
	void setmax_lat (double _lat) {max_lat = _lat;}

	double getmin_lon (void) {return min_lon;}
	double getmax_lon (void) {return max_lon;}
	double getmin_lat (void) {return min_lat;}
	double getmax_lat (void) {return max_lat;}

	void setgrid_lat (double _lat) {grid_lat = _lat;}
	void setgrid_lon (double _lon) {grid_lon = _lon;	}
	void setdownscalefactor(int _d) {downscalefactor=_d;}
//	void setupscalefactor(int _up) {upscalefactor=_up;}

	int getsteps_lat (void) const {return steps_lat;}
	int getsteps_lon (void) const {return steps_lon; }

	//int getnoftriangles (void);

	short int getmax_h(void) const {return max_h;}
	void setmax_h(int m) {max_h = m;}
	short int getmin_h(void) const {return min_h;}
	void setmin_h(int m) {min_h= m;}

	void set_proj_pointer(Projection *p) { proj_pointer = p;}
	void set_conf_pointer(Config *c) { conf_pointer = c;}
	void set_sealevel(double sl) {sealevel = sl;}
	void set_sealevel2(double sl) {sealevel2 = sl;}
	void set_sealevel3(double sl) {sealevel3 = sl;}

   protected:

	bool CLEARFLAG;
	bool CLEARFLAG_SURFACE;

	void getlandcolor(float *, short int, int );
	double center_lat;
	double center_lon;

	// borders for the piece of terrain to be displayed
	double min_lon;
	double max_lon;
	double min_lat;
	double max_lat;

	short int **surface_h;  	// -22222 means this is watersurface at NN

	short int max_h;
	short int min_h;


        // maybe these should come from conf     ?
	double sealevel;  	//this is the border between the upper and lower colormap
	double sealevel2;	 //if this is set (!= -22222) then the blue ocean surface is set to this alt
			// if set to "0" water will be plotted, where the water-flag is set in dem
	double sealevel3; 	// if this is set (!= -22222) then a transparent blue surface will be plottet

	float **surface_x;
	float **surface_y;
	float ***normalvectors;
	int steps_lat;
	int steps_lon;

	double grid_lat;
	double grid_lon;

	int downscalefactor;
//	int upscalefactor;

	float tex_top_km;
	float tex_bottom_km;
	float tex_right_bottom_km;
	float tex_right_top_km;
	float tex_left_top_km;
	float tex_left_bottom_km;

	void vectorproduct (double a1, double a2, double a3, double b1, double b2, double b3, double &c1, double &c2,
			    double &c3);
	void norm (double &a1, double &a2, double &a3);

	Projection *proj_pointer;		//the projection-class pointer
	Config *conf_pointer;		// config-pointer

};

#endif
