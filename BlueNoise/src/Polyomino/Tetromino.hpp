/*
  Tetromino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _TETROMINO_HPP
#define _TETROMINO_HPP

#include "Polyomino.hpp"

class Tetromino : public Polyomino
{
public:
    Tetromino(void);
    Tetromino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    Tetromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    virtual ~Tetromino(void) = 0;

    Tetromino * Clone(void) const;

    virtual int NumSubdivisionRules(void) const;
    virtual int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    virtual void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
