/*
  GHexominoSubdivisionTiler.cpp

  Li-Yi Wei
  11/12/2007

*/

#include <iostream>
using namespace std;

#include "GHexominoSubdivisionTiler.hpp"
#include "GHexomino.hpp"
#include "Random.hpp"
#include "Exception.hpp"

#include <math.h>

GHexominoSubdivisionTiler::GHexominoSubdivisionTiler(void) : _grid_height(27), _grid_width(27), _dlx_solver(0)
{
    const int height = _grid_height;
    const int width = _grid_width;
    
    // init dancing links stuff
    const GHexomino::Orientation orientations[] = {GHexomino::ON, GHexomino::OE, GHexomino::OS, GHexomino::OW};
    const int num_orientations = 4;
    const GHexomino::Reflection reflections[] = {GHexomino::R1, GHexomino::R2};
    const int num_reflections = 2;

    vector<GHexomino> ordered;
    
    vector<GHexomino::Int2> cells(6);
    
    for(int row = 0; row < height; row++)
        for(int col = 0; col < width; col++)
            for(int rot = 0; rot < num_orientations; rot++)
                for(int ref = 0; ref < num_reflections; ref++)
                {
                    GHexomino g_hexomino(GHexomino::Int2(row, col), orientations[rot], reflections[ref]);

                    GHexomino::Int2 center;
                    g_hexomino.GetCenter(center);
                    g_hexomino.GetCells(cells);

                    int all_inside = 1;
                    for(unsigned int i = 0; i < cells.size(); i++)
                    {
                        const int cell_row = center.row + cells[i].row;
                        const int cell_col = center.col + cells[i].col;

                        if(Serialize(cell_row, cell_col) < 0)
                        {
                            all_inside = 0;
                            break;
                        }
                    }

                    if(all_inside)
                    {
                        ordered.push_back(g_hexomino);
                    }
                }
    
    // randomize
    _rows.clear();
    while(ordered.size() > 0)
    {
        int selection = static_cast<int>(floor(ordered.size()*Random::UniformRandom()));
        if(selection < 0) selection = 0;
        if(selection >= ordered.size()) selection = ordered.size()-1;

        _rows.push_back(ordered[selection]);
        ordered[selection] = ordered[ordered.size()-1];
        ordered.pop_back();
    }

    // init dlx solver
    const int num_cols = height*width*6/9;
    _dlx_solver = new dlx(num_cols, num_cols, _rows.size()*6);

    GHexomino::Int2 center;
    for(unsigned int i = 0; i < _rows.size(); i++)
    {
        _rows[i].GetCenter(center);
        _rows[i].GetCells(cells);

        for(unsigned int j = 0; j < cells.size(); j++)
        {
            GHexomino::Int2 cell_corner(center.row + cells[j].row, center.col + cells[j].col);

            const int which_element = Serialize(cell_corner.row, cell_corner.col);
            if((which_element < 0) || (which_element >= num_cols))
            {
                cerr << "(" << cell_corner.row << ", " << cell_corner.col << ") - " << which_element << endl;
                throw Exception("GHexominoSubdivisionTiler::GHexominoSubdivisionTiler(): which_element out of bound");
            }
            
            _dlx_solver->insert_element(which_element);
        }
        
        _dlx_solver->end_row();
    }

    _solution_rows = new int[_rows.size()];
}

GHexominoSubdivisionTiler::~GHexominoSubdivisionTiler(void)
{
    delete _dlx_solver;
    delete[] _solution_rows;
}
    
PolyominoTiling * GHexominoSubdivisionTiler::Next(void)
{
    int solution_length = 0;
    const int ok = _dlx_solver->get_solution(_solution_rows, solution_length);

    PolyominoTiling * tiling = (ok ? new PolyominoTiling(_grid_height, _grid_width): 0);

    if(tiling)
    {
        vector<const Polyomino *> tiles(solution_length);

        for(unsigned int i = 0; i < tiles.size(); i++)
        {
            tiles[i] = new GHexomino(_rows[_solution_rows[i]]);
        }
        
        if(! tiling->SetTiles(tiles))
        {
            tiling->SetMessage("cannot set tiles");
        }

        // build neighbors list
        BuildNeighborList(*tiling);
    }

    // done
    return tiling;
}

int GHexominoSubdivisionTiler::Serialize(const int row, const int col) const
{
    const int big_cell_height = _grid_height/3; // i.e. 9
    const int big_cell_width = _grid_width/3; // i.e. 9
    const int num_cells_per_big_cell = big_cell_height*big_cell_width; // i.e. 81

    int answer = -1;

    if(row < 0)
    {
        answer = -1;
    }
    else if(row < big_cell_height)
    {
        if((col < 0) || (col >= big_cell_width))
        {
            answer = -1;
        }
        else
        {
            answer = row*big_cell_width + col;
        }
    }
    else if(row < 2*big_cell_height)
    {
        if((col < 0) || (col >= 3*big_cell_width))
        {
            answer = -1;
        }
        else
        {
            answer = num_cells_per_big_cell + (row-big_cell_height)*3*big_cell_width + col;
        }
    }
    else if(row < 3*big_cell_height)
    {
        if((col < 0) || (col >= 2*big_cell_width))
        {
            answer = -1;
        }
        else
        {
           answer = 4*num_cells_per_big_cell + (row-2*big_cell_height)*2*big_cell_width + col; 
        }
    }
    else
    {
        answer = -1;
    }

    return answer;
}
