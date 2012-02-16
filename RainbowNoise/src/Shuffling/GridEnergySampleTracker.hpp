/*
  GridEnergySampleTracker.hpp

  Li-Yi Wei
  09/03/2009

*/

#ifndef _GRID_ENERGY_SAMPLE_TRACKER_HPP
#define _GRID_ENERGY_SAMPLE_TRACKER_HPP

#include <vector>
using namespace std;

#include "EnergySampleTracker.hpp"
#include "Domain.hpp"
#include "Grid.hpp"
#include "Array.hpp"

class GridEnergySampleTracker : public EnergySampleTracker
{
public:
    typedef Grid::DomainSpec GridSpec;

public:
    // class_id_min/max specify the range of class_ids that could be stored here
    GridEnergySampleTracker(const GridSpec & domain_spec, const int class_id_min, const int class_id_max);
    virtual ~GridEnergySampleTracker(void);

    virtual int Set(const vector<const EnergySample *> & samples);

    virtual const EnergySample * Peak(void) const;
    virtual const EnergySample * Trough(const EnergySample & peak) const;

    virtual int Trough(Sample & sample) const;

    // compute the desired grid size
    // scale is for conservatism: default is 1
    static vector<int> GridSize(const int dimension, const int num_samples_per_class, const int scale);

protected:
    virtual int PlusMinus(const EnergySample & sample, const PlusMinusOption plus_or_minus);

    // map from external class id to internal index
    int MapClassID(const int external_class_id) const;

    int FindCell(const EnergySample & query, vector<int> & cell_index) const;
    int FindCell(const Sample & query, vector<int> & cell_index) const;

protected:
    // const Domain & _domain;

    const int _class_id_min;
    const int _class_id_max;

    // binned by internal class id
    typedef vector< vector<const EnergySample *> > GridElement;

    Array<GridElement> _array;
    const float _cell_size;

    struct IndexElement
    {
        vector<int> index;
        
        int operator<(const IndexElement & rhs) const;
    };

    // available cells per (internal) class id
    vector< vector<IndexElement> > _pools;

    mutable Sample _sample;
    mutable vector<int> _cell_index;

    mutable IndexElement _index_element;
};

#endif
