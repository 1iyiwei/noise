/*
  PPentominoSubdivider.hpp

  implement the 2 subdividing rules for 2^2-rep p-pentomino
  
  Li-Yi Wei
  10/27/2007

*/

#ifndef _P_PENTOMINO_SUBDIVIDER_HPP
#define _P_PENTOMINO_SUBDIVIDER_HPP

#include "PPentomino.hpp"
#include "PolyominoSubdivider.hpp"

class PPentominoSubdivider : public PolyominoSubdivider
{
public:
    static void Subdivide(const PPentomino * mother, vector<PPentomino *> & children, const int which_rule);
    
protected:
    static void SubdivideCanonical(const PPentomino * mother, vector<PPentomino *> & children, const int which_rule);
    static void Reflect(const PPentomino * mother, const vector<PPentomino *> & children);
    static void Rotate(const PPentomino * mother, const vector<PPentomino *> & children);
    static void Shift(const PPentomino * mother, const vector<PPentomino *> & children);
};

#endif
