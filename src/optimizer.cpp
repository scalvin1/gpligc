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

/*
usage:

optimizer
	-f firstpoint
	-l lastpoint		if not given all will be assumed
	-w number 		of wps
	-p pl1 pl2 ... pln-1	points for legs (n-1 arguments)
	-s random seed
	-d datafile (3d.dat from GPLIGC)

	-igc igcfile // planned, not yet implemented

	-devisor n  		uses every n-th record only

	-refine n   		do a brute force optimisation for each wp +-n  (only for 7 wps)

	-m montecarlo guesses 	(to be done before Metropolis Monte Carlo)
	-mmc metropolis mc cycles
	-progress n   progression method n steps (testing for live-optimisation)
	-sima	simulated annealing
	-sapara simulated annealing parameter a
	-saparb simulated annealing parameter b  (see temperature function)
	-sacycles aimulated annealing cycles for each temp

	-c overall cycles (GPLIGC uses many times -c 1 (to get the progress bar) otherwise -c 15 is OK (and default)

	-v verbose
	-debug lots of output

                For a general optimization task:
                	task with n waypoints (n-1 legs) and defined scoring
		max altitude difference between  min((-f),w1) and max((-l),wn) of 1000m
	use:  -f x -l y -w n -p 1 1 . .   -delta 1000 -m 5000 -c 15 -v -d 3d.dat


	-dmst  dmst scoring +15% for fai-triangle, but -w 5 -p 1 1 1 1 have to be given too
		altitude checking included

	-holc  holc scoring -w5 -p 1.5 1.5 1.5 1.5 should be given too
		checking for altitude (holc limit is 2500ft AGL for UL-aero-tow (!winch))

	-delta m  (max alt-difference between start and finish
		lowest point between release and start, and highest point between finish and landing)
		"olc-rules"
		(this check is only done if !HOLC && !DMST)


	for olc 2004 give: -delta 1000 -w 7 -p 1 1 1 1 0.8 0.6
	for dmst 2004:    -dmst -w 5 -p 1 1 1 1
	for holc 2004:    -holc -w 5 -p 1.5 1.5 1.5 1.5

*/


#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

#include "oglexfunc.h"

// srandom and random are POSIX, not available on MinGW
#ifdef __WIN32__
#define srandom srand
#define random rand
#endif


//const std::string rcsid_optimizer_cpp =
//    "$Id: optimizer.cpp 3 2014-07-31 09:59:20Z kruegerh $";
using namespace std;

//global variables
vector <float> lat;
vector <float> lon;
vector <int> alt;
vector <float> scoring;
vector <float> cycleresults;
int wps = 0;
int delta_h = 1000;
int low_index = 0;
int high_index = 0;

int mmc_done=0;
int temp_done=0;

int progression = 0;
int progression_high_index = 0;

bool PROGRESSION = false;	// progression method, stucks in local minima often...
bool SIMANNEALING = false;	// simulated annealing
bool VERBOSE = false;

bool OLC_CLASS = false;
bool OLC_FAI = false;
bool DMST = false;
bool HOLC = false; // OLC-PHG
bool SOLUTION = false;
bool DEBUG = false;

bool SA_CHANGED = false;

int sa_i = 1;
int sa_t = 0;
int sa_nt = 1000;
float sa_temp;
float sa_param_a=15;
float sa_param_b=0.03;
bool SA_LINEAR = false;
bool SA_EXP = true;

int devisor = 1;		// uses only every n-th record
int refine = 0;			// brute force refinement points +- n

// Working wp vector
vector <int> p;

// overall maximum variables
vector <int> max_task;
float opt_km = 0;
float max_points = 0;

// cycle maximum variables
vector < int >max_task_cycle;
float opt_km_cycle = 0;
float max_points_cycle = 0;

void montecarlo(void);
void metropolismontecarlo(void);
void calc_pts_km(vector < int >_p);
float temperature(int _t);
double binomial(int n, int k);
double factorial(int n);


int main(int argc, char *argv[])
{
	if (argc == 1) {
		exit(0);
	}


	int cycles = 15;	//overall cycles (default)
	int n = -1;		// line-index (datafile)

	unsigned long int mc_cycles = 5000;	// number of monte carlo cycles
	unsigned long int met_mc_cycles = 25000;

	float x, y, _f;		//some tmp-floats
	int temp_alt;

	unsigned long int seed = 0;	//random seed from GPLIGC
	char *filename;

	// parsing of options...
	for (int i = 0; i < argc; i++) {

		//cerr << i << "  " << argc << "   " << argv[i] << "---" << endl;

		if (strcmp(argv[i], "-f") == 0)
			sscanf(argv[i + 1], "%d", &low_index);
		if (strcmp(argv[i], "-l") == 0)
			sscanf(argv[i + 1], "%d", &high_index);
		if (strcmp(argv[i], "-w") == 0) {
			sscanf(argv[i + 1], "%d", &wps);
			p.resize(wps);
			max_task.resize(wps);
			max_task_cycle.resize(wps);
		}

		if (strcmp(argv[i], "-progression") == 0) {
			sscanf(argv[i + 1], "%d", &progression);
			PROGRESSION = true;
		}

		if (strcmp(argv[i], "-sima") == 0) {
			SIMANNEALING = true;
		}

		if (strcmp(argv[i], "-salin") == 0) {
			SA_LINEAR = true;
			SA_EXP = false;
		}
		if (strcmp(argv[i], "-saexp") == 0) {
			SA_EXP = true;
			SA_LINEAR = false;
		}

		if (strcmp(argv[i], "-d") == 0)
			filename = argv[i + 1];
		if (strcmp(argv[i], "-m") == 0)
			sscanf(argv[i + 1], "%lu", &mc_cycles);
		if (strcmp(argv[i], "-mmc") == 0)
			sscanf(argv[i + 1], "%lu", &met_mc_cycles);
		if (strcmp(argv[i], "-sacycles") == 0)
			sscanf(argv[i + 1], "%d", &sa_nt);
		if (strcmp(argv[i], "-sapara") == 0)
			sscanf(argv[i + 1], "%f", &sa_param_a);
		if (strcmp(argv[i], "-saparb") == 0)
			sscanf(argv[i + 1], "%f", &sa_param_b);
		if (strcmp(argv[i], "-s") == 0)
			sscanf(argv[i + 1], "%lu", &seed);
		if (strcmp(argv[i], "-c") == 0)
			sscanf(argv[i + 1], "%d", &cycles);
		if (strcmp(argv[i], "-devisor") == 0)
			sscanf(argv[i + 1], "%d", &devisor);
		if (strcmp(argv[i], "-refine") == 0)
			sscanf(argv[i + 1], "%d", &refine);
		if (strcmp(argv[i], "-delta") == 0)
			sscanf(argv[i + 1], "%d", &delta_h);

		if (strcmp(argv[i], "-p") == 0) {
			for (int j = 1; j < wps; j++) {
				sscanf(argv[i + j], "%f", &_f);
				//cerr << _f << endl;
				scoring.push_back(_f);
			}
		}

		if (strcmp(argv[i], "-v") == 0)
			VERBOSE = true;

		if (strcmp(argv[i], "-debug") == 0)  {
			DEBUG = true;
			VERBOSE = true;
		}

		if (strcmp(argv[i], "-dmst") == 0) {
			DMST = true;
			delta_h = 1000; //check is hard-coded
		}

		if (strcmp(argv[i], "-olc-class") == 0) {
			OLC_CLASS = true;
			delta_h = 1000; //check is hard-coded
		}

		if (strcmp(argv[i], "-olc-fai") == 0) {
			OLC_FAI = true;
			delta_h = 1000; //check is hard-coded
		}

		if (strcmp(argv[i], "-holc") == 0)
			HOLC = true;


	}

	if (PROGRESSION) {
//		cycles = 1;
		progression_high_index = low_index+progression;
	}

/*	if (SIMANNEALING) {
		//cycles = 1;
		mc_cycles = 1;

	}
*/


	if (VERBOSE) {
		cerr << "Optimizer by Hannes Krueger (c) 2002-2016 " << endl; // << rcsid_optimizer_cpp << endl;
		cerr << "Optimize \"" << filename << "\"" << endl
		    << "between " << low_index << " and " << high_index <<
		    endl << wps << " Waypoints" << endl;
		for (int i = 0; i < wps - 1; i++)
			cerr << "Leg " << i +
			    1 << " = " << scoring[i] << " Points/km" <<
			    endl;
		cerr << mc_cycles << " Initial MC Random guesses"
		    << endl << "Random seed given: " << seed << endl <<
		    "Overall cycles: " << cycles << endl;

		if (OLC_CLASS)
			cerr << "Optimize for OLC-classic (rules oct/2007)" << endl;
		if (OLC_FAI)
			cerr << "Optimize for OLC-Fai (rules oct/2007)" << endl;

		if (DMST)
			cerr << "Optimize for DMSt (rules 2004)" << endl;

		if (HOLC)
			cerr << "Optimize for HOLC 2004 rules" << endl;


		//if (!DMST && !HOLC)
		//	cerr << "Max Alt difference is " << delta_h << " m" <<
		//   endl;

		if (PROGRESSION) {
			cerr << "Progression method: steps: "<< progression << endl;
		}

		if (SIMANNEALING){
			cerr << "Simulated annealing will be performed. Cylces: " <<sa_nt  <<endl;
			if (SA_EXP)
			cerr << "Temp function: T(x) = " << sa_param_a<<"*exp(-"<<sa_param_b<<"*x)"<<endl;
			if (SA_LINEAR)
			cerr << "Temp function: T(x) = -" << sa_param_a<<"*x + "<<sa_param_b<<endl;
		}

		if (refine > 0) {
			cerr << "Brute Force refinement will be performed +- " << refine << endl;
		}

		if (devisor > 1) {
			cerr << "Devisor: " << devisor << "  Data subsampling" << endl;
		}
	}


	// open the datafile and read in
	ifstream cfile(filename);

	if (!cfile) {
		cerr << "Can not open datafile " << filename << endl;
		return -1;
	}

	char zeile[80];

	while (cfile) {
		cfile.getline(zeile, 80, '\n');

		//cerr << "Zeile: " << zeile << endl;
		n++;
		if (sscanf(zeile, "%f %f %d", &x, &y, &temp_alt) != 3) {
			//cerr << "empty line in km.dat (why?)" << endl;
			n--;
		} else {

			lat.push_back(y);
			lon.push_back(x);
			alt.push_back(temp_alt);
			//cerr << temp_alt << endl;

		}
	}

	if (high_index==0) {high_index=lat.size()-1;}

	if (VERBOSE)
		cerr << high_index-low_index << " Records. " << wps << " Waypoints. Gives Binomial("<<high_index-low_index<<","<<wps<<") possibilities." << endl;
	if (VERBOSE && devisor > 1)
		cerr << (float)(high_index-low_index)/(float)devisor << " Records/devisor " << wps << " Waypoints. Gives Binomial("<<(float)(high_index-low_index)/(float)devisor<<","<<wps<<") possibilities." << endl;


// initialize random by jmw krueger modified by kruegerh
// Seed RNG *******************************************
	//int  seed = 0;
	FILE *devrand;
	devrand = fopen("/dev/urandom", "r");

// if no dev urandom available use seed from cmdl
	if (devrand) {
		fread(&seed, sizeof(int), 1, devrand);
		fclose(devrand);
	} else {
		// if no seed from commandline available... exit
		if (seed == 0) {

			cerr << "No random seed available..." << endl;
			return -1;
		}
	}

	//cerr << "used seed to initialize random number generator: " << seed << endl;
	srandom(seed);


// ****************************************************

	// too high release point warning! (applies in case of UL-tow)
	if (HOLC && VERBOSE) {
		//VALID = true;
		if (alt[low_index] - alt[0] > 762.195) { // 2500ft
			cerr << "WARNING: Altitude difference between take-off location and release point: " <<
			alt[low_index]-alt[0] << "m.  This violates 4.5 in DHV Online contest Rules for 2004 (Rel. 1.5)" <<
			" in the case you were towed by an UL. According to 4.5 winch launching seems to be OK." << endl;
		}
	}


//starting optimization cycles:
	for (int u = 1; u <= cycles; u++) {

		if (DEBUG)
			cerr << "Cycle: " << u << endl;
		max_points_cycle = 0;

		if (PROGRESSION)
			progression_high_index = low_index+progression;

		progress_point:

		if (PROGRESSION && VERBOSE) cerr << "Progression:  " << low_index << " <===> " << progression_high_index << "                 " << '\r' << flush;



		// initial model (random guesses or equidistant (index) distribution)
		if (mc_cycles != 0) {
			// loop for random guesses
			for (unsigned long int i = 0; i <= mc_cycles; i++) {
				//if (DEBUG)
				//	cerr << "Monte Carlo random guess: " << i << "         " << '\r' << flush;
				montecarlo();

			}
		} else  {	//start with uniform distribution
			if (PROGRESSION) {exit(0);}
			if (DEBUG)
				cerr << "No Monte Carlo guesses. Starting with \"uniform distribution\"" << endl;
			int intervall =
			    (high_index - low_index) / (wps + 1);
			//cerr << low_index << "  <--->  " << high_index << endl;
			for (int z = 0; z < wps; z++) {
				max_task_cycle[z] =
				    low_index + (z + 1) * intervall;
			}
		}



		if (!SIMANNEALING) {
// Metropolis MC cycles
			unsigned long int zaehler = 0;
			while (zaehler < met_mc_cycles) {
				zaehler++;
				//if (DEBUG)
				//	cerr << "Metropolis Monte Carlo:  " << zaehler << "         " << '\r' << flush;
				metropolismontecarlo();
			}
		} else {
// SIMULATED ANNEALING
			sa_t=0;
			sa_temp=temperature(sa_t);

			while (sa_temp >= 0.0001) {
				SA_CHANGED = false;
				if (sa_i < sa_nt) {
					metropolismontecarlo();
					if (!SA_CHANGED)
						sa_i++;
				} else {
					sa_i = 1;
					sa_t++;
					sa_temp=temperature(sa_t);
					temp_done++;
					if (VERBOSE)
					cerr << "Temp: " << sa_temp << "  Pts: " << max_points_cycle << "  KM: " << opt_km_cycle << "     "<<'\r' << flush;
				}
			}
		}

		//if (DEBUG) cerr << endl;


		if (PROGRESSION) {
			progression_high_index += progression;
			if (progression_high_index <= high_index) {goto progress_point;}
		}

		// brute force refinement +- n
		vector <int> _p = max_task_cycle; // this one stays constant (for the loops)
		vector <int> _pp = max_task_cycle;// this one contains the actual try


		// so far this only works for wps == 7
		if (refine > 0) {
			unsigned long int bfrefc =0;
			if (DEBUG) cerr << "Refine started..." <<endl;

			if (max_task_cycle.size() == 7) {

			for (int p0=_p[0]-refine; p0<=_p[0]+refine; p0++) {
			_pp[0] = p0;

			for (int p1=_p[1]-refine; p1<=_p[1]+refine; p1++) {
			_pp[1] = p1;

			for (int p2=_p[2]-refine; p2<=_p[2]+refine; p2++) {
			_pp[2] = p2;

			for (int p3=_p[3]-refine; p3<=_p[3]+refine; p3++) {
			_pp[3] = p3;

			for (int p4=_p[4]-refine; p4<=_p[4]+refine; p4++) {
			_pp[4] = p4;

			for (int p5=_p[5]-refine; p5<=_p[5]+refine; p5++) {
			_pp[5] = p5;

			for (int p6=_p[6]-refine; p6<=_p[6]+refine; p6++) {
			_pp[6] = p6;

				bfrefc++;
				calc_pts_km(_pp);

				//for (int s = 0; s < (int) _pp.size(); s++) {
				//cerr << "  " << _pp[s];
				//} cerr << endl;
			}}}}}}}}

			if (max_task_cycle.size() == 5) {

			for (int p0=_p[0]-refine; p0<=_p[0]+refine; p0++) {
			_pp[0] = p0;

			for (int p1=_p[1]-refine; p1<=_p[1]+refine; p1++) {
			_pp[1] = p1;

			for (int p2=_p[2]-refine; p2<=_p[2]+refine; p2++) {
			_pp[2] = p2;

			for (int p3=_p[3]-refine; p3<=_p[3]+refine; p3++) {
			_pp[3] = p3;

			for (int p4=_p[4]-refine; p4<=_p[4]+refine; p4++) {
			_pp[4] = p4;

				bfrefc++;
				calc_pts_km(_pp);

				//for (int s = 0; s < (int) _pp.size(); s++) {
				//cerr << "  " << _pp[s];
				//} cerr << endl;
			}}}}}}

			if (DEBUG)
			cerr << "Refinement cycles: " << bfrefc << endl;
		}

		if (max_points_cycle > max_points) {
			max_points = max_points_cycle;
			opt_km = opt_km_cycle;
			max_task = max_task_cycle;
		}

		if (VERBOSE) {
			//cerr << u << " cycle:      km" << opt_km_cycle << "   pts: " << max_points_cycle << endl;
			//cerr << "Cycle " << u << " results:     km: " << opt_km << "    pts: "
			//   << max_points << endl;
			cerr << "Cycle " << u << " results:     pts: " << max_points_cycle << "    km: "
			    << opt_km_cycle << "  (best: "<< max_points<<"pts / "<<opt_km<<"km)"<<endl;

		}

		cycleresults.push_back(max_points_cycle);



	}

	if (!SOLUTION && VERBOSE)
		cerr << "No solution! - ??? why? -delta too negative?" <<
		    endl;

	// this is the real result, to be get by GPLIGC (the only output to stdout (without -v))
	cout << wps << "  " << opt_km << "  " << max_points;
	for (int s = 0; s < (int) max_task.size(); s++) {
		cout << "  " << max_task[s];
	}
	cout << endl;

	// gives an estimate how often the best solution comes out (% of cycles)
	if (VERBOSE) {
		int numbest=0;
		//int cycles=1;
		for (int a=0;a<=(int)cycleresults.size()-1;a++) {
			//cycles++;
			if (cycleresults[a] > max_points-0.001) {numbest++;}
		}

		float per = ((float)numbest/(float)cycleresults.size())*100.0;
		cerr << "cyc: " << cycleresults.size() << "   best: " << numbest << "  percent: " << per << endl;
		cerr << "Average mmc/cycle: " << (float) mmc_done / (float) cycles << "  Steps in cooling: " << (float) temp_done / (float) cycles << endl;
		cerr << "above per Trackpoint: " << ((float) mmc_done / (float) cycles) / ((float) (high_index-low_index)/(float)devisor) << endl;
		cerr << "above per %: " << (((float) mmc_done / (float)cycles) / ((float) (high_index-low_index)/(float)devisor))/per << endl;
	}

	return 1;


}

// random number between zero and max (by   jan mw krueger)
inline int random_number(double max)
{
	return (int) ((max + 1) * random() / (RAND_MAX + 1.0));
}


void montecarlo(void)
{

	// get the guesses for each wp
	int hi;
	if (PROGRESSION) {hi = progression_high_index;}
		else {hi = high_index;}
	for (int m = 0; m < wps; m++) {
		p[m] = (
			(int) (random_number((hi - low_index +1) / devisor)) * devisor) + low_index;
		//cerr << "ZZZ: " << p[m] << " ";
	}

	sort(p.begin(), p.end());
	calc_pts_km(p);

}


// ######################### MMC
void metropolismontecarlo(void)
{
	int but, top, q, indexregister;

	//random number (take one wp to change)
	q = (int) random_number((wps - 2) - 0 + 1) + 0;

	if (q == 0) {
		but = low_index;
	} else {
		but = max_task_cycle[q - 1];
	}			//+1;}
	if (q == wps - 1) {
		top = high_index;
		if (PROGRESSION) {top=progression_high_index;}
	} else {
		top = max_task_cycle[q + 1];
	}			//-1;}

	//guess new wp between last and next of choosen
	indexregister = ((int) (random_number((top - but)/devisor)) *devisor) + but;
	p = max_task_cycle;
	p[q] = indexregister;
	calc_pts_km(p);

	mmc_done++;
}




// calculate points and km for given indices
void calc_pts_km(vector < int >_p)
{

	//sort given indices
	//sort(_p.begin(), _p.end());  // PERFORMANCE ... not needed for metropolis and sim anneal

	//set km and points to 0
	float km = 0;
	float pts = 0;

	// temporary km in calc-cycle
	float cyckm = 0;

	// this should bes visible in the whole calc_pts_km function (for some debug-related output)
	bool FAI = false;
	bool TRI = false; // only for debugging output

	// The "legs" vector is filles with the lengths of each leg
	vector < float >legs;

	// calculate distance

	// General case (doesnt need to be calc for olc-fai
	//if (!OLC_FAI) {    ALWAYS NEEDED
		for (int n = 0; n < (int) _p.size() - 1; n++) {

			cyckm =
			    great_circle_dist(lat[_p[n]], lon[_p[n]],
					      lat[_p[n + 1]],
					      lon[_p[n + 1]]);

			legs.push_back(cyckm);
			km = km + cyckm;
			pts = pts + cyckm * scoring[n];

		}
	//}

	// DMST rules and HOLC rules OLC_FAI
	// check for triangular task an fai-tri
	if (DMST || HOLC || OLC_FAI) {

		//these will  be the distance of the triangular task
		float km3;

		// in the case of holc we need to calculate this first, because the start-end limit is 20% of km3

		// triangle wp 123
		if (HOLC)
		km3 = legs[1] + legs[2] +
			    great_circle_dist(lat[_p[1]], lon[_p[1]],
					      lat[_p[3]], lon[_p[3]]);


		// if track is closed - end point within a radius of 1km of start point   (DMST and olc-fai)
		// triangular
		// for holc this is 20% of triangular task
		float limit = 1.0;
		if (HOLC) limit = 0.2 * km3;

		//  check the triangular task, if it is one start-end <= limit!
		if (great_circle_dist (lat[_p[0]], lon[_p[0]], lat[_p[4]], lon[_p[4]]) <= limit) {

			// distance from wp2-wp3-wp4-wp2   (wp1 is start, wp5 is finish)
			// calc only for dmst here, for holc we have done this before (saves some time)
			if (DMST || OLC_FAI)
			km3 = legs[1] + legs[2] +
			    great_circle_dist(lat[_p[1]], lon[_p[1]],
					      lat[_p[3]], lon[_p[3]]);

			// lengths of legs of triangular task
			vector < float >legs3;
			legs3.push_back(legs[1]);
			legs3.push_back(legs[2]);
			legs3.push_back(great_circle_dist
				      (lat[_p[1]], lon[_p[1]], lat[_p[3]],
				       lon[_p[3]]));

			// sort them for FAI task checking
			sort(legs3.begin(), legs3.end());

			// check for FAI task
			FAI = false;

			if (km3 < 500) {
				// leg3[0] is the shortest leg
				if ((legs3[0] / km3) >= 0.28)
					FAI = true;
			}

			if (km3 >= 500) {
				if ((legs3[0] / km3) >= 0.25
				    && (legs[2] / km3) <= 0.45)
					FAI = true;
			}

			// add 15% if its an FAI task (DMSt WO 4.2)
			// calc points seperate, to compare with "plain task" later
			float fai_pts;
			float tri_pts;
			//float tri_km;

			// OLC_FAI 3 legs, 1pt/km <== scheint so zu sein!
			if (OLC_FAI && FAI) {
				pts = km3;
				km = km3;
			}



			// FAI adds 15% for DMST, and check if the FAI-task will have more points
			if (DMST && FAI) {
				fai_pts = km3 * 1.15;
				if (fai_pts > pts) {
					pts = fai_pts;
					km = km3;
				}
			}

			// FAI task will have km3 * 2 pts!
			if (HOLC && FAI) {
				fai_pts = (km3
					- great_circle_dist(lat[_p[0]], lon[_p[0]], lat[_p[4]],lon[_p[4]])  ) * 2.0;
				if (fai_pts > pts) {
					pts = fai_pts;
					km = km3- great_circle_dist(lat[_p[0]], lon[_p[0]], lat[_p[4]],lon[_p[4]]);
				}
			}

			// triangluar task will have km3 * 1.75 pts!
			if (HOLC && !FAI) {
				tri_pts = (km3 - great_circle_dist(lat[_p[0]], lon[_p[0]], lat[_p[4]],lon[_p[4]])) *1.75;
				if (tri_pts > pts) {
					pts = tri_pts;
					km = km3- great_circle_dist(lat[_p[0]], lon[_p[0]], lat[_p[4]],lon[_p[4]]);
					TRI = true;
				}
			}
		}
	}


// ######## CHECKING THE TASK
	// now points and km are calculated
	// now we need to check if the task is valid or not (alt difference, etc)
	// save better task, check altitude difference...
	//altitude diff for olc differs from alt diff for dmst (is like FAI sporting code III (1999,AL4, 1.2.8 and 1.4.7)
	if (pts > max_points_cycle || SIMANNEALING) {

		// valid-task-flag.
		bool VALID = false;

		int start_alt = 100000;
		int finish_alt = -100000;


		// this is the olc-style altitude difference checking!
		if (OLC_CLASS || OLC_FAI) {

			// get lowest point between release and first wp (start)
			for (int n = low_index; n <= _p[0]; n++) {
				if (alt[n] < start_alt)
					start_alt = alt[n];
			}

			// get highest point between finish (last wp) and landing (or engine start etc)
			for (int n = _p[_p.size() - 1]; n <= high_index; n++) {
				if (alt[n] > finish_alt)
					finish_alt = alt[n];
			}

			// avoid task with too big altitude difference
			if (start_alt - finish_alt < delta_h)
				VALID = true;
		}

		if (OLC_FAI && !FAI)
			VALID=false;

		if (DMST) {
			// dmst alt diff like FAI Sporting Code III
			// 1999, AL4, 1.2.8 1.4.7
			start_alt = alt[_p[0]];
			finish_alt = alt[_p.size()-1];
			int release_alt = alt[low_index];

			int diff1 = start_alt - finish_alt;
			int diff2 = release_alt - finish_alt;

			if (diff1 < 1000 || diff2 < 1000)
				VALID=true;
		}


		if (HOLC)
			VALID = true;


		// check for minimum distance between WP's (DMSt WO 4.1)
		// nur zwischen WPs, also nicht start und endpunkt???
		// this seems to be removed from 4.1 in 2004 ?
		// check this out too!

		if (DMST) {

			if (great_circle_dist
			    (lat[_p[1]], lon[_p[1]], lat[_p[2]],
			     lon[_p[2]]) < 10.0
			    || great_circle_dist(lat[_p[2]], lon[_p[2]],
						 lat[_p[3]],
						 lon[_p[3]]) < 10.0
			    || great_circle_dist(lat[_p[3]], lon[_p[3]],
						 lat[_p[1]],
						 lon[_p[1]]) < 10.0)
				VALID = false;
		}


		// save better and valid task
		if (VALID && !SIMANNEALING) {
			// save better task...
			max_task_cycle = _p;
			opt_km_cycle = km;
			max_points_cycle = pts;
			SOLUTION = true;
		}

		if (VALID && SIMANNEALING) {
			if (pts > max_points_cycle) {
				max_task_cycle = _p;
				opt_km_cycle = km;
				max_points_cycle = pts;
				SOLUTION = true;
				SA_CHANGED=true;
				//cerr << "pts: " << pts << endl;

			} else {
				float prob = exp(- (max_points_cycle-pts)/sa_temp);
				//cerr << "diff: " << max_points_cycle-pts << endl;
                float randno = (float) random() / (float) RAND_MAX;
                //cerr << "prob = " << prob << "  rand = " << randno << endl;
                if (randno < prob && !(prob==1.0)) {
					max_task_cycle = _p;
					opt_km_cycle = km;
					max_points_cycle = pts;
					SOLUTION = true;
					SA_CHANGED=true;
					//cerr << "pts: " << pts << "       !!" << endl;
				}
			}
		}

		if (DEBUG && !VALID)
			cerr << endl << "Invalid Task omitted!" << endl;



		if (DEBUG && VALID && !SIMANNEALING) {
			cerr <<  "Task improved:  " << km << " "  << pts << "    " << endl;
			if (DMST && FAI) cerr << "DMST and FAI! 15% bonus applied!"<<endl;
			if (HOLC && FAI) cerr << "HOLC and FAI! 2pts/km!" << endl;
			if (HOLC && TRI) cerr << "HOLC triangular! 1.75pts/km!" << endl;;
			// cerr << endl;
		}

	}


}

// temperature function for sim anneal
float temperature (int _t) {
	//simple linear temp function

	if (SA_LINEAR)
	return (-sa_param_a * (float) _t + sa_param_b);

	if (SA_EXP)
	return (sa_param_a*exp(-sa_param_b*(float)_t));

	return 0;
}


// not usefull. range of double is  1.7E +/- 308 (too small)
double factorial(int n) {
	double fac=1;
	for (int x=n;x>1;x--) {
		fac*=x;
		//cerr << fac << "  ";
	}
	return fac;
}

double binomial(int n, int k) {
	return factorial(n)/(factorial(k)*factorial(n-k));
}
