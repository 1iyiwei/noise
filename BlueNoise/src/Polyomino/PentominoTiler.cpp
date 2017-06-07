/*
  PentominoTiler.cpp

  Li-Yi Wei
  11/11/2007

*/

#include <iostream>
using namespace std;

#include "PentominoTiler.hpp"

#include "FPentomino.hpp"
#include "IPentomino.hpp"
#include "LPentomino.hpp"
#include "NPentomino.hpp"
#include "PPentomino.hpp"
#include "TPentomino.hpp"
#include "UPentomino.hpp"
#include "VPentomino.hpp"
#include "WPentomino.hpp"
#include "XPentomino.hpp"
#include "YPentomino.hpp"
#include "ZPentomino.hpp"

#include "Random.hpp"
#include "Exception.hpp"

#include <math.h>

PentominoTiler::PentominoTiler(const int height, const int width) : _grid_height(height), _grid_width(width), _dlx_solver(0)
{
    // init dancing links stuff
    const Pentomino::Orientation orientations[] = {Pentomino::ON, Pentomino::OE, Pentomino::OS, Pentomino::OW};
    const int num_orientations = 4;
    const Pentomino::Reflection reflections[] = {Pentomino::R1, Pentomino::R2};
    const int num_reflections = 2;

    vector<const Pentomino *> ordered;
    
    vector<Pentomino::Int2> cells(5);
    
    for(int row = 0; row < height; row++)
        for(int col = 0; col < width; col++)
            for(int rot = 0; rot < num_orientations; rot++)
                for(int ref = 0; ref < num_reflections; ref++)
                    for(int kind = 0; kind < 12; kind++)
                    {
                        Pentomino * pentomino = 0;

                        switch(kind)
                        {
                            case 0:
                                pentomino = new FPentomino(Pentomino::Int2(row, col), orientations[rot], reflections[ref]);
                                break;
                                
                            case 1:
                                pentomino = new IPentomino(Pentomino::Int2(row, col), orientations[rot], reflections[ref]);
                                break;
                                
                            case 2:
                                pentomino = new LPentomino(Pentomino::Int2(row, col), orientations[rot], reflections[ref]);
                                break;
                                
                            case 3:
                                pentomino = new NPentomino(Pentomino::Int2(row, col), orientations[rot], reflections[ref]);
                                break;
                                
                            case 4:
                                pentomino = new PPentomino(Pentomino::Int2(row, col), orientations[rot], reflections[ref]);
                                break;
                                
                            case 5:
                                pentomino = new TPentomino(Pentomino::Int2(row, col), orientations[rot], reflections[ref]);
                                break;
                                
                            case 6:
                                pentomino = new UPentomino(Pentomino::Int2(row, col), orientations[rot], reflections[ref]);
                                break;
                                
                            case 7:
                                pentomino = new VPentomino(Pentomino::Int2(row, col), orientations[rot], reflections[ref]);
                                break;
                                
                            case 8:
                                pentomino = new WPentomino(Pentomino::Int2(row, col), orientations[rot], reflections[ref]);
                                break;
                                
                            case 9:
                                pentomino = new XPentomino(Pentomino::Int2(row, col), orientations[rot], reflections[ref]);
                                break;
                                
                            case 10:
                                pentomino = new YPentomino(Pentomino::Int2(row, col), orientations[rot], reflections[ref]);
                                break;
                                
                            case 11:
                                pentomino = new ZPentomino(Pentomino::Int2(row, col), orientations[rot], reflections[ref]);
                                break;

                        default:
                            throw Exception("unknown pentomino");
                        }

                        Pentomino::Int2 center;
                        pentomino->GetCenter(center);
                        pentomino->GetCells(cells);

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
                            ordered.push_back(pentomino);
                        }
                        else
                        {
                            delete pentomino;
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

    Pentomino::Int2 center;
    for(unsigned int i = 0; i < _rows.size(); i++)
    {
        _rows[i]->GetCenter(center);
        _rows[i]->GetCells(cells);

        if(cells.size() != 5)
        {
            throw Exception("not pentomino");
        }
        
        for(unsigned int j = 0; j < cells.size(); j++)
        {
            Pentomino::Int2 cell_corner(center.row + cells[j].row, center.col + cells[j].col);

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

PentominoTiler::~PentominoTiler(void)
{
    for(unsigned int i = 0; i < _rows.size(); i++)
    {
        delete _rows[i];
    }
    
    delete _dlx_solver;
    delete[] _solution_rows;
}
    
PolyominoTiling * PentominoTiler::Next(void)
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
