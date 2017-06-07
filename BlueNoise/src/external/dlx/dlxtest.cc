/* A very simple example (sorry for the lack of a better tutorial for the moment)

 Let's suppose we want to solve this matrix:

 101
 111
 100

 where all rows are primary

 every row is introduced to the C++ class in a SPARSE way: only column positions with a 1 are introduced:
 
  r\c| 012  
 ----------
  0  | 101  -> columns 0 and 2
  1  | 111  -> columns 0, 1, 2
  2  | 100  -> only column 0

 results are given as sets of row indices in a vector of size the number of rows

 Constructor needs the number of rows, columns and the maximum number
 of 1s in the matrix (columns*rows allways work since this is an upper
 bound)

*/

using namespace std;
#include <iostream>
#include "dlx.cc"

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

