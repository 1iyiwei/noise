/*
  PhaseGroup.cpp

  Li-Yi Wei
  07/25/2007

*/

#include <math.h>

#include "PhaseGroup.hpp"
#include "Random.hpp"
#include "SequentialCounter.hpp"
#include "OnionLayerCounter.hpp"

int PhaseGroup::Compute(const Order order, KDTree & tree, const int level, vector< vector<KDTree::Cell *> > & phase_groups)
{
    
    if(order == SEQUENTIAL_RANDOM)
    {
        return ComputeSequentialRandom(tree, level, phase_groups);
    }
    else if(order == PARALLEL_ONION)
    {
        return ComputeParallelOnion(tree, level, phase_groups);
    }
    else if(order == PARALLEL_RANDOM_TEMPLATE)
    {
        return ComputeParallelRandomTemplate(tree, level, phase_groups);
    }
    else
    {
        return 0;
    }
}

int PhaseGroup::ComputeSequentialRandom(KDTree & tree, const int level, vector< vector<KDTree::Cell *> > & phase_groups)
{
    vector<KDTree::Cell *> cells;
    
    if(! tree.GetCells(level, cells))
    {
        return 0;
    }
    
    // randomize into one phase group
    phase_groups.clear();
    phase_groups = vector< vector<KDTree::Cell *> >(1);

    vector<KDTree::Cell *> input_cells = cells;

    while(input_cells.size() > 0)
    {
        const int index = static_cast<int>(floor(Random::UniformRandom()*(input_cells.size()-1)));
        
        phase_groups[0].push_back(input_cells[index]);
        input_cells[index] = input_cells[input_cells.size()-1];
        input_cells.pop_back();
    }
        
    // done
    return 1;
}

int PhaseGroup::ComputeParallelOnion(KDTree & tree, const int level, vector< vector<KDTree::Cell *> > & phase_groups)
{
    const int clique_size = static_cast<int>(ceil(sqrt(tree.ConflictRadiusSquare())));

    // use smart tricks to ensure that OnionLayer would work for both even and odd clique_size. For the even case, the integer lattice locates at corners. Ror the odd case, the integer lattice locates at centers.
    const float clique_radius = (clique_size+1)*0.5*sqrt(tree.Dimension()*1.0);
    const int clique_center = clique_size/2;
    
    OnionLayerCounter counter(tree.Dimension(), clique_radius*clique_radius);

    vector< vector<int> > clique_postal;
    vector<int> value;
    counter.Reset();

    do
    {
        counter.Get(value);

        int inside_clique = 1;
        for(unsigned int i = 0; i < value.size(); i++)
        {
            value[i] += clique_center;
            if((value[i] < 0) || (value[i] >= clique_size))
            {
                inside_clique = 0;
                break;
            }
        }

        if(inside_clique)
        {
            clique_postal.push_back(value);
        }
    }
    while(counter.Next());

    // build the mapping table from sequential to onion order
    int mapping_table_size = 1;
    {
        for(int i = 0; i < tree.Dimension(); i++)
        {
            mapping_table_size *= clique_size;
        }
    }

    vector<int> mapping_table(mapping_table_size);
    {
        SequentialCounter sequential_counter(tree.Dimension(), 0, clique_size-1);

        sequential_counter.Reset();

        do
        {
            sequential_counter.Get(value);
            const int mapping_table_entry = MappingTableEntry(clique_size, value);

            if((mapping_table_entry < 0) || (mapping_table_entry >= mapping_table.size()))
            {
                return 0;
            }

            int onion_index = -1;
            for(unsigned int i = 0; i < clique_postal.size(); i++)
            {
                int same = 1;

                for(unsigned int j = 0; j < value.size(); j++)
                {
                    if(value[j] != clique_postal[i][j])
                    {
                        same = 0;
                        break;
                    }
                }

                if(same)
                {
                    onion_index = i;
                }
            }

            if(onion_index < 0)
            {
                return 0;
            }

            mapping_table[mapping_table_entry] = onion_index;
        }
        while(sequential_counter.Next());
    }
    
    // build the actual phase group
    phase_groups.clear();
    phase_groups = vector< vector<KDTree::Cell *> >(clique_postal.size());

    vector<KDTree::Cell *> cells;
    
    if(! tree.GetCells(level, cells))
    {
        return 0;
    }

    for(unsigned int i = 0; i < cells.size(); i++)
    {
        assert(cells[i]);
        cells[i]->GetIndex(value);
        const int mapping_table_entry = MappingTableEntry(clique_size, value);

        phase_groups[mapping_table[mapping_table_entry]].push_back(cells[i]);
    }
    
    // done
    return 1;
}

int PhaseGroup::ComputeParallelRandomTemplate(KDTree & tree, const int level, vector< vector<KDTree::Cell *> > & phase_groups)
{
    const int scale_factor = 2;
    const int clique_size = static_cast<int>(ceil(sqrt(tree.ConflictRadiusSquare())));
    const int template_size = scale_factor*clique_size;

    // randomization
    SequentialCounter counter(tree.Dimension(), 0, template_size-1);
    counter.Reset();

    vector<int> value(tree.Dimension());

    vector<int> mapping_table;
    
    do
    {
        counter.Get(value);
        mapping_table.push_back(MappingTableEntry(template_size, value));
    }
    while(counter.Next());

    {
        vector<int> mapping_table_randomized;

        while(mapping_table.size() > 0)
        {
            const int selection = static_cast<int>(floor((Random::UniformRandom())*(mapping_table.size()-1)));

            mapping_table_randomized.push_back(mapping_table[selection]);
            mapping_table[selection] = mapping_table[mapping_table.size()-1];
            mapping_table.pop_back();
        }

        mapping_table = mapping_table_randomized;
    }
    
    // build the actual phase group
    phase_groups.clear();
    phase_groups = vector< vector<KDTree::Cell *> >(mapping_table.size());

    vector<KDTree::Cell *> cells;
    
    if(! tree.GetCells(level, cells))
    {
        return 0;
    }

    for(unsigned int i = 0; i < cells.size(); i++)
    {
        assert(cells[i]);
        cells[i]->GetIndex(value);
        const int mapping_table_entry = MappingTableEntry(template_size, value);

        phase_groups[mapping_table[mapping_table_entry]].push_back(cells[i]);
    }
    
    // done
    return 1;
}

int PhaseGroup::MappingTableEntry(const int template_size, const vector<int> & index)
{
    int entry = 0;

    for(unsigned int i = 0; i < index.size(); i++)
    {
        entry *= template_size;
        entry += index[i]%template_size;
    }

    return entry;
}
