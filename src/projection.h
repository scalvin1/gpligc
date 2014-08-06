/*
 * $Id: projection.h 3 2014-07-31 09:59:20Z kruegerh $
 * (c) 2002-2013 Hannes Krueger
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


#ifndef PROJECTION_H
#define PROJECTION_H   "$Id: projection.h 3 2014-07-31 09:59:20Z kruegerh $"


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
