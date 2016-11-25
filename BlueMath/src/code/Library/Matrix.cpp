/*
  Matrix.cpp

  Li-Yi Wei
  10/15/2010

*/

#include <math.h>

#include "Matrix.hpp"
#include "Exception.hpp"

Matrix::Matrix(void) : Array<CoordinateType>(vector<int>(MatrixDimension(), 0))
{
    // nothing else to do
}

Matrix::Matrix(const vector<int> & spec) : Array<CoordinateType>(spec)
{
    if(spec.size() != MatrixDimension())
    {
        throw Exception("Matrix::Matrix(): has to be an 2D array!");
    }
}

Matrix::Matrix(const Array<CoordinateType> & rhs) : Array<CoordinateType>(rhs)
{
    if(Dimension() != MatrixDimension())
    {
        throw Exception("Matrix::Matrix(): has to be an 2D array!");
    }
}

Matrix::~Matrix(void)
{
    // nothing else to do
}

int Matrix::MatrixDimension(void)
{
    return 2;
}

int Matrix::Add(const Matrix & rhs_in, Matrix & output) const
{
    Matrix rhs(rhs_in); // in case rhs_in and result are the same

    if((rhs.Size(0) != Size(0)) || (rhs.Size(1) != Size(1)))
    {
        throw Exception("Matrix::Add(): size mismatch");
        return 0;
    }

    // make sure the output is of right size
    if((output.Size(0) != Size(0)) || (output.Size(1) != Size(1)))
    {
        vector<int> size(MatrixDimension());
        size[0] = Size(0);
        size[1] = Size(1);

        output = Matrix(size);
    }

    vector<int> index(MatrixDimension());
    float value, value_rhs;

    for(index[0] = 0; index[0] < Size(0); index[0]++)
        for(index[1] = 0; index[1] < Size(1); index[1]++)
        {
            if(! Get(index, value)) throw Exception("Matrix::Add(): cannot get element");
            if(! rhs.Get(index, value_rhs)) throw Exception("Matrix::Add(): cannot get element");

            if(! output.Put(index, value + value_rhs)) throw Exception("Matrix::Add(): cannot put element");
        }

    return 1;
}

int Matrix::Multiply(const Coordinate & rhs_in, Coordinate & result) const
{
    Coordinate rhs(rhs_in); // in case rhs_in and result are the same

    if(rhs.Dimension() != Size(1))
    {
        throw Exception("Matrix::Multiply(): rhs.Dimension() != Size(1)");
        return 0;
    }

    if(result.Dimension() != Size(0)) result = Coordinate(Size(0));

    vector<int> index(MatrixDimension());
    CoordinateType value;

    for(index[0] = 0; index[0] < result.Dimension(); index[0]++)
    {
        result[index[0]] = 0;

        for(index[1] = 0; index[1] < rhs.Dimension(); index[1]++)
        {
            if(! Get(index, value)) throw Exception("Matrix::Multiply(): cannot get value");

            result[index[0]] += value * rhs[index[1]];
        }
    }

    return 1;
}

int Matrix::Multiply(const Matrix & rhs_in, Matrix & output) const
{
    Matrix rhs = rhs_in;

    // check
    if(Size(1) != rhs.Size(0))
    {
        return 0;
    }

    // make sure the output is of right size
    if((output.Size(0) != Size(0)) || (output.Size(1) != rhs.Size(1)))
    {
        vector<int> size(MatrixDimension());
        size[0] = Size(0);
        size[1] = rhs.Size(1);

        output = Matrix(size);
    }

    // compute
    vector<int> index(MatrixDimension()), index1(MatrixDimension()), index2(MatrixDimension());
    float value, value1, value2;
    for(index[0] = 0; index[0] < output.Size(0); index[0]++)
    {
        for(index[1] = 0; index[1] < output.Size(1); index[1]++)
        {
            value = 0;
            for(int k = 0; k < Size(1); k++)
            {
                index1 = index; index1[1] = k;
                if(! Get(index1, value1)) throw Exception("Matrix::Multiply(): cannot get my element");

                index2 = index; index2[0] = k;
                if(! rhs.Get(index2, value2)) throw Exception("Matrix::Multiply(): cannot get rhs element");

                value += value1*value2;
            }

            if(! output.Put(index, value))  throw Exception("Matrix::Multiply(): cannot put output element");
        }
    }

    // done
    return 1;
}

int Matrix::Multiply(const CoordinateType c)
{
    vector<CoordinateType> values;
    if(!Get(values)) return 0;
    
    for(unsigned int i = 0; i < values.size(); i++)
    {
        values[i] *= c;
    }

    if(!Put(values)) return 0;

    return 1;
}

int Matrix::Multiply(const CoordinateType c, Matrix & result) const
{
    result = *this;

    return result.Multiply(c);
}

int Matrix::Identity(void)
{
    if((Size(0) <= 0) || (Size(0) != Size(1))) return 0;

    Clear(0);

    for(int i = 0; i < Size(0); i++)
    {
        if(! Put(vector<int>(MatrixDimension(), i), 1.0)) throw Exception("Matrix::Identity(): cannot put 1 into diagonal element");
    }

    return 1;
}

int Matrix::Determinant(CoordinateType & det) const
{
    if(Size(0) != Size(1))
    {
        // det only makes sense for square matrix
        return 0;
    }
    else
    {
        Convert(*this, _m);
    
        if(Size(0) == 1)
        {
            det = _m[0][0];
        }
        else if(Size(0) == 2)
        {
            det = _m[0][0]*_m[1][1] - _m[0][1]*_m[1][0];
        }
        else if(Size(0) == 3)
        {
            det = 
                _m[0][0]*(_m[1][1]*_m[2][2] - _m[1][2]*_m[2][1]) +
                _m[0][1]*(_m[1][2]*_m[2][0] - _m[1][0]*_m[2][2]) +
                _m[0][2]*(_m[1][0]*_m[2][1] - _m[1][1]*_m[2][0]);
        }
        else
        {
            throw Exception("Matrix::Determinant(): matrix size not yet handled");
        }

        return 1;
    }
}

int Matrix::Inverse(Matrix & inverse) const
{
    if(Size(0) != Size(1))
    {
        // inv only makes sense for square matrix
        return 0;
    }
    else
    {
        float det = 0;
        if(! Determinant(det))
        {
            return 0;
        }

        if(det == 0)
        {
            return 0;
        }

        const float det_inv = 1.0/det;

        Convert(*this, _m);
        _inv = _m;

        if(Size(0) == 1)
        {
            _inv[0][0] = det_inv;
        }
        else if(Size(0) == 2)
        {
            _inv[0][0] =  _m[1][1]*det_inv;
            _inv[0][1] = -_m[0][1]*det_inv;
            _inv[1][0] = -_m[1][0]*det_inv;
            _inv[1][1] =  _m[0][0]*det_inv;
        }
        else if(Size(0) == 3)
        {
            _inv[0][0] = (_m[1][1]*_m[2][2] - _m[1][2]*_m[2][1])*det_inv;
            _inv[0][1] = (_m[0][2]*_m[2][1] - _m[0][1]*_m[2][2])*det_inv;
            _inv[0][2] = (_m[0][1]*_m[1][2] - _m[0][2]*_m[1][1])*det_inv;
            _inv[1][0] = (_m[1][2]*_m[2][0] - _m[1][0]*_m[2][2])*det_inv;
            _inv[1][1] = (_m[0][0]*_m[2][2] - _m[0][2]*_m[2][0])*det_inv;
            _inv[1][2] = (_m[0][2]*_m[1][0] - _m[0][0]*_m[1][2])*det_inv;
            _inv[2][0] = (_m[1][0]*_m[2][1] - _m[1][1]*_m[2][0])*det_inv;
            _inv[2][1] = (_m[0][1]*_m[2][0] - _m[0][0]*_m[2][1])*det_inv;
            _inv[2][2] = (_m[0][0]*_m[1][1] - _m[0][1]*_m[1][0])*det_inv;
        }
        else
        {
            throw Exception("Matrix::Inverse(): matrix size not yet handled");
        }

        Convert(_inv, inverse);

        return 1;
    }
}

int Matrix::Transpose(Matrix & transpose) const
{
    if((Size(0) != transpose.Size(1)) || (Size(1) != transpose.Size(0)))
    {
        vector<int> size(MatrixDimension());
        size[0] = Size(1);
        size[1] = Size(0);

        transpose = Matrix(size);
    }

    vector<int> index(MatrixDimension());
    vector<int> tindex(MatrixDimension());
    float value = 0;

    for(tindex[0] = 0; tindex[0] < transpose.Size(0); tindex[0]++)
        for(tindex[1] = 0; tindex[1] < transpose.Size(1); tindex[1]++)
        {
            index[0] = tindex[1];
            index[1] = tindex[0];

            if(! Get(index, value)) return 0;
            if(! transpose.Put(tindex, value)) return 0;
        }

    return 1;
}

// from http://mathworld.wolfram.com/Eigenvalue.html
int Matrix::EigenValue(vector<CoordinateType> & values) const
{
    if(Size(0) != Size(1))
    {
        return 0;
    }
    
    Convert(*this, _m);

    if(Size(0) == 2)
    {
        if(values.size() != 2) values = vector<CoordinateType>(2);

        const float diag_sum = _m[0][0] + _m[1][1];
        const float diag_diff = _m[0][0] - _m[1][1];
        const float core = 4*_m[0][1]*_m[1][0] + diag_diff*diag_diff;

        const float sqrt_core = sqrt(fabs(core)); // hmm how should I handle non-elliptical situation? use fabs for now as hack!

        values[0] = 0.5*(diag_sum + sqrt_core);
        values[1] = 0.5*(diag_sum - sqrt_core);
      
        if(core < 0) return 0;
    }
    else
    {
        throw Exception("Matrix::EigenValue(): matrix size not yet handled");
    }

    return 1;
}

CoordinateType Matrix::EigenProduct(void) const
{
    if(Size(0) != Size(1))
    {
        return 0;
    }
    
    Convert(*this, _m);

    float value = 0;

    if(Size(0) == 2)
    {
        value = _m[0][0]*_m[1][1] - _m[0][1]*_m[1][0];
    }
    else
    {
        throw Exception("Matrix::EigenValue(): matrix size not yet handled");
    }

    return value;
}

void Matrix::Convert(const Matrix & input, vector< vector<float> > & output)
{
    if(output.size() != input.Size(0))
    {
        output = vector< vector<float> > (input.Size(0));
    }

    for(unsigned int i = 0; i < output.size(); i++)
    {
        if(output[i].size() != input.Size(1))
        {
            output[i] = vector<float>(input.Size(1));
        }
    }

    vector<int> index(2);

    for(index[0] = 0; index[0] < output.size(); index[0]++)
    {
        for(index[1] = 0; index[1] < output[index[0]].size(); index[1]++)
        {
            if(! input.Get(index, output[index[0]][index[1]])) throw Exception("Matrix::Convert(): cannot get input element");
        }
    }
}

void Matrix::Convert(const vector< vector<float> > & input, Matrix & output)
{
    if((output.Size(0) != input.size()) || (output.Size(1) != input[0].size()))
    {
        vector<int> size(MatrixDimension());
        size[0] = input.size();
        size[1] = input[0].size();

        output = Matrix(size);
    }

    vector<int> index(MatrixDimension());

    for(index[0] = 0; index[0] < output.Size(0); index[0]++)
    {
        for(index[1] = 0; index[1] < output.Size(1); index[1]++)
        {
            if(! output.Put(index, input[index[0]][index[1]])) throw Exception("Matrix::Convert(): cannot put output element");
        }
    }
}
