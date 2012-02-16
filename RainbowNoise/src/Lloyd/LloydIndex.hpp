/*
  LloydIndex.hpp

  the data structure for storing the site information on points

  Li-Yi Wei
  07/25/2009

*/

#ifndef _LLOYD_INDEX_HPP
#define _LLOYD_INDEX_HPP

#include <vector>
using namespace std;

#include "Math.hpp"

class LloydIndex
{
public:
    virtual ~LloydIndex(void) = 0;
    
    virtual LloydIndex * Clone(void) const;

    virtual int NumClass(void) const;

    virtual void Clear(void);
    virtual void Clear(const int num_class);
    
    // return 1 if successful, 0 else
    virtual int Set(const int which_class, const int lord, const float dist);
    virtual int Get(const int which_class, int & lord, float & dist) const;
    
    virtual int Set(const vector<int> & class_combo, const int lord, const float dist);
    virtual int Get(const vector<int> & class_combo, int & lord, float & dist) const;

protected:
    struct Data
    {
        Data(void) : lord(-1), dist(Math::INF) {};

        int lord;
        float dist;
    };
};

#endif
