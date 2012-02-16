/*
  LLoyd.hpp

  the basic class for all Lloyd relaxation

  Li-Yi Wei
  07/15/2009

*/

#ifndef _LLOYD_HPP
#define _LLOYD_HPP

#include <vector>
using namespace std;

#include "Domain.hpp"
#include "ClassWeight.hpp"
#include "Sample.hpp"
#include "LloydSite.hpp"

class Lloyd
{
public:
    Lloyd(void);
    virtual ~Lloyd(void) = 0;

    // return 1 if sucessful, 0 else
    virtual int Set(const vector<LloydSite> & sites);
    virtual int Get(vector<LloydSite> & sites) const;
 
    // return 1 if successful, 0 else
    virtual int Voronoi(void);
    // return the maximum distance moved by the sites (potenial stop criteria)
    // return negative if error
    virtual float Centroid(void);

protected:
    vector<LloydSite> _sites;
};

#endif
