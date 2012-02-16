/*
  DiscreteRelaxation.hpp

  Lloyd Relaxation over a fixed discrete set of samples

  Li-Yi Wei
  05/15/2008

*/

#ifndef _DISCRETE_RELAXATION_HPP
#define _DISCRETE_RELAXATION_HPP

#include <string>
using namespace std;

#include "Sample.hpp"

class DiscreteRelaxation
{
public:
    typedef enum {NONE, TOROIDAL} BoundaryCondition;

    // rectangular domain only
    struct DomainSpec
    {
        DomainSpec(const Coordinate & corner_min_i, const Coordinate & corner_max_i) : corner_min(corner_min_i), corner_max(corner_max_i) {};
 
        Coordinate corner_min;
        Coordinate corner_max;
    };

    // boundary condition will be handled toroidally so we need domain_min/max
    // representative_samples will be an input/output variable
    // and its size will remain invariant
    static string RunOnce(const DomainSpec & domain, const BoundaryCondition boundary_condition, const vector<Sample> & all_samples, vector<Sample> & representative_samples);

#if 0
    static float Energy(void);
#endif
    static float Energy(const DomainSpec & domain, const BoundaryCondition boundary_condition, const vector<Sample> & all_samples, const vector<Sample> & representative_samples);

protected:
    // the 2 famous steps in Lloyd relaxation, adopted for the discrete version
    // the corrected_voter is the toridally corrected voter for winner
    // return the winner id (within [0 representatives.size()-1]), or else error
    static int Nearest(const DomainSpec & domain, const BoundaryCondition boundary_condition, const vector<Sample> & representatives, const Sample & voter, Sample & corrected_voter);
    // all the samples must have been toroidally corrected
    static int Median(const DomainSpec & domain, const BoundaryCondition boundary_condition, const vector<Sample> & voters, const vector<Sample> & corrected_voters, Sample & representative);

    static float Distance(const Sample & s1, const Sample & s2);

    // return distance between representative and corrected voter
    static float Correct(const DomainSpec & domain, const BoundaryCondition boundary_condition, const Sample & representative, const Sample & voter, Sample & corrected_voter);
    static float Correct(const DomainSpec & domain, const vector<int> & correction, const Sample & representative, const Sample & voter, Sample & corrected_voter);

#if 0
protected:
    // debug
    static float _energy;
#endif
};

#endif

