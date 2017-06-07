/*
  PolyominoSubdivider.hpp

  base class for all polyomino subdividers (i.e. production rules)
  
  Li-Yi Wei
  11/12/2007

*/

#ifndef _POLYOMINO_SUBDIVIDER_HPP
#define _POLYOMINO_SUBDIVIDER_HPP

#include <vector>
using namespace std;

#include "Polyomino.hpp"

class PolyominoSubdivider
{
public:
protected:
    static void Reflect(const Polyomino * mother, const vector<Polyomino *> & children);
    static void Rotate(const Polyomino * mother, const vector<Polyomino *> & children);
    static void Shift(const Polyomino * mother, const vector<Polyomino *> & children, const int scaling);
};

#endif

