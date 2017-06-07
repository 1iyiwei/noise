/*
  PPentominoSubdivider.cpp

  Li-Yi Wei
  10/27/2007

*/

#include "PPentominoSubdivider.hpp"

void PPentominoSubdivider::Subdivide(const PPentomino * mother, vector<PPentomino *> & children, const int which_rule)
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
        SubdivideCanonical(mother, children, which_rule);
        Reflect(mother, children);
        Rotate(mother, children);
        Shift(mother, children);
    }
}
    
void PPentominoSubdivider::SubdivideCanonical(const PPentomino * mother, vector<PPentomino *> & children, const int which_rule)
{
    if(children.size() > 0)
    {
        for(unsigned int i = 0; i < children.size(); i++)
        {
            delete children[i];
        }

        children.clear();
    }

    children = vector<PPentomino *>(4);
    for(unsigned int i = 0; i < children.size(); i++)
    {
        children[i] = new PPentomino;
    }
    
    if(which_rule%2)
    {
        children[0]->SetCenter(Polyomino::Int2(-1, -1));
        children[0]->SetReflection(Polyomino::R1);
        children[0]->SetOrientation(Polyomino::ON);

        children[1]->SetCenter(Polyomino::Int2(-1,  1));
        children[1]->SetReflection(Polyomino::R2);
        children[1]->SetOrientation(Polyomino::ON);

        children[2]->SetCenter(Polyomino::Int2( 1,  0));
        children[2]->SetReflection(Polyomino::R1);
        children[2]->SetOrientation(Polyomino::OW);

        children[3]->SetCenter(Polyomino::Int2( 3, -1));
        children[3]->SetReflection(Polyomino::R2);
        children[3]->SetOrientation(Polyomino::OS);
    }
    else
    {
        children[0]->SetCenter(Polyomino::Int2( 0, -1));
        children[0]->SetReflection(Polyomino::R2);
        children[0]->SetOrientation(Polyomino::OS);

        children[1]->SetCenter(Polyomino::Int2(-1,  1));
        children[1]->SetReflection(Polyomino::R1);
        children[1]->SetOrientation(Polyomino::OE);

        children[2]->SetCenter(Polyomino::Int2( 1,  1));
        children[2]->SetReflection(Polyomino::R2);
        children[2]->SetOrientation(Polyomino::OE);

        children[3]->SetCenter(Polyomino::Int2( 3, -1));
        children[3]->SetReflection(Polyomino::R2);
        children[3]->SetOrientation(Polyomino::OS);
    }
}

void PPentominoSubdivider::Reflect(const PPentomino * mother, const vector<PPentomino *> & children_input)
{
    vector<Polyomino *> children(children_input.size());

    for(unsigned int i = 0; i < children.size(); i++)
    {
        children[i] = children_input[i];
    }

    PolyominoSubdivider::Reflect(mother, children);
}

void PPentominoSubdivider::Rotate(const PPentomino * mother, const vector<PPentomino *> & children_input)
{
    vector<Polyomino *> children(children_input.size());

    for(unsigned int i = 0; i < children.size(); i++)
    {
        children[i] = children_input[i];
    }

    PolyominoSubdivider::Rotate(mother, children);
}

void PPentominoSubdivider::Shift(const PPentomino * mother, const vector<PPentomino *> & children_input)
{
    vector<Polyomino *> children(children_input.size());

    for(unsigned int i = 0; i < children.size(); i++)
    {
        children[i] = children_input[i];
    }

    PolyominoSubdivider::Shift(mother, children, 2); // 2^2 subdivision rule
}
