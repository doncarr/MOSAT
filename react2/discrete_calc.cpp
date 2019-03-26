
/************************************************************************

Dcalc.c

The member functions for discrete calculation points.

*************************************************************************/


#include <stdio.h>
#include <malloc.h>

#include "rtcommon.h"

#include "reactpoint.h"
#include "exp.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"
#include "error.h"

/************************************************************************/

void dcalc_point_t::evaluate()
{
  /* Evaluate the dcalc point. */

  pv = (discrete_value_t) expression.evaluate();
  //printf("Discrete calc %s: expr: %s, val: %s\n", tag, expr_string, pv ? "TRUE" : "FALSE");
  check_alarms();
  display_pv();
}

/*********************************************************************/

void dcalc_point_t::init_values()
{
  pv_string = lo_desc;
  pv = false;
  json_str = NULL;
  switch (this->alarm_state_str[0])
  {
      case 'H':
      case 'h':
	this->alarm_state = DISCRETE_HI;
	break;
      case 'L':
      case 'l':
	this->alarm_state = DISCRETE_LO;
	break;
      case 'N':
      case 'n':
	this->alarm_state = DISCRETE_NONE;
	break;
      default:
        logfile->vprint("  %s Bad value for alarm state: %s\n", 
                  tag, alarm_state_str);
	this->alarm_state = DISCRETE_NONE;
	break;
  }
  switch (this->shutdown_state_str[0])
  {
      case 'H':
      case 'h':
	this->shutdown_state = DISCRETE_HI;
	break;
      case 'L':
      case 'l':
	this->shutdown_state = DISCRETE_LO;
	break;
      case 'N':
      case 'n':
	this->shutdown_state = DISCRETE_NONE;
	break;
      default:
	this->shutdown_state = DISCRETE_NONE;
        logfile->vprint("  %s Bad value for shutdown state: %s\n", 
                  tag, shutdown_state_str);
	break;
  }
}

/*********************************************************************/

void dcalc_point_t::parse_expr(void)
{
  /* Parse the expression for the dcalc point. */

  logfile->vprint("%s Parsing Expression: %s\n", tag, expr_string);
  expression.expr = make_expr(expr_string);
  if (expression.expr == NULL)
  {
    logfile->vprint("  Discrete calc %s: Bad Expression: %s\n", tag, expr_string);
    expression.expr = new expr_op_t[2];
    expression.expr[0].token_type = LOGICAL_VAL;
    expression.expr[0].val.logical_val = 0;
    expression.expr[1].token_type = END_EXPR;
  }
}

/***********************************************************************/
