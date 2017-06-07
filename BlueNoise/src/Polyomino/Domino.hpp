/*
  Domino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _DOMINO_HPP
#define _DOMINO_HPP

#include "Polyomino.hpp"

class Domino : public Polyomino
{
public:
    Domino(void);
    Domino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    Domino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~Domino(void);

    Domino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
