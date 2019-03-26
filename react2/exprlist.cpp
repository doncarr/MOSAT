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
Exprlist.c

Procedures for expression lists.

*************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "exp.h"
#include "error.h"

static expr_list_t next_token(char *line);

static point_type_t (*get_point_type_fn)(char *tag) = NULL;

/*********************************************************************/

void set_get_point_type_fn(point_type_t (*fn)(char *tag))
{
  get_point_type_fn = fn;
}

/*********************************************************************/

int get_expr_token_list(char *line, expr_list_t list[], int max)
{
  /* Get a token list using the character expression. */
  int i;

  if (get_point_type_fn == NULL)
  {
    rtexperror.set(EXP_FUNCTION_NOT_ASSIGNED,
            "No get_point_type function assigned.");
    return FAILED;
  }

  list[0] = next_token(line);
  i = 0;
  while ( (list[i].token != BAD_TOKEN) &&
	  (list[i].token != END_EXPR) &&
	  (i < (max-1)) )
  {
    i++;
    list[i] = next_token(NULL);
  }
  if (list[i].token == BAD_TOKEN)
  {
    //rtexperror.set(EXP_BAD_TOKEN, "Bad Token");
    return FAILED;
  }
  if (i == 0)
  {
   rtexperror.set(EXP_EMPTY_EXPR, "Empty expression");
    return FAILED;
  }
  return i + 1;
}

/**********************************************************************/

char *skip_spaces(char *p)
{
  /* Skip over spaces. */
  for ( ; (*p != '\0') && isspace(*p); p++);
  return p;
}

/**********************************************************************/

char *find_end_of_tag(char *p)
{
  /* Search to find the end of a tag. */
  while (isalnum(*p) || (*p == '_'))
  {
    p++;
  }
  return p;
}

/**********************************************************************/

static expr_list_t next_token(char *line)
{
  /* Get the next token in the line. */
  char *p, *p2;
  char temp[80];
  expr_list_t list;
  static char *next;
  int i;
  point_type_t point_type;

  if (line == NULL)
  {
    line = next;
  }

  list.data.tag[0] = '\0';
  list.data.number = 0;


  p = skip_spaces(line);
  switch (*p)
  {
    case '\n':
    case '\r':
    case '\0':
      list.token = END_EXPR;
      return list;
    case '+':
      next = p + 1;
      list.token = PLUS_OP;
      return list;
    case '-':
      next = p + 1;
      list.token = MINUS_OP;
      return list;
    case '*':
      next = p + 1;
      list.token = MULTIPLY_OP;
      return list;
    case '/':
      next = p + 1;
      list.token = DIVIDE_OP;
      return list;

    case '^':
      next = p + 1;
      list.token = POWER_OP;
      return list;

    case  '(':
      next = p + 1;
      list.token = LEFT_PAREN;
      return list;
    case  ')':
      next = p + 1;
      list.token = RIGHT_PAREN;
      return list;
    case  '<':
      if (*(p + 1) == '=')
      {
	next = p + 2;
	list.token = LE_OP;
	return list;
      }
      else if (*(p + 1) == '>')
      {
	next = p + 2;
	list.token = NE_OP;
	return list;
      }
      else
      {
	next = p + 1;
	list.token = LT_OP;
	return list;
      }
    case  '>':
      if (*(p + 1) == '=')
      {
	next = p + 2;
	list.token = GE_OP;
      }
      else
      {
	next = p + 1;
	list.token = GT_OP;
      }
      return list;
    case  '=':
      if (*(p + 1) == '=')
      {
	next = p + 2;
        list.token = EQ_OP;
      }
      else
      {
	rtexperror.set(EXP_UNEXPECTED_CHAR, "Unexpected character: %c", *(p+1));
	list.token = BAD_TOKEN;
      }
      return list;
    case '\'':
      if (*(p + 2) != '\'')
      {
	rtexperror.set(EXP_UNEXPECTED_CHAR, "Unexpected character: '%c'", *(p+2));
	list.token = BAD_TOKEN;
	return list;
      }
      next = p + 3;
      list.token = NUMBER;
      list.data.number = double(*(p + 1));
      return list;
    case  '!':
      if (*(p + 1) == '=')
      {
	next = p + 2;
	list.token = NE_OP;
      }
      else
      {
	rtexperror.set(EXP_UNEXPECTED_CHAR, "Unexpected character: %c", *(p+1));
	list.token = BAD_TOKEN;
      }
      return list;
    default:
      if (isalpha(*p))
      {
	for (i=0; isalnum(*p) || (*p == '_'); i++)
	{
	  temp[i] = *(p++);
	}
	next = p;
	temp[i] = '\0';
	if (0 == strcasecmp(temp, "AND"))
	{
	  list.token = AND_OP;
	}
	else if (0 == strcasecmp(temp, "OR"))
	{
	  list.token =  OR_OP;
	}
	else if (0 == strcasecmp(temp, "NOT"))
	{
          //printf("******** Found a NOT operator\n");
	  list.token = NOT_OP;
	}
	else if (0 == strcasecmp(temp, "SIN"))
	{
	  list.token = SIN_OP;
	}
	else if (0 == strcasecmp(temp, "COS"))
	{
	  list.token = COS_OP;
	}
	else if (0 == strcasecmp(temp, "TAN"))
	{
	  list.token = TAN_OP;
	}
	else if (0 == strcasecmp(temp, "ASIN"))
	{
	  list.token = ASIN_OP;
	}
	else if (0 == strcasecmp(temp, "ACOS"))
	{
	  list.token = ACOS_OP;
	}
	else if (0 == strcasecmp(temp, "ATAN"))
	{
	  list.token = ATAN_OP;
	}
	else if (0 == strcasecmp(temp, "SQRT"))
	{
	  list.token = SQRT_OP;
	}
	else if (0 == strcasecmp(temp, "SQR"))
	{
	  list.token = SQR_OP;
	}
	else if (0 == strcasecmp(temp, "LOG"))
	{
	  list.token = LOG_OP;
	}
	else if (0 == strcasecmp(temp, "LOG10"))
	{
	  list.token = LOG10_OP;
	}
	else if (0 == strcasecmp(temp, "EXP"))
	{
	  list.token = EXP_OP;
	}
	else if (0 == strcasecmp(temp, "ABS"))
	{
	  list.token = ABS_OP;
	}
	else if (0 == strcasecmp(temp, "FALSE"))
	{
          printf("LOGICAL_VAL: false\n");
          list.token = LOGICAL_VAL;
          list.data.boolean = false;
	}
	else if (0 == strcasecmp(temp, "F"))
	{
          printf("LOGICAL_VAL: false\n");
          list.token = LOGICAL_VAL;
          list.data.boolean = false;
	}
	else if (0 == strcasecmp(temp, "TRUE"))
	{
          printf("LOGICAL_VAL: true\n");
          list.token = LOGICAL_VAL;
          list.data.boolean = true;
	}
	else if (0 == strcasecmp(temp, "T"))
	{
          printf("LOGICAL_VAL: true\n");
          list.token = LOGICAL_VAL;
          list.data.boolean = true;
	}
	else
	{
	  strcpy(list.data.tag, temp);
	  point_type = (*get_point_type_fn)(temp);
	  if (point_type == UNDEFINED_POINT)
	  {
	    rtexperror.set(EXP_POINT_DOESNT_EXIST,
	       "Point does not exist: %s", temp);
	    list.token = BAD_TOKEN;
	    return list;
	  }
	  switch (get_pv_type(point_type))
	  {
	    case ANALOG_VALUE:
	      list.token = ANALOG_TAG;
	      return list;
	    case DISCRETE_VALUE:
	      list.token = DISCRETE_TAG;
	      return list;
	    //case UNDEFINED_VALUE:
            default:
	      rtexperror.set(EXP_BAD_POINT_TYPE, "Bad point type for tag: %s",
			 temp);
	      list.token = BAD_TOKEN;
	      return list;
	  }
	  if ((temp[0] == 'A') || (temp[0] == 'a'))
	  {
	    list.token = ANALOG_TAG;
	  }
	  else
	  {
	    list.token = DISCRETE_TAG;
	  }
	}
	return list;
      }
      else if (isdigit(*p))
      {
	p2 = temp;
	bool found_period = false;
	while (isdigit(*p) || (*p == '.'))
	{
	  if (*p == '.')
	  {
	    if (found_period)
	    {
	      rtexperror.set(EXP_UNEXPECTED_CHAR, "Unexpected character: '.'");
	      list.token = BAD_TOKEN;
	      return list;
	    }
	    found_period = true;
	  }
	  *(p2++) = *(p++);
	}
	*p2 = '\0';
	next = p;
	list.token = NUMBER;
	list.data.number = (double) atof(temp);
	return list;
      }
      else
      {
	rtexperror.set(EXP_UNEXPECTED_CHAR, "Unexpected character: %c", *p);
	list.token = BAD_TOKEN;
	return list;
      }
  }
}

/**********************************************************************/
