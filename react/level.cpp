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

/******
  level point

*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <string.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"
#include "ap_config.h"

static char plog_home[200];

/********************************************************************/

void level_point_t::update(void)
{
  /* Update the given level point. */
  //double current_level;
  bool lo_state;
  bool hi_state;
  time_t now;

  if ((level_fp == NULL) || (level_point == NULL) || 
      (di_lo == NULL) || (di_hi == NULL) )
  {
    return;
  }

  now = time(NULL);
  //current_level = level_point->get_pv();
  lo_state = di_lo->get_pv();
  hi_state = di_hi->get_pv();
  
  if (rt_hour_changed(now, this_hour))
  {
    struct tm mytm;
    localtime_r(&this_hour, &mytm);
    double fill_rate = total_hour_fill_rates / total_hour_samples;
    fprintf(level_fp, "%d\t%0.2lf\t%0.2lf\n", mytm.tm_hour, 
        fill_rate, fill_rate * 60.0);
    fflush(level_fp);
    total_hour_fill_rates = 0.0;
    total_hour_samples = 0;
    this_hour =  now;
  }

  if (rt_day_changed(now, this_day))
  {
    double fill_rate = total_day_fill_rates / total_day_samples;
    fprintf(level_fp, "%s\t%0.2lf\t%0.2lf\n", "dia", 
        fill_rate, fill_rate * 60.0 * 24.0);
    fflush(level_fp);
    total_day_fill_rates = 0.0;
    total_day_samples = 0;
    this_day = now;
    level_fp = rt_open_day_history_file(NULL, "_agua.txt", plog_home, level_fp);
  }

  if (pumping)
  {
    if (!lo_state && !hi_state) // We reached the bottom
    {
      //printf("Bottom reached\n"); 
      time_at_bottom = time(NULL);
      pumping = false;
    }
  }
  else
  {
    if (lo_state && hi_state) // We reached the top 
    {
      double fill_rate = 
        (60.0 * volume_lo_hi) / double (now - time_at_bottom);
      //printf("Top reached, fill rate = %0.2lf\n", fill_rate); 
      total_day_fill_rates += fill_rate;
      total_day_samples++;
      total_hour_fill_rates += fill_rate;
      total_hour_samples++;
      pumping = true;
    }
  }

}

/********************************************************************/

level_point_t **level_point_t::read(int *cnt, const char *home_dir)
{
  /* Read the level point from the database. */
  db_point_t *db_point;

  int max_points = 20;
  level_point_t **level_points =
	(level_point_t **) malloc(max_points * sizeof(level_point_t*));
  MALLOC_CHECK(level_points);

  int count = 0;

  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/level.dat", sizeof(path));
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    logfile->perror(path);
    logfile->vprint("Can't open %s\n", path);
    *cnt = 0;
    return NULL;
  }
  char line[300];

//const char *html_home = ap_config.get_config("htmlhome");

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[25];
    ltrim(line);
    rtrim(line);

    safe_strcpy(tmp, (const char*)line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '|', 25);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }
    else if (argc != 6)
    {
      logfile->vprint("%s: Wrong number of args, line %d\n", path, i+1);
      continue;
    }
    logfile->vprint("%s\n", line);
    level_point_t *lvl = new level_point_t;

    safe_strcpy(lvl->tag, (const char*) argv[0], sizeof(lvl->tag));
    safe_strcpy(lvl->description, (const char*) argv[1], sizeof(lvl->description));

    char temp_tag[30];

    safe_strcpy(temp_tag, (const char*) argv[2], sizeof(temp_tag));
    rtrim(temp_tag);
    db_point = db->get_db_point(temp_tag);
    if ((db_point == NULL) || (db_point->point_type() != DISCRETE_INPUT))
    {
      lvl->di_lo = NULL;
      logfile->vprint("%s - bad discrete input point: %s\n", lvl->tag, temp_tag);
    }
    else
    {
      lvl->di_lo = (di_point_t *) db_point;
    }

    safe_strcpy(temp_tag, (const char*) argv[3], sizeof(temp_tag));
    rtrim(temp_tag);
    db_point = db->get_db_point(temp_tag);
    if ((db_point == NULL) || (db_point->point_type() != DISCRETE_INPUT))
    {
      lvl->di_hi = NULL;
      logfile->vprint("%s - bad discrete input point: %s\n", lvl->tag, temp_tag);
    }
    else
    {
      lvl->di_hi = (di_point_t *) db_point;
    }

    safe_strcpy(temp_tag, (const char*) argv[4], sizeof(temp_tag));
    rtrim(temp_tag);
    db_point = db->get_db_point(temp_tag);
    if ((db_point == NULL) || (db_point->point_type() != ANALOG_INPUT))
    {
      lvl->level_point = NULL;
      logfile->vprint("%s - bad analog input point: %s\n", lvl->tag, temp_tag);
    }
    else
    {
      lvl->level_point = (ai_point_t *) db_point;
    }

    lvl->volume_lo_hi = atof(argv[5]);

    lvl->total_hour_fill_rates = 0.0;
    lvl->total_hour_samples = 0;
    lvl->total_day_fill_rates = 0.0;
    lvl->total_day_samples = 0;
    lvl->pumping = true;
    lvl->this_hour = lvl->this_day = time(NULL);
    lvl->time_at_bottom = time(NULL);
 
    const char *html_home = ap_config.get_config("htmlhome");
    if (html_home == NULL)
    {
      logfile->vprint("Warning: htmlhome variable not set\n");
      snprintf(plog_home, sizeof(plog_home), "./log/");
    }
    else
    {
      snprintf(plog_home, sizeof(plog_home), "%s/atemajac/", html_home);
    }

    lvl->level_fp = rt_open_day_history_file(NULL, "_agua.txt", plog_home, NULL);

    level_points[count] = lvl;
    count++;
    if (count > max_points)
    {
      max_points += 10;
      level_points = (level_point_t **) realloc(level_points,
	       max_points * sizeof(level_point_t*));
      MALLOC_CHECK(level_points);
    }
  }

  *cnt = count;
  return level_points;
}

/******************************************************************/

