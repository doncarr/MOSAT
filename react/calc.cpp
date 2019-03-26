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

/*************************************************************************/

calc_point_t *calc_point_t::read_one(int argc, char *argv[], char *err, int esz)
{
  if (argc != 14)
  {
    snprintf(err, esz, "Wrong number of args for ai");
    return NULL;
  }
  calc_point_t *p = new calc_point_t;

  safe_strcpy(p->tag, argv[0], sizeof(p->tag));
  safe_strcpy(p->description, argv[1], sizeof(p->description));
  safe_strcpy(p->eu, argv[2], sizeof(p->eu));

  char temp[200];
  safe_strcpy(temp, argv[3], sizeof(temp));
  ltrim(temp);
  rtrim(temp);
  p->expr_string = strdup(temp);
  MALLOC_CHECK(p->expr_string);
  logfile->vprint("Expression: %s\n", p->expr_string);

  p->decimal_places = atoi(argv[4]);
  p->lo_alarm = atof(argv[5]);
  p->lo_caution = atof(argv[6]);
  p->hi_caution = atof(argv[7]);
  p->hi_alarm = atof(argv[8]);
  p->deadband = atof(argv[9]);
  p->lo_alarm_enable = (argv[10][0] == '1');
  p->lo_caution_enable = (argv[11][0] == '1');
  p->hi_caution_enable = (argv[12][0] == '1');
  p->hi_alarm_enable = (argv[13][0] == '1');
  p->expression.expr = NULL;

    //p->window_size = 0;
    //p->window = NULL;
  p->set_format();
  return p;
}

/*************************************************************************/

calc_point_t **calc_point_t::read(int *cnt, const char *home_dir)
{

  int max_points = 20;
  calc_point_t **calc_points =
	(calc_point_t **) malloc(max_points * sizeof(calc_point_t*));
  MALLOC_CHECK(calc_points);

  *cnt = 0;
  int count = 0;

  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/calc.dat", sizeof(path));
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    return NULL;
  }
  char line[300];

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    int argc;
    char *argv[25];
    char tmp[300];
    ltrim(line);
    rtrim(line);
    safe_strcpy(tmp, line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '|', 25);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }
    else if (argc != 14)
    {
      logfile->vprint("calc.dat: Wrong number of args, line %d", i+1);
      continue;
    }
    logfile->vprint("%s\n", line);
    /*****
    tag
    description
    engineering units
    calc expression
    decimals
    low alarm
    low caution
    high caution
    high alarm
    deadband
    low alarm enable
    low caution enable
    high caution enable
    high alarm enable
    ****/
    if (count > max_points)
    {
      max_points += 10;
      calc_points = (calc_point_t **) realloc(calc_points,
	       max_points * sizeof(calc_point_t*));
      MALLOC_CHECK(calc_points);
    }

    char errbuf[100];
    calc_points[count] = calc_point_t::read_one(argc, argv, errbuf, sizeof(errbuf));
    if (calc_points[count] == NULL)
    {
      logfile->vprint("%s:%d %s\n", path, i+1, errbuf);
      continue;
    }
    count++;
  }

  fclose(fp);
  if (count == 0)
  {
    free(calc_points);
    return NULL;
  }
  *cnt = count;
  return calc_points;
}

/************************************************************************/

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
    logfile->vprint("Analog Calc %s: Bad Expression: \"%s\", %s\n",
          tag, expr_string, rtexperror.str());
  }
}

/************************************************************************/
