/*
  SoloLloydIndex.hpp

  allow specification of index information for individual classes only
  used when the Voronoi step is geometrical, i.e. the traditional Lloyd

  Li-Yi Wei
  07/25/2009

*/

#ifndef _SOLO_LLOYD_INDEX_HPP
#define _SOLO_LLOYD_INDEX_HPP

#include "LloydIndex.hpp"

class SoloLloydIndex : public LloydIndex
{
public:
    SoloLloydIndex(const int num_class);
    SoloLloydIndex(void);
    virtual ~SoloLloydIndex(void);
    
    virtual SoloLloydIndex * Clone(void) const;

    virtual int NumClass(void) const;

    virtual void Clear(void);
    virtual void Clear(const int num_class);
    
    // return 1 if successful, 0 else
    virtual int Set(const int which_class, const int lord, const float dist);
    virtual int Get(const int which_class, int & lord, float & dist) const;
    
    virtual int Set(const vector<int> & class_combo, const int lord, const float dist);
    virtual int Get(const vector<int> & class_combo, int & lord, float & dist) const;

protected:
    vector<Data> _data;
};

#endif
