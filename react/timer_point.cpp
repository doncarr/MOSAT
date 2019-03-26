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
timer_point.cpp

Member functions for timer points.

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

void timer_point_t::stop(void)
{
  running = false;
}

/************************************************************************/

void timer_point_t::start(void)
{
  running = true;
  last_time = db->get_time();
}

/************************************************************************/

void timer_point_t::count_down(void)
{
  mode_count_up = false;
}

/************************************************************************/

void timer_point_t::count_up(void)
{
  mode_count_up = true;
}

/************************************************************************/

void timer_point_t::update()
{
  /* update the timer */

  double now = db->get_time();
  if (not running)
  {
    return;
  }
  if (mode_count_up)
  {
    pv += now - last_time;
  }
  else 
  {
    pv += last_time - now;
  }
  last_time = now;
  display_pv();
}

/*************************************************************************/

timer_point_t *timer_point_t::read_one(int argc, char *argv[], char *err, int esz)
{
  if (argc != 5)
  {
    snprintf(err, esz, "Should be 5 args for timer point, found: %d\n", argc);
    return NULL;
  }
  timer_point_t *p = new timer_point_t;

  safe_strcpy(p->tag, argv[0], sizeof(p->tag));
  safe_strcpy(p->description, argv[1], sizeof(p->description));
  safe_strcpy(p->eu, argv[2], sizeof(p->eu));

  p->decimal_places = atoi(argv[3]);
  p->mode_count_up = ((argv[4][0] == '1') || 
                 (argv[4][0] == 'T') || 
                 (argv[4][0] == 't'));
  p->pv = 0.0;
  p->running = false;
  p->last_time = 0.0;

  return p;
}

/*************************************************************************/

timer_point_t **timer_point_t::read(int *cnt, const char *home_dir)
{

  int max_points = 20;
  timer_point_t **timer_points =
	(timer_point_t **) malloc(max_points * sizeof(timer_point_t*));
  MALLOC_CHECK(timer_points);

  *cnt = 0;
  int count = 0;

  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/timer.dat", sizeof(path));
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    logfile->perror(path);
    logfile->vprint("Can't open file: %s\n", path);
    return NULL;
  }
  char line[300];

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    int argc;
    char *argv[25];
    char tmp[300];
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
    else if (argc != 5)
    {
      logfile->vprint("timer.dat, line %d: Should be 5 args, found %d\n", i+1, argc);
      continue;
    }
    logfile->vprint("%s\n", line);
    /*****
    tag
    description
    engineering units
    decimals
    count down? 
    ****/
    if (count > max_points)
    {
      max_points += 10;
      timer_points = (timer_point_t **) realloc(timer_points,
	       max_points * sizeof(timer_point_t*));
      MALLOC_CHECK(timer_points);
    }

    char errbuf[100];
    timer_points[count] = timer_point_t::read_one(argc, argv, errbuf, sizeof(errbuf));
    if (timer_points[count] == NULL)
    {
      logfile->vprint("%s:%d %s\n", path, i+1, errbuf);
      continue;
    }
    count++;
  }

  fclose(fp);
  if (count == 0)
  {
    free(timer_points);
    return NULL;
  }
  *cnt = count;
  return timer_points;
}

/************************************************************************/

