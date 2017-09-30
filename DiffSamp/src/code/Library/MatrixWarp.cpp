/*
  MatrixWarp.cpp

  Li-Yi Wei
  10/15/2010

*/

#include "MatrixWarp.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

MatrixWarp::MatrixWarp(const Matrix & forward) : _forward(forward), _inverse(forward)
{
    if(! forward.Inverse(_inverse))
    {
        throw Exception("MatrixWarp::MatrixWarp(): cannot invert matrix!");
    }
}

MatrixWarp::MatrixWarp(const Matrix & forward, const Matrix & inverse) : _forward(forward), _inverse(inverse)
{
    // nothing else to do
}

MatrixWarp::~MatrixWarp(void)
{
    // nothing else to do
}

int MatrixWarp::ForwardMap(const Coordinate & source, Coordinate & target) const
{
    return Map(_forward, source, target);
}

int MatrixWarp::InverseMap(const Coordinate & source, Coordinate & target) const
{
    return Map(_inverse, source, target);
}

int MatrixWarp::ForwardJacobian(const Coordinate & query, Matrix & jacobian) const
{
    Coordinate inverse_query;
    if(! InverseMap(query, inverse_query) ) return 0;

    return Jacobian(_forward, inverse_query, jacobian);
}

int MatrixWarp::InverseJacobian(const Coordinate & query, Matrix & jacobian) const
{
    return Jacobian(_inverse, query, jacobian);
}

int MatrixWarp::Map(const Matrix & map, const Coordinate & source, Coordinate & target) const
{
    /* static */ Coordinate source1, target1;

    PushBack(source, 1, source1);

    if(map.Size(1) != source1.Dimension())
    {
        throw Exception("MatrixWarp::Map(): map.Size(1) != source1.Dimension()");
        return 0;
    }

    if(! map.Multiply(source1, target1)) 
    {
        return 0;
    }
    
    if((target1.Dimension() <= 0) || (target1[target1.Dimension()-1] == 0))
    {
        return 0;
    }

    const float last = 1.0/target1[target1.Dimension()-1];
    for(int i = 0; i < target1.Dimension(); i++)
    {
        target1[i] *= last;
    }

    if(! PopBack(target1, target))
    {
        return 0;
    }

    return 1;
}

int MatrixWarp::Jacobian(const Matrix & map, const Coordinate & query, Matrix & jacobian) const
{
    /*static*/ Coordinate source1, target1;

    PushBack(query, 1, source1);

    if(! map.Multiply(source1, target1)) 
    {
        return 0;
    }

    if((target1.Dimension() <= 0) || (target1[target1.Dimension()-1] == 0))
    {
        return 0;
    }
    
    const float last = 1/target1[target1.Dimension()-1];
    const float last2 = last*last;

    int wrong_size = 0;
    for(int i = 0; i < map.Dimension(); i++)
    {
        if(map.Size(i) != (jacobian.Size(i) + 1))
        {
            wrong_size = 1;
        }
    }

    if(wrong_size)
    {
        vector<int> size;
        map.Size(size);

        jacobian = Matrix(Utility::Minus1(size));
    }

    vector<int> index(2);
    vector<int> map_homo_index(2);
    float value = 0;
    float map_homo_value = 0;
    for(index[0] = 0; index[0] < jacobian.Size(0); index[0]++)
        for(index[1] = 0; index[1] < jacobian.Size(1); index[1]++)
        {
            if(! map.Get(index, value)) throw Exception("MatrixWarp::Jacobian(): cannot get map value!");

            map_homo_index[0] = map.Size(0)-1;
            map_homo_index[1] = index[1];
            
            if(! map.Get(map_homo_index, map_homo_value)) throw Exception("MatrixWarp::Jacobian(): cannot get map homo value!");

            value = last*value - last2*target1[index[0]]*map_homo_value;

            if(! jacobian.Put(index, value)) throw Exception("MatrixWarp::Jacobian(): cannot put jacobian value!");
        }
    
    return 1;
}

void MatrixWarp::PushBack(const Coordinate & source, const CoordinateType tail, Coordinate & target)
{
    if(target.Dimension() != (source.Dimension() + 1))
    {
        target = Coordinate(source.Dimension() + 1);
    }

    for(int i = 0; i < target.Dimension()-1; i++)
    {
        target[i] = source[i];
    }

    target[target.Dimension()-1] = tail;
}

int MatrixWarp::PopBack(const Coordinate & source, Coordinate & target)
{
    if(source.Dimension() <= 0) return 0;

    if(target.Dimension() != (source.Dimension()-1))
    {
        target = Coordinate(source.Dimension()-1);
    }

    for(int i = 0; i < target.Dimension(); i++)
    {
        target[i] = source[i];
    }

    return 1;
}
