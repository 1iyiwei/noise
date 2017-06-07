/*
 * Face2D.hpp
 *
 * a 2D polygon/face
 *
 * Li-Yi Wei
 * 9/9/00
 * 10/30/2007 modified
 */

#ifndef _FACE_2D_HPP
#define _FACE_2D_HPP

#include "GenericFace.hpp"
#include "Vertex2D.hpp"

typedef GenericFace<Vertex2D> Face2D;

/*
class Face2D : public Face
{
public:
  Face2D(void);
  Face2D(const PlyElement & plyElement) throw(Exception);
  virtual ~Face2D(void);

  // get the list of vertices surrounding this face in CCW ordering
  int GetVertices(Array1D<const Vertex2D *> & vertices) const;

protected:
};
*/

#endif
