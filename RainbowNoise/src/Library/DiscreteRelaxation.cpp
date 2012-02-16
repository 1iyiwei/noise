/*
  DiscreteRelaxation.cpp

  Li-Yi Wei
  05/15/2008

*/

#include <iostream>
using namespace std;

#include "DiscreteRelaxation.hpp"
#include "SequentialCounter.hpp"
#include "Exception.hpp"

string DiscreteRelaxation::RunOnce(const DomainSpec & domain, const BoundaryCondition boundary_condition, const vector<Sample> & all_samples, vector<Sample> & representative_samples)
{
    if(domain.corner_min.Dimension() != domain.corner_max.Dimension())
    {
        return "domain.corner_min.Dimension() != domain.corner_max.Dimension()";
    }

    if(representative_samples.size() > all_samples.size())
    {
        return "representative_samples.size() > all_samples.size()";
    }

    if(all_samples.size() <= 0)
    {
        return "all_samples.size() <= 0";
    }

    vector< vector<Sample> > voters(representative_samples.size());
    vector< vector<Sample> > corrected_voters(representative_samples.size());

    // nearest
    Sample corrected_voter(all_samples[0]);
    for(unsigned int i = 0; i < all_samples.size(); i++)
    {
        const Sample & voter = all_samples[i];
        const int winner = Nearest(domain, boundary_condition, representative_samples, voter, corrected_voter);
        if((winner < 0) || (winner >= corrected_voters.size()))
        {
            return "error in Nearest()";
        }
        else
        {
            voters[winner].push_back(voter);
            corrected_voters[winner].push_back(corrected_voter);
        }
    }

    for(unsigned int i = 0; i < corrected_voters.size(); i++)
    {
        if(corrected_voters[i].size() <= 0)
        {
            return "corrected_voters[i].size() <= 0";
        }
    }

    // median
    for(unsigned int i = 0; i < representative_samples.size(); i++)
    {
        if(! Median(domain, boundary_condition, voters[i], corrected_voters[i], representative_samples[i]))
        {
            return "error in Median()";
        }
    }
#if 0
    // energy
    _energy = 0;
    {
        Sample foo(all_samples[0]);

        for(unsigned int i = 0; i < representative_samples.size(); i++)
        {
            for(unsigned int j = 0; j < corrected_voters[i].size(); j++)
            {
                _energy += Correct(domain, boundary_condition, representative_samples[i], corrected_voters[i][j], foo);
            }
        }
    }
#endif
    // done
    return "";
}

float DiscreteRelaxation::Energy(const DomainSpec & domain, const BoundaryCondition boundary_condition, const vector<Sample> & all_samples, const vector<Sample> & representative_samples)
{
    float energy = 0;

    if(all_samples.size() <= 0) return 0;

    vector< vector<Sample> > voters(representative_samples.size());
    vector< vector<Sample> > corrected_voters(representative_samples.size());

    // nearest
    Sample corrected_voter(all_samples[0]);
    for(unsigned int i = 0; i < all_samples.size(); i++)
    {
        const Sample & voter = all_samples[i];
        const int winner = Nearest(domain, boundary_condition, representative_samples, voter, corrected_voter);
        if((winner < 0) || (winner >= corrected_voters.size()))
        {
            throw Exception("error in Nearest()");
            return -1;
        }
        else
        {
            voters[winner].push_back(voter);
            corrected_voters[winner].push_back(corrected_voter);
        }
    }

#if 0
    // some clusters can become empty if there are continuous relaxations intermingled
    for(unsigned int i = 0; i < corrected_voters.size(); i++)
    {
        if(corrected_voters[i].size() <= 0)
        {
            throw Exception("corrected_voters[i].size() <= 0");
            return -1;
        }
    }
#endif

    Sample foo(all_samples[0]);

    for(unsigned int i = 0; i < representative_samples.size(); i++)
    {
        for(unsigned int j = 0; j < corrected_voters[i].size(); j++)
        {
            energy += Correct(domain, boundary_condition, representative_samples[i], corrected_voters[i][j], foo);
        }
    }

    return energy;
}

#if 0
float DiscreteRelaxation::_energy;

float DiscreteRelaxation::Energy(void)
{
    return _energy;
}
#endif

int DiscreteRelaxation::Nearest(const DomainSpec & domain, const BoundaryCondition boundary_condition, const vector<Sample> & representatives, const Sample & voter, Sample & corrected_voter)
{
    if(representatives.size() <= 0)
    {
        throw Exception("DiscreteRelaxation::Nearest(): representatives.size() <= 0");
        return -1;
    }

    Sample min_corrected_voter(voter);
    float min_distance = Correct(domain, boundary_condition, representatives[0], voter, min_corrected_voter);
    int winner = 0;

    for(unsigned int i = 1; i < representatives.size(); i++)
    {
        const float distance = Correct(domain, boundary_condition, representatives[i], voter, corrected_voter);

        if(distance < 0)
        {
            throw Exception("DiscreteRelaxation::Nearest(): distance < 0");
            return -1;
        }

        if(distance < min_distance)
        {
            min_distance = distance;
            min_corrected_voter = corrected_voter;
            winner = i;
        }
    }

    corrected_voter = min_corrected_voter;

    return winner;
}

int DiscreteRelaxation::Median(const DomainSpec & domain, const BoundaryCondition boundary_condition, const vector<Sample> & voters, const vector<Sample> & corrected_voters, Sample & representative)
{
    if(corrected_voters.size() <= 0)
    {
        throw Exception("DiscreteRelaxation::Median(): corrected_voters.size() <= 0");
        return 0;
    }

    // mean
    Sample mean = corrected_voters[0];
    for(unsigned int i = 1; i < corrected_voters.size(); i++)
    {
        if(mean.coordinate.Dimension() != corrected_voters[i].coordinate.Dimension())
        {
            return 0;
        }

        for(int j = 0; j < mean.coordinate.Dimension(); j++)
        {
            mean.coordinate[j] += corrected_voters[i].coordinate[j];
        }
    }
 
    for(int j = 0; j < mean.coordinate.Dimension(); j++)
    {
        mean.coordinate[j] /= corrected_voters.size();
    }

    // guy nearest to mean
    representative = voters[0];
    Sample foo(mean);
    float min_distance = Correct(domain, boundary_condition, mean, representative, foo); // Distance(representative, mean);

    for(unsigned int i = 1; i < voters.size(); i++)
    {
        const float distance = Correct(domain, boundary_condition, mean, voters[i], foo); //Distance(voters[i], mean);

        if(distance < 0)
        {
            throw Exception("DiscreteRelaxation::Median(): distance < 0");
            return 0;
        }

        if(distance < min_distance)
        {
            representative = voters[i];
            min_distance = distance;
        }
    }

    // done
    return 1;
}

float DiscreteRelaxation::Distance(const Sample & s1, const Sample & s2)
{
    if(s1.coordinate.Dimension() != s2.coordinate.Dimension())
    {
        return -1;
    }
    else
    {
        float value = 0;
        for(int i = 0; i < s1.coordinate.Dimension(); i++)
        {
            const float foo = s1.coordinate[i] - s2.coordinate[i];
            value += foo*foo;
        }

        return value;
    }
}

float DiscreteRelaxation::Correct(const DomainSpec & domain, const BoundaryCondition boundary_condition, const Sample & representative, const Sample & voter, Sample & corrected_voter)
{
    if(boundary_condition == NONE)
    {
        corrected_voter = voter;
        return Distance(representative, corrected_voter);
    }

    vector<int> correction(voter.coordinate.Dimension(), 0);

    Sample min_corrected_voter(voter);
    float min_distance = Correct(domain, correction, representative, voter, min_corrected_voter);

    SequentialCounter counter(domain.corner_min.Dimension(), -1, 1);
    counter.Reset();
    do
    {
        counter.Get(correction);
        
        const float distance = Correct(domain, correction, representative, voter, corrected_voter);

        if(distance < 0) return -1;

        if(distance < min_distance)
        {
            min_corrected_voter = corrected_voter;
            min_distance = distance;
        }
    }
    while(counter.Next());

    corrected_voter = min_corrected_voter;
    return min_distance;
}

float DiscreteRelaxation::Correct(const DomainSpec & domain, const vector<int> & correction, const Sample & representative, const Sample & voter, Sample & corrected_voter)
{
    corrected_voter = voter;

    for(int i = 0; i < corrected_voter.coordinate.Dimension(); i++)
    {
        corrected_voter.coordinate[i] += correction[i]*(domain.corner_max[i]-domain.corner_min[i]);
    }

    return Distance(representative, corrected_voter);
}
