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

/************************************************************************

Expr.c

Contains code for making executable instructions.

*************************************************************************/


#include <stdio.h>
#include <malloc.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "exp.h"
#include "dbref.h"

//typedef double *(*get_analog_ptr_fn_t)(char *tag);
//typedef bool *(*get_discrete_ptr_fn_t)(char *tag);

static get_analog_ptr_fn_t get_analog_ptr_fn;
static get_discrete_ptr_fn_t get_discrete_ptr_fn;


/*********************************************************************/

void set_get_discrete_ptr_fn(get_discrete_ptr_fn_t fn)
{
  get_discrete_ptr_fn = fn;
}

/*********************************************************************/

void set_get_analog_ptr_fn(get_analog_ptr_fn_t fn)
{
  get_analog_ptr_fn = fn;
}

/********************************************************************/

expr_op_t *make_expr(char *line)
{
  /* Make an expression from the given character string. */

  expr_tree_t *tree;
  expr_op_t *expr;
  int n_nodes;
  int n_made;

  if ((NULL == get_analog_ptr_fn) || (NULL == get_discrete_ptr_fn))
  {
    return NULL;
  }

  tree = get_expr_tree(line);
  if (tree == NULL)
  {
    return NULL;
  }

  n_nodes = tree->count_nodes();
  if (n_nodes == 0)
  {
    return NULL;
  }

  expr = new expr_op_t[n_nodes + 1];

  n_made = tree->make_post_fix(expr);

  if (n_made != n_nodes)
  {
    free(expr);
    delete tree;
    return NULL;
  }

  expr[n_nodes].token_type = END_EXPR;

  //expr_type_t expr_type;
  //expr_type = tree->type;

  delete tree;

  return expr;
}

/********************************************************************/

int expr_tree_t::make_post_fix(expr_op_t *op)
{
  /* This procedure makes an executable post-fix expression from the
     given expression tree. */

  int n = 0;
  /***
  db_point_t *db_point;
  analog_point_t *analog_point;
  discrete_point_t *discrete_point;
  ****/
  if (left != NULL)
  {
    n += left->make_post_fix(op);
  }
  if (right != NULL)
  {
    n += right->make_post_fix(op + n);
  }
  op[n].token_type = token;
  switch(token)
  {
    case DISCRETE_TAG:
      rt_bool_ref_t *bref;
      bref = react_get_discrete_ref_fn(data.tag);
      if (bref == NULL)
      {
         printf("BAD tag for expression: %s\n", data.tag);
	 op[n].token_type = LOGICAL_VAL;
	 op[n].val.logical_val = DISCRETE_LO;
      }
      else
      {
        printf("Ref created: good discrete tag for expression: %s\n", data.tag);
        op[n].token_type = LOGICAL_REF;
        op[n].val.logical_ref = bref;
      }
      /**
      bool *dptr;
      dptr = (*get_discrete_ptr_fn)(data.tag);
      if (dptr == NULL)
      {
         printf("BAD tag for expression: %s\n", data.tag);
	 op[n].token_type = LOGICAL_VAL;
	 op[n].val.logical_val = DISCRETE_LO;
      }
      else
      {
         printf("Good tag for expression: %s\n", data.tag);
        op[n].token_type = LOGICAL_PTR;
	op[n].val.logical_ptr = dptr;
      }
      ***/
      /*****
      db_point = db->get_db_point(data.tag);
      if (db_point->pv_type() != DISCRETE_VALUE)
      {
	 op[n].token_type = LOGICAL_VAL;
	 op[n].val.logical_val = DISCRETE_LO;
      }
      else
      {
	discrete_point = (discrete_point_t *) db_point;
	op[n].val.logical_ptr = discrete_point->pv_ptr();
      }
      ******/
      break;

    case ANALOG_TAG:
      rt_double_ref_t *aref;
      aref = react_get_analog_ref_fn(data.tag);
      if (aref == NULL)
      {
        printf("BAD tag for expression: %s\n", data.tag);
        op[n].token_type = FLOAT_VAL;
        op[n].val.float_val = 0.0;
      }
      else
      {
        printf("Ref created: good analog tag for expression: %s\n", data.tag);
	op[n].token_type = FLOAT_REF;
	op[n].val.float_ref = aref;
      }
      /***
      double *fptr;
      fptr = (*get_analog_ptr_fn)(data.tag);
      if (fptr == NULL)
      {
        op[n].token_type = FLOAT_VAL;
        op[n].val.float_val = 0.0;
      }
      else
      {
	op[n].token_type = FLOAT_PTR;
	op[n].val.float_ptr = fptr;
      }
      ****/
      /**********
      db_point = db->get_db_point(data.tag);
      if ((db_point == NULL) ||
	  (db_point->pv_type() != ANALOG_VALUE))
      {
	op[n].token_type = FLOAT_VAL;
	op[n].val.float_val = 0.0;
      }
      else
      {
	op[n].token_type = FLOAT_PTR;
	analog_point = (analog_point_t *) db_point;
	op[n].val.float_ptr = analog_point->pv_ptr();
      }
      *******/
      break;
    case NUMBER:
      printf("FLOAT_VAL: %lf\n", data.number);
      op[n].token_type = FLOAT_VAL;
      op[n].val.float_val = data.number;
      break;
    case LOGICAL_VAL:
      printf("LOGICAL_VAL: %s\n", data.boolean ? "true" : "false");
      op[n].token_type = LOGICAL_VAL;
      op[n].val.logical_val = data.boolean ? DISCRETE_HI : DISCRETE_LO;
      break;
    default:
      break;
  }
  return n + 1;
}

/*************************************************************************/
