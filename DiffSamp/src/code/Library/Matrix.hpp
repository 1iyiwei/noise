/*
  Matrix.hpp

  2D array

  Li-Yi Wei
  10/15/2010

*/

#ifndef _MATRIX_HPP
#define _MATRIX_HPP

#include "Array.hpp"
#include "Sample.hpp"

class Matrix : public Array<CoordinateType>
{
public:
    Matrix(void);
    Matrix(const vector<int> & spec);
    Matrix(const Array<CoordinateType> & rhs);
    virtual ~Matrix(void);

    static int MatrixDimension(void);

    virtual int Add(const Matrix & rhs, Matrix & result) const;

    // multiply a *column* vector on the right hand side
    // return 1 if successful, 0 else
    virtual int Multiply(const Coordinate & rhs, Coordinate & result) const;
    virtual int Multiply(const Matrix & rhs, Matrix & result) const;
    virtual int Multiply(const CoordinateType c);
    virtual int Multiply(const CoordinateType c, Matrix & result) const;

    virtual int Identity(void); // make itself identity matrix

    virtual int Determinant(CoordinateType & det) const;
    virtual int Inverse(Matrix & inverse) const;
    virtual int Transpose(Matrix & inverse) const;

    virtual int EigenValue(vector<CoordinateType> & values) const;

    // product of all eigen values
    virtual CoordinateType EigenProduct(void) const;

protected:
    static void Convert(const Matrix & input, vector< vector<float> > & output);

    static void Convert(const vector< vector<float> > & input, Matrix & output);

protected:
    mutable vector< vector<float> > _m, _inv;
};

#endif
