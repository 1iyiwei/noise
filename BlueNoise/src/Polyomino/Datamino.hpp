/*
  Datamino.hpp

  general polyomino with data
  
  Li-Yi Wei

  10/27/2007

*/

#ifndef _DATAMINO_HPP
#define _DATAMINO_HPP

#include "Polyomino.hpp"

class Datamino : public Polyomino
{
public:
    Datamino(void);
    Datamino(const Int2 & center, const vector<Int2> & cells);
    Datamino(const Int2 & center, const vector<Int2> & cells, const Flt2 & sample_offset);
    ~Datamino(void);

    Datamino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;
    
    void ReflectRow(void);
    void ReflectCol(void);
    void Rotate90(void);
    
protected:
    vector<Int2> _cells;
};

#endif
