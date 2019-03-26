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
#include <string.h>
#include <math.h>
#include <stdlib.h>


#include "common.h"
#include "reactpoint.h"
#include "exp.h"
#include "error.h"

/*********************************************************************/

point_type_t my_get_point_type(char *tag)
{
   if (tag[0] == 'd')
   {
     return DISCRETE_INPUT;
   }
   else if (tag[0] == 'u')
   {
     return UNDEFINED_POINT;
   }
   return ANALOG_INPUT;
}

int nf = 0;
double myfloats[100];
bool mydiscrete = false;

/*********************************************************************/

bool *my_get_discrete_ptr_fn(char * tag)
{
  return &mydiscrete;
}

/*********************************************************************/

double *my_get_analog_ptr_fn(char *tag)
{
  nf++;
  if (nf >= 100) nf = 0;
  myfloats[nf] = atof(tag+1);
  return &myfloats[nf];
}

/*********************************************************************/

main()
{
  char line[80];
  expr_type_t etype;

  set_get_point_type_fn(my_get_point_type);
  set_get_discrete_ptr_fn(my_get_discrete_ptr_fn);
  set_get_analog_ptr_fn(my_get_analog_ptr_fn);

   while (1)
   {
     printf("Enter expression, exit to quit: \n");
     fgets(line, sizeof(line), stdin);
     if (0 == strncasecmp(line, "exit", 4))
     {
       exit(0);
     }
     switch (check_expr(line))
     {
       case ANALOG_EXPR:
	 printf("Good ANALOG expression\n");
         analog_expr_t expression;
         expression.expr = make_expr(line);
         if (expression.expr == NULL)
         {
           expression.expr = new expr_op_t[2];
           expression.expr[0].token_type = FLOAT_VAL;
           expression.expr[0].val.float_val = 0.0;
           expression.expr[1].token_type = END_EXPR;
           printf("Bad Expression\n");
         }
         double fval;
         fval = expression.evaluate();
         printf("val = %lf\n", fval);

         break;
       case DISCRETE_EXPR:
	 printf("Good DISCRETE expression\n");
         discrete_expr_t dexpression;
         dexpression.expr = make_expr(line);
         if (dexpression.expr == NULL)
         {
           dexpression.expr = new expr_op_t[2];
           dexpression.expr[0].token_type = LOGICAL_VAL;
           dexpression.expr[0].val.logical_val = false;
           dexpression.expr[1].token_type = END_EXPR;
           printf("Bad Expression\n");
         }
         int dval;
         dval = dexpression.evaluate();
         printf("val = %s\n", dval ? "True" : "False");
	 break;
       case BAD_EXPR:
	 printf("%s\n", error.str());
	 printf("BAAAAD\n");
         continue;
	 break;
     }
   }
}

/*********************************************************************/


