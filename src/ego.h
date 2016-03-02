/*
 * (c) 2002-2016 Hannes Krueger
 * This file is part of the GPLIGC/ogie package
 */

/*
DESCRIPTION:
small class to track a position in cartesian coordinatesystem and to move/rotate
*/

#ifndef  EGO_H
#define  EGO_H  1

#include<string>

class Ego {

      public:
	Ego (double _x, double _y, double _z, double _spinx, double _spiny, double _spinz);

	double getx () const { return pos[0];	}
	double gety () const {return pos[1];	}
	double getz () const {return pos[2];	}
	void setz (double _z) {pos[2] = _z;}
	void setx (double _x) {pos[0] = _x;}
	void sety (double _y) {pos[1] = _y;}

	double getOx() const {return orthopos[0];}
	double getOy() const {return orthopos[1];}
	double getOz() const {return orthopos[2];}

	void setOx(double _x) {orthopos[0]=_x;}
	void setOy(double _y) {orthopos[1]=_y;}
	void setOz(double _z) {orthopos[2]=_z;}

	void shiftx(double x) {orthopos[0] += x;}
	void shifty(double y) {orthopos[1] += y;}
	void shiftz(double z) {orthopos[2] += z;}

	//diffrerence vector position for marker_locked_pos
	double getdx () const { return dpos[0];	}
	double getdy () const {return dpos[1];	}
	double getdz () const {return dpos[2];	}
	void setdz (double _z) {dpos[2] = _z;}
	void setdx (double _x) {dpos[0] = _x;}
	void setdy (double _y) {dpos[1] = _y;}

	double getdox () const { return dorthopos[0];	}
	double getdoy () const {return dorthopos[1];	}
	double getdoz () const {return dorthopos[2];	}
	void setdoz (double _z) {dorthopos[2] = _z;}
	void setdox (double _x) {dorthopos[0] = _x;}
	void setdoy (double _y) {dorthopos[1] = _y;}

	double getspinx () const {   return spin[0];}
	double getspiny () const {return spin[1];}
	double getspinz () const {return spin[2];}
	void setspinx (double _sx) {spin[0] = _sx;}
	void setspiny (double _sy) {spin[1] = _sy;}
	void setspinz (double _sz) {spin[2] = _sz;}

	void spinmoveradial (double); // spin the ego around the viewing plane
	void spinmovevertical (double);
	void spinmoveradialpoint (double, double, double); // spin the ego around a point
	void spinmoveverticalpoint (double, double, double);

	void spinx (double); // Spin is the direction we are looking in
	void spiny (double);
	void spinz (double);

	void movex (double);
	void movey (double);
	void movez (double);

	void movef (double);    //move forward
	void moves (double);    //move sidewards
	void moveu (double);    //move up

      protected:
	double pos[3];
	double dpos[3];
	double spin[3];

	double orthopos[3];
	double dorthopos[3];

      private:
	Ego ();

};

#endif
