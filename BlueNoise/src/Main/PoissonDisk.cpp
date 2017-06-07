//
// pd.cpp
//
// 2D Poisson-Disk sampling pattern generator
//
// Copyright (C) 1999, Matt Pharr <mmp@graphics.stanford.edu>
// 
// This software is placed in the public domain and is provided as is
// without express or implied warranty.
//

// Usage: pd [low] [high]
//   Generates a distribution of at least [low] but no more than [high]
//   points distributed in a posson disk distribution (i.e. no two points
//   are closer than some fixed distance together.)  A toroidal topology is
//   used, so that the pattern can be repeated across an image plane and
//   still have the Poisson Disk property at the edges.  [low] and [high]
//   should have some reasonable amount of separation  (i.e. at least 20 
//   or so)if this program is to terminate in a finite amount of time.
//
//   See Don Mitchell's classic paper, "Generating Antialiased Images at 
//   Low Sampling Densities," Proc. SIGGRAPH 1987.
//

#include <iostream>
#include <vector>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#define GRID_RES 250

static void usage()
{
    cerr << "usage: pd [min num points] [max num points]" << endl;
    exit(1);
}

struct Point2 {
    double u, v;
};

// Replace these as needed with a decent random number generator.

#define INIT_RANDOM_NUMBER_GENERATOR() srand48(time(NULL))
#define UNIFORM_RANDOM()                  drand48()

#define Point2Grid(p, r2, low, high) \
     ((low) = (int)floor(GRID_RES * ((p)-(r2))), \
      (high) = (int)ceil(GRID_RES * ((p)+(r2))))

static int throwDarts(int nThrows, double r, vector<Point2> *result);

int main(int argc, char *argv[])
{
    int minPoints = 0, maxPoints = 0;

    if (argc != 3)
	usage();

    INIT_RANDOM_NUMBER_GENERATOR();

    minPoints = atoi(argv[1]);
    maxPoints = atoi(argv[2]);
    if (minPoints == 0 || maxPoints == 0 || maxPoints <= minPoints)
	usage();

    // Take two radii that should(!) generate numbers of p.d. points
    // that bracket the range we're interested in.  If this doesn't
    // work out, we got mighty unlucky with our random numbers.
    double r0  = .775 / sqrt(maxPoints*1.0);
    cerr << "Trying low radius " << r0 << endl;
    int n0 = throwDarts(500*maxPoints, r0, NULL);
    cerr << "Got " << n0 << endl;
    assert(n0 > minPoints);

    double r1 = .925 / sqrt(minPoints*1.0);
    cerr << "Trying high radius " << r1 << endl;
    int n1 = throwDarts(500*maxPoints, r1, NULL);
    cerr << "Got " << n1 << endl;
    assert(n1 < maxPoints);

    // Now do a binary search until we get a number of points inside
    // the desired range.
    while (1) {
	double mid = (r0+r1)/2.;
	vector<Point2> result;

	cerr << "current range [ " << r0 << " (" << n0 << ") - " << 
	    mid << " (?) - " << r1 << " (" << n1 << ")  ]" << endl;
	int found = throwDarts(2000 * minPoints, mid, &result);
	cerr << "Found " << found << endl;

	if (found >= minPoints && found <= maxPoints) {
	    // success!
	    for (unsigned int i = 0; i < result.size(); ++i)
		cout << result[i].u << " \t" << result[i].v << " " << endl;
	    break;
	}

	if (found > maxPoints && found < n0) {
	    r0 = mid;
	    n0 = found;
	}
	else if (found < minPoints && found > n1) {
	    r1 = mid;
	    n1 = found;
	}
	else
	    cerr << "yow! holding steady..." << endl;
    }

    return 0;
}

static int throwDarts(int nThrows, double tryR, vector<Point2> *result)
{
    vector<Point2> grid[GRID_RES][GRID_RES];
    int nFound = 0;
    double tryR2 = tryR * tryR;
    int reportRate = nThrows / 78;

    while (nThrows--) {
	Point2 pt;
	pt.u = UNIFORM_RANDOM();
	pt.v = UNIFORM_RANDOM();

	if (nThrows % reportRate == 0)
	    cerr << '+';

	int i0, j0, i1, j1;
	Point2Grid(pt.u, tryR, i0, i1);
	Point2Grid(pt.v, tryR, j0, j1);
	
	for (int i = i0; i <= i1; ++i) {
	    int ii = i;
	    float ufix = pt.u;
	    
	    if (i < 0) {
		ii = GRID_RES + i;
		ufix = pt.u + 1.;
	    }
	    else if (i >= GRID_RES) {
		ii = i - GRID_RES;
		ufix = pt.u - 1.;
	    }
	    
	    for (int j = j0; j <= j1; ++j) {
		int jj = j;
		float vfix = pt.v;
		
		if (j < 0) {
		    jj = GRID_RES + j;
		    vfix = pt.v + 1.;
		}
		else if (j >= GRID_RES) {
		    jj = j - GRID_RES;
		    vfix = pt.v - 1.;
		}
		
		for (unsigned int k = 0; k < grid[ii][jj].size(); ++k) {
		    double du = ufix - grid[ii][jj][k].u;
		    double dv = vfix - grid[ii][jj][k].v;
		    
		    if (du*du + dv*dv < tryR2)
			goto nogood;
		}
	    }
	}
	
	++nFound;
	grid[int(GRID_RES*pt.u)][int(GRID_RES*pt.v)].push_back(pt);

    nogood:
	;
    }
    cerr << endl;

    if (result) {
	for (int i = 0; i < GRID_RES; ++i)
	    for (int j = 0; j < GRID_RES; ++j)
		for (unsigned int k = 0; k < grid[i][j].size(); ++k)
		    result->push_back(grid[i][j][k]);
    }
    return nFound;
}
