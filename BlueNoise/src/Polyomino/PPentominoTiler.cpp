/*
  PPentominoTiler.cpp

  Li-Yi Wei
  10/25/2007

*/

#include <iostream>
using namespace std;

#include "PPentominoTiler.hpp"
#include "PPentomino.hpp"
#include "Random.hpp"
#include "Exception.hpp"

#include <math.h>

PPentominoTiler::PPentominoTiler(const int height, const int width) : _grid_height(height), _grid_width(width), _dlx_solver(0)
{
    // init dancing links stuff
    const PPentomino::Orientation orientations[] = {PPentomino::ON, PPentomino::OE, PPentomino::OS, PPentomino::OW};
    const int num_orientations = 4;
    const PPentomino::Reflection reflections[] = {PPentomino::R1, PPentomino::R2};
    const int num_reflections = 2;

    vector<PPentomino> ordered;
    
    vector<PPentomino::Int2> cells(5);
    
    for(int row = 0; row < height; row++)
        for(int col = 0; col < width; col++)
            for(int rot = 0; rot < num_orientations; rot++)
                for(int ref = 0; ref < num_reflections; ref++)
                {
                    PPentomino p_pentomino(PPentomino::Int2(row, col), orientations[rot], reflections[ref]);

                    PPentomino::Int2 center;
                    p_pentomino.GetCenter(center);
                    p_pentomino.GetCells(cells);

                    int all_inside = 1;
                    for(unsigned int i = 0; i < cells.size(); i++)
                    {
                        const int cell_row = center.row + cells[i].row;
                        const int cell_col = center.col + cells[i].col;
                        
                        if((cell_row < 0) || (cell_row >= height) || (cell_col < 0) || (cell_col >= width))
                        {
                            all_inside = 0;
                            break;
                        }
                    }

                    if(all_inside)
                    {
                        ordered.push_back(p_pentomino);
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
    _dlx_solver = new dlx(height*width, height*width, _rows.size()*5);

    PPentomino::Int2 center;
    for(unsigned int i = 0; i < _rows.size(); i++)
    {
        _rows[i].GetCenter(center);
        _rows[i].GetCells(cells);

        for(unsigned int j = 0; j < cells.size(); j++)
        {
            PPentomino::Int2 cell_corner(center.row + cells[j].row, center.col + cells[j].col);
            _dlx_solver->insert_element(cell_corner.row*width + cell_corner.col);
        }
        
        _dlx_solver->end_row();
    }

    _solution_rows = new int[_rows.size()];

    if(0)
    {
        // debug
        vector< vector<int> > matrix(_rows.size());
        
        for(unsigned int i = 0; i < _rows.size(); i++)
        {
            matrix[i] = vector<int>(height*width);

            for(unsigned int j = 0; j < matrix[i].size(); j++)
            {
                matrix[i][j] = 0;
            }
            
            _rows[i].GetCenter(center);
            _rows[i].GetCells(cells);

            for(unsigned int j = 0; j < cells.size(); j++)
            {
                PPentomino::Int2 cell_corner(center.row + cells[j].row, center.col + cells[j].col);
                matrix[i][cell_corner.row*width + cell_corner.col] = 1;
            }
        }

        cout << matrix.size() << " " << height*width << endl;
        for(unsigned int i = 0; i < matrix.size(); i++)
        {
            for(unsigned int j = 0; j < matrix[i].size(); j++)
            {
                cout << matrix[i][j] << " ";
            }
            cout << endl;
        }
    }
}

PPentominoTiler::~PPentominoTiler(void)
{
    delete _dlx_solver;
    delete[] _solution_rows;
}
    
PolyominoTiling * PPentominoTiler::Next(void)
{
    int solution_length = 0;
    const int ok = _dlx_solver->get_solution(_solution_rows, solution_length);

    PolyominoTiling * tiling = (ok ? new PolyominoTiling(_grid_height, _grid_width): 0);

    if(tiling)
    {
        vector<const Polyomino *> tiles(solution_length);

        for(unsigned int i = 0; i < tiles.size(); i++)
        {
            tiles[i] = new PPentomino(_rows[_solution_rows[i]]);
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
