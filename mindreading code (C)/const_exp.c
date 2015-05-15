/*************************************************************************
**
** const_exp.c
**
** This family of experts parameterizes the constant functions 0 and 1.
**
*************************************************************************/
#include "expert.h"

double
const_exp(command, parm)
int command, parm;
{
  switch(command) {
  case NUMBER:
    return 2.0;
  case INITIALIZE:
    return;
  case PREDICTION:   /* if parm = 0, then constant function 0*/
    return parm;     /* if parm = 1, then constant function 1*/
  case UPDATE:
    return;
  case SEND_ARCHIVE_ID:
    archive_expert_id("const_exp");
  }
}
