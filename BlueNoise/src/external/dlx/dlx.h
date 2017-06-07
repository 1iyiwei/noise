/*

Dancing Links c++ implementation.

Author: Salvador Espana <sespana@dsic.upv.es>
You are encouraged to tell me where this code is useful.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifndef DLX_H
#define DLX_H

struct dlx_column; // forward declaration

struct dlx_node {
  dlx_node *left, *right, *up, *down;
  int row;
  dlx_column *col;
};

struct dlx_column {
  dlx_node h;
  int len;
  dlx_column *prev, *next;
};

class dlx {
  int num_cols;
  int num_rows;
  int level;
  bool recover;

  dlx_column *root;
  dlx_column *column;
  dlx_column *best_col;

  dlx_node *setofnodes;
  dlx_node *cur_node, *prv_node;
  dlx_node *row_start;
  dlx_node **choice;

  void cover(dlx_column*c);
  void uncover(dlx_column*c);
public:
  dlx(int numcols, int num_primary, int max_ones);
  ~dlx();
  void insert_element(int col); // insert 1 in sparse matrix
  void end_row(); // finish current row
  int get_num_rows() { return num_rows; }
  bool get_solution(int *rows, int &length); // true means solution found
};

#endif // DLX_H
