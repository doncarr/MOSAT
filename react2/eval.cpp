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

/*******
Eval.c

Procedures to evaluate expressions.

**************************************************************************/
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "exp.h"

analog_point_handler_t analog_point_handler = NULL;
discrete_point_handler_t discrete_point_handler = NULL;

/***********************************************************************

void set_eval_analog_point_handler(analog_point_handler_t handler)
{
  analog_point_handler = handler;
}


void set_eval_discrete_point_handler(discrete_point_handler_t handler)
{
  discrete_point_handler = handler;
}

***********************************************************************/

double analog_expr_t::evaluate(void)
{
  /* Evaluate an analog expression. */
  return eval_expr(expr)->float_val;
}
/***********************************************************************/

bool discrete_expr_t::evaluate(void)
{
  /* Evaluate a discrete expression. */
  //printf("################### evaluating discrete expression\n");
  return eval_expr(expr)->logical_val;
}

/**********************************************************************/

stack_value_t *eval_expr(expr_op_t *expr_ptr)
{
  /* Evaluate an expressioin. */
  static stack_value_t stack[100];
  stack_value_t *stack_ptr;
  double fop1, fop2, *fresult;
  bool lop1, lop2, *lresult;

  int done;

  stack_ptr = stack;
  done = 0;
  while (!done)
  {
    switch (expr_ptr->token_type)
    {
      case PLUS_OP:
      case MINUS_OP:
      case MULTIPLY_OP:
      case DIVIDE_OP:
      case POWER_OP:
	stack_ptr--;
	fop1 = (stack_ptr - 1)->float_val;
	fop2 = stack_ptr->float_val;
	if ((fop1 == FLT_MAX) || (fop2 == FLT_MAX))
	{
	  stack[0].float_val = FLT_MAX;
	  stack[0].logical_val = false;

	  return &stack[0];
	}
	fresult = &((stack_ptr - 1)->float_val);
	break;
      case LE_OP:
      case GE_OP:
      case LT_OP:
      case GT_OP:
      case EQ_OP:
      case NE_OP:
	stack_ptr--;
	fop1 = (stack_ptr - 1)->float_val;
	fop2 = stack_ptr->float_val;
	if ((fop1 == FLT_MAX) || (fop2 == FLT_MAX))
	{
	  stack[0].float_val = FLT_MAX;
	  stack[0].logical_val = false;
	  return &stack[0];
	}
	lresult = &((stack_ptr - 1)->logical_val);
	break;
      case AND_OP:
      case OR_OP:
	stack_ptr--; // added 9/16/99
	lop1 = (stack_ptr - 1)->logical_val;
	lop2 = stack_ptr->logical_val;
        /*****
	if ((lop1 == DISCRETE_FAILED) || (lop2 == DISCRETE_FAILED))
	{
	  stack[0].float_val = FLT_MAX;
	  stack[0].logical_val = DISCRETE_FAILED;
	  return &stack[0];
	}
        ******/
	lresult = &((stack_ptr - 1)->logical_val);
	break;
      case NOT_OP:
	lop1 = (stack_ptr - 1)->logical_val;
        /*****
	if (lop1 == DISCRETE_FAILED)
	{
	  stack[0].float_val = FLT_MAX;
	  stack[0].logical_val = DISCRETE_FAILED;
	  return &stack[0];
	}
        ********/
	lresult = &((stack_ptr - 1)->logical_val);
	break;
      case UNARY_MINUS_OP:
      case SIN_OP:
      case COS_OP:
      case TAN_OP:
      case ASIN_OP:
      case ACOS_OP:
      case ATAN_OP:
      case SQR_OP:
      case SQRT_OP:
      case LOG_OP:
      case LOG10_OP:
      case EXP_OP:
      case ABS_OP:
	fop1 = (stack_ptr - 1)->float_val;
	fresult = &((stack_ptr - 1)->float_val);
	if (fop1 == FLT_MAX)
	{
	  stack[0].float_val = FLT_MAX;
	  stack[0].logical_val = false;
	  return &stack[0];
	}
	break;
      default:
        break;
    }

    switch (expr_ptr->token_type)
    {
      case PLUS_OP:
	*fresult = fop1 + fop2;
	break;
      case MINUS_OP:
	*fresult = fop1 - fop2;
	break;
      case MULTIPLY_OP:
	*fresult = fop1 * fop2;
	break;
      case DIVIDE_OP:
	if (fop2  != 0)
	{
	  *fresult = fop1 / fop2;
	}
	else
	{
	  *fresult = 0.0;
	}
	break;
      case POWER_OP:
	*fresult = (double) pow(fop1, fop2);
	break;
      case LE_OP:
	*lresult = (fop1 <= fop2);// ? DISCRETE_HI : DISCRETE_LO;
	break;
      case GE_OP:
	*lresult = (fop1 >= fop2);// ? DISCRETE_HI : DISCRETE_LO;
	break;
      case LT_OP:
	*lresult = (fop1 < fop2);// ? DISCRETE_HI : DISCRETE_LO;
	break;
      case GT_OP:
	*lresult = (fop1 > fop2);// ? DISCRETE_HI : DISCRETE_LO;
	break;
      case EQ_OP:
	*lresult = (fop1 == fop2);// ? DISCRETE_HI : DISCRETE_LO;
	break;
      case NE_OP:
	*lresult = (fop1 != fop2);// ? DISCRETE_HI : DISCRETE_LO;
	break;
      case AND_OP:
        //printf("And\n");
	*lresult = (lop1 && lop2);// ? DISCRETE_HI : DISCRETE_LO;
	break;
      case OR_OP:
	*lresult = (lop1 || lop2) ;//? DISCRETE_HI : DISCRETE_LO;
	break;
      case NOT_OP:
        //printf("%c %c\n", (*lresult) ? 'T':'F',  (lop1) ? 'T':'F');
	*lresult = (!lop1) ;//? DISCRETE_LO : DISCRETE_HI;
        //printf("%c %c\n", (*lresult) ? 'T':'F',  (lop1) ? 'T':'F');
	break;
      case UNARY_MINUS_OP:
        //printf("Unary Minus\n");
	*fresult = -fop1;
	break;
      case SIN_OP:
	*fresult = (double) sin(fop1);
	break;
      case COS_OP:
	*fresult = (double) cos(fop1);
	break;
      case TAN_OP:
	*fresult = (double) tan(fop1);
	break;
      case ASIN_OP:
	if ((fop1 >= -1.0) && (fop1 <= 1.0))
	{
	  *fresult = (double) asin(fop1);
	}
	else
	{
	  *fresult = 0.0;
	}
	break;
      case ACOS_OP:
	if ((fop1 >= -1.0) && (fop1 <= 1.0))
	{
	  *fresult = (double) acos(fop1);
	}
	else
	{
	  *fresult = 0.0;
	}
	break;
       case ATAN_OP:
	*fresult = (double) atan(fop1);
	break;
      case SQR_OP:
	*fresult = (double) pow(fop1, 2.0);
	break;
      case SQRT_OP:
	*fresult = (double) sqrt(fop1);
	break;
      case EXP_OP:
	*fresult = (double) exp(fop1);
	break;
      case LOG_OP:
	if (fop1 <= 0.0)
	{
	  *fresult = 0.0;
	  break;
	}
	*fresult = (double) log(fop1);
	break;
      case LOG10_OP:
	if (fop1 <= 0.0)
	{
	  *fresult = 0.0;
	  break;
	}
	*fresult = (double) log10(fop1);
	break;
      case ABS_OP:
	*fresult = (double) fabs(fop1);
	break;
      case LOGICAL_PTR:
	(stack_ptr++)->logical_val = *expr_ptr->val.logical_ptr;
	break;
      case LOGICAL_REF:
        //printf("ref val = %s\n", expr_ptr->val.logical_ref->val() ? "T":"F");
	(stack_ptr++)->logical_val = expr_ptr->val.logical_ref->val();
	break;
      /***
      case DISCRETE_POINT:
	if (discrete_point_handler == NULL)
	{
	  (stack_ptr++)->logical_val = false;
	  break;
        }
	(stack_ptr++)->logical_val =
	      (*discrete_point_handler)(expr_ptr->val.discrete_point);
	break;
      ***/
      case FLOAT_PTR:
	(stack_ptr++)->float_val = *expr_ptr->val.float_ptr;
	break;
      case FLOAT_REF:
        //printf("ref val = %lf\n", expr_ptr->val.float_ref->val());
	(stack_ptr++)->float_val = expr_ptr->val.float_ref->val();
	break;
      /***
      case ANALOG_POINT:
	if (analog_point_handler == NULL)
	{
	  (stack_ptr++)->float_val = 0;
	  break;
	}
	fop1 = (*analog_point_handler)(expr_ptr->val.analog_point);
	(stack_ptr++)->float_val = fop1;
	break;
      ***/
      case INT_PTR:
	(stack_ptr++)->float_val = double(*expr_ptr->val.int_ptr);
	break;
      case FLOAT_VAL:
	(stack_ptr++)->float_val = expr_ptr->val.float_val;
	break;
      case INT_VAL:
	(stack_ptr++)->float_val = double(expr_ptr->val.int_val);
	break;
      case LOGICAL_VAL: (stack_ptr++)->logical_val = expr_ptr->val.logical_val;
	break;
      case END_EXPR:
	done = 1;
	break;
      default:
	/* Severe error */
	stack[0].float_val = FLT_MAX;
	stack[0].logical_val = false;
	return &stack[0];
    }
    expr_ptr++;
  }
  stack_ptr--;
  return (stack_ptr);
}

/*********************************************************************/
