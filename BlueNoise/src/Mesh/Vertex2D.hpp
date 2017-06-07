/*
 * Vertex2D.hpp
 *
 * 2d vertices
 *
 * Li-Yi Wei
 * 9/8/00
 * 10/30/07 modified
 */

#ifndef _VERTEX2D_HPP
#define _VERTEX2D_HPP

#include "Vertex.hpp"

class Vertex2D : public Vertex
{
public:
    Vertex2D(void);
    Vertex2D(const float x, const float y);
    ~Vertex2D(void);

    float x, y;
};

#endif
