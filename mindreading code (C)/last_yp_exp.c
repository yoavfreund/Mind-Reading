/*************************************************************************
**
** last_yp_exp.c
**
** Author: Robert Schapire (schapire@research.att.com)
**
** This family of experts predicts the next bit based on the last
** few of the user's bits AND the last few of the computer's predictions.
** Specifically, each expert can be viewed as a look-up table that maps
** the last few of the user's and computer's bits  to {0,1}.  The
** predictions are symmetric with respect to 0 and 1; thus, each expert
** is actually predicting whether or not the user will change his/her
** prediction.
**
*************************************************************************/
#include "expert.h"
#include <math.h>

#define  POW2(N)     (1 << (N))           /* 2^N */
#define  BIT(X, I)   (((X) >> (I)) & 1)   /* Ith l.s. bit of X */

#define PASTyp_DEPTH  (2)    /* number of trials in past to remember */

double
last_yp_exp(command, parm)
int command, parm;
{
  static lasty[PASTyp_DEPTH];
  static lastp[PASTyp_DEPTH];
  int p, s, i, j;
  
  switch(command) {
  case NUMBER:
    return ((double) (POW2(POW2(2*PASTyp_DEPTH-1))));
  case INITIALIZE:
    for (i = 0; i < PASTyp_DEPTH; i++)
      lasty[i] = -1;
    return;
  case PREDICTION:     /* parm gives values of look-up table for */
		       /* particular expert */
    if (lasty[0] == -1)        /* if first trial, return 1/2 */
      return 0.5;
    p = (lasty[0] != lastp[0] ? POW2(2*PASTyp_DEPTH-2) : 0);
    for (i = 1; i < PASTyp_DEPTH; i++) {
      if (lasty[i] == -1) {    /* if one of first few trials, then */
	s = 0;                 /* take average of predictions  */
	for(j = 0; j < POW2(2*(PASTyp_DEPTH - i)); j++)
	  s += BIT(parm, p+j);
	return fabs(((double) s)/POW2(2*(PASTyp_DEPTH - i)) - lasty[0]);
      }
      if (lasty[i] != lasty[0])
	p += POW2(2*(PASTyp_DEPTH - i)-1);
      if (lastp[i] != lasty[0])
	p += POW2(2*(PASTyp_DEPTH - i - 1));
    }
    return fabs((double) (BIT(parm, p) - lasty[0]));
  case UPDATE:
    for (i = PASTyp_DEPTH-1; i > 0; i--) {
      lasty[i] = lasty[i-1];
      lastp[i] = lastp[i-1];
    }
    lasty[0] = parm & 1;
    lastp[0] = parm >> 1;
    return;
  case SEND_ARCHIVE_ID:
    archive_expert_id("last_yp_exp");
    return;
  }
}
