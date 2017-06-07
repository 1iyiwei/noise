/*
  Monomino.hpp

  Li-Yi Wei

  10/21/2007

*/

#ifndef _MONOMINO_HPP
#define _MONOMINO_HPP

#include "Polyomino.hpp"

class Monomino : public Polyomino
{
public:
    Monomino(void);
    Monomino(const Int2 & center);
    Monomino(const Int2 & center, const Flt2 & sample_offset);
    ~Monomino(void);

    Monomino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;

    int IsInside(const Flt2 & sample_offset) const;
    
protected:
};

#endif
