/*************************************************************************
**
** hagelbarger.c
**
** This expert is a direct implementation of Hagelbarger's original
** mind reading machine.
**
*************************************************************************/
#include "expert.h"
#include <math.h>

#define  BIT(X, I)   (((X) >> (I)) & 1)   /* Ith l.s. bit of X */

double
hagelbarger_exp(command, parm)
int command, parm;
{
  static int wonlast, wonbefore, playedsame;

  static int a_mem[8];  /* counter of outcomes given current state */
  static int b_mem[8];  /* lsb = won last time; next bit = won time before */
  static int lastpred;

  double preddiff;   /* prob. of predicting different */
  int s;

  switch(command) {
  case NUMBER:
    return 1.0;
  case INITIALIZE:
    for (s = 0; s < 8; s++) {
      a_mem[s] = 0;
      b_mem[s] = 0;
    }
    wonlast = 0;
    wonbefore = 0;
    playedsame = 0;
    lastpred = 0;
    return;
  case PREDICTION:
    s = 4*wonlast + 2*wonbefore + playedsame;
    if (b_mem[s] == 0)   /* if lost last two times, predict randomly */
      preddiff = 0.5;
    else if (b_mem[s] == 3) /* if won last two times predict what's in a_mem */
      preddiff = (a_mem[s] > 0 ? 0.0 : (a_mem[s] < 0 ? 1.0 : 0.5));
    else /* if won one of last two times, then 3-1 odds */
                            /* predict what's in a_mem */
      preddiff = (a_mem[s] > 0 ? 0.25 : (a_mem[s] < 0 ? 0.75 : 0.5));
    return fabs(lastpred - preddiff);
  case UPDATE:
    s = 4*wonlast + 2*wonbefore + playedsame;
    wonbefore = wonlast;
    playedsame = (BIT(parm,1) == lastpred);
    wonlast = (BIT(parm,0) == BIT(parm,1));
    a_mem[s] += (lastpred == BIT(parm,0) ? 1 : -1);
    if (a_mem[s] < -3)
      a_mem[s] = -3;
    if (a_mem[s] > 3)
      a_mem[s] = 3;
    b_mem[s] = (BIT(b_mem[s],0) << 1) | wonlast;
    lastpred = BIT(parm,1);
    return;
  case SEND_ARCHIVE_ID:
    archive_expert_id("hagelbarger");
    return;
  }
}
