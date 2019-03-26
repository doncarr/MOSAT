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

/***
int.cpp

Member functions for discrete calculations.

*************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"

/**********************************************************************/

int int_t::eval_expr(void)
{
  // Ok, do a bitwise or of all the points.
  int tmp = 0;
  bool val;
  for (int i=0; i < n_dpoints; i++)
  {
    if (dpoints[i] == NULL) 
    {
      val = false; 
    }
    else 
    {
      val = dpoints[i]->get_pv();
    }

    if (val)
    {
      tmp |= 0x1;
    }

    if (i < (n_dpoints - 1))
    {
      tmp = tmp << 1;
    }
  }
  return tmp;
}

/**********************************************************************/

void int_t::update(void)
{
  /* Update the discrete calculation. */

  pv_last = pv;
  pv  = eval_expr();

  display_pv();
  //check_alarms();
}

/*********************************************************************/

void int_t::init_values(void)
{
}

/*********************************************************************/

int_t **int_t::read(int *cnt, const char *home_dir)
{
  int max_points = 20;
  int_t **ints =
	(int_t **) malloc(max_points * sizeof(int_t*));
  MALLOC_CHECK(ints);

  *cnt = 0;
  int count = 0;
  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/int.dat", sizeof(path));
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    logfile->perror(path);
    logfile->vprint("Can't open: %s\n", path);
    return NULL;
  }
  char line[300];

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[25];
    ltrim(line);
    rtrim(line);
    safe_strcpy(tmp, (const char*) line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '|', 25);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }
    else if (argc < 3)
    {
      logfile->vprint("%s: Wrong number of args, line %d\n", path, i+1);
      continue;
    }

    logfile->vprint("%s\n", line);
    int_t *p = new int_t;

    /*****
    Tag
    Description
    Low Description
    High Description
    Alarm State
    Shutdown State
    Calculation
    ******/

    safe_strcpy(p->tag, (const char*) argv[0], sizeof(p->tag));
    safe_strcpy(p->description, (const char*) argv[1], sizeof(p->description));

    p->pv = 0;

    int n_errors = 0;
    p->n_dpoints = 0;
    logfile->vprint("Discrete points: ");
    for (int j=2; j < argc; j++)
    {
      char temp_tag[50];
      db_point_t *db_point;
      safe_strcpy(temp_tag, (const char *) argv[j], sizeof(temp_tag));
      rtrim(temp_tag);
      db_point = db->get_db_point(temp_tag);
      if ((db_point == NULL) || (db_point->pv_type() != DISCRETE_VALUE))
      {
        logfile->vprint("Bad discrete point: %s\n", temp_tag);
        n_errors++;
      }
      else
      {
        logfile->vprint("%s ", temp_tag);
        p->dpoints[j - 2 - n_errors] = (discrete_point_t *) db_point;
        p->n_dpoints++;
      }
    }
    logfile->vprint("\n");


    ints[count] = p;
    logfile->vprint("tag: %s\n", ints[count]->tag);
    count++;
    if (count >= max_points)
    {
      max_points += 10;
      ints = (int_t **) realloc(ints,
		  max_points * sizeof(int_t*));
      MALLOC_CHECK(ints);
    }
  }

  if (count == 0)
  {
    free(ints);
    return NULL;
  }
  *cnt = count;
  return ints;

}

/*************************************************************************/

