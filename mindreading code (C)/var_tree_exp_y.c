/*************************************************************************
**
** var_tree_exp_y.c
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
var_tree_exp_y(command, parm)
int command, parm;
{
  static NODE tree;
  static BITSTRING ex;
  int y, p;

  switch(command) {
  case NUMBER:
    return 1.;
  case INITIALIZE:
    tree = init_node();
    ex = NULL;
    return 0.;
  case PREDICTION:
    return F(get_pred_wt(tree, ex)/get_sum_wt(tree));
  case UPDATE:
    y = parm & 1;
    update_wt(tree, ex, y, BETA);
    ex = add_bit(y, ex);
    return 0.;
  case SEND_ARCHIVE_ID:
    archive_expert_id("var_tree_exp_y");
    return 0.;
  }
}
