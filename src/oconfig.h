/*
 * (c) 2002-2016 Hannes Krueger
 * This file is part of the GPLIGC/ogie package
 */

#ifndef OCONFIG_H
#define OCONFIG_H  1

#include <vector>
#include <string>
#include <deque>

#include <sys/time.h>
#include "value.h"
#include "config.h"

#ifdef HAVE_LIBOSMESA
#define WITH_OSMESA
#endif

#ifdef HAVE_LIBGPS
#define __GPSD__
#endif


#ifdef __WIN32__
const std::string SEPERATOR = "\\";
#else
const std::string SEPERATOR = "/";
#endif

using namespace std;

class Switch {

	public:
		Switch();

		void off(void) {state = false;}
		void on(void) {state = true;}
		bool get(void) const {return state;}
		void toggle(void);

	protected:
		bool state;
};


class Config {

	public:
		Config();
		~Config();

		int    readConfigFile(void);
		void   setConfigFileName(string);
		string getConfigFileName(void) const {return configfile;}

		string getAirspaceFileName(void) const {return airspacefile;}
		void   setAirspaceFileName(string name) {airspacefile = name;}

		string getLiftsFileName(void) const {return liftsfile;}
		void   setLiftsFileName(string name) {liftsfile = name;}

		string getWaypointsFileName(void) const {return waypointsfile;}
		void   setWaypointsFileName(string name) {waypointsfile = name;}

		string getDemFileName(void) const {return demfile;}

		void   setIGCFileName(string _name) {igcfilename = _name;}
		string getIGCFileName(void) const {return igcfilename;}

		Value<int>	aov;
		Value<float> 	z_scale;
		Value<float> 	input_dem_factor;
		Value<float> 	fogdensity;
		Value<int>	ignore_elev_min;
		Value<int>	ignore_elev_max;

		// eye distance for stereo
		Value<float> 	eye_dist;

		// in milliseconds
		Value<int> 	movie_msecs;
		Value<int> 	movie_repeat_factor;
		Value<float> 	idle_auto_repeat_limit;

		Value<float>	SPINNING;

		Value<float> 	flightstrip_width;
		Value<int> 	flightstrip_mode;
		Value<int> 	flightstrip_colmap;
		Value<float> 	flightstrip_col_rup;
		Value<float> 	flightstrip_col_gup;
		Value<float> 	flightstrip_col_bup;
		Value<float>	flightstrip_col_rdown;
		Value<float>	flightstrip_col_gdown;
		Value<float>	flightstrip_col_bdown;
		Value<float>	markercol_r;
		Value<float>	markercol_g;
		Value<float>	markercol_b;

		Value<float>	MAXTRIANGLES;

		Value<int>	shadedirection;

		Value<int>	colmapnumber;
		Value<int>	colmap2number;
		Value<int>	style;        //1=new, 2 = old
		Value<int>	background;  //1=like old, 2=vertical, 3=horiz gradient
		Value<float>	background_color_1r;
		Value<float>	background_color_1g;
		Value<float>	background_color_1b;
		Value<float>	background_color_2r;
		Value<float>	background_color_2g;
		Value<float>	background_color_2b;
		Value<float>	airspace_wire_color_r;
		Value<float>	airspace_wire_color_g;
		Value<float>	airspace_wire_color_b;

		Value<float> text_color_r;
		Value<float> text_color_g;
		Value<float> text_color_b;
		Value<float> text_width;

		Value<float> lifts_color_r;
		Value<float> lifts_color_g;
		Value<float> lifts_color_b;
		Value<int>   lifts_info_mode;

		Value<float> waypoints_color_r;
		Value<float> waypoints_color_g;
		Value<float> waypoints_color_b;
		Value<float> waypoints_info_mode;
		Value<int>   waypoints_offset;

		Value<float> airspace_wire_width;
		Value<int>   info_fontsize;
		Value<float> info_fontwidth;
		Value<int>   offset;
		Value<float> orthoclipping;
		Value<int>   ActiveMapSet;
		Value<int>   jpeg_quality;
		Value<float> MARKER_SIZE;
		Value<float> pTextSize;
		Value<int>   fglut_version;	// to save freeglut version information
		Value<float> joyfac_x;
		Value<float> joyfac_y;
		Value<float> joyfac_z;

		float getborder(void) const {return border;}
		float getborder_land_lat(void) const { return border_land_lat;}
		float getborder_land_lon(void) const { return border_land_lon;}
		void  setborder_land_lat(float b) {border_land_lat = b;}
		void  setborder_land_lon(float b) {border_land_lon = b;}

		float getlat(void) const {return lat;}
		float getlon(void) const {return lon;}
		void  setlat(float l) {lat = l;}
		void  setlon(float l) {lon =l;}

		void setwidth(int w) {width = w;}
		void setinitwidth(int w) {initwidth =w;}
		void setheight(int h) {height = h;}
		void setinitheight(int h) {initheight = h;}
		int  getwidth(void) const {return width;}
		int  getheight(void) const {return height;}
		int  getinitwidth(void) const {return initwidth;}
		int  getinitheight(void) const {return initheight;}

		int  getnextframewriternumber(void) {return framewritercounter++;}

		int  getairfield_elevation(void) const {return airfield_elevation;}
		void setairfield_elevation(int _ae) {airfield_elevation=_ae;}
		void clearairfield_elevation(void) {airfield_elevation=-9999;}

		float getorthoshift(void) const {return orthoclipping.get() / 100.0;}

		int   getdownscalefactor(void) const {return downscalefactor;}
		void  setdownscalefactor(int d) {downscalefactor=d;}

		float getshadescale(void) const {return shadescale;}
		void  setshadescale(float d) {shadescale=d;}

		int   getupscalefactor(void) const {return upscalefactor;}
		void  setupscalefactor(int u) {upscalefactor=u;}

		int   getdem_rows(void) {return dem_rows;}
		int   getdem_columns(void) {return dem_columns;}
		float getdem_lat_min(void) {return dem_lat_min;}
		float getdem_lat_max(void) {return dem_lat_max;}
		float getdem_lon_min(void) {return dem_lon_min;}
		float getdem_lon_max(void) {return dem_lon_max;}

		float getdem_grid_lat(void) {return dem_grid_lat;}
		float getdem_grid_lon(void) {return dem_grid_lon;}

		short int getmax_h(void) const {return max_h;}
		void      setmax_h(int m) {max_h = m;}
		short int getmin_h(void) const {return min_h;}
		void      setmin_h(int m) {min_h= m;}

		//char* getMapFilename(int i) const {return (char*)  MapFilename[i].c_str();}
		string getMapFilename(int i) const {return MapFilename[i];}
		int    getNumberOfMaps(void) const {return NumberOfMaps;}
		int    getNumberOfMapSets(void) const {return NumberOfMapSets;}

		int set_ActiveMapSet(string);
		vector<string> getMapSetName(void) const {return MapSetName;}
		int getMapIndexLow(void); // const {return mapcuts[ActiveMapSet-1];}
		int getMapIndexHigh(void);//  const {return mapcuts[ActiveMapSet];}

		float getmap_top(int i) const {return map_top[i];}
		float getmap_bottom(int i) const {return map_bottom[i];}
		float getmap_right(int i) const {return map_right[i];}
		float getmap_left(int i) const {return map_left[i];}

		int getmap_height(int i) const {return map_height[i];}
		int getmap_width(int i) const {return map_width[i];}

		float getalt_unit_fac(void) const {return alt_unit_fac;}
		string getalt_unit_name(void) const {return alt_unit_name;}

		float getspeed_unit_fac(void) const {return speed_unit_fac;}
		string getspeed_unit_name(void) const {return speed_unit_name;}

		float getvspeed_unit_fac(void) const {return vspeed_unit_fac;}
		string getvspeed_unit_name(void) const {return vspeed_unit_name;}

		int get_marker_back(void) const {return marker_back;}
		int get_marker_ahead(void) const {return marker_ahead;}
		void set_marker_back(int _x) {marker_back = _x;}
		void set_marker_ahead(int _x) {marker_ahead = _x;}

		timeval get_time_mem(void) const {return time_mem;}
		void set_time_mem(timeval t) {time_mem = t;}

		void set_framerate(float f) {framerate = f;}
		float get_framerate(void) const {return framerate;}
		void set_fps(float f);
		float get_fps(void);

		unsigned int get_projection(void) const {return projection;}

		int get_sealevel(void) const {return sealevel;}
		int get_sealevel2(void) const {return sealevel2;}
		int get_sealevel3(void) const {return sealevel3;}

		void set_sealevel(int sl) {sealevel = sl;}
		void set_sealevel2(int sl) {sealevel2 = sl;}
		void set_sealevel3(int sl) {sealevel3 = sl;}

		void setTimeZone(int z) {timeZone = z;}
		void setTimeZoneName(char* n) {timeZoneName = n;}
		int getTimeZone(void) const {return timeZone;}
		string getTimeZoneName(void) const {return timeZoneName;}

		string get_image_format(void) const {return image_format;}
		void set_image_format(string i) {image_format = i;}

		string get_save_path(void) const {return save_path;}
		string get_basename(void) const {return basename;}

		Switch COMPRESSION;
		Switch MOVIE;			//screenshot and movie stuff....
		Switch FULLSCREEN;		//we wont start fullscreen
		Switch MOUSE;			//but with mousecursor visible in GL window
		Switch LANDSCAPE;		//do we need the green grass?
		Switch VERBOSE;			//want to have lots of mostly useless output?
		Switch QUIET;			//needed temporarily while reading the conf-file
						// indicates if --quiet is given...
		Switch DEBUG;			//even more of useless output ;-)
		Switch WIRE;			//Wireframe Model?
		Switch SHADE;			//Gouraud-Shading
		Switch NOLIGHTING;
		Switch TERRAINSHADING;		// terrain gradient shading
		Switch QUADS;			// use quads!
		Switch FLIGHT;			//with or with out flightdatafile
		Switch DEM;			//digital elevation model
		Switch AUTOREDUCE;
		Switch AIRSPACE;
		Switch AIRSPACE_WIRE;
		Switch LIFTS;
		Switch WAYPOINTS;
		Switch WAYPOINTS_OFFSET_TEXT_ONLY;
		Switch ORTHOVIEW;		//glOrtho Projection?
		Switch ORTHOONTOP;		// track in orthomode much higher, always on top
		Switch CURTAIN;
		Switch FOG;
		Switch EGG1;
		Switch EGG2;
		Switch MAP;			//texture map - map :-)
		Switch INFO;			//Info text
		Switch MARKER;			//marker
		Switch MODULATE;		//modulate dem and texture colours
		Switch MAPS_UNLIGHTED;		//used when !MODULATE
		Switch STEREO;			//stereographic rendering (2 pictures)
		Switch SWAP_STEREO;
		Switch STEREO_RG;		//red green stereo
		Switch STEREO_RB;		//red blue stereo
		Switch STEREO_HW;		 //hardware stereo (quadbuffers-shutterglasses)
		Switch BW;			// Grayscale
		Switch OFFSCREEN;		//offscreen rendering  (with glx)
		Switch OSMESA;			//offscreen with osmesa
		Switch FGLUT;			// if freeglut is available... (checked at runtime
						// 	but how???) !!! FREEGLUT is reserved by freeglut
		Switch FGLUT_CHECK;		// should I check for Freeglut-Version?
						// to avoid warning on old-glut systems
		Switch JOYSTICK;		// joystick on/off
		Switch GPSALT;			// use gps alt or baro-alt
		Switch MARKER_RANGE;		// position lock, relative to marker
		Switch FOLLOW;			// follow mode (position will follow maker)
		Switch FLYING;
		Switch FLYING_REPEAT;
		Switch FLYING_AUTO_REPEAT;
		Switch BIGENDIAN;

		// these switches control, whether an airspace is displayed or not
		Switch AIRSPACE_P;
		Switch AIRSPACE_Q;
		Switch AIRSPACE_R;
		Switch AIRSPACE_D;
		Switch AIRSPACE_C;
		Switch AIRSPACE_CTR;

		Switch GETELEV_WARN;
		Switch GETELEV_ERROR;
		Switch IGNORE_ELEV_RANGE;

		Switch GPSD;
		Switch GPSDGNDELEV;

		Value <int> AirspaceLimit;

	protected:
		string configfile;
		vector <string> configfiles;
		string airspacefile;
		string liftsfile;
		string waypointsfile;
		string demfile;
		//char* igcfilename;
		string igcfilename;

		string image_format;
		string save_path;
		string basename;

		string alt_unit_name;
		float alt_unit_fac;
		string speed_unit_name;
		float speed_unit_fac;
		string vspeed_unit_name;
		float vspeed_unit_fac;

		float border;			//in km for the green underground
		float border_land_lat;		//in km for landscape-mode
		float border_land_lon;
		int width;				//width and height for glut window
		int height;
		int initheight;
		int initwidth;

		float lat;				//center for views without igcfile
		float lon;

		int downscalefactor;     		//changable at start-time only
		int upscalefactor;

		float shadescale;


		unsigned int projection;                       //changable at start-time only

		int NumberOfMaps;           	//for texture maps
		vector<string> MapFilename;
		vector<string> MapSetName;      // Map set names

		int NumberOfMapSets;
		vector<int> mapcuts;
		vector<float> map_top;
		vector<float> map_bottom;
		vector<float> map_right;
		vector<float> map_left;
		vector<int> map_height;
		vector<int> map_width;

		float map_shift_lat;
		float map_shift_lon;

		int airfield_elevation;		//elevation of take-off location in m

		int dem_rows;
		int dem_columns;
		float dem_lat_min;
		float dem_lat_max;
		float dem_lon_min;
		float dem_lon_max;

		float dem_grid_lat;
		float dem_grid_lon;

		short int max_h;			//max and minumim height for colormap-scaling in landscape
		short int min_h;

		int framewritercounter;		//for movie-stuff

		int marker_ahead;
		int marker_back;

		timeval time_mem;
		float framerate;
		deque<float>fps;

		int sealevel;
		int sealevel2;
		int sealevel3;

		int timeZone;
		string timeZoneName;

		// dies sollte char const* sein !? geht das?
		int checkBOOL(char* val, const char* key="not given");
		int checkINT(char* val, const char* key="not given");
		int checkFLOAT(char* val, const char* key="not given");
};

#endif
