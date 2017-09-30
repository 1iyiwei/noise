/*
  CosImportanceField.hpp

  Li-Yi Wei
  10/12/2010

*/

#ifndef _COS_IMPORTANCE_FIELD_HPP
#define _COS_IMPORTANCE_FIELD_HPP

#include "ImportanceField.hpp"

class CosImportanceField : public ImportanceField
{
public:

    // cos(2*pi*freq*query) + 1
    CosImportanceField(const Coordinate & frequency);
    ~CosImportanceField(void);

    int Dimension(void) const;

    float Query(const Sample & sample) const;

protected:
    const Coordinate _frequency;
};

#endif
