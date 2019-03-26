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

/*****

dcalc.cpp

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

bool dcalc_t::eval_expr(void)
{
  // Ok, right now, drop dead simple, only or together all the points.
  for (int i=0; i < n_dpoints; i++)
  {
    if (dpoints[i]->get_pv())
    {
      return true;
    }
  }
  return false;
}

/**********************************************************************/

void dcalc_t::update(void)
{
  /* Update the discrete calculation. */

  pv_last = pv;
  pv  = eval_expr();

  display_pv();
  check_alarms();
}

/*********************************************************************/

dcalc_t **dcalc_t::read(int *cnt, const char *home_dir)
{
  int max_points = 20;
  dcalc_t **dcalcs =
	(dcalc_t **) malloc(max_points * sizeof(dcalc_t*));
  MALLOC_CHECK(dcalcs);

  *cnt = 0;
  int count = 0;

  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/dcalc.dat", sizeof(path));
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
    else if (argc < 7)
    {
      logfile->vprint("%s: Wrong number of args, line %d\n", path, i+1);
      continue;
    }

    logfile->vprint("%s\n", line);
    dcalc_t *p = new dcalc_t;

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

    safe_strcpy(p->lo_desc, (const char*) argv[2], sizeof(p->lo_desc));
    safe_strcpy(p->hi_desc, (const char*) argv[3], sizeof(p->hi_desc));

    switch (argv[4][0])
    {
      case 'H':
      case 'h':
	p->alarm_state = DISCRETE_HI;
	break;
      case 'L':
      case 'l':
	p->alarm_state = DISCRETE_LO;
	break;
      case 'N':
      case 'n':
	p->alarm_state = DISCRETE_NONE;
	break;
      default:
	p->alarm_state = DISCRETE_NONE;
	break;
    }

    switch (argv[5][0])
    {
      case 'H':
      case 'h':
	p->shutdown_state = DISCRETE_HI;
	break;
      case 'L':
      case 'l':
	p->shutdown_state = DISCRETE_LO;
	break;
      case 'N':
      case 'n':
	p->shutdown_state = DISCRETE_NONE;
	break;
      default:
	p->shutdown_state = DISCRETE_NONE;
	break;
    }
    //safe_strcpy(p->calc, (const char*) argv[6], sizeof(p->calc));

    p->pv_string = p->lo_desc;
    p->pv = false;

    int n_errors = 0;
    p->n_dpoints = 0;
    logfile->vprint("Discrete points: ");
    for (int j=6; j < argc; j++)
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
        p->dpoints[j - 6 - n_errors] = (discrete_point_t *) db_point;
        p->n_dpoints++;
      }
    }
    logfile->vprint("\n");


    dcalcs[count] = p;
    count++;
    if (count > max_points)
    {
      max_points += 10;
      dcalcs = (dcalc_t **) realloc(dcalcs,
		  max_points * sizeof(dcalc_t*));
      MALLOC_CHECK(dcalcs);
    }
  }

  if (count == 0)
  {
    free(dcalcs);
    return NULL;
  }
  *cnt = count;
  return dcalcs;

}

/*************************************************************************/

