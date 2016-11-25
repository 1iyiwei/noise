/*
  DiffScatter.hpp

  scatter diff kernel values into a diff domain

  Li-Yi Wei
  09/05/2010

*/

#ifndef _DIFF_SCATTER_HPP
#define _DIFF_SCATTER_HPP

#include <vector>
#include <string>
using namespace std;

#include "Sample.hpp"
#include "Array.hpp"
#include "DiffKernel.hpp"
#include "DiffGather.hpp"

class DiffScatter
{
public:
    struct Answer
    {
        Answer(void) : count(0) {};

        string message;
        float count;
    };

    typedef DiffGather::Diff Diff;

    // scatter the diffs into output, which is *not* cleared via this function
    // cell_size is the size per output cell
    // query_scale: cell_size for Gaussian, and 1 for cos
    // return empty string if successful, error message else
    Answer Run(const vector<Diff> & diffs, const DiffKernel & kernel, const float cell_size, const float query_scale, Array<float> & output) const;
};

#endif
