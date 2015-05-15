#include <stdio.h>
#include <stdlib.h>
#include "expert.h"
#include "dutch_trees.h"

void
update_wt(node, ex, y, beta)
NODE node;
BITSTRING ex;
int y;
double beta;
{
  node->wt[1-y] *= beta;

  if (ex == NULL)
    node->end_wt = 0.5*(1.+beta);
  else
    update_wt(node->son[ex->bit], ex->next, y, beta);

  node->sum_wt =
    0.25*(node->wt[0] + node->wt[1]) +
    0.5 * get_sum_wt(node->son[0]) * get_sum_wt(node->son[1]) * node->end_wt;
}

double
get_sum_wt(node)
NODE node;
{
  if (node == NULL)
    return 1.;
  else
    return node->sum_wt;
}

double
get_pred_wt(node, ex)
NODE node;
BITSTRING ex;
{
  if (ex == NULL)
    return 0.25 * (node->wt[1] +
		   get_sum_wt(node->son[0]) * get_sum_wt(node->son[1]));
  else {
    int b = ex->bit;
    if (node->son[b] == NULL)
      node->son[b] = init_node();
    return 0.25 * node->wt[1] +
      0.5 * get_sum_wt(node->son[1-b]) * get_pred_wt(node->son[b], ex->next)
	  * node->end_wt;
  }
}

NODE
init_node()
{
  NODE node = (NODE) (malloc(sizeof(NODE_REC)));
  node->son[0] = node->son[1] = NULL;
  node->wt[0] = node->wt[1] = node->end_wt = node->sum_wt = 1.;
  return node;
}

BITSTRING
add_bit(b, s)
int b;
BITSTRING s;
{
  BITSTRING ns = (BITSTRING) (malloc(sizeof(BIT_REC)));
  ns->bit = b;
  ns->next = s;
  return ns;
}
