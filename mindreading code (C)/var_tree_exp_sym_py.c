/*************************************************************************
**
** var_tree_exp_sym_py.c
**
** Author: Robert Schapire (schapire@research.att.com)
**
**
*************************************************************************/
#include <stdio.h>
#include <math.h>
#include "expert.h"
#include "dutch_trees.h"

#define BETA   (.55)

static double
F(r)
double r;
{
  static double cbeta = -1.0;

  if (cbeta < 0.)
    cbeta = (BETA == 1.0 ? 0.5 : (1.+BETA)*log(2./(1.+BETA)) / (2.*(1.-BETA)));
  return (r <= 0.5 - cbeta ? 0. :
	 (r >= 0.5 + cbeta ? 1. :
	  0.5 * (1. - (1. - 2.*r)/(2.*cbeta))));
}

double
var_tree_exp_sym_py(command, parm)
int command, parm;
{
  static NODE tree;
  static BITSTRING ex;
  static int last_y;
  int y, dy, p, win;

  switch(command) {
  case NUMBER:
    return 1.;
  case INITIALIZE:
    last_y = -1;
    tree = init_node();
    ex = NULL;
    return 0.;
  case PREDICTION:
    if (last_y == -1)
      return 0.5;
    else
      return fabs(last_y - F(get_pred_wt(tree, ex)/get_sum_wt(tree)));
  case UPDATE:
    p = parm >> 1;
    y = parm & 1;
    win = (p == y);
    if (last_y != -1) {
      dy = (y != last_y);
      update_wt(tree, ex, dy, BETA);
      ex = add_bit(dy, ex);
    }
    ex = add_bit(win, ex);
    last_y = y;
    return 0.;
  case SEND_ARCHIVE_ID:
    archive_expert_id("var_tree_exp_py");
    return 0.;
  }
}
