/************************************************************************
This software is part of React, a control engine
Copyright (C) 2005,2006 Donald Wayne Carr 

This program is free software; you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation; either version 2 of the License, or 
(at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You should have received a copy of the GNU General Public License along 
with this program; if not, write to the Free Software Foundation, Inc., 
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
***********************************************************************/

/***********
Calc.c

Member functions for analog calculation points.

*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "exp.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"
#include "error.h"

/************************************************************************/

void calc_point_t::update()
{
  /* Evaluate the expression for the given calculation point. */

  if (expression.expr != NULL) pv = expression.evaluate();
  else pv = 0.0;
  //printf("%s.pv = %lf\n", tag, pv);
  check_alarms();
  display_pv();
  //update_window();
}

/*********************************************************************/

void calc_point_t::init_values(void)
{
  this->expression.expr = NULL;
  this->set_format();
}

/*********************************************************************/

void calc_point_t::parse_expr(void)
{
  /* Parse the expression for the given calc point. */
  logfile->vprint("%s Parsing \"%s\"\n", tag, expr_string);
  expression.expr = make_expr(expr_string);
  if (expression.expr == NULL)
  {
    expression.expr = new expr_op_t[2];
    expression.expr[0].token_type = FLOAT_VAL;
    expression.expr[0].val.float_val = 0.0;
    expression.expr[1].token_type = END_EXPR;
    logfile->vprint("  Analog Calc %s: Bad Expression: \"%s\", %s\n",
          tag, expr_string, rtexperror.str());
  }
}

/************************************************************************/
