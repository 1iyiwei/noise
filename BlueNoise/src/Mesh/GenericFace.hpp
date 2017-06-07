/*
 * GenericFace.hpp
 *
 * a generic class for polygon faces
 *
 * Li-Yi Wei
 * 4/3/01
 * 10/30/07 modified
 */

#ifndef _GENERIC_FACE_HPP
#define _GENERIC_FACE_HPP

#include <vector>
using namespace std;

#include "Face.hpp"

template<class Vertex>
class GenericFace : public Face
{
public:
    GenericFace(void);
    virtual ~GenericFace(void);

    // get the list of vertices surrounding this face in CCW ordering
    int GetVertices(vector<const Vertex *> & vertices) const;

protected:
};

template<class Vertex>
GenericFace<Vertex>::GenericFace(void)
{
    // nothing to do
}

template<class Vertex>
GenericFace<Vertex>::~GenericFace(void)
{
    // nothing to do
}

template<class Vertex>
int GenericFace<Vertex>::GetVertices(vector<const Vertex *> & vertices) const
{ 
    return Face::GetVertices(vertices);
}

#endif
