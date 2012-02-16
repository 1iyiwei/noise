/*
  MultiTree.cpp

  Li-Yi Wei
  09/17/2008

*/

#include "MultiTree.hpp"
#include "Exception.hpp"

MultiTree::MultiTree(const DistanceField & distance_field, const ConflictChecker & conflict_checker) : _trees(distance_field.NumClasses())
{
    for(unsigned int i = 0; i < _trees.size(); i++)
    {
        _trees[i] = new Tree(distance_field, conflict_checker);
    }
}

MultiTree::~MultiTree(void)
{
    for(unsigned int i = 0; i < _trees.size(); i++)
    {
        delete _trees[i];
    }

    _trees.clear();
}

int MultiTree::NumTrees(void) const
{
    return _trees.size();
}

Tree & MultiTree::GetTree(const int which_tree)
{
    if((which_tree < 0) || (which_tree >= _trees.size()))
    {
        throw Exception("MultiTree::GetTree(): illegal which tree");
    }

    return *_trees[which_tree];
}

int MultiTree::Conflict(const Sample & sample) const
{
    for(unsigned int i = 0; i < _trees.size(); i++)
    {
        // check the containing tree first as it most likely to cause conflict
        if(_trees[(i+sample.id)%_trees.size()]->Conflict(sample, i==0))
        {
            return 1;
        }
    }

    return 0;
}

int MultiTree::GetConflicts(const Sample & sample, vector<const Sample *> & neighbors) const
{
    neighbors.clear();
    
    for(unsigned int i = 0; i < _trees.size(); i++)
    {
        vector<const Sample *> tree_neighbors;
        if(! _trees[i]->GetConflicts(sample, (i == sample.id), tree_neighbors))
        {
            return 0;
        }
        else
        {
            // append
            neighbors.insert(neighbors.end(), tree_neighbors.begin(), tree_neighbors.end());
        }
    }

    return 1;
}

int MultiTree::Add(const Sample & sample)
{
    if((sample.id >= 0) && (sample.id <= _trees.size()))
    {
        return _trees[sample.id]->Add(sample);
    }
    else
    {
        return 0;
    }
}

int MultiTree::Remove(const Sample & sample)
{
    if((sample.id >= 0) && (sample.id <= _trees.size()))
    {
        return _trees[sample.id]->Remove(sample);
    }
    else
    {
        return 0;
    }
}

int MultiTree::AddDeadZone(const Sample & sample)
{
    for(unsigned int i = 0; i < _trees.size(); i++)
    {
        if(! _trees[i]->AddDeadZone(sample)) return 0;
    }

    return 1;
}

int MultiTree::RemoveDeadZone(const Sample & sample)
{
    for(unsigned int i = 0; i < _trees.size(); i++)
    {
        if(! _trees[i]->RemoveDeadZone(sample)) return 0;
    }

    return 1;
}

void MultiTree::GetSamples(vector<const Sample *> & samples) const
{
    samples.clear();

    for(unsigned int i = 0; i < _trees.size(); i++)
    {
        vector<const Sample *> tree_samples;

        _trees[i]->GetSamples(tree_samples);
        samples.insert(samples.end(), tree_samples.begin(), tree_samples.end());
    }
}
