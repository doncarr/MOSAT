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
  pump point

  pump.cpp

  This was originally written for SITEUR pumping station near the Atemajac station

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

static FILE *pump_fp = NULL;
//static const char *plog_home = "/home/react/public_html/log/atemajac/";
//static const char *plog_home = "/home/carr/hplap/control/react/log/";
static char plog_home[200];

/********************************************************************/

void pump_point_t::update(void)
{
  /* Update the given pump point. */
  double amps;
  bool state;
  time_t now;
  double level;

  if ((pump_fp == NULL) || (ai_point == NULL) || (di_point == NULL))
  {
    return;
  }

  now = time(NULL);
  amps = ai_point->get_pv();
  state = di_point->get_pv();

  //printf("state: %s, last: %s, amps: %0.1f\n", 
  // state ? "1":"0", last_state_at_change ? "1":"0", amps);

  if (level_ai_point == NULL)
  {
    level = 0.0;
  }
  else
  {
    level = level_ai_point->get_pv();
  }


  if (rt_hour_changed(now, this_hour))
  {
    struct tm mytm;
    localtime_r(&this_hour, &mytm);
    double avg_amps = hour_total_amps / double(hour_num_on_readings);
    double fraction_on = (double(hour_num_on_readings) / 
         (double(hour_num_on_readings) + double(hour_num_off_reading)));
    fprintf(history_fp, "%d\t%0.2lf\t%0.2lf\t%0.2lf\n", mytm.tm_hour, 
            100.0 * fraction_on, avg_amps, 
         // calculate kwh for 3 phase
       (220.0 * sqrt(3.0) * avg_amps * fraction_on) / 1000.0);

    fflush(history_fp);
    hour_total_amps = 0.0;
    hour_num_on_readings = 0;
    hour_num_off_reading = 0;
    this_hour =  now;
  }

  if (rt_day_changed(now, this_day))
  {
    double avg_amps = day_total_amps / double(day_num_on_readings);
    double fraction_on = (double(day_num_on_readings) / 
         (double(day_num_on_readings) + double(day_num_off_reading)));
    fprintf(history_fp, "%s\t%0.2lf\t%0.2lf\t%0.2lf\n", "dia", 
      100.0 * fraction_on, avg_amps,
         // calculate kwh for 3 phase
         (24 * 220.0 * sqrt(3.0) * avg_amps * fraction_on) / 1000.0);

    fflush(history_fp);
    day_total_amps = 0.0;
    day_num_on_readings = 0;
    day_num_off_reading = 0;
    this_day = now;
    pump_fp = rt_open_day_history_file(NULL, "_log.txt", plog_home, NULL);
    char tbuf[50];
    snprintf(tbuf, sizeof(tbuf), "_%s.txt", this->tag);
    history_fp = rt_open_day_history_file(NULL, tbuf, plog_home, NULL);
  }


  // Below is a simple method for caluclating % on.
  // - just count on readings vs off readings. Close enough! 
  if (state)
  { 
    hour_num_off_reading++;
    day_num_off_reading++;
  }
  else
  {
    hour_total_amps += amps;
    day_total_amps += amps;
    hour_num_on_readings++;
    day_num_on_readings++;
  }

  //printf("PUMP UPDATE: %s\n", tag);
  if (change_started)
  {
    time_t now = time(NULL);
    if (now > (last_change_time + int(delay) + 1))
    {
      //printf("%s", change_start_line);
      //printf("%0.3lf\n", amps);
      fprintf(pump_fp, "%s", change_start_line);
      fprintf(pump_fp, "%0.3lf\n", amps);
      fflush(pump_fp);
      change_started = false;
    }
  }

  if (state != last_state_at_change)
  {
    char datestr[40];
    time_t now = time(NULL); 
    struct tm mytm;
    localtime_r(&now, &mytm);
    strftime(datestr, sizeof(datestr), "%Y-%m-%dT%H:%M:%S%z", &mytm); // Changed to use the ISO standard for timestamp.
 
    /***
    printf("%s\t%s\t%s\t%0.2lf\t%0.3lf\t", datestr, tag, 
            di_point->pv_string, level, amps);
    fflush(stdout);
    fprintf(pump_fp, "%s\t%s\t%s\t%0.2lf\t%0.3lf\t", datestr, tag, 
            di_point->pv_string, level, amps);
    ****/
    snprintf(change_start_line, sizeof(change_start_line), 
            "%s\t%s\t%s\t%0.2lf\t%0.3lf\t", datestr, tag, 
            di_point->pv_string, level, amps);
    last_state_at_change = state;
    last_change_time = now;
    change_started = true;
  }
  
}

/***************************/

pump_point_t *pump_point_t::create_one(int argc, char *argv[], char *err, int esz)
{
  pump_point_t *objp;
  objp = new pump_point_t;
  if (objp == NULL)
  {
    perror("new pump_point_t");
    return NULL;
  }
  safe_strcpy(objp->tag, (const char*) argv[0], sizeof(objp->tag));
  safe_strcpy(objp->description, (const char*) argv[1], sizeof(objp->description));
  safe_strcpy(objp->pump_on_tag, (const char*) argv[2], sizeof(objp->pump_on_tag));
  safe_strcpy(objp->amps_tag, (const char*) argv[3], sizeof(objp->amps_tag));
  safe_strcpy(objp->level_tag, (const char*) argv[4], sizeof(objp->level_tag));
  objp->min_amps = atof(argv[5]);
  objp->max_amps = atof(argv[6]);
  objp->delay = atof(argv[7]);
  return objp;
}

/********************************************************************/

pump_point_t **pump_point_t::read(int *cnt, const char *home_dir)
{
  /* Read the pump point from the database. */
  db_point_t *db_point;

  int max_points = 20;
  pump_point_t **pump_points =
	(pump_point_t **) malloc(max_points * sizeof(pump_point_t*));
  MALLOC_CHECK(pump_points);

  int count = 0;


  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/pump.dat", sizeof(path));
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
    else if (argc != 8)
    {
      logfile->vprint("%s: Wrong number of args, line %d\n", path, i+1);
      continue;
    }
    logfile->vprint("%s\n", line);

    char err[20];
    pump_point_t *pmp = pump_point_t::create_one(argc, argv, err, sizeof(err));

    rtrim(pmp->pump_on_tag);

    db_point = db->get_db_point(pmp->pump_on_tag);
    if ((db_point == NULL) || (db_point->point_type() != DISCRETE_INPUT))
    {
      pmp->di_point = NULL;
      logfile->vprint("%s - bad discrete input point: %s\n", pmp->tag, pmp->pump_on_tag);
    }
    else
    {
      pmp->di_point = (di_point_t *) db_point;
    }

    rtrim(pmp->amps_tag);
    db_point = db->get_db_point(pmp->amps_tag);
    if ((db_point == NULL) || (db_point->point_type() != ANALOG_INPUT))
    {
      pmp->ai_point = NULL;
      logfile->vprint("%s - bad analog input point (amps): %s\n", pmp->tag, pmp->amps_tag);
    }
    else
    {
      pmp->ai_point = (ai_point_t *) db_point;
    }

    rtrim(pmp->level_tag);
    db_point = db->get_db_point(pmp->level_tag);
    if ((db_point == NULL) || (db_point->point_type() != ANALOG_INPUT))
    {
      pmp->ai_point = NULL;
      logfile->vprint("%s - bad analog input point (level): %s\n", pmp->tag, pmp->level_tag);
    }
    else
    {
      pmp->level_ai_point = (ai_point_t *) db_point;
    }

    logfile->vprint("%s: Min: %lf, Max %lf, Delay %lf\n", 
           pmp->tag, pmp->min_amps, pmp->max_amps, pmp->delay);

    pmp->last_state_at_change = true;
    pmp->last_change_time = time(NULL);
    pmp->last_current = 0.0;
    pmp->change_started = false;
    pmp->this_hour = pmp->this_day = time(NULL);
    pmp->hour_total_amps = 0.0;
    pmp->hour_num_on_readings = 0;
    pmp->hour_num_off_reading = 0;
    pmp->day_total_amps = 0.0;
    pmp->day_num_on_readings = 0;
    pmp->day_num_off_reading = 0;

    const char *html_home = ap_config.get_config("htmlhome");
    if (html_home == NULL)
    {
      logfile->vprint("Warning: htmlhome variable not set\n");
      snprintf(plog_home, sizeof(plog_home), "./log/");
    }
    else
    {
      snprintf(plog_home, sizeof(plog_home), "%s/", html_home);
    }

    if (pump_fp == NULL)
    {
      pump_fp = rt_open_day_history_file(NULL, "_log.txt", plog_home, NULL);
    }

    char tbuf[50];
    snprintf(tbuf, sizeof(tbuf), "_%s.txt", pmp->tag);
    pmp->history_fp = rt_open_day_history_file(NULL, tbuf, plog_home, NULL);
    
    pump_points[count] = pmp;
    count++;
    if (count > max_points)
    {
      max_points += 10;
      pump_points = (pump_point_t **) realloc(pump_points,
	       max_points * sizeof(pump_point_t*));
      MALLOC_CHECK(pump_points);
    }
  }

  *cnt = count;
  return pump_points;
}

/******************************************************************/

