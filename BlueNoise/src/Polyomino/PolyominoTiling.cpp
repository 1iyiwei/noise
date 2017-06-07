/*
  PolyominoTiling.cpp

  Li-Yi Wei

  10/21/2007

*/

#include <iostream>
using namespace std;

#include "PolyominoTiling.hpp"
#include "PolyominoTiler.hpp"
#include "Exception.hpp"

PolyominoTiling::PolyominoTiling(void)
{
    // nothing to do
}

PolyominoTiling::PolyominoTiling(const int height, const int width)
{
    BuildGrid(height, width);
}

PolyominoTiling::PolyominoTiling(const char * file_name)
{
    if(! Read(file_name))
    {
        throw Exception("PolyominoTiling::PolyominoTiling() : error in reading input file");
    }
}

PolyominoTiling::~PolyominoTiling(void)
{
    for(unsigned int i = 0; i < _tiles.size(); i++)
    {
        delete _tiles[i];
    }
}

Polyomino::Int2 PolyominoTiling::Size(void) const
{
    return Polyomino::Int2(_grid.size(), _grid.size() > 0 ? _grid[0].size() : 0);
}

int PolyominoTiling::NumTiles(void) const
{
    return _tiles.size();
}
    
int PolyominoTiling::SetTiles(const vector<const Polyomino *> & tiles)
{
    for(unsigned int i = 0; i < _tiles.size(); i++)
    {
        delete _tiles[i];
    }

    _tiles = tiles;

    // consistency check
    for(unsigned int i = 0; i < _grid.size(); i++)
        for(unsigned int j = 0; j < _grid[i].size(); j++)
        {
            _grid[i][j] = 0;
        }

    vector<Int2> cells;
    const Int2 grid_size = GridSize();
    
    for(unsigned int i = 0; i < _tiles.size(); i++)
    {
        if(!_tiles[i])
        {
            _message = "null tile";
            return 0;
        }

        _tiles[i]->GetCells(cells);

        Int2 center; _tiles[i]->GetCenter(center);

        for(unsigned int j = 0; j < cells.size(); j++)
        {
            const Int2 cell_index(center.row + cells[j].row, center.col + cells[j].col);
            if((cell_index.row < 0) || (cell_index.row >= grid_size.row) ||
               (cell_index.col < 0) || (cell_index.col >= grid_size.col))
            {
                _message = "out of range cell";
                return 0;
            }

            if(_grid[cell_index.row][cell_index.col] &&
               (_grid[cell_index.row][cell_index.col] != _tiles[i]))
            {
                _message = "tiles overlap in cells";
                return 0;
            }
            else
            {
                _grid[cell_index.row][cell_index.col] = _tiles[i];
            }
        }
    }

    // all grid cells must be covered ? not enforced for now

    // done
    _message = "";
    return 1;
}

const Polyomino * PolyominoTiling::GetTile(const int index) const
{
    if((index >= 0) && (static_cast<unsigned int>(index) < _tiles.size()))
    {
        return _tiles[index];
    }
    else
    {
        return 0;
    }
}

Polyomino * PolyominoTiling::GetTile(const int index)
{
    if((index >= 0) && (static_cast<unsigned int>(index) < _tiles.size()))
    {
        return const_cast<Polyomino *>(_tiles[index]);
    }
    else
    {
        return 0;
    }
}
    
PolyominoTiling::Int2 PolyominoTiling::GridSize(void) const
{
    return Int2(_grid.size(), _grid.size() > 0 ? _grid[0].size() : 0);
}

const Polyomino * PolyominoTiling::GetGridCell(const int row, const int col) const
{
    if( (row >= 0) && (static_cast<unsigned int>(row) < _grid.size()) &&
        (col >= 0) && (static_cast<unsigned int>(col) < _grid[row].size()))
    {
        return _grid[row][col];
    }
    else
    {
        return 0;
    }
}

int PolyominoTiling::SetGridCell(const int row, const int col, const Polyomino * poly)
{
    if( (row >= 0) && (static_cast<unsigned int>(row) < _grid.size()) &&
        (col >= 0) && (static_cast<unsigned int>(col) < _grid[row].size()))
    {
        _grid[row][col] = poly;
        return 1;
    }
    else
    {
        return 0;
    }
}
    
PolyominoTiling::PolyominoTiling(const PolyominoTiling &)
{
    throw Exception("PolyominoTiling::PolyominoTiling(const PolyominoTiling &) called");
}

PolyominoTiling & PolyominoTiling::operator=(const PolyominoTiling &)
{
    throw Exception("PolyominoTiling::operator=(const PolyominoTiling &) called");
}

string PolyominoTiling::GetMessage(void) const
{
    return _message;
}

void PolyominoTiling::SetMessage(const string & input)
{
    _message = input;
}

int PolyominoTiling::Read(const char * file_name)
{
    int height = 0; int width = 0;
    vector<const Polyomino *> tiles;
    
    if(! Read(file_name, height, width, tiles))
    {
        SetMessage("cannot read from input file");
        return 1;
    }

    BuildGrid(height, width);
    
    if(! SetTiles(tiles))
    {
        SetMessage("cannot set tiles");
        return 1;
    }

    PolyominoTiler::BuildNeighborList(*this);
    return 1;
}

int PolyominoTiling::Write(const char * file_name) const
{
    if(! Write(file_name, _grid.size(), _grid.size() > 0 ? _grid[0].size() : 0, _tiles))
    {
        return 0;
    }
}

#include "Datamino.hpp"

#include <fstream>
//#include <sstream>
#include <string>
using namespace std;

int PolyominoTiling::Read(const char * file_name, int & height, int & width, vector<const Polyomino *> & tiles)
{
    if(tiles.size() > 0)
    {
        return 0;
    }
    
    ifstream input(file_name);

    if(! input) return 0;
    
    string magic;
    input >> magic;

    if(magic != "polyomino") return 0;

    input >> height >> width;
    if(input.bad()) return 0;

    int num_tiles = 0;
    input >> num_tiles;
    if(input.bad()) return 0;
    if(num_tiles <= 0) return 0;

    for(int i = 0; i < num_tiles; i++)
    {
        // cell count
        int num_cells = 0;
        input >> num_cells;    
        if(input.bad()) return 0;
        if(num_cells <= 0) return 0;
    
        // center
        Polyomino::Int2 center;

        input >> center.row >> center.col;
        if(input.bad()) return 0;
        
        // cells
        vector<Polyomino::Int2> cells(num_cells);
        for(int j = 0; j < num_cells; j++)
        {
            input >> cells[j].row >> cells[j].col;
            if(input.bad()) return 0;
        }

        tiles.push_back(new Datamino(center, cells));
    }
    
    // done
    return 1;
}

int PolyominoTiling::Write(const char * file_name, const int height, const int width, const vector<const Polyomino *> & tiles)
{
    ofstream output(file_name);

    if(! output) return 0;

    output << "polyomino" << endl;

    output << height << " " << width << endl;

    output << tiles.size() << endl;

    for(unsigned int i = 0; i < tiles.size(); i++)
    {
        if(!tiles[i]) return 0;
        
        vector<Polyomino::Int2> cells;
        tiles[i]->GetCells(cells);
        output << cells.size() << " ";

        Polyomino::Int2 center;
        tiles[i]->GetCenter(center);
        output << center.row << " " << center.col << " ";

        for(unsigned int j = 0; j < cells.size(); j++)
        {
            output << cells[j].row << " " << cells[j].col << " ";
        }

        output << endl;
    }
    
    // done
    return 1;
}

void PolyominoTiling::BuildGrid(const int height, const int width)
{
    _grid = vector< vector<const Polyomino *> > (height);

    for(unsigned int i = 0; i < _grid.size(); i++)
    {
        _grid[i] = vector<const Polyomino *>(width);

        for(unsigned int j = 0; j < _grid[i].size(); j++)
        {
            _grid[i][j] = 0;
        }
    }
}
