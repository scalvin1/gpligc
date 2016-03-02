/*
 * (c) 2002-2016 Hannes Krueger
 * This file is part of the GPLIGC/ogie package
 */

#ifndef OGLEXFUNC_H
#define OGLEXFUNC_H 1

#ifndef OPTIMIZER

#ifndef __OSX__
#include <GL/gl.h>
#include <GL/glut.h>
#else
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#endif

#include <vector>
#include <string>
#include <cstdlib>
#include <sys/time.h>
#include "oconfig.h"

using namespace std;

#ifndef OPTIMIZER

// the old text function, no shaded background for text (to be used in help, about "loading"
void gltext (GLuint x, GLuint y, GLfloat scale, const char* txt, int _width, int _height, float _r=0.0, float _g=0.0, float _b=0.0);

// antonios new text function, with nice shaded background for text, to be used for on-screen info
void gltextvect (GLuint x, GLuint y, GLfloat scale, vector <string> lines, int _width, int _height, bool ortho=false);
// the last parameter isnt necessary anymore...(ortho)

void go(float lat1, float lon1, float heading, float distance,  float& lat2, float& lon2, float radius=6371.0);
float heading (float lat1, float lon1, float lat2, float lon2);

void get_gnd_elevations(vector<int>*, vector<float>*, vector<float>*, Config*);
int get_1gnd_elevation(float, float, Config*);

void HelpScreen();
void InfoScreen();

void getcolor(float* colorpointer, int colormap, float value, float min, float max);
float* graycolor(float*);

void glWindowPos4fMESAemulate(GLfloat x,GLfloat y,GLfloat z,GLfloat w);
void glWindowPos2fMESAemulate(GLfloat x, GLfloat y);

string dec2humantime(float t, int n);

// this is for SysV IPC communication with GPLIGC
struct gpligc_ipc_block {
int marker_pos;
};

class Timecheck {
	public:
		Timecheck();
		Timecheck(string _name,Config* _c);
		void checknow(string _here);

	protected:
		Config* conf;
		string name;
		timeval start;
		timeval last;
		timeval now;
};

#endif

float great_circle_dist (float lat1, float lon1, float lat2 , float lon2 , float radius=6371.0);

#endif
