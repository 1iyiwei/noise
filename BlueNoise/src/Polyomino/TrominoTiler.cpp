/*
  TrominoTiler.cpp

  Li-Yi Wei
  11/11/2007

*/

#include <iostream>
using namespace std;

#include "TrominoTiler.hpp"

#include "ITromino.hpp"
#include "LTromino.hpp"

#include "Random.hpp"
#include "Exception.hpp"

#include <math.h>

TrominoTiler::TrominoTiler(const int height, const int width) : _grid_height(height), _grid_width(width), _dlx_solver(0)
{
    // init dancing links stuff
    const Tromino::Orientation orientations[] = {Tromino::ON, Tromino::OE, Tromino::OS, Tromino::OW};
    const int num_orientations = 4;
    const Tromino::Reflection reflections[] = {Tromino::R1, Tromino::R2};
    const int num_reflections = 2;

    vector<const Tromino *> ordered;
    
    vector<Tromino::Int2> cells(3);
    
    for(int row = 0; row < height; row++)
        for(int col = 0; col < width; col++)
            for(int rot = 0; rot < num_orientations; rot++)
                for(int ref = 0; ref < num_reflections; ref++)
                    for(int kind = 0; kind < 2; kind++)
                    {
                        Tromino * tromino = 0;

                        switch(kind)
                        {
                            case 0:
                                tromino = new ITromino(Tromino::Int2(row, col), orientations[rot], reflections[ref]);
                                break;
                                
                            case 1:
                                tromino = new LTromino(Tromino::Int2(row, col), orientations[rot], reflections[ref]);
                                break;
                                
                        default:
                            throw Exception("unknown tromino");
                        }

                        Tromino::Int2 center;
                        tromino->GetCenter(center);
                        tromino->GetCells(cells);

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
                            ordered.push_back(tromino);
                        }
                        else
                        {
                            delete tromino;
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

    Tromino::Int2 center;
    for(unsigned int i = 0; i < _rows.size(); i++)
    {
        _rows[i]->GetCenter(center);
        _rows[i]->GetCells(cells);

        if(cells.size() != 3)
        {
            throw Exception("not tromino");
        }
        
        for(unsigned int j = 0; j < cells.size(); j++)
        {
            Tromino::Int2 cell_corner(center.row + cells[j].row, center.col + cells[j].col);

            if((cell_corner.row < 0) || (cell_corner.row >= height) || (cell_corner.col < 0) || (cell_corner.col >= width))
            {
                cerr << "(" << center.row << ", " << center.col << ") + (" << cells[j].row << ", " << cells[j].col << ")" << endl;
                throw Exception("cell corner out of bound");
            }
            
            _dlx_solver->insert_element(cell_corner.row*width + cell_corner.col);
        }
        
        _dlx_solver->end_row();
    }

    _solution_rows = new int[_rows.size()];
}

TrominoTiler::~TrominoTiler(void)
{
    for(unsigned int i = 0; i < _rows.size(); i++)
    {
        delete _rows[i];
    }
    
    delete _dlx_solver;
    delete[] _solution_rows;
}
    
PolyominoTiling * TrominoTiler::Next(void)
{
    int solution_length = 0;
    const int ok = _dlx_solver->get_solution(_solution_rows, solution_length);

    PolyominoTiling * tiling = (ok ? new PolyominoTiling(_grid_height, _grid_width): 0);

    if(tiling)
    {
        vector<const Polyomino *> tiles(solution_length);

        for(unsigned int i = 0; i < tiles.size(); i++)
        {
            tiles[i] = _rows[_solution_rows[i]]->Clone();
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
