/*
  HaarWarp.cpp

  Li-Yi Wei
  12/30/2010

*/

#include <vector>
using namespace std;

#include <math.h>

#include "HaarWarp.hpp"
#include "SequentialCounter.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

HaarWarp::HaarWarp(const Array<float> & importance, const float cell_size)
{
    if(! BuildPyramid(importance, cell_size, _pyramid)) throw Exception("HaarWarp::HaarWarp(): cannot build pyramid");
}

HaarWarp::~HaarWarp(void)
{
    // nothing else to do
}

int HaarWarp::ForwardMap(const Coordinate & source, Coordinate & target) const
{
    target = source;

    for(unsigned int i = 0; i < _pyramid.size(); i++)
    {
        if(! ForwardMap(_pyramid[i], target, target))
        {
            return 0;
        }
    }

    return 1;
}

int HaarWarp::InverseMap(const Coordinate & source, Coordinate & target) const
{
    throw Exception("HaarWarp::InverseMap(): not yet done");
    return 0;
}
    
int HaarWarp::ForwardJacobian(const Coordinate & query, Matrix & jacobian) const
{
    throw Exception("HaarWarp::ForwardJacobian(): not yet done");
    return 0;
}

int HaarWarp::InverseJacobian(const Coordinate & query, Matrix & jacobian) const
{
    throw Exception("HaarWarp::InverseJacobian(): not yet done");
    return 0;
}

int HaarWarp::BuildPyramid(const Array<float> & importance, const float cell_size, deque<Level> & pyramid)
{
    // make sure importance is a power-of-2 hypercube
    for(int i = 0; i < importance.Dimension(); i++)
    {
        if(importance.Size(i) != importance.Size(0))
        {
            return 0;
        }
    }

    if(importance.Dimension() <= 0) return 0;

    int size_1d = importance.Size(0);

    if(size_1d <= 0) return 0;

    while(size_1d > 1)
    {
        if(size_1d%2) return 0;

        size_1d /= 2;
    }

    // clear
    pyramid.clear();

    // build
    Level level;

    level.data = importance;
    level.cell_size = cell_size;
    vector<int> data_size;

    const int dimension = importance.Dimension();

    while(level.data.Size(0) > 1)
    {
        pyramid.push_front(level);

        level.data.Size(data_size);

        SequentialCounter counter(dimension, vector<int>(dimension, 0), Utility::Minus1(data_size));

        // new data for next coarser level
        for(unsigned int i = 0; i < data_size.size(); i++)
        {
            data_size[i] /= 2;
        }

        Array<float> new_data(data_size);
        new_data.Clear(0);

        vector<int> index(dimension);
        float fine_value = 0;
        float coarse_value = 0;
        counter.Reset();

        do
        {
            counter.Get(index);
            
            if(! level.data.Get(index, fine_value)) throw Exception("HaarWarp::BuildPyramid(): cannot get fine value");

            for(unsigned int i = 0; i < index.size(); i++)
            {
                index[i] /= 2;
            }

            if(! new_data.Get(index, coarse_value)) throw Exception("HaarWarp::BuildPyramid(): cannot get coarse value");

            coarse_value += fine_value;

            if(! new_data.Put(index, coarse_value)) throw Exception("HaarWarp::BuildPyramid(): cannot put coarse value");
        }
        while(counter.Next());

        // update
        level.data = new_data;
        level.cell_size = cell_size/pow(2.0, 1.0*pyramid.size());
    }

    // done
    return 1;
}

int HaarWarp::ForwardMap(const Level & level, const Coordinate & source, Coordinate & target) const
{
    if(level.data.Dimension() != source.Dimension())
    {
        throw Exception("HaarWarp::ForwardMap(): dimension mismatch");
        return 0;
    }

    target = source;

    vector<int> index(target.Dimension());
    for(unsigned int i = 0; i < index.size(); i++)
    {
        index[i] = static_cast<int>(floor(target[i]/level.cell_size));

        if(index[i] < 0) index[i] = 0;
        if(index[i] >= level.data.Size(i)) index[i] = level.data.Size(i)-1;
    }

    // move individual dimension of the target coordinate
    for(int dim = 0; dim < level.data.Dimension(); dim++)
    {
        // determine dividing line
        const float source_line = Divider(level, index, dim);
        const float target_line = 1.0;
        const int lo_index = index[dim]/2*2;
        const float lo_my_line = target[dim]/level.cell_size - lo_index;

        // move sample
        if(lo_my_line < source_line)
        {
            target[dim] = lo_my_line*target_line/source_line + lo_index;
        }
        else
        {
            target[dim] = -(2-lo_my_line)*(2-target_line)/(2-source_line) + lo_index + 2;
        }

        target[dim] *= level.cell_size;

        // update cell index
        index[dim] = static_cast<int>(floor(target[dim]/level.cell_size));
    }

    // done
    return 1;
}

float HaarWarp::Divider(const Level & level, const vector<int> & query_index, const int which_dim) const
{
    if(level.data.Dimension() != query_index.size()) throw Exception("HaarWarp::Divider(): dimension mismatch");

    if((which_dim < 0) || (which_dim >= level.data.Dimension())) throw Exception("HaarWarp::Divider(): illegal dim");

    vector<int> lo_index(query_index), hi_index(query_index);
    for(unsigned int i = which_dim; i < lo_index.size(); i++)
    {
        lo_index[i] = query_index[i]/2*2;
        hi_index[i] = lo_index[i] + 1;
    }
    
    SequentialCounter counter(level.data.Dimension(), lo_index, hi_index);

    vector<float> sums(2, 0.0);

    vector<int> index(query_index);
    float value = 0;
    counter.Reset();
    do
    {
        counter.Get(index);

        if(! level.data.Get(index, value)) throw Exception("HaarWarp::Divider(): cannot get value");

        sums[index[which_dim]%2] += value;
    }
    while(counter.Next());

    const float total = sums[0] + sums[1];

    if(total <= 0) throw Exception("HaarWarp::Divider(): zero sum");

    return 2*sums[0]/total;
}
