/*
  EmptyMineField.hpp

  a friendly mine field that always lets you pass

  Li-Yi Wei
  11/09/2008

*/

#ifndef _EMPTY_MINE_FIELD_HPP
#define _EMPTY_MINE_FIELD_HPP

#include "MineField.hpp"

class EmptyMineField : public MineField
{
public:
    EmptyMineField(void);
    ~EmptyMineField(void);

    int Alive(const Sample & query) const;
};

#endif
