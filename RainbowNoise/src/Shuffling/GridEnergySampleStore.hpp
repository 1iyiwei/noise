/*
  GridEnergySampleStore.hpp

  Li-Yi Wei
  07/08/2009

*/

#ifndef _GRID_ENERGY_SAMPLE_STORE_HPP
#define _GRID_ENERGY_SAMPLE_STORE_HPP

#include <vector>
using namespace std;

#include "EnergySampleStore.hpp"
#include "Domain.hpp"
#include "Grid.hpp"
#include "UniformKernelSize.hpp" // for estimating max neighborhood size
#include "Array.hpp"
#include "SequentialCounter.hpp"

class GridEnergySampleStore : public EnergySampleStore
{
public:
    typedef Grid::DomainSpec GridSpec;

public:
    // kernel_radius_scale: large enough for kernel to be 0; 3 would be good?
    GridEnergySampleStore(const Domain & domain, const GridSpec & domain_spec, const UniformKernelSize & kernel_size, const float kernel_radius_scale);
    ~GridEnergySampleStore(void);
    
    GridEnergySampleStore * Clone(void) const;

    int Set(const vector<EnergySample> & samples);

#if 0 // same as the parent class
    int Get(vector<EnergySample> & samples) const;
    const vector<const EnergySample *> & Get(void) const;
    const vector<EnergySample *> & Get(void);
#endif

    const vector<const EnergySample *> & Get(const Sample & query) const;
    const vector<EnergySample *> & Get(const Sample & query);

    int Plus(const EnergySample & rookie);
    int Minus(const int retiree_index);

    int Move(EnergySample & source, const Sample & target);

protected:
    // managing search radius
    class RadiusStore
    {
    public:
        RadiusStore(const UniformKernelSize & kernel_size, const float kernel_radius_scale);
        
        float Get(const Sample & query) const;

    protected:
        vector<float> _class_radius; // radius per class
        float _radius; // largest value among all
    };

    class Grid
    {
    public:
        Grid(const Domain & domain, const GridSpec & spec);
        ~Grid(void);

        void Clear(void);

        // get neighbors
        const vector<const EnergySample *> & Get(const Sample & query, const float radius) const;
        const vector<EnergySample *> & Get(const Sample & query, const float radius);

    protected:
        const vector<const EnergySample *> & GetNeighbors(const Sample & query, const float radius) const;
        
    public:
        int Plus(const EnergySample * sample);
        int Minus(const EnergySample * sample);

    protected:
        int PlusMinus(const EnergySample * sample, const int plus_or_minus);
        int FindCell(const EnergySample & query, vector<int> & cell_index) const;
        int FindCell(const Sample & query, vector<int> & cell_index) const;

    protected:
        const Domain & _domain;
        typedef vector<const EnergySample *> GridElement;

        Array<GridElement> _array;
        const float _cell_size;

        mutable Sample _sample;
        mutable vector<int> _cell_index;

        mutable SequentialCounter _counter;
        mutable vector<int> _center_index;
        mutable vector<int> _original_index;
        mutable vector<int> _corrected_index;
        mutable vector<const EnergySample *> _element_list;
        mutable Sample _original_neighbor, _corrected_neighbor;

        mutable vector<const EnergySample *> _const_sample_list;
        mutable vector<EnergySample *> _sample_list;
    };

protected:
    // const Domain & _domain;

    // still use the vector from parent
    // but use an additional grid structure for tracking samples
    Grid _grid;

    RadiusStore _radius_store;

    mutable EnergySample _victim; // for Minus()
    mutable Sample _rookie_sample, _victim_sample, _sample;
};

#endif
