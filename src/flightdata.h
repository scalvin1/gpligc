/*
 * (c) 2002-2016 Hannes Krueger
 * This file is part of the GPLIGC/ogie package
 */

#ifndef  FLIGHTDATA_H
#define  FLIGHTDATA_H  1

#include <vector>
#include <string>
#include <sstream>

#ifndef __OSX__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif

#include "oconfig.h"
#include "oglexfunc.h"
#include "projection.h"
#include "config.h"

#ifdef HAVE_LIBGPS
#include "libgpsmm.h"
#else
#define DEFAULT_GPSD_PORT "2497"
#endif


class Flightdata {

	public:

	Flightdata();
	~Flightdata();

	int readIGCFile (string, bool USE_V=false);	//read igc datafile

	void initgps(const char* gpsdserver="localhost", const char* port=DEFAULT_GPSD_PORT);
	string serverstring;

	void readGNDElevation (Config * _config) {get_gnd_elevations(&gnd_elevation, &lat, &lon, _config);}
	double getx (int _i) const {return x[_i];}
	double gety (int _i) const {return y[_i];}
	double getz (int _i) const {return z[_i];}

	double getmarkerx () const {return (n!=0) ? x[marker_pos] : 0;}
	double getmarkery () const {return (n!=0) ? y[marker_pos] : 0;}
	double getmarkerz () const {return (n!=0) ? z[marker_pos] : 0;}

	void set_proj_pointer(Projection *p) { proj_pointer = p;}
	void set_conf_pointer(Config *c) { conf_pointer = c;}

	int querygps(void);

	void usegps(void);

	int getstartelevation(bool GPS);

	double getxcenter () const {return xcenter;}
	void setxcenter(double x) {xcenter =x;}

	double getycenter () const {return ycenter;}
	void setycenter(double y) {ycenter =y;}

	double getzcenter () const {return zcenter;}
	void setzcenter(double z) {zcenter =z;}

	double getxmax () const {return xmax;}
	void setxmax(double x) {xmax =x;}

	double getxmin () const {return xmin;	}
	void setxmin(double x) {xmin =x;}

	double getymax () const {return ymax;}
	void setymax(double y) {ymax =y;}

	double getymin () const {return ymin;}
	void setymin(double y) {ymin =y;}

	double getzmax () const {return zmax;	}
	void setzmax(double z) {zmax =z;}

	double getzmin () const {return zmin;}
	void setzmin(double z) {zmin =z;}

	double getcenter_lat () const {return center_lat;}
	void setcenter_lat(double l) {center_lat =l;}

	double getcenter_lon () const {return center_lon;}
	void setcenter_lon(double l) {center_lon =l;}

	double getlatmin () const {return latmin;}
	void setlatmin(double l) {latmin =l;}

	double getlatmax () const {return latmax;}
	void setlatmax(double l) {latmax =l;}

	double getlonmin () const {return lonmin;}
	void setlonmin(double l) {lonmin =l;}

	int getcycles () const {return cycles;}
	void setcycles(int c) {cycles=c;}

	double getlonmax () const {return lonmax;}
	void setlonmax(double l) { lonmax =l;}

	int getn () const {return n;}
	void setglEvaluator (void);

	int getmarker(void) const {return int(marker_pos);}
	void setmarkerpos(int _pos);

	void setmarkertime(string _time);

	void forward_marker (void);
	void fforward_marker(void);
	void backward_marker(void);
	void fbackward_marker(void);
	void marker_head(void);

	void draw_marker(int, float, bool);
	void draw_marker_ortho(void);

	//double getdectime(void) const {return dectime[marker_pos];}
	string gettime(void) const {return time[marker_pos];}
	double getalt(void) const {return alt[marker_pos];}
	double getlat(void) const {return lat[marker_pos];}
	double getlon(void) const {return lon[marker_pos];}

	double getgpsalt(void) const {return gpsalt[marker_pos];}
	int getgndelevation(void) const {return gnd_elevation[marker_pos];}

	double getsatused (void) const {return satused[marker_pos];}
	double getsatvis (void) const {return satvis[marker_pos];}
	double getgpsmode (void) const {return gpsmode[marker_pos];}
	int getgpsinterruptions(void) const {return gps_interruptions;}
	int getvalidfix(void) const {return consecutive_valid_fixes;}


	// estimated errors climb speed vertical horizontal
	double getepc (void) const {return epc[marker_pos];}
	double geteps (void) const {return eps[marker_pos];}
	double getepv (void) const {return epv[marker_pos];}
	double geteph (void) const {return eph[marker_pos];}

	double getvario(void);
	double getgpsvario(void);
	double getintvario(int);
	double getgpsintvario(int);
	double getspeed(void);

	void draw_linestrip(int _offset,bool BW, float _r_up ,float _g_up ,float _b_up ,float _r_down ,float _g_down, float _b_down);
	// another draw linestrip...
	void draw_curtain(int _offset, bool BW, float, float, float, float);
	void draw_shadow(int _offset, bool BW, float _width, float _r = 0.0, float _g = 0.0, float _b = 0.0);
	void draw_shadow_terrain(bool BW, float _width, float _r = 0.0, float _g = 0.0, float _b = 0.0);

	protected:
	int n;			//number of data points
	double xmin;
	double xmax;
	double xcenter;
	double ymin;
	double ymax;
	double ycenter;
	double zmin;
	double zmax;
	double zcenter;

	double center_lat;
	double center_lon;
	double center_alt;
	double center_gps;
	double latmax;
	double latmin;
	double lonmax;
	double lonmin;
	double altmax;
	double altmin;
	double gpsmax;
	double gpsmin;

	vector < float > x;	//km from center
	vector < float > y;
	vector < float > z;	//height in km
	vector < float > lat;
	vector < float > lon;
	vector < float > alt;	//height in m
	vector < float > gpsalt;	// -"-
	vector <int> gnd_elevation;  // ground elevation at position
	vector < float > dectime;
	vector < string > time;

	vector <int> satused;
	vector <int> satvis;
	vector <int> gpsmode;
	int oldgpstime;
	int consecutive_valid_fixes;
	int gps_interruptions;

	vector < float > speed;
	vector < float > eps;
	vector < float > vario;
	vector < float > epc;

	vector < float > epv;
	vector < float > eph;

        float speedmax;
        float speedmin;
        float variomax;
        float variomin;

	//name fdatafile
	string filename;

	Projection *proj_pointer;	//the projection-class pointer
	Config *conf_pointer;		// config-pointer

	int marker_pos;
	int cycles;

#ifdef HAVE_LIBGPS
#if GPSD_API_MAJOR_VERSION == 5
	gpsmm *gps; //("localhost", DEFAULT_GPSD_PORT);
	struct gps_data_t *gpsdata;
//	struct gps_fix_t *gpsfix;
#endif
#endif


};

#endif
