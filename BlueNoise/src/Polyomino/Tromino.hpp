/*
  Tromino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _TROMINO_HPP
#define _TROMINO_HPP

#include "Polyomino.hpp"

class Tromino : public Polyomino
{
public:
    Tromino(void);
    Tromino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    Tromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    virtual ~Tromino(void) = 0;

    Tromino * Clone(void) const;

    virtual int NumSubdivisionRules(void) const;
    virtual int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    virtual void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
