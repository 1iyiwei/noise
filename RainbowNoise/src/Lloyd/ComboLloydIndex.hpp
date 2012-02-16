/*
  ComboLloydIndex.hpp

  allow specification of index information for combo classes only
  used when the Voronoi step is non-geometrical, i.e. Balzer algorithm

  Li-Yi Wei
  07/25/2009

*/

#ifndef _COMBO_LLOYD_INDEX_HPP
#define _COMBO_LLOYD_INDEX_HPP

#include "LloydIndex.hpp"
#include "Array.hpp"

class ComboLloydIndex : public LloydIndex
{
public:
    ComboLloydIndex(const int num_class);
    ComboLloydIndex(void);
    virtual ~ComboLloydIndex(void);
    
    virtual ComboLloydIndex * Clone(void) const;

    virtual int NumClass(void) const;

    virtual void Clear(void);
    virtual void Clear(const int num_class);
    
    // return 1 if successful, 0 else
    virtual int Set(const int which_class, const int lord, const float dist);
    virtual int Get(const int which_class, int & lord, float & dist) const;
    
    virtual int Set(const vector<int> & class_combo, const int lord, const float dist);
    virtual int Get(const vector<int> & class_combo, int & lord, float & dist) const;

protected:
    Array<Data> _data;
    mutable Data _item;
};

#endif
