/*
  ClassWeight.hpp

  base class
  specifying range weights for different classes under different combinations

  Li-Yi Wei
  05/04/2009

*/

#ifndef _CLASS_WEIGHT_HPP
#define _CLASS_WEIGHT_HPP

#include <vector>
using namespace std;

class ClassWeight
{
public:
    virtual ~ClassWeight(void) = 0;

    // return the weights of source_classes
    // source_classes is a binary array indicating 
    // the presence/absence of individual classes
    // return 1 if ok, 0 if error
    virtual int Get(const vector<int> & source_classes, vector<float> & weights) const;
    
    // per id version of above
    // return >= 0 if ok, < 0 if error
    virtual float Get(const int class_id, const vector<int> & source_classes) const;

    // return weight for a combination of 2 classes
    virtual float Get(const int class_id0, const int class_id1) const;

    // no source class_id info, used for Lloyd, not Shuffling
    virtual float Get(const vector<int> & source_classes) const;

    virtual int NumClass(void) const;
};

// no duplication of the role played by KernelSize
// affects implementation of offspring class
#define _UNITY_WEIGHT

#endif
