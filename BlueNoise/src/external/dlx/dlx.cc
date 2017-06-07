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

#include "dlx.h"

dlx::dlx(int numcols, int num_primary, int max_ones) {
  // num_primary must be <= numcols
  num_cols  = numcols;
  num_rows  = 0;
  setofnodes= new dlx_node[max_ones];
  choice    = 0;
  root      = new dlx_column[num_cols+1];
  column    = root+1;
  // initialize data structures:
  for (int i=0;i<num_primary; i++) {
    column[i].h.up   = column[i].h.down = &column[i].h;
    column[i].len    = 0;
    column[i].prev   = &column[i-1];
    column[i-1].next = &column[i];
  }
  column[num_primary-1].next = root;
  root->prev = &column[num_primary-1];
  for (int i=num_primary;i<num_cols; i++) {
    column[i].h.up   = column[i].h.down = &column[i].h;
    column[i].len    = 0;
    column[i].prev   = column[i].next = &column[i];
  }
  //
  cur_node   = setofnodes-1;
  // setofnodes-1 because cur_node++ instead of cur_node = new node;
  row_start  = 0;
  // to solve the problem:
  best_col   = 0;
  recover    = false;
  level      = 0;
}

dlx::~dlx() {
  delete[] setofnodes;
  delete[] root;
  delete[] choice;
}

void dlx::insert_element(int col) {
  dlx_column *ccol = column+col;
  cur_node++; //cur_node = new dlx_node;
  if (row_start) {
    cur_node->left = prv_node;
    prv_node->right= cur_node;
  } else {
    row_start      = cur_node;
  }
  cur_node->col    = ccol;
  cur_node->row    = num_rows;
  cur_node->up     = ccol->h.up;
  ccol->h.up->down = cur_node;
  ccol->h.up       = cur_node;
  cur_node->down   = &ccol->h;
  prv_node         = cur_node;
  ccol->len++;
}

void dlx::end_row() {
  row_start->left = prv_node;
  prv_node->right = row_start;
  row_start       = 0;
  num_rows++;
}

inline void dlx::cover(dlx_column *c) { 
  dlx_column *l = c->prev;
  dlx_column *r = c->next;
  l->next = r;
  r->prev = l;
  for(dlx_node *rr = c->h.down; rr != &(c->h); rr = rr->down)
    for(dlx_node *nn = rr->right; nn != rr; nn = nn->right) {
      dlx_node *uu = nn->up;
      dlx_node *dd = nn->down;
      uu->down = dd;
      dd->up   = uu;
      nn->col->len--;
    }
}

inline void dlx::uncover(dlx_column *c) { 
  for(dlx_node *rr = c->h.up; rr != &(c->h); rr = rr->up)
    for(dlx_node *nn = rr->left; nn != rr; nn = nn->left){
      dlx_node *uu = nn->up;
      dlx_node *dd = nn->down;
      uu->down = dd->up = nn;
      nn->col->len++;
    }
  dlx_column *l = c->prev;
  dlx_column *r = c->next;
  l->next = r->prev= c;
}

bool dlx::get_solution(int *rows, int &length) {
  if (!choice) choice = new dlx_node*[num_rows+1];
  while (1) {
    if (recover) {
      for (dlx_node *pp = cur_node->left; pp != cur_node; pp = pp->left)
	uncover(pp->col);
      cur_node = choice[level] = cur_node->down;
    } else {
      int minlen = num_rows+1;
      for(dlx_column *cur_col = root->next; cur_col != root; cur_col = cur_col->next)
	if (cur_col->len < minlen) {
	  best_col = cur_col;
	  minlen   = cur_col->len;
	}
      cover(best_col);
      cur_node = choice[level] = best_col->h.down;
    }
    if ((recover = (cur_node == &(best_col->h)))) {
      uncover(best_col);
      if (level == 0) {
	recover = false; 
	return false;
      }
      level--;
      cur_node = choice[level];
      best_col = cur_node->col;
    } else {
      for (dlx_node *pp = cur_node->right; pp != cur_node; pp = pp->right)
	cover(pp->col);
      if (root->next == root) {
	recover = true;
	for(int k = 0; k <= level; k++)
	  rows[k] = choice[k]->row;
	length = level+1;
	return true;
      }
      level++;
    }
  }
}

