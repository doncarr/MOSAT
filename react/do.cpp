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

Do.c

Member functions for discrete output points.

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

/********************************************************************/

void do_point_t::send(bool val)
{
  tdo_on = false; // if tdo is on, turn it off;
  blink_on = false; // if blink is on, turn it off;
  send_it(val);
}

/********************************************************************/

void do_point_t::send_it(bool val)
{
  /* Send the discrete output. */

  db->send_do(driver, card, channel, val);
  pv = val ? DISCRETE_HI: DISCRETE_LO;
  if (pv)
  {
    pv_string = hi_desc;
  }
  else
  {
    pv_string = lo_desc;
  }
  /***
  switch (pv)
  {
    case DISCRETE_HI:
      pv_string = hi_desc;
      //pv_attr = NORMAL_PV_ATTR;
      break;
    case DISCRETE_LO:
      pv_string = lo_desc;
      //pv_attr = NORMAL_PV_ATTR;
      break;
    case DISCRETE_FAILED:
      pv_string = "Failed";
      point_state = STATE_FAILED;
      //pv_attr = FAILED_ATTR;
      break;
    default:
      pv_string = "*******";
      break;
  }
  **/
  display_pv();
}

/********************************************************************/

void do_point_t::blink(double val)
{
  //printf("%s: Turning on blink, %0.1lf\n", tag, val);
  blink_on = true;
  tdo_on = false;
  blink_time = val;
  double tnow = db->get_time();
  operation_end_time = tnow + val;
  if (pv ==  DISCRETE_LO)
  {
    this->send_it(true);
  }
  else
  {
    this->send_it(false);
  }
}

/********************************************************************/

void do_point_t::tdo(double val)
{
  this->send_it(true);
  double tnow = db->get_time();
  operation_end_time = tnow + val;
  tdo_on = true;
  blink_on = false;
}


/********************************************************************/

void do_point_t::update(void)
{
  double tnow = db->get_time();
  //printf("///////// %s: Now: %0.1lf, Next%0.1lf, %s, %s\n", 
  //     tag, tnow, operation_end_time, tdo_on ? "T":"F", blink_on ? "T":"F");
  if (tdo_on)
  {
    if (tnow > operation_end_time)
    {
      this->send_it(false);
      tdo_on = false;
    }
  }

  if (blink_on)
  {
    //printf("//////////////////// %s: Now: %0.1lf, Next%0.1lf\n", 
    //      tag, tnow, operation_end_time);
    if (tnow > operation_end_time)
    {
      operation_end_time += blink_time;
      if (pv ==  DISCRETE_LO)
      {
        //printf("------------------ %s: BLINK ON\n", tag);
        this->send_it(true);
      }
      else
      {
        //printf("-------------------%s: BLINK OFF\n", tag);
        this->send_it(false);
      }
    }
  }
}


/********************************************************************/

do_point_t **do_point_t::read(int *cnt, const char *home_dir)
{
  /*************/

  int max_points = 50;
  do_point_t **do_points =
	(do_point_t **) malloc(max_points * sizeof(do_point_t*));
  MALLOC_CHECK(do_points);

  *cnt = 0;


  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/do.dat", sizeof(path));
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
    char tmp[300];
    int argc;
    char *argv[25];
    //DO|Discrete Output 1|0|0|1|HI|LO|
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
    else if (argc != 7)
    {
      logfile->vprint("%s: Wrong number of args, line %d", path, i+1);
      continue;
    }
    logfile->vprint("%s\n", line);
    do_point_t *p = new do_point_t;

    /*****
    Tag
    Description
    Driver Number
    Card
    Channel
    Low Description
    High Description
    ******/

    safe_strcpy(p->tag, (const char*) argv[0], sizeof(p->tag));
    safe_strcpy(p->description, (const char*) argv[1], sizeof(p->description));

    p->driver = atol(argv[2]);
    p->card = atol(argv[3]);
    p->channel = atol(argv[4]);

    safe_strcpy(p->lo_desc, (const char*) argv[5], sizeof(p->lo_desc));
    safe_strcpy(p->hi_desc, (const char*) argv[6], sizeof(p->hi_desc));

    p->pv = DISCRETE_LO;
    p->pv_string = p->lo_desc;
    //p->pv_attr = NORMAL_PV_ATTR;
    p->tdo_on = false;
    p->blink_on = false;
    p->operation_end_time = 0.0;

    do_points[*cnt] = p;
    (*cnt)++;
    if (*cnt > max_points)
    {
      max_points += 5;
      do_points = (do_point_t **) realloc(do_points,
	    max_points * sizeof(do_point_t*));
      MALLOC_CHECK(do_points);
    }
  }
  if (*cnt == 0)
  {
    free(do_points);
    return NULL;
  }
  return do_points;
}

/************************************************************************/
