/*
  PolyominoSubdivider.cpp

  Li-Yi Wei
  11/12/2007

*/

#include "PolyominoSubdivider.hpp"

void PolyominoSubdivider::Reflect(const Polyomino * mother, const vector<Polyomino *> & children)
{
    Polyomino::Reflection reflection;
    mother->GetReflection(reflection);
    
    if(reflection == Polyomino::R1)
    {
        // nothing to do
    }
    else
    {
        for(unsigned int i = 0; i < children.size(); i++)
        {
            children[i]->ReflectCol();
        }
    }
}

void PolyominoSubdivider::Rotate(const Polyomino * mother, const vector<Polyomino *> & children)
{
    Polyomino::Orientation orientation;
    mother->GetOrientation(orientation);

    for(unsigned int i = 0; i < children.size(); i++)
    {
        if(orientation == Polyomino::ON)
        {
            // nothing to do
        }
        else if(orientation == Polyomino::OW)
        {
            children[i]->Rotate90();
        }
        else if(orientation == Polyomino::OS)
        {
            children[i]->Rotate180();
        }
        else if(orientation == Polyomino::OE)
        {
            children[i]->Rotate270();
        }
    }
}

void PolyominoSubdivider::Shift(const Polyomino * mother, const vector<Polyomino *> & children, const int scaling)
{
    Polyomino::Int2 mother_center_scaled;
    mother->GetCenter(mother_center_scaled);
    mother_center_scaled.row *= scaling;
    mother_center_scaled.col *= scaling;

    for(unsigned int i = 0; i < children.size(); i++)
    {
        Polyomino::Int2 center;
        children[i]->GetCenter(center);
        center.row += mother_center_scaled.row;
        center.col += mother_center_scaled.col;
        children[i]->SetCenter(center);
    }
}
