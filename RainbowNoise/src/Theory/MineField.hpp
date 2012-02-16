/*
  MineField.hpp

  the base class for all minefields

  Li-Yi Wei
  11/09/2008

*/

#ifndef _MINE_FIELD_HPP
#define _MINE_FIELD_HPP

#include "Sample.hpp"

class MineField
{
public:
    MineField(void);
    virtual ~MineField(void) = 0;

    // return 1 if the query survives the mine field, 0 else
    virtual int Alive(const Sample & query) const;
};

#endif
