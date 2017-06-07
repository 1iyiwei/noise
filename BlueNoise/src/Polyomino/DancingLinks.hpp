/*
  DancingLinks.hpp

  a beautiful algorithm from Knuth

  Li-Yi Wei
  10/24/2007

*/

#ifndef _DANCING_LINKS_HPP
#define _DANCING_LINKS_HPP

#include <vector>
using namespace std;

class DancingLinks
{
public:
    struct Int2
    {
        Int2(void) : row(0), col(0) {};
        Int2(const int rowi, const int coli) : row(rowi), col(coli) {};
        int row, col;
    };

    typedef vector<Int2> SparseBinaryMatrix;

    // entries of input specifying the locations of "1"
    DancingLinks(const SparseBinaryMatrix & input);
    ~DancingLinks(void);

    // the solution would contain the rows that would partition all columns
    // an empty solution means no more can be found
    void NextSolution(vector<int> & solution);
    
protected:
    // return 1 if movable, 0 else (i.e. done)
    int Move(void);
    
    // data structures adopted from Knuth's paper on Dancing Links
    // I know only abstract classes should be inherited
    // but it is really handy here
    struct Data
    {
        Data(void) : left(0), right(0), up(0), down(0), center(0) {};
        virtual ~Data(void) {};
        
        Data * left; Data * right; Data * up; Data * down; Data * center;
    };

    struct Column : public Data
    {
        Column(const int name): Data(), id(name), count(0) {};
        const int id;
        int count;
    };

    static Data * Initialize(const SparseBinaryMatrix & input);
    
    void Cover(Column * column);
    void Uncover(Column * column);
    
protected:
    Data * _root;

    vector<Data *> _data_pool; // use this to track all memory allocated

    // stacks for dancing
    vector<Column *> _c_stack;
    vector<Data *> _r_stack;
};

#endif
