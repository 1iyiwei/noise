/*
  MatrixWarp.hpp

  linear warp via matrix multiplication

  Li-Yi Wei
  10/15/2010

*/

#ifndef _MATRIX_WARP_HPP
#define _MATRIX_WARP_HPP

#include "Warp.hpp"
#include "Matrix.hpp"

class MatrixWarp: public Warp
{
public:
    // the matrix is specified in perspective format
    // i.e. one more row/col than the dimension of query coordinate
    MatrixWarp(const Matrix & forward);
    MatrixWarp(const Matrix & forward, const Matrix & backward);
    virtual ~MatrixWarp(void);

    int ForwardMap(const Coordinate & source, Coordinate & target) const;
    int InverseMap(const Coordinate & source, Coordinate & target) const;
    
    int ForwardJacobian(const Coordinate & query, Matrix & jacobian) const;
    int InverseJacobian(const Coordinate & query, Matrix & jacobian) const;

protected:
    int Map(const Matrix & map, const Coordinate & source, Coordinate & target) const;

    int Jacobian(const Matrix & map, const Coordinate & query, Matrix & jacobian) const;

    static void PushBack(const Coordinate & source, const CoordinateType tail, Coordinate & target);
    static int PopBack(const Coordinate & source, Coordinate & target);

protected:
    Matrix _forward, _inverse;
};      

#endif
