/*
  PolyomioTiling.cpp

  Li-Yi Wei
  10/25/2007

*/

#include "PolyominoTiler.hpp"
#include "Exception.hpp"

PolyominoTiling * PolyominoTiler::Next(void)
{
    throw Exception("PolyominoTiler::Next() should not be called");
    return 0;
}

void PolyominoTiler::BuildNeighborList(PolyominoTiling & tiling)
{
    const Polyomino::Int2 grid_size = tiling.Size();

    const int grid_height = grid_size.row;
    const int grid_width = grid_size.col;
    
    vector<Polyomino::Int2> cells;
    for(int i = 0; i < tiling.NumTiles(); i++)
    {
        Polyomino * tile = tiling.GetTile(i);

        tile->ClearNeighborList();
        
        Polyomino::Int2 center;
        tile->GetCenter(center);
        tile->GetCells(cells);

        for(unsigned int j = 0; j < cells.size(); j++)
        {
            for(int m = -1; m <= 1; m++)
                for(int n = -1; n <= 1; n++)
                {
                    const int neighbor_row = ((center.row+cells[j].row+m)%grid_height+grid_height)%grid_height;
                    const int neighbor_col = ((center.col+cells[j].col+n)%grid_width+grid_width)%grid_width;
                
                    const Polyomino * neighbor = tiling.GetGridCell(neighbor_row, neighbor_col);
                    
                    int already_neighbor = (neighbor == tile);
                    for(int k = 0; k < tile->NumNeighbors(); k++)
                    {
                        if((tile->GetNeighbor(k)).polyomino == neighbor)
                        {
                            already_neighbor = 1;
                        }
                    }

                    if(neighbor && !already_neighbor)
                    {
                        // add it
                        Polyomino::Int2 neighbor_center;
                        neighbor->GetCenter(neighbor_center);
                        
                        tile->AddNeighbor(Polyomino::Neighbor(neighbor, Polyomino::Int2(neighbor_center.row - center.row, neighbor_center.col - center.col)));
                    }
                }
        }   
    }
}
