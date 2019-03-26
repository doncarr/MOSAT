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

/*********
Exptree.c

Expression tree procedures.

*************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "exp.h"
#include "error.h"

/*********************************************************************/

expr_type_t check_expr(char *line)
{
  /* Check an expression. */
  expr_tree_t *tree;
  expr_type_t expr_type;

  tree = get_expr_tree(line);
  if (tree == NULL)
  {
    return BAD_EXPR;
  }
  else
  {
    /******
    tree->print();
    printf(",  type = %s\n", (tree->type == ANALOG_EXPR) ? "ANALOG" : "DISCRETE");
    *******/
    expr_type = tree->type;
    delete tree;
    return expr_type;
  }
}

/*********************************************************************/

expr_tree_t *get_expr_tree(char *line)
{
  /* Make an expression tree. */
  expr_list_t list[100];
  int n_tokens;

  n_tokens = get_expr_token_list(line, list, 100);

  if (n_tokens == FAILED)
  {
    return NULL;
  }

  return get_expr_tree(list, n_tokens);
}

/**********************************************************************/

expr_tree_t *check_end(expr_list_t *list, int n_tokens,
		       expr_tree_t *tree)
{
  /* Check the end for another expression. */
  expr_tree_t *node;

  if (n_tokens <= 0)
  {
    return tree;
  }
  switch (list[0].token)
  {
    case LE_OP:
    case LT_OP:
    case GT_OP:
    case GE_OP:
    case EQ_OP:
    case NE_OP:
      node = new expr_tree_t;
      node->token = list[0].token;
      node->type = DISCRETE_EXPR;
      node->left = tree;
      node->right = get_expr_tree(list + 1, n_tokens - 1);
      if (node->right != NULL)
      {
	if ((node->left->type != ANALOG_EXPR) ||
	    (node->right->type != ANALOG_EXPR))
	{
	  delete node;
	  rtexperror.set(EXP_ANALOG_EXPR_EXPECTED,
               "Sub-expression should be analog");
	  return NULL;
	}
	return node;
      }
      else
      {
	delete node;
	return NULL;
      }
    case AND_OP:
    case OR_OP:
      node = new expr_tree_t;
      node->token = list[0].token;
      node->type = DISCRETE_EXPR;
      node->left = tree;
      node->right = get_expr_tree(list + 1, n_tokens - 1);
      if (node->right != NULL)
      {
	if ((node->left->type != DISCRETE_EXPR) ||
	    (node->right->type != DISCRETE_EXPR))
	{
	  delete node;
	  rtexperror.set(EXP_DISCRETE_EXPR_EXPECTED,
                "Sub-expression should be discrete");
	  return NULL;
	}
	return node;
      }
      else
      {
	delete tree;
	return NULL;
      }


    case PLUS_OP:
    case MINUS_OP:
    case MULTIPLY_OP:
    case DIVIDE_OP:
    case POWER_OP:
      node = new expr_tree_t;
      node->token = list[0].token;
      node->type = ANALOG_EXPR;
      node->left = tree;
      node->right = get_expr_tree(list + 1, n_tokens - 1);
      if (node->right != NULL)
      {
	if ((node->left->type != ANALOG_EXPR) ||
	    (node->right->type != ANALOG_EXPR))
	{
	  delete node;
	  rtexperror.set(EXP_ANALOG_EXPR_EXPECTED,
                "Sub-expression should be analog");
	  return NULL;
	}
	return node;
      }
      else
      {
	delete node;
	return NULL;
      }
    case END_EXPR:
      return tree;
    default:
      rtexperror.set(EXP_UNEXPECTED_SYMBOL,
	   "Unexpected symbol at end of expression: '%s'",
	     expr_token_string(list[0].token));
      delete tree;
      return NULL;
  }
}

/**********************************************************************/

int close_paren_position(expr_list_t *list, int n_tokens)
{
  /* Find the position of the closing paren. */
  int i, position, count;
  bool found;

  found = false;
  count = 0;
  for (i=0; (i < n_tokens) && !found; i++)
  {
    switch (list[i].token)
    {
      case LEFT_PAREN:
	count++;
	break;
      case RIGHT_PAREN:
	count--;
	if (count == 0)
	{
	  position = i;
	  found = true;
	}
	break;
      default:
	break;
    }
  }
  if (!found)
  {
    rtexperror.set(EXP_MATCHING_PAREN, "No matching right paren found");
    return -1;
  }
  else
  {
    return position;
  }
}

/**********************************************************************/
expr_tree_t *get_expr_tree(expr_list_t *list, int n_tokens)
{
   /* Get the expression tree. */
   int position;
   expr_tree_t *tree;

   switch (list[0].token)
   {
     case LEFT_PAREN:
       position = close_paren_position(list, n_tokens);
       if (position == -1)
       {
	 return NULL;
       }
       tree = get_expr_tree(list + 1, position - 1);
       if (tree == NULL)
       {
	 return NULL;
       }
       tree = check_end(list + position + 1,
			n_tokens - position - 1, tree);
       return tree;
     case NOT_OP:
       if (list[1].token == DISCRETE_TAG)
       {
	 tree = new expr_tree_t;
	 tree->token = NOT_OP;
	 tree->type = DISCRETE_EXPR;
	 tree->right = NULL;
	 tree->left = new expr_tree_t;
	 tree->left->token = list[1].token;
	 tree->left->type = DISCRETE_EXPR;
	 tree->left->left = NULL;
	 tree->left->right = NULL;
	 //strcpy(tree->left->data.tag, list[1].data.tag);
	 snprintf(tree->left->data.tag, sizeof(tree->left->data.tag), "%s", 
                list[1].data.tag);
	 tree = check_end(list + 2, n_tokens - 2, tree);
	 return tree;
       }
       else if (list[1].token == LEFT_PAREN)
       {
	 position = close_paren_position(list, n_tokens);
	 if (position == -1)
	 {
	   return NULL;
	 }
	 tree = new expr_tree_t;
	 tree->token = NOT_OP;
	 tree->type = DISCRETE_EXPR;
	 tree->right = NULL;
	 tree->left = get_expr_tree(list + 2, position - 2);
	 if (tree == NULL)
	 {
	   delete tree;
	   return NULL;
	 }
	 if (tree->left->type != DISCRETE_EXPR)
	 {
	   delete tree;
	   rtexperror.set(EXP_DISCRETE_EXPR_EXPECTED,
                 "Sub-expression should be discrete");
	   return NULL;
	 }
	 tree = check_end(list + position + 1, n_tokens - position - 1,
			  tree);
	 return tree;
       }
       else
       {
	 rtexperror.set(EXP_UNEXPECTED_SYMBOL,
	     "Unexpected symbol after NOT operator: '%s'",
	       expr_token_string(list[1].token));
	 return NULL;
       }

     case MINUS_OP:
       if (list[1].token == NUMBER)
       {
	 tree = new expr_tree_t;
	 tree->token = list[1].token;
	 tree->type = ANALOG_EXPR;
	 tree->left = NULL;
	 tree->right = NULL;
	 tree->data.number = -list[1].data.number;
	 tree = check_end(list + 2, n_tokens - 2, tree);
	 return tree;
       }
       else if (list[1].token == ANALOG_TAG)
       {
	 tree = new expr_tree_t;
	 tree->token = UNARY_MINUS_OP;
	 tree->type = ANALOG_EXPR;
	 tree->right = NULL;
	 tree->left = new expr_tree_t;
	 tree->left->token = list[1].token;
	 tree->left->type = ANALOG_EXPR;
	 tree->left->left = NULL;
	 tree->left->right = NULL;
	 //strcpy(tree->left->data.tag, list[1].data.tag);
	 snprintf(tree->left->data.tag, sizeof(tree->left->data.tag), "%s", 
                list[1].data.tag);
	 tree = check_end(list + 2, n_tokens - 2, tree);
	 return tree;
       }
       else if (list[1].token == LEFT_PAREN)
       {
	 position = close_paren_position(list, n_tokens);
	 if (position == -1)
	 {
	   return NULL;
	 }
	 tree = new expr_tree_t;
	 tree->token = UNARY_MINUS_OP;
	 tree->type = ANALOG_EXPR;
	 tree->right = NULL;
	 tree->left = get_expr_tree(list + 2, position - 2);
	 if (tree == NULL)
	 {
	   delete tree;
	   return NULL;
	 }
	 if (tree->left->type != ANALOG_EXPR)
	 {
	   delete tree;
	   rtexperror.set(EXP_ANALOG_EXPR_EXPECTED,
               "Sub-expression should be analog");
	   return NULL;
	 }
	 tree = check_end(list + position + 1, n_tokens - position - 1,
			  tree);
	 return tree;
       }
       else
       {
	 rtexperror.set(EXP_UNEXPECTED_SYMBOL,
	    "Unexpected symbol after unary minus: '%s'",
	      expr_token_string(list[1].token));
	 return NULL;
       }
     case SIN_OP:
     case COS_OP:
     case TAN_OP:
     case LOG_OP:
     case LOG10_OP:
     case EXP_OP:
     case ASIN_OP:
     case ACOS_OP:
     case ATAN_OP:
     case ABS_OP:
     case SQRT_OP:
     case SQR_OP:
       if (list[1].token == LEFT_PAREN)
       {
	 position = close_paren_position(list, n_tokens);
	 if (position == -1)
	 {
	   return NULL;
	 }
	 tree = new expr_tree_t;
	 tree->token = list[0].token;
	 tree->type = ANALOG_EXPR;
	 tree->right = NULL;
	 tree->left = get_expr_tree(list + 2, position - 2);
	 if (tree == NULL)
	 {
	   delete tree;
	   return NULL;
	 }
	 if (tree->left->type != ANALOG_EXPR)
	 {
	   delete tree;
	   rtexperror.set(EXP_ANALOG_EXPR_EXPECTED,
               "Sub-expression should be analog");
	   return NULL;
	 }
	 tree = check_end(list + position + 1, n_tokens - position - 1,
			  tree);
	 return tree;
       }
       else
       {
	 rtexperror.set(EXP_UNEXPECTED_SYMBOL,
	    "Expected '(' after %s", expr_token_string(list[0].token));
	 return NULL;
       }

     case ANALOG_TAG:
       tree = new expr_tree_t;
       tree->token = list[0].token;
       tree->type = ANALOG_EXPR;
       tree->left = NULL;
       tree->right = NULL;
       //strcpy(tree->data.tag, list[0].data.tag);
       snprintf(tree->data.tag, sizeof(tree->data.tag), "%s", 
                list[0].data.tag);
       tree = check_end(list + 1, n_tokens - 1, tree);
       return tree;
     case DISCRETE_TAG:
       tree = new expr_tree_t;
       tree->token = list[0].token;
       tree->type = DISCRETE_EXPR;
       tree->left = NULL;
       tree->right = NULL;
       //strcpy(tree->data.tag, list[0].data.tag);
       snprintf(tree->data.tag, sizeof(tree->data.tag), "%s", 
                list[0].data.tag);
       tree = check_end(list + 1, n_tokens - 1, tree);
       return tree;
     case NUMBER:
       tree = new expr_tree_t;
       tree->token = list[0].token;
       tree->type = ANALOG_EXPR;
       tree->left = NULL;
       tree->right = NULL;
       tree->data.number = list[0].data.number;
       tree = check_end(list + 1, n_tokens - 1, tree);
       return tree;
     case LOGICAL_VAL:
       tree = new expr_tree_t;
       tree->token = list[0].token;
       tree->type = DISCRETE_EXPR;
       tree->left = NULL;
       tree->right = NULL;
       tree->data.boolean = list[0].data.boolean;
       tree = check_end(list + 1, n_tokens - 1, tree);
       return tree;
     default:
       rtexperror.set(EXP_UNEXPECTED_SYMBOL, "Unexpected symbol: '%s'",
	   expr_token_string(list[0].token));
       return NULL;
   }
 }

/**********************************************************************/

int expr_tree_t::count_nodes(void)
{
  /* Count the nodes in a tree. */
  int count = 1;
  if (left != NULL)
  {
    count += left->count_nodes();
  }
  if (right != NULL)
  {
    count += right->count_nodes();
  }
  return count;
}

/***********************************************************************/

void expr_tree_t::print(void)
{
  /* Print an expression tree. */
  if (left != NULL)
  {
    left->print();
  }
  if (right != NULL)
  {
    right->print();
  }
  switch (token)
  {
    case DISCRETE_TAG:
    case ANALOG_TAG:
    case POINT_TAG: //printf(" %s", data.tag); break;
    case NUMBER: //printf(" %0.1f", data.number); break;
    default:
       ;
      //printf(" %s", expr_token_string(token)); break;
  }
}

/*********************************************************************/

const char *expr_token_string(expr_token_t token)
{
  /* Return a char string representation of a token. */
  switch (token)
  {
    case PLUS_OP: return "+";
    case MINUS_OP: return "-";
    case UNARY_MINUS_OP: return "@";
    case MULTIPLY_OP: return "*";
    case DIVIDE_OP: return "/";
    case POWER_OP: return "^";
    case LE_OP: return "<=";
    case LT_OP: return "<";
    case GT_OP: return ">";
    case GE_OP: return ">=";
    case EQ_OP: return "=";
    case NE_OP: return "<>";
    case NOT_OP: return "NOT";
    case AND_OP: return "AND";
    case OR_OP: return "OR";
    case FLOAT_PTR: return "FLOAT_PTR";
    case FLOAT_VAL: return "FLOAT_VAL";
    case LOGICAL_PTR: return "LOGICAL_PTR";
    case VALUE_PTR: return "VALUE_PTR";
    case DISCRETE_TAG: return "DISCRETE_TAG";
    case ANALOG_TAG: return "ANALOG_TAG";
    case POINT_TAG: return "POINT_TAG";
    case LEFT_PAREN: return "(";
    case RIGHT_PAREN: return ")";
    case NUMBER: return "NUMBER";
    case LOGICAL_VAL: return "LOGICAL_VAL";
    case END_EXPR: return "END_EXPR";
    case BAD_TOKEN: return "BAD_TOKEN";
    case SIN_OP: return "SIN";
    case COS_OP: return "COS";
    case TAN_OP: return "TAN";
    case ASIN_OP: return "ASIN";
    case ACOS_OP: return "ACOS";
    case ATAN_OP: return "ATAN";
    case SQR_OP: return "SQR";
    case SQRT_OP: return "SQRT";
    case LOG_OP: return "LOG";
    case LOG10_OP: return "LOG10";
    case EXP_OP: return "EXP";
    case ABS_OP: return "ABS";
    default: return "UNKNOWN_TOKEN";
  }
}

/************************************************************************/

expr_tree_t::expr_tree_t(void)
{
  /* Expression tree constructor. */
  left = NULL;
  right = NULL;
}

/************************************************************************/

expr_tree_t::~expr_tree_t(void)
{
  /* Expression tree destructor. */
  if (left != NULL)
  {
    delete left;
  }
  if (right != NULL)
  {
    delete right;
  }
}

/***********************************************************************/
