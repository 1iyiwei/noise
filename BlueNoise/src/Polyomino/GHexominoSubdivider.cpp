/*
  GHexominoSubdivider.cpp

  Li-Yi Wei
  11/12/2007

*/

#include "GHexominoSubdivider.hpp"
#include "Exception.hpp"

void GHexominoSubdivider::Subdivide(const GHexomino * mother, vector<GHexomino *> & children)
{
    if(children.size() > 0)
    {
        for(unsigned int i = 0; i < children.size(); i++)
        {
            delete children[i];
        }

        children.clear();
    }

    if(mother)
    {
        SubdivideCanonical(mother, children);
        Reflect(mother, children);
        Rotate(mother, children);
        Shift(mother, children);
    }
}
    
void GHexominoSubdivider::SubdivideCanonical(const GHexomino * mother, vector<GHexomino *> & children)
{
    if(children.size() > 0)
    {
        for(unsigned int i = 0; i < children.size(); i++)
        {
            delete children[i];
        }

        children.clear();
    }

    // computed from GHexominoSubdivisionTiler class
    const int num_records = 81;
    const TileRecord records[num_records] =
        {
            {1, 1, Polyomino::OS, Polyomino::R2},
            {1, 4, Polyomino::OW, Polyomino::R2},
            {1, 8, Polyomino::OW, Polyomino::R2},
            {2, 6, Polyomino::OW, Polyomino::R1},
            {4, 8, Polyomino::ON, Polyomino::R2},
            {10, 26, Polyomino::OS, Polyomino::R1},
            {10, 23, Polyomino::OE, Polyomino::R1},
            {17, 26, Polyomino::OW, Polyomino::R1},
            {14, 26, Polyomino::OS, Polyomino::R1},
            {12, 25, Polyomino::OS, Polyomino::R2},
            {26, 1, Polyomino::OE, Polyomino::R2},
            {23, 1, Polyomino::OS, Polyomino::R2},
            {26, 17, Polyomino::OW, Polyomino::R1},
            {23, 17, Polyomino::OS, Polyomino::R1},
            {3, 2, Polyomino::OS, Polyomino::R1},
            {5, 1, Polyomino::OS, Polyomino::R2},
            {4, 4, Polyomino::OE, Polyomino::R1},
            {5, 6, Polyomino::OS, Polyomino::R2},
            {7, 8, Polyomino::ON, Polyomino::R2},
            {7, 3, Polyomino::ON, Polyomino::R2},
            {8, 5, Polyomino::ON, Polyomino::R1},
            {8, 1, Polyomino::OS, Polyomino::R2},
            {10, 3, Polyomino::ON, Polyomino::R2},
            {11, 1, Polyomino::OS, Polyomino::R2},
            {10, 20, Polyomino::OE, Polyomino::R1},
            {12, 22, Polyomino::OW, Polyomino::R1},
            {14, 23, Polyomino::ON, Polyomino::R2},
            {16, 24, Polyomino::OW, Polyomino::R2},
            {17, 22, Polyomino::OW, Polyomino::R1},
            {15, 20, Polyomino::OE, Polyomino::R1},
            {13, 19, Polyomino::OS, Polyomino::R2},
            {11, 18, Polyomino::OE, Polyomino::R2},
            {10, 16, Polyomino::OE, Polyomino::R1},
            {17, 19, Polyomino::OW, Polyomino::R1},
            {10, 13, Polyomino::OE, Polyomino::R1},
            {12, 15, Polyomino::OW, Polyomino::R1},
            {13, 17, Polyomino::OS, Polyomino::R1},
            {11, 11, Polyomino::OE, Polyomino::R2},
            {10, 9, Polyomino::OE, Polyomino::R1},
            {9, 7, Polyomino::OE, Polyomino::R1},
            {10, 5, Polyomino::OS, Polyomino::R2},
            {13, 3, Polyomino::ON, Polyomino::R2},
            {14, 1, Polyomino::OS, Polyomino::R2},
            {16, 17, Polyomino::OW, Polyomino::R1},
            {15, 15, Polyomino::OW, Polyomino::R1},
            {14, 13, Polyomino::OW, Polyomino::R1},
            {13, 11, Polyomino::OW, Polyomino::R2},
            {12, 7, Polyomino::OE, Polyomino::R1},
            {13, 5, Polyomino::OE, Polyomino::R2},
            {14, 9, Polyomino::OW, Polyomino::R1},
            {20, 1, Polyomino::OS, Polyomino::R2},
            {22, 3, Polyomino::ON, Polyomino::R2},
            {17, 1, Polyomino::OS, Polyomino::R2},
            {16, 3, Polyomino::ON, Polyomino::R2},
            {19, 3, Polyomino::ON, Polyomino::R2},
            {15, 5, Polyomino::OE, Polyomino::R1},
            {18, 5, Polyomino::ON, Polyomino::R1},
            {16, 7, Polyomino::OE, Polyomino::R1},
            {20, 5, Polyomino::OE, Polyomino::R1},
            {19, 8, Polyomino::ON, Polyomino::R2},
            {23, 5, Polyomino::OW, Polyomino::R1},
            {25, 3, Polyomino::OE, Polyomino::R1},
            {26, 5, Polyomino::OE, Polyomino::R2},
            {16, 10, Polyomino::OS, Polyomino::R1},
            {19, 10, Polyomino::ON, Polyomino::R1},
            {16, 12, Polyomino::OS, Polyomino::R2},
            {18, 14, Polyomino::ON, Polyomino::R2},
            {18, 16, Polyomino::OS, Polyomino::R2},
            {20, 17, Polyomino::OS, Polyomino::R1},
            {22, 15, Polyomino::ON, Polyomino::R1},
            {20, 13, Polyomino::ON, Polyomino::R1},
            {21, 11, Polyomino::OE, Polyomino::R2},
            {23, 13, Polyomino::OE, Polyomino::R2},
            {25, 15, Polyomino::OW, Polyomino::R2},
            {26, 13, Polyomino::OW, Polyomino::R1},
            {21, 7, Polyomino::OE, Polyomino::R1},
            {22, 9, Polyomino::OE, Polyomino::R2},
            {24, 7, Polyomino::ON, Polyomino::R1},
            {24, 11, Polyomino::ON, Polyomino::R2},
            {26, 8, Polyomino::ON, Polyomino::R2},
            {26, 10, Polyomino::ON, Polyomino::R1}
        };

    children = vector<GHexomino *>(num_records);

    for(unsigned int i = 0; i < children.size(); i++)
    {
        const TileRecord record = records[i];
        children[i] = new GHexomino(Polyomino::Int2(record.row, record.col), record.orientation, record.reflection);
    }
        
    // coordinate shift
    for(unsigned int i = 0; i < children.size(); i++)
    {
        GHexomino::Int2 center;
        children[i]->GetCenter(center);
        center.row -= 2*9;
        center.col -= 1*9;
        children[i]->SetCenter(center);
    }
}

void GHexominoSubdivider::Reflect(const GHexomino * mother, const vector<GHexomino *> & children_input)
{
    vector<Polyomino *> children(children_input.size());

    for(unsigned int i = 0; i < children.size(); i++)
    {
        children[i] = children_input[i];
    }

    PolyominoSubdivider::Reflect(mother, children);
}

void GHexominoSubdivider::Rotate(const GHexomino * mother, const vector<GHexomino *> & children_input)
{
    vector<Polyomino *> children(children_input.size());

    for(unsigned int i = 0; i < children.size(); i++)
    {
        children[i] = children_input[i];
    }

    PolyominoSubdivider::Rotate(mother, children);
}

void GHexominoSubdivider::Shift(const GHexomino * mother, const vector<GHexomino *> & children_input)
{
    vector<Polyomino *> children(children_input.size());

    for(unsigned int i = 0; i < children.size(); i++)
    {
        children[i] = children_input[i];
    }

    PolyominoSubdivider::Shift(mother, children, 9); // 9^2 subdivision rule
}
