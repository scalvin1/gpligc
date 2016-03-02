/*
 * (c) 2002-2016 Hannes Krueger
 * This file is part of the GPLIGC/ogie package
 */

#ifndef AIRSPC_H
#define AIRSPC_H  1

#include<vector>
#include<string>

#include "oconfig.h"
#include "projection.h"


class Airspace {
	public:

	Airspace(float _center_lat, float _center_lon, float _latmax, float _latmin, float _lonmax, float _lonmin);
	Airspace() ;

	void initAirspace(float _center_lat, float _center_lon, float _latmax, float _latmin, float _lonmax, float _lonmin);

	void drawAirspace(bool BW);

	void readOpenAirFile(string);

	void setConfig(Config* _c) {conf= _c;};
	void set_proj_pointer(Projection *p) { proj = p;}

	protected:

	Config *conf;
	Projection *proj;		//the projection-class pointer

	float center_lat;
	float center_lon;

	float latmax;
	float lonmax;
	float latmin;
	float lonmin;

	float coor2dec(char*, char*);

	vector<int> lower;		// altitudes in m
	vector<string>lower_unit;	// can be MSL, GND or STD
	vector<int> upper;
	vector<string>upper_unit;
	//int n;
	vector<vector<float> >xcoor;
	vector<vector<float> >ycoor;
	vector<vector<float> >lat;
	vector<vector<float> >lon;
	vector<string> type;
	vector<string> name;
	vector<string> direction;	// only for testing
};

#endif




