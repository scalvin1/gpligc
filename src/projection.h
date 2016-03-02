/*
 * (c) 2002-2016 Hannes Krueger
 * This file is part of the GPLIGC/ogie package
 */


#ifndef PROJECTION_H
#define PROJECTION_H  1


//definitions of usable projections:

const int MERCATOR_MAX_LAT = 80;

// cylindrical "platt" projection
const unsigned int  PROJ_CYL_PLATT = 1;
const unsigned int  PROJ_CYL_MERCATOR = 2;

// laengentreu auf meridianen
const unsigned int PROJ_CYL_NO1 = 3;

// laengentreu auf meridianen und breitenkreisen (old default)
const unsigned int  PROJ_PSEUDO_CYL_NO1 = 4;


class Projection {

	public:
		Projection();
		void set_center_lat (float lat) {center_lat = lat;}
		void set_center_lon (float lon) {center_lon = lon;}
		float get_center_lat (void) const {return center_lat;}
		float get_center_lon (void) const {return center_lon;}

		void set_projection (unsigned int p) {projection = p;}
		unsigned int get_projection (void) const {return projection;}

		void get_xy (float, float, float&, float&);
		void get_latlon (float, float, float&, float&);


	protected:
		float center_lat;
		float center_lon;

		unsigned int projection;

		float sphere_radius;
		float degree_dist;

};

#endif
