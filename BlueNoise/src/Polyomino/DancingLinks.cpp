/*
  DancingLinks.cpp

  Li-Yi Wei
  10/24/2007

*/

#include "DancingLinks.hpp"
#include "Exception.hpp"

#include <assert.h>

DancingLinks::DancingLinks(const SparseBinaryMatrix & input) : _root(Initialize(input))
{
    if(! _root) throw Exception("error in constructing dancing links root from input sparse matrix specifications");

    // fill all data nodes into the data pool
    Data * column_head = _root;
    do
    {
        Data * data = column_head;

        do
        {
            _data_pool.push_back(data);
            data = column_head->down;
        }
        while(data != column_head);
        
        column_head = column_head->right;
    }
    while(column_head != _root);
}

DancingLinks::~DancingLinks(void)
{
    for(unsigned int i = 0; i < _data_pool.size(); i++)
    {
        if(_data_pool[i]) delete _data_pool[i];
    }
}

void DancingLinks::NextSolution(vector<int> & solution)
{
    throw Exception("not yet done");
}

int DancingLinks::Move(void)
{
    if(_r_stack.size() <= 0)
    {
        if(_root->right == _root)
        {
            // c_stack full, done
            return 0;
        }
        else
        {
            // look for column with minimum count
            Column * minimum_column = dynamic_cast<Column *>(_root->right);
            int minimum_count = minimum_column->count;

            for(Column * column = dynamic_cast<Column *>(_root->right); column != _root; column = dynamic_cast<Column *>(column->right))
            {
                if(column->count < minimum_count)
                {
                    minimum_column = column;
                    minimum_count = column->count;
                }
            }

            _c_stack.push_back(minimum_column);
            Cover(minimum_column);

            Data * row = minimum_column->down;
            _r_stack.push_back(row);

            assert((_r_stack.size() > 0) && (_c_stack.size() > 0));

            if(_r_stack[_r_stack.size()-1] == _c_stack[_c_stack.size()-1])
            {
                _r_stack.pop_back();

                throw Exception("not yet done");
                return 1;
            }
            else
            {
                for(Data * data = _r_stack[_r_stack.size()-1]->right; data != _r_stack[_r_stack.size()-1]; data = data->right)
                {
                    Cover(dynamic_cast<Column *>(data->center));
                }

                throw Exception("not yet done");
                return 1;
            }
        }
    }
    else // (_r_stack.size() > 0)
    {
        throw Exception("not yet done");
        return 1;
    }
}

DancingLinks::Data * DancingLinks::Initialize(const SparseBinaryMatrix & input)
{
    Data * root = 0;

    if(input.size() > 0)
    {
        // figure out size
        int row_max = -1; int col_max = -1;

        for(unsigned int i = 0; i < input.size(); i++)
        {
            if(input[i].row < 0) throw Exception("illegal input row in DancingLinks::Initialize");
            if(input[i].col < 0) throw Exception("illegal input col in DancingLinks::Initialize");

            if(input[i].row > row_max) row_max = input[i].row;
            if(input[i].col > col_max) col_max = input[i].col;
        }

        // width will include root and
        // height will include column heads as well
        const int height = row_max+1+1; const int width = col_max+1+1;

        if((height <= 1) || (width <= 1)) throw Exception("null height or width");

        vector< vector<Data *> > matrix(height);
        for(unsigned int i = 0; i < matrix.size(); i++)
        {
            matrix[i] = vector<Data *>(width);

            for(unsigned int j = 0; j < matrix[i].size(); j++)
            {
                matrix[i][j] = 0;
            }
        }

        for(unsigned int i = 0; i < input.size(); i++)
        {
            matrix[input[i].row+1][input[i].col+1] = new Data;
        }

        for(unsigned int i = 0; i < matrix[0].size(); i++)
        {
            matrix[0][i] = new Column(i);
        }

        root = matrix[0][0];

        // link data rows
        for(int i = 0; i < height; i++)
        {
            for(int j = 0; j < width; j++)
            {
                if(matrix[i][j])
                {
                    matrix[i][j]->right = matrix[i][j];
                    
                    for(int k = (j+1)%width; k != j; k = (k+1)%width)
                    {
                        if(matrix[i][k])
                        {
                            matrix[i][j]->right = matrix[i][k];
                            break;
                        }
                    }

                    matrix[i][j]->left = matrix[i][j];

                    for(int k = (j+width-1)%width; k != j; k = (k+width-1)%width)
                    {
                        if(matrix[i][k])
                        {
                            matrix[i][j]->left = matrix[i][k];
                            break;
                        }
                    }
                }
            }

        }

        // link data columns
        for(int j = 0; j < width; j++)
        {
            for(int i = 0; i < height; i++)
            {
                if(matrix[i][j])
                {
                    matrix[i][j]->down = matrix[i][j];

                    for(int k = (i+1)%height; k != i; k = (k+1)%height)
                    {
                        if(matrix[k][j])
                        {
                            matrix[i][j]->down = matrix[k][j];
                            break;
                        }
                    }

                    matrix[i][j]->up = matrix[i][j];
                    for(int k = (i+height-1)%height; k != i; k = (k+height-1)%height)
                    {
                        if(matrix[k][j])
                        {
                            matrix[i][j]->up = matrix[k][j];
                            break;
                        }
                    }
                }
            }
        }
        
        // link data centers
        for(int j = 0; j < width; j++)
        {
            for(int i = 0; i < height; i++)
            {
                if(matrix[i][j])
                {
                    matrix[i][j]->center = matrix[0][j];
                    (dynamic_cast<Column *>(matrix[0][j]))->count++;
                }
            }
        }
    }

    // done
    return root;
}

void DancingLinks::Cover(Column * column)
{
    column->right->left = column->left;
    column->left->right = column->right;

    for(Data * i = column->down; i != column; i = i->down)
    {
        for(Data * j = i->right; j != i; j = j->right)
        {
            j->down->up = j->up;
            j->up->down = j->down;

            (dynamic_cast<Column *>(j->center))->count--;
        }   
    }
}

void DancingLinks::Uncover(Column * column)
{
    for(Data * i = column->up; i != column; i = i->up)
    {
        for(Data * j = i->left; j != i; j = j->left)
        {
            (dynamic_cast<Column *>(j->center))->count++;
            j->down->up = j;
            j->up->down = j;
        }
    }

    column->right->left = column;
    column->left->right = column;
}
