/*
  LloydSite.hpp

  the "sites" (in the jargon of [Balzer et al. SIGGRAPH 2009]) 

  Li-Yi Wei
  08/11/2009

*/

#ifndef _LLOYD_SITE_HPP
#define _LLOYD_SITE_HPP

#include <vector>
using namespace std;

#include "Sample.hpp"

class LloydSite
{
public:
    LloydSite(void);
    ~LloydSite(void);

public:
    Sample sample;
    int fixed; // is this site fixed and thus non-movable?
};

#endif
