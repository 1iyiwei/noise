/*
  GHexominoSubdivider.hpp

  implement the subdividing rule for 9^2-rep g-hexomino
  
  Li-Yi Wei
  11/12/2007

*/

#ifndef _G_HEXOMINO_SUBDIVIDER_HPP
#define _G_HEXOMINO_SUBDIVIDER_HPP

#include "GHexomino.hpp"
#include "PolyominoSubdivider.hpp"

class GHexominoSubdivider : public PolyominoSubdivider
{
public:
    static void Subdivide(const GHexomino * mother, vector<GHexomino *> & children);
    
protected:
    static void SubdivideCanonical(const GHexomino * mother, vector<GHexomino *> & children);
    static void Reflect(const GHexomino * mother, const vector<GHexomino *> & children);
    static void Rotate(const GHexomino * mother, const vector<GHexomino *> & children);
    static void Shift(const GHexomino * mother, const vector<GHexomino *> & children);

    struct TileRecord
    {
        int row, col;
        Polyomino::Orientation orientation;
        Polyomino::Reflection reflection;
    };
};

#endif
