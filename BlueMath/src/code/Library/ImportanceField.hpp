/*
  ImportanceField.hpp

  the base class for all isotropic scalar importance fields

  Li-Yi Wei
  10/11/2010

*/

#ifndef _IMPORTANCE_FIELD_HPP
#define _IMPORTANCE_FIELD_HPP

#include "Sample.hpp"

class ImportanceField
{
public:
    virtual ~ImportanceField(void) = 0;

    virtual int Dimension(void) const;

    virtual float Query(const Sample & sample) const;
};

#endif
