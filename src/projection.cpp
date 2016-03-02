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

#include "projection.h"

#include <string>
#include <iostream>
#include <cmath>
#include <cstdlib>

//const std::string rcsid_projection_cpp =
//    "$Id: projection.cpp 3 2014-07-31 09:59:20Z kruegerh $";
//const std::string rcsid_projection_h = PROJECTION_H;

Projection::Projection()
{

	sphere_radius = 6371.0;

	// this is 1 in km on a great circle
	degree_dist = (2 * M_PI * sphere_radius) / 360.0;

	// this is the default
	projection = PROJ_PSEUDO_CYL_NO1;

	// 9999 is the "not set"-flag
	center_lat = 9999;
	center_lon = 9999;
}


void
Projection::get_xy(float lat, float lon, float &x, float &y)
{
	//std::cout << "ENTERING proj::get_xy" << std::flush;
	//std::cout << "ENTERING proj::get_xy" << center_lat << " " << center_lon << std::endl << std::flush;
	if (center_lat == 9999 || center_lon == 9999) {
		std::
		    cerr << "Projection class: get_xy: center isn't set!"
		    << std::endl;
		return;
	}

	if (lat > 90)
		lat = 90;
	if (lat < -90)
		lat = -90;

	// This pseudo cylindrical projection has true lengths on meridians and latitude circles...
	if (projection == PROJ_PSEUDO_CYL_NO1) {

		float latdiff = center_lat - lat;
		float londiff = center_lon - lon;

		y = latdiff * degree_dist;
		x = londiff * degree_dist * cos((M_PI / 180.0) * lat);

		return;
	}

	// This cylindrical projection has true lengths on meridian only
	if (projection == PROJ_CYL_NO1) {

		y = (center_lat - lat) * degree_dist;
		x = (center_lon - lon) * degree_dist;

		return;
	}

	// platt(?) (Flaechentreu)
	if (projection == PROJ_CYL_PLATT) {

		float aequatdist_center =
		    sin(center_lat * (M_PI / 180.0)) * sphere_radius;
		float aequatdist_point =
		    sin(lat * (M_PI / 180.0)) * sphere_radius;
		y = aequatdist_center - aequatdist_point;
		x = (center_lon - lon) * degree_dist;
		return;
	}


	if (projection == PROJ_CYL_MERCATOR) {

		x = sphere_radius * (center_lon * (M_PI / 180.0) -
				     lon * (M_PI / 180.0));
		//x = (center_lon - lon) * degree_dist;
		//std::cout << x << std::endl;
		float sign = 1.0;
		if (lat < 0)
			sign = -1.0;
		float aequatdist_point =
		    -sign * sphere_radius *
		    log(tan
			((M_PI / 4.0) +
			 ((fabs(lat) * (M_PI / 180.0)) / 2.0)));
		float sign_c = 1.0;
		if (center_lat < 0)
			sign_c = -1.0;
		float aequatdist_center =
		    -sign_c * sphere_radius *
		    log(tan
			((M_PI / 4.0) +
			 ((fabs(center_lat) * (M_PI / 180.0)) / 2.0)));

		y = -(aequatdist_center - aequatdist_point);

		return;
	}


	std::cerr << "No valid projection....." << std::endl;
	exit(1);
}


void Projection::get_latlon(float x, float y, float &lat, float &lon)
{
	if (center_lat == 9999 || center_lon == 9999) {
		std::
		    cerr <<
		    "Projection class: get_latlon: center isn't set!" <<
		    std::endl;
		return;
	}

	// this is 1 in km on a great circle
	degree_dist = (2 * M_PI * sphere_radius) / 360.0;

	if (projection == PROJ_PSEUDO_CYL_NO1) {
		lat = center_lat + (y / degree_dist);
		lon =
		    center_lon +
		    (x / (degree_dist * cos((M_PI / 180.0) * lat)));
		return;
	}


	if (projection == PROJ_CYL_NO1) {
		lat = center_lat + (y / degree_dist);
		lon = center_lon + (x / degree_dist);
		return;
	}

	if (projection == PROJ_CYL_PLATT) {

		lon = center_lon + (x / degree_dist);
		float aequatdist_center =
		    sin(center_lat * (M_PI / 180.0)) * sphere_radius;
		lat =
		    asin((y +
			  aequatdist_center) / sphere_radius) / (M_PI /
								 180.0);
		return;
	}

	if (projection == PROJ_CYL_MERCATOR) {

		lon = center_lon + (x / degree_dist);


		float sign_c = 1.0;
		if (center_lat < 0)
			sign_c = -1.0;
		float aequatdist_center =
		    -sign_c * sphere_radius *
		    log(tan
			((M_PI / 4.0) +
			 ((fabs(center_lat) * (M_PI / 180.0)) / 2.0)));

		float northing = aequatdist_center - y;

		lat =
		    -(2.0 * atan(exp(northing / sphere_radius)) -
		      (M_PI / 2.0)) / (M_PI / 180.0);

		return;
	}

	std::cerr << "No valid projection....." << std::endl;
	exit(1);
}
