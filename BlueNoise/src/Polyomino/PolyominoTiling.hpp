/*
  PolyominoTiling.hpp

  the tiling output of polyominoes
  duals as a data structure for tiler classes
  
  Li-Yi Wei
  10/21/2007

*/

#ifndef _POLYOMINO_TILING_HPP
#define _POLYOMINO_TILING_HPP

#include "Polyomino.hpp"

#include <string>
using namespace std;

class PolyominoTiling
{
public:
    PolyominoTiling(void);
    PolyominoTiling(const int height, const int width);
    PolyominoTiling(const char * file_name);
    ~PolyominoTiling(void);

    Polyomino::Int2 Size(void) const;
    
    int NumTiles(void) const;
    
    // will perform consistency check
    // return 1 if successful, 0 else
    // tiles will then belong to this class
    int SetTiles(const vector<const Polyomino *> & tiles);
    const Polyomino * GetTile(const int index) const;
    Polyomino * GetTile(const int index);

    typedef Polyomino::Int2 Int2;
    
    Int2 GridSize(void) const;
    const Polyomino * GetGridCell(const int row, const int col) const;
    int SetGridCell(const int row, const int col, const Polyomino * poly);

    string GetMessage(void) const;
    void SetMessage(const string & input);

    // call IO format functions below and
    // responsible for internal maintainance
    // with role essentially equal to DataminoTiler, a non-existance class
    int Read(const char * file_name);
    int Write(const char * file_name) const;

    // responsible for IO formats
    static int Read(const char * file_name, int & height, int & width, vector<const Polyomino *> & tiles);
    static int Write(const char * file_name, const int height, const int width, const vector<const Polyomino *> & tiles);

protected:
    void BuildGrid(const int height, const int width);
    
private:
    // no copy
    PolyominoTiling(const PolyominoTiling &);
    PolyominoTiling & operator=(const PolyominoTiling &);
    
protected:
    vector<const Polyomino *> _tiles;
    vector< vector<const Polyomino *> > _grid; // store polyomino id per cell

    string _message; // for debug
};

#endif
