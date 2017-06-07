/*
  DancingLinks.cpp

  test the dlx code from http://www.dsic.upv.es/~sespana/DANCING_LINKS/

  Li-Yi Wei
  10/25/2007
  
*/

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include "dlx/dlx.hpp"

#if 1
int main(int argc, char **argv)
{
    if(argc < 2)
    {
        cerr << "Usage: " << argv[0] << " input_matrix" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];

    ifstream input(input_file_name);

    if(!input)
    {
        cerr << "error in opening " << input_file_name << endl;
        return 1;
    }

    int num_rows = 0;
    int num_cols = 0;

    input >> num_rows; input >> num_cols;

    if( (num_rows <= 0) || (num_cols <= 0))
    {
        cerr << "illegal matrix size" << endl;
        return 1;
    }

    vector< vector<int> > input_matrix(num_rows);
    int num_ones = 0;
    for(unsigned int i = 0; i < input_matrix.size(); i++)
    {
        input_matrix[i] = vector<int>(num_cols);

        for(unsigned int j = 0; j < input_matrix[i].size(); j++)
        {
            input >> input_matrix[i][j];

            if(input_matrix[i][j]) num_ones++;
        }
    }

    // solve
    dlx dlx_solver(num_cols, num_cols, num_ones);

    for(unsigned int i = 0; i < input_matrix.size(); i++)
    {
        for(unsigned int j = 0; j < input_matrix[i].size(); j++)
        {
            if(input_matrix[i][j]) dlx_solver.insert_element(j);
        }
        dlx_solver.end_row();
    }

    int * rows = new int[num_rows];
    int length;

    cout << "number of rows: "<< dlx_solver.get_num_rows() << endl;

    while (dlx_solver.get_solution(rows,length))
    { // one iteration for every solution:
        cout << "A solution:" << endl;
        for(int i=0; i<length; i++)
            cout << "row number " << rows[i] << endl;
    }

    delete[] rows;
    return 0;
}
#else
int main() {
  dlx dlx_solver(3,3,6);         // FIRST ROW  101
  dlx_solver.insert_element(0);  // column index 0
  dlx_solver.insert_element(2);  // column index 2
  dlx_solver.end_row();          // SECOND ROW 111
  dlx_solver.insert_element(0);  // column index 0
  dlx_solver.insert_element(1);  // column index 1
  dlx_solver.insert_element(2);  // column index 2
  dlx_solver.end_row();          // THIRD ROW
  dlx_solver.insert_element(0);  // column index 0
  dlx_solver.end_row();
  int rows[3];
  int length;
  cout << "Numrows: "<< dlx_solver.get_num_rows() << endl;
  while (dlx_solver.get_solution(rows,length)) { // one iteration for every solution:
    cout << "A solution:\n";
    for (int i=0;i<length;i++)
      cout << "row number " << rows[i] << endl;
  }
  return 0;
}
#endif

