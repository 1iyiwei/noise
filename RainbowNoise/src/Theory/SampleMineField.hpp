/*
  SampleMineField.hpp

  a mine field occupied and controlled by samples

  Li-Yi Wei
  11/09/2008

*/

#ifndef _SAMPLE_MINE_FIELD_HPP
#define _SAMPLE_MINE_FIELD_HPP

#include <vector>
using namespace std;

#include "MineField.hpp"

#include "Grid.hpp"
#include "ConflictChecker.hpp"

class SampleMineField : public MineField
{
public:
    // the samples are contained within the grid
    SampleMineField(const Grid & grid, const ConflictChecker & conflict_checker);
    ~SampleMineField(void);

    int Alive(const Sample & query) const;

protected:
    const Grid & _grid;
    const ConflictChecker & _conflict_checker;
};

#endif
