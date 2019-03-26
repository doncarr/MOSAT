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
    //time_t now = time(NULL);
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
    //time_t now = time(NULL); 
    struct tm mytm;
    localtime_r(&now, &mytm);
    strftime(datestr, sizeof(datestr), "%Y-%m-%dT%H:%M:%S%z", &mytm); // Changed to use the ISO standard for timestamp.
 
    /***
    printf("%s\t%s\t%s\t%0.2lf\t%0.3lf\t", datestr, tag, 
            di_point->get_pv_string_ptr(), level, amps);
    fflush(stdout);
    fprintf(pump_fp, "%s\t%s\t%s\t%0.2lf\t%0.3lf\t", datestr, tag, 
            di_point->get_pv_string_ptr(), level, amps);
    ****/
    snprintf(change_start_line, sizeof(change_start_line), 
            "%s\t%s\t%s\t%0.2lf\t%0.3lf\t", datestr, tag, 
            di_point->get_pv_string_ptr(), level, amps);
    last_state_at_change = state;
    last_change_time = now;
    change_started = true;
  }
  
}

/********************************************************************/

void pump_point_t::init_values(void)
{
  db_point_t *db_point;
  rtrim(this->pump_on_tag);
  db_point = db->get_db_point(this->pump_on_tag);
  printf("tagb = %s, db_point = %p\n", this->pump_on_tag, db_point); 
  if ((db_point == NULL) || (db_point->point_type() != DISCRETE_INPUT))
  {
    this->di_point = NULL;
    logfile->vprint("%s - bad discrete input point: %s\n", this->tag, this->pump_on_tag);
  }
  else
  {
    this->di_point = (di_point_t *) db_point;
  }

  rtrim(this->amps_tag);
  db_point = db->get_db_point(this->amps_tag);
  if ((db_point == NULL) || (db_point->point_type() != ANALOG_INPUT))
  {
    this->ai_point = NULL;
    logfile->vprint("%s - bad analog input point (amps): %s\n", this->tag, this->amps_tag);
  }
  else
  {
    this->ai_point = (ai_point_t *) db_point;
  }

  rtrim(this->level_tag);
  db_point = db->get_db_point(this->level_tag);
  if ((db_point == NULL) || (db_point->point_type() != ANALOG_INPUT))
  {
    this->ai_point = NULL;
    logfile->vprint("%s - bad analog input point (level): %s\n", this->tag, this->level_tag);
  }
  else
  {
    this->level_ai_point = (ai_point_t *) db_point;
  }

  logfile->vprint("%s: Min: %lf, Max %lf, Delay %lf\n", 
         this->tag, this->min_amps, this->max_amps, this->delay);

  this->last_state_at_change = true;
  this->last_change_time = time(NULL);
  this->last_current = 0.0;
  this->change_started = false;
  this->this_hour = this->this_day = time(NULL);
  this->hour_total_amps = 0.0;
  this->hour_num_on_readings = 0;
  this->hour_num_off_reading = 0;
  this->day_total_amps = 0.0;
  this->day_num_on_readings = 0;
  this->day_num_off_reading = 0;

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
  snprintf(tbuf, sizeof(tbuf), "_%s.txt", this->tag);
  this->history_fp = rt_open_day_history_file(NULL, tbuf, plog_home, NULL);
}

/********************************************************************/

