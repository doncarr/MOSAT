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

/********
Exp.h

Header file for expression routines in exp.lib.

*************************************************************************/


#ifndef __EXP_INC__
#define __EXP_INC__

#include "dbref.h"

typedef enum
{
  PLUS_OP,
  MINUS_OP,
  UNARY_MINUS_OP,
  MULTIPLY_OP,
  DIVIDE_OP,
  LE_OP,
  LT_OP,
  GT_OP,
  GE_OP,
  EQ_OP,
  NE_OP,
  NOT_OP,
  AND_OP,
  OR_OP,
  POWER_OP,
  FLOAT_PTR,
  FLOAT_REF,
  FLOAT_VAL,
  INT_VAL,
  INT_PTR,
  INT_REF,
  LOGICAL_PTR,
  LOGICAL_REF,
  LOGICAL_VAL,
  VALUE_PTR,
  DISCRETE_TAG,
  //DISCRETE_POINT,
  ANALOG_TAG,
  //ANALOG_POINT,
  POINT_TAG,
  LEFT_PAREN,
  RIGHT_PAREN,
  NUMBER,
  END_EXPR,
  BAD_TOKEN,
  SIN_OP,
  COS_OP,
  TAN_OP,
  ASIN_OP,
  ACOS_OP,
  ATAN_OP,
  LOG_OP,
  LOG10_OP,
  EXP_OP,
  ABS_OP,
  SQRT_OP,
  SQR_OP,
}  expr_token_t;

enum  expr_type_t
{
  ANALOG_EXPR = 0,
  DISCRETE_EXPR = 1,
  BAD_EXPR = 2
};

union expr_data_t
{
  tag_t tag;
  double number;
  bool boolean;
};



typedef union
{
  double *float_ptr;
  rt_double_ref_t *float_ref;
  //void *analog_point;
  bool *logical_ptr;
  rt_bool_ref_t *logical_ref; 
  //void *discrete_point;
  int *int_ptr;
  double float_val;
  bool logical_val;
  int int_val;
}  exp_value_t;

typedef struct
{
  double float_val;
  bool logical_val;
}  stack_value_t;

struct expr_op_t
{
  expr_token_t token_type;
  exp_value_t val;
};

class analog_expr_t
{
public:
  expr_op_t *expr;
  double evaluate(void);
};

class discrete_expr_t
{
public:
  expr_op_t *expr;
  bool evaluate(void);
};


struct expr_list_t
{
  expr_token_t token;
  expr_data_t data;
};


struct expr_tree_t
{
  expr_token_t token;
  expr_type_t type;
  expr_data_t data;
  expr_tree_t *left;
  expr_tree_t *right;
  void print(void);
  ~expr_tree_t(void);
  expr_tree_t(void);
  int count_nodes(void);
  int make_post_fix(expr_op_t *expr);
};

const char *expr_token_string(expr_token_t token);

stack_value_t *eval_expr(expr_op_t *expr_ptr);
expr_op_t *make_expr(char *line);

int get_expr_token_list(char *line, expr_list_t list[], int max);

void set_get_point_type_fn(point_type_t (*fn)(char *tag));
expr_tree_t *get_expr_tree(char *line);

expr_tree_t *get_expr_tree(expr_list_t *list , int n_tokens);
expr_type_t check_expr(char *line);

typedef double (*analog_point_handler_t)(void *p);
typedef bool (*discrete_point_handler_t)(void *p);

void set_eval_analog_point_handler(analog_point_handler_t handler);
void set_eval_discrete_point_handler(discrete_point_handler_t handler);

typedef const double *(*get_analog_ptr_fn_t)(char *tag);
typedef const bool *(*get_discrete_ptr_fn_t)(char *tag);

void set_get_discrete_ptr_fn(get_discrete_ptr_fn_t fn);
void set_get_analog_ptr_fn(get_analog_ptr_fn_t fn);





#endif

