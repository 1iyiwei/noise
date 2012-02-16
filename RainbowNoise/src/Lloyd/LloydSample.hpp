/*
  LloydSample.hpp

  the "points" (in the jargon of [Balzer et al. SIGGRAPH 2009]) 

  Li-Yi Wei
  07/15/2009

*/

#ifndef _LLOYD_SAMPLE_HPP
#define _LLOYD_SAMPLE_HPP

#include <vector>
using namespace std;

#include "Sample.hpp"
#include "LloydIndex.hpp"

class LloydSample
{
public:
    LloydSample(void);
    LloydSample(const int dimension, const LloydIndex & index);
    ~LloydSample(void);

    class Index // index to the site/owner for each class
    {
    public:
        Index(const LloydIndex & index);
        Index(void);
        ~Index(void);
        
        int NumClass(void) const;

        void Clear(void);
        void Clear(const int num_class);

        // return 1 if successful, 0 else
        int Set(const int which_class, const int lord, const float dist);
        int Get(const int which_class, int & lord, float & dist) const;
        int Set(const vector<int> & class_combo, const int lord, const float dist);
        int Get(const vector<int> & class_combo, int & lord, float & dist) const;

        // find the lord id with minimum dist under a given class combination
        int Get(const vector<int> & class_combo) const;
        
        Index(const Index & rhs);
        Index & operator=(const Index & rhs);

    protected:
        LloydIndex * _index;
    };

public:
    Sample sample;
    float density;
    Index index;
};

#endif
