/*
  a collection of tools relevant to Lloyd stuff

  Li-Yi Wei
  07/29/2009

*/

#ifndef _LLOYD_TOOL_HPP
#define _LLOYD_TOOL_HPP

#include <string>
#include <vector>
using namespace std;

#include "LloydSite.hpp"
#include "LloydSample.hpp"

class LloydTools
{
public:
    // both return 1 if successful, 0 else
    static int FastenClassID(int & id);
    static int LoosenClassID(int & id);

    static int CountSamples(const vector<LloydSite> & sites, vector<int> & num_sites);

    static int ReadPoints(const int dimension, const int num_class, const LloydIndex & model_index, const string & input_file_name, vector<LloydSample> & points);

    // class_combo: binary mask selecting which info to write out
    // if 0 < class_combo, write out the specified combo only
    // else, write out everything
    static int WritePoints(const int num_class, const int class_combo_mask, const vector<LloydSample> & points, const string & output_file_name);

protected:
    static void Convert(const int mask_in, vector<int> & mask_out);
};
#endif
