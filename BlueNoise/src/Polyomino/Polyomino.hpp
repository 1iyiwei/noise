/*
  Polyomino.hpp

  Li-Yi Wei
  10/21/2007

*/

#ifndef _POLYOMINO_HPP
#define _POLYOMINO_HPP

#include <vector>
using namespace std;

class Polyomino
{
public:

    // first entry are for null (e.g. monomino)
    typedef enum {O0, ON, OE, OS, OW} Orientation;
    // reflection axis: vertical through center in canonical tile
    typedef enum {R0, R1, R2} Reflection;
    
    struct Int2
    {
        Int2(void) : row(0), col(0) {};
        Int2(const int rowi, const int coli) : row(rowi), col(coli) {};
        int row, col;
    };

    struct Flt2
    {
        Flt2(void) : row(0), col(0) {};
        Flt2(const float rowi, const float coli) : row(rowi), col(coli) {};
        float row, col;
    };
    
public:
    Polyomino(void);
    Polyomino(const Int2 & center);
    Polyomino(const Int2 & center, const Flt2 & sample_offset);
    Polyomino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    Polyomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    virtual ~Polyomino(void) = 0;

    virtual Polyomino * Clone(void) const;

    virtual int NumSubdivisionRules(void) const;
    virtual int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    // get the list of square cells occupied by this polyomino
    // in local coordinate system
    virtual void GetCells(vector<Int2> & cells) const;

    // providing a general base class implementation but not very efficient
    virtual int IsInside(const Flt2 & sample_offset) const;

    void GetCenter(Int2 & center) const;
    void SetCenter(const Int2 & center);
    
    void GetOrientation(Orientation & orientation) const;
    void SetOrientation(const Orientation & orientation);
    
    void GetReflection(Reflection & reflection) const;
    void SetReflection(const Reflection & reflection);

    void GetOffset(Flt2 & sample_offset) const;
    void SetOffset(const Flt2 & sample_offset); // no insideness checking

    // all reflections and rotation are with respect to global (0,0)
    // so the center of each polyomino is taken into account
    virtual void ReflectRow(void); // horizontal reflection with x-axix
    virtual void ReflectCol(void); // vertical reflection with y-axis
    virtual void Rotate90(void); // rotate 90 counter-clock-wise
    virtual void Rotate180(void); // rotate 180 counter-clock-wise
    virtual void Rotate270(void); // rotate 270 counter-clock-wise
    
    struct Neighbor
    {
        Neighbor(void) : polyomino(0) {};
        Neighbor(const Polyomino * polyomino_i, const Int2 & offset_i) : polyomino(polyomino_i), offset(offset_i) {};
        
        const Polyomino * polyomino;
        Int2 offset; // diff of neighbor's center to my center in myoptic view
    };
    
    void ClearNeighborList(void);
    void AddNeighbor(const Neighbor & neighbor);
    int NumNeighbors(void) const;
    Neighbor GetNeighbor(const int which) const;

    // uniform random sample within the polyomino cells
    Flt2 RandomOffset(void) const;
    // given an offset, compute the nearest offset that is inside the polyomino
    Flt2 PullInOffset(const Flt2 & input) const;

protected:

    // for internal tile construction
    typedef enum {O0_R0, ON_R1, OE_R1, OS_R1, OW_R1, OE_R2, OS_R2, OW_R2, ON_R2} OrientationReflection;
    
    Int2 _center;
    Orientation _orientation;
    Reflection _reflection;
    
    // in local (unscaled but rotated) coordinate frame of grid units
    // i.e. same unit as center
    Flt2 _sample_offset;

    vector<Neighbor> _neighbors;
};

#endif
