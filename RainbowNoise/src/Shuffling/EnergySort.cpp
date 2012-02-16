/*
  EnergySort.cpp

  Li-Yi Wei
  08/25/2009

*/

#include <algorithm>
using namespace std;

#include "EnergySort.hpp"
#include "Exception.hpp"

// #define _RANK_DONE

EnergySort::EnergySort(void)
{
    // nothing else to do
}

EnergySort::EnergySort(const int num_class) : _lists(num_class)
{
    // nothing else to do
}

EnergySort::~EnergySort(void)
{
    // nothing else to do
}

int EnergySort::Set(const vector<const EnergySample *> & samples)
{
    const int num_class = _lists.size();

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if(!samples[i] || (samples[i]->NumClass() != num_class))
        {
            return 0;
        }
    }

    for(unsigned int i = 0; i < _lists.size(); i++)
    {
        _lists[i].clear();
    }

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        for(unsigned int j = 0; j < _lists.size(); j++)
        {
            _lists[j].push_back(SortElement(j, *samples[i]));
        }
    }

    for(unsigned int i = 0; i < _lists.size(); i++)
    {
        sort(_lists[i].begin(), _lists[i].end());

#ifdef _RANK_DONE
        for(unsigned int j = 0; j < _lists[i].size(); j++)
        {
            if(_lists[i][j].sample->Rank(i, j) < 0)
            {
                return 0;
            }
        }
#endif

    }

    // done
    return 1;
}
    
const EnergySample * EnergySort::Get(const int which_class, const int rank) const
{
    if((which_class < 0) || (which_class >= _lists.size()))
    {
        return 0;
    }

    if((rank < 0) || (rank >= _lists[which_class].size()))
    {
        return 0;
    }

    return _lists[which_class][rank].sample;
}

int EnergySort::Update(const int which_class, const int rank, EnergySample & sample)
{
    // error check first
    if((! &sample) || (Get(which_class, rank) != &sample))
    {
        return 0;
    }

    vector<SortElement> & mylist = _lists[which_class];

    if(mylist.size() <= 1)
    {
        // do nothing
        return 1;
    }

    _element = mylist[rank];

    // erase
    mylist.erase(mylist.begin()+rank);

    // binary search
    const int insert_pos = upper_bound(mylist.begin(), mylist.end(), _element) - mylist.begin();

    // insert
    mylist.insert(mylist.begin()+insert_pos, _element);

    throw Exception("This is stupid, because I still have update the rank information for all subsquent elements. not useful");

    // done
    return 1;
}
