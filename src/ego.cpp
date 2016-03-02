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

#include "ego.h"
#include "GLexplorer.h"
#include <cmath>
#include <string>

//const std::string rcsid_ego_cpp =
//    "$Id: ego.cpp 3 2014-07-31 09:59:20Z kruegerh $";
//const std::string rcsid_ego_h = EGO_H;



Ego::Ego(double _x, double _y, double _z, double _spinx, double _spiny,
	 double _spinz)
{
	pos[0] = _x;
	pos[1] = _y;
	pos[2] = _z;
	spin[0] = _spinx;
	spin[1] = _spiny;
	spin[2] = _spinz;

	dpos[0] = 0;
	dpos[1] = 0;
	dpos[2] = 0;

	dorthopos[0]=0;
	dorthopos[1]=0;
	dorthopos[2]=0;

	orthopos[0] = 0;
	orthopos[1] = 0;
	orthopos[2] = 100;

}

void
 Ego::spinmoveradial(double _spinmoveradial)
// This spins the ego around the centre of the view plane
// while adjusting the view angle appropriately
// Once this is working we can try spinning around the place we are looking at.
{
	double tmp;
	tmp = pos[0];
	pos[0] =
	    pos[0] * cos(_spinmoveradial * PI_180) +
	    pos[1] * sin(_spinmoveradial * PI_180);
	pos[1] =
	    pos[1] * cos(_spinmoveradial * PI_180) -
	    tmp * sin(_spinmoveradial * PI_180);
	spin[2] = spin[2] + _spinmoveradial;
	if (spin[2] < 0.0)
		spin[2] = spin[2] + 360.0;
	if (spin[2] > 360.0)
		spin[2] = spin[2] - 360.0;
}

void
 Ego::spinmoveradialpoint(double _spinmoveradial, double x, double y)
// This spins the ego around point (x,y)
// while adjusting the view angle appropriately
{
	double tmp0, tmp1;
	tmp0 = pos[0] + x;
	tmp1 = pos[1] + y;
	pos[0] =
	    tmp0 * cos(_spinmoveradial * PI_180) +
	    tmp1 * sin(_spinmoveradial * PI_180) -
	    x;
	pos[1] =
	    tmp1 * cos(_spinmoveradial * PI_180) -
	    tmp0 * sin(_spinmoveradial * PI_180) -
	    y;


	spin[2] = spin[2] + _spinmoveradial;
	if (spin[2] < 0.0)
		spin[2] = spin[2] + 360.0;
	if (spin[2] > 360.0)
		spin[2] = spin[2] - 360.0;
}

void Ego::spinmovevertical(double _spinmovevertical)
// Spin the ego position up/down around centre of view plane
{/*
	double radial, radialnew, tmp;
	tmp = pos[2];
	radial = sqrt(pow(pos[0], 2) + pow(pos[1], 2));

	// avoid weird robotic moronness ;-) Restrict to 80 deg azimuth
	if ((_spinmovevertical > 0 && atan2(pos[2], radial) > 80 * PI_180)
	    || (_spinmovevertical < 0
		&& atan2(pos[2], radial) < 1 * PI_180)) {
		return;
	}

	pos[2] =
	    pos[2] * cos(_spinmovevertical * PI_180) +
	    radial * sin(_spinmovevertical * PI_180);
	radialnew =
	    radial * cos(_spinmovevertical * PI_180) -
	    tmp * sin(_spinmovevertical * PI_180);
	pos[0] = pos[0] * radialnew / radial;
	pos[1] = pos[1] * radialnew / radial;

	tmp = spin[0];
	spin[0] =
	    spin[0] +
	    _spinmovevertical *
	    cos(atan2(spin[1] * PI_180, spin[0] * PI_180));
	if (spin[0] < 0.0)
		spin[0] = spin[0] + 360.0;
	if (spin[0] > 360.0)
		spin[0] = spin[0] - 360.0;

	spin[1] =
	    spin[1] +
	    _spinmovevertical * sin(atan2(spin[1] * PI_180, tmp * PI_180));
	if (spin[1] < 0.0)
		spin[1] = spin[1] + 360.0;
	if (spin[1] > 360.0)
		spin[1] = spin[1] - 360.0;
*/
  spinmoveverticalpoint(_spinmovevertical, 0, 0);
}

void Ego::spinmoveverticalpoint(double _spinmovevertical, double x, double y) //, double z)
// Spin the ego position up/down around a point (x, y, z)
{
	double radial, radialnew,  tmp2;
	tmp2 = pos[2];
	radial = sqrt(pow(pos[0] + x, 2) + pow(pos[1] + y, 2));

	// avoid weird robotic moronness ;-) Restrict to 80 deg azimuth
	if ((_spinmovevertical > 0 && atan2(pos[2], radial) > 80 * PI_180)
	    || (_spinmovevertical < 0
		&& atan2(pos[2], radial) < 1 * PI_180)) {
		return;
	}

	pos[2] =
	    pos[2] * cos(_spinmovevertical * PI_180) +
	    radial * sin(_spinmovevertical * PI_180);
	radialnew =
	    radial * cos(_spinmovevertical * PI_180) -
	    tmp2 * sin(_spinmovevertical * PI_180);
	pos[0] = ( (pos[0] + x) * radialnew / radial ) - x;
	pos[1] = ( (pos[1] + y) * radialnew / radial ) - y;

	tmp2 = spin[0];
	spin[0] =
	    spin[0] +
	    _spinmovevertical *
	    cos(atan2(spin[1] * PI_180, spin[0] * PI_180));
	if (spin[0] < 0.0)
		spin[0] = spin[0] + 360.0;
	if (spin[0] > 360.0)
		spin[0] = spin[0] - 360.0;

	spin[1] =
	    spin[1] +
	    _spinmovevertical * sin(atan2(spin[1] * PI_180, tmp2 * PI_180));
	if (spin[1] < 0.0)
		spin[1] = spin[1] + 360.0;
	if (spin[1] > 360.0)
		spin[1] = spin[1] - 360.0;

}

void Ego::spinx(double _spinx)
{
	spin[0] = spin[0] + _spinx;
	if (spin[0] < 0.0)
		spin[0] = spin[0] + 360.0;
	if (spin[0] > 360.0)
		spin[0] = spin[0] - 360.0;
}

void Ego::spiny(double _spiny)
{
	spin[1] = spin[1] + _spiny;
	if (spin[1] < 0.0)
		spin[1] = spin[1] + 360.0;
	if (spin[1] > 360.0)
		spin[1] = spin[1] - 360.0;
}

void Ego::spinz(double _spinz)
{
	spin[2] = spin[2] + _spinz;
	if (spin[2] < 0.0)
		spin[2] = spin[2] + 360.0;
	if (spin[2] > 360.0)
		spin[2] = spin[2] - 360.0;
}

void Ego::movex(double _move)
{
	pos[0] = pos[0] + _move;
}

void Ego::movey(double _move)
{
	pos[1] = pos[1] + _move;
}

void Ego::movez(double _move)
{
	pos[2] = pos[2] + _move;
}

void Ego::movef(double _move)
{
	double _move_horizontal;
	pos[2] = pos[2] - _move * sin((270.0 - spin[0]) * PI_180);
	_move_horizontal = _move * cos((270.0 - spin[0]) * PI_180);

	pos[0] = pos[0] + _move_horizontal * sin(PI_180 * spin[2]);
	pos[1] = pos[1] + _move_horizontal * cos(PI_180 * spin[2]);
}

void Ego::moves(double _move)
{
	pos[0] = pos[0] + _move * sin(PI_180 * (spin[2] + 90.0));
	pos[1] = pos[1] + _move * cos(PI_180 * (spin[2] + 90.0));
}


void Ego::moveu(double _move)
{
	double _move_horizontal;
	pos[2] = pos[2] + _move * sin((spin[0] - 180.0) * PI_180);
	_move_horizontal = _move * cos((spin[0] - 180.0) * PI_180);

	pos[0] = pos[0] + _move_horizontal * sin(PI_180 * spin[2]);
	pos[1] = pos[1] + _move_horizontal * cos(PI_180 * spin[2]);

}
