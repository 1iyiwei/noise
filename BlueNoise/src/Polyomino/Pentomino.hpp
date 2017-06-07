/*
  Pentomino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _PENTOMINO_HPP
#define _PENTOMINO_HPP

#include "Polyomino.hpp"

class Pentomino : public Polyomino
{
public:
    Pentomino(void);
    Pentomino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    Pentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    virtual ~Pentomino(void) = 0;

    Pentomino * Clone(void) const;

    virtual int NumSubdivisionRules(void) const;
    virtual int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    virtual void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
