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
  ac point

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

static FILE *ac_fp = NULL;
static char ac_log_home[200];

/********************************************************************/

void ac_point_t::update(void)
{
  /* Update the given ac point. */
  //double amps;
  time_t now;

  double cold_temp;
  double hot_temp;
  bool unit_running;

/**
  ai_point_t *cold_temp_point;
  ai_point_t *hot_temp_point;
  di_point_t *unit_running_point;
  do_point_t *unit_disable_point;
***/

  if ((ac_fp == NULL) || (cold_temp_point == NULL) || (hot_temp_point == NULL) || 
     (unit_running_point == NULL) || (unit_disable_point == NULL))
  {
    return;
  }

  now = time(NULL);

  cold_temp = cold_temp_point->get_pv();
  hot_temp = hot_temp_point->get_pv();
  unit_running = unit_running_point->get_pv();

  double avg_cold;
  double avg_hot;
  double fraction_on;
  int total_readings;

  if (rt_hour_changed(now, this_hour))
  {
    struct tm mytm;
    localtime_r(&this_hour, &mytm);
    

    if (hour_num_temp_readings == 0)
    {
      avg_cold = -1.0; 
      avg_hot = -1.0;
    }
    else
    {
      avg_cold = hour_total_cold / double(hour_num_temp_readings);
      avg_hot = hour_total_hot / double(hour_num_temp_readings);
    }

    total_readings = hour_num_on_readings + hour_num_off_readings;
    if (total_readings == 0)
    {
      fraction_on = -1.0;
    }
    else
    {
      fraction_on = double(hour_num_on_readings) / double(total_readings); 
    }
    fprintf(history_fp, "%d\t%0.1lf\t%0.1lf\t%0.1lf\n", mytm.tm_hour, 
            100.0 * fraction_on, avg_cold, avg_hot); 
    fflush(history_fp);

    hour_total_cold = 0.0;
    hour_total_hot = 0.0;
    hour_num_on_readings = 0;
    hour_num_off_readings = 0;
    hour_num_temp_readings = 0;
    this_hour =  now;
  }

  if (rt_day_changed(now, this_day))
  {
    if (day_num_temp_readings == 0)
    {
      avg_cold = -1.0; 
      avg_hot = -1.0;
    }
    else
    {
      avg_cold = day_total_cold / double(day_num_temp_readings);
      avg_hot = day_total_hot / double(day_num_temp_readings);
    }

    total_readings = day_num_on_readings + day_num_off_readings;
    if (total_readings == 0)
    {
      fraction_on = -1.0;
    }
    else
    {
      fraction_on = double(day_num_on_readings) / double(total_readings); 
    }
    fprintf(history_fp, "%s\t%0.1lf\t%0.1lf\t%0.1lf\n", "dia", 
            100.0 * fraction_on, avg_cold, avg_hot); 
    fflush(history_fp);

    day_total_cold = 0.0;
    day_total_hot = 0.0;
    day_num_on_readings = 0;
    day_num_off_readings = 0;
    day_num_temp_readings = 0;
    this_day = now;
    ac_fp = rt_open_day_history_file(NULL, "_ac_log.txt", ac_log_home, NULL);
    char tbuf[50];
    snprintf(tbuf, sizeof(tbuf), "_%s.txt", this->tag);
    history_fp = rt_open_day_history_file(NULL, tbuf, ac_log_home, NULL);
  }


  // Below is a simple method for caluclating % on.
  // - just count on readings vs off readings. Close enough! 
  if (unit_running)
  {
    hour_num_on_readings++;
    day_num_on_readings++;
    if (delay_elapsed)
    {
      hour_total_cold += cold_temp;
      hour_total_hot += hot_temp;
      hour_num_temp_readings++;
      day_total_cold += cold_temp;
      day_total_hot += hot_temp;
      day_num_temp_readings++;
    }
  }
  else
  { 
    hour_num_off_readings++;
    day_num_off_readings++;
  }

  if (change_started)
  {
    time_t now = time(NULL);
    if (now > (last_change_time + int(delay) + 1))
    {
      logfile->vprint("------- %s: Delay expired, %0.1lf\n", tag, delay);
      fflush(ac_fp);
      delay_elapsed = true;
      change_started = false;
    }
  }

  if (unit_running != last_state_at_change)
  {
    logfile->vprint("------- %s: AC change state, %s\n", tag, unit_running ? "RUN" : "OFF");
    last_state_at_change = unit_running;
    last_change_time = now;
    if (unit_running) change_started = true;
    delay_elapsed = false;
  }
  //printf("%s: %s, %s\n", tag, unit_running ? "RUN" : "OFF", delay_elapsed ? "Checking alarms" : "");
 
  if (delay_elapsed)
  {
    if ((hot_temp - cold_temp) < 30.0)
    {
      logfile->vprint("%s: Difference is low: %0.1lf %s, %ld %ld\n", 
          tag, hot_temp - cold_temp, diff_alarm ? "ALARM" : "OK",
                      now, small_difference_detect_time);
      if (!small_diference_dectected)
      {
        small_diference_dectected = true;
        small_difference_detect_time = now;
      }
      else 
      {
        if (now > (small_difference_detect_time + 5))
        {
          if (!diff_alarm)
          {
            logfile->vprint("Sending diff alarm SMS *********************************************************************************\n");
            send_sms_group("Unit NOT working", "NORMAL");
            diff_alarm = true;
            unit_disable_point->send(true);
          }
        }
      }
    }
    else
    {
      small_diference_dectected = false;
      small_difference_detect_time = 0.0;
    }
    
    if (cold_temp < 2.0)
    {
      logfile->vprint("%s: Freeze Danger: %0.1lf %s\n", 
          tag, cold_temp,  cold_alarm ? "ALARM" : "OK");
      if (!cold_detected)
      {
        cold_detected = true;
        cold_detect_time = now;
      }
      else 
      {
        if (now > (cold_detect_time + 5))
        {
          if (!cold_alarm)
          {
            logfile->vprint("Sending cold alarm SMS *********************************************************************************\n");
            send_sms_group("Unit TOO cold", "NORMAL");
            cold_alarm = true;
            unit_disable_point->send(true);
          }
        }
      }
    }
    else
    {
      cold_detected = false;
      cold_detect_time = 0;
    }
  }
}

/********************************************************************/

const char *ac_point_t::is_disabled(bool *disable)
{
   *disable = true;
   return "Way too cold"; 
}

/********************************************************************/

void ac_point_t::reset(void)
{
}
 
/********************************************************************/

void ac_point_t::get_status(char *status, int len)
{
  if (cold_alarm)
  {
    snprintf(status, len, "%s: COLD ALARM\n", tag);
  }
  else if (diff_alarm)
  {
    snprintf(status, len, "%s: DIFF ALARM\n", tag);
  }
  else
  {
    snprintf(status, len, "%s: NORMAL\n", tag);
  }
}

/********************************************************************/
double ac_point_t::get_hot_average(void)
{
  return 33.7;
}

/********************************************************************/

double ac_point_t::get_cold_average(void)
{
  return 3.7;
}

/********************************************************************/

ac_point_t **ac_point_t::read(int *cnt, const char *home_dir)
{
  /* Read the ac point from the database. */
  db_point_t *db_point;

  int max_points = 20;
  ac_point_t **ac_points =
	(ac_point_t **) malloc(max_points * sizeof(ac_point_t*));
  MALLOC_CHECK(ac_points);

  int count = 0;

  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/ac.dat", sizeof(path));
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    logfile->perror(path);
    logfile->vprint("Can't open %s\n", path);
    *cnt = 0;
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
    else if (argc != 7)
    {
      logfile->vprint("%s: Wrong number of args, line %d\n", path, i+1);
      continue;
    }
    logfile->vprint("%s\n", line);
    ac_point_t *ac = new ac_point_t;

    safe_strcpy(ac->tag, (const char*) argv[0], sizeof(ac->tag));
    safe_strcpy(ac->description, (const char*) argv[1], sizeof(ac->description));

    char temp_tag[30];

    safe_strcpy(temp_tag, (const char*) argv[2], sizeof(temp_tag));
    rtrim(temp_tag);
    db_point = db->get_db_point(temp_tag);
    if ((db_point == NULL) || (db_point->point_type() != ANALOG_INPUT))
    {
      ac->cold_temp_point = NULL;
      logfile->vprint("%s - bad analog input point: %s\n", ac->tag, temp_tag);
    }
    else
    {
      ac->cold_temp_point = (ai_point_t *) db_point;
    }

    safe_strcpy(temp_tag, (const char*) argv[3], sizeof(temp_tag));
    rtrim(temp_tag);
    db_point = db->get_db_point(temp_tag);
    if ((db_point == NULL) || (db_point->point_type() != ANALOG_INPUT))
    {
      ac->hot_temp_point = NULL;
      logfile->vprint("%s - bad analog input point: %s\n", ac->tag, temp_tag);
    }
    else
    {
      ac->hot_temp_point = (ai_point_t *) db_point;
    }

    safe_strcpy(temp_tag, (const char*) argv[4], sizeof(temp_tag));
    rtrim(temp_tag);
    db_point = db->get_db_point(temp_tag);
    if ((db_point == NULL) || (db_point->point_type() != DISCRETE_INPUT))
    {
      ac->unit_running_point = NULL;
      logfile->vprint("%s - bad discrete input point: %s\n", ac->tag, temp_tag);
    }
    else
    {
      ac->unit_running_point = (di_point_t *) db_point;
    }

    safe_strcpy(temp_tag, (const char*) argv[5], sizeof(temp_tag));
    rtrim(temp_tag);
    db_point = db->get_db_point(temp_tag);
    if ((db_point == NULL) || (db_point->point_type() != DISCRETE_OUTPUT))
    {
      ac->unit_disable_point = NULL;
      logfile->vprint("%s - bad discrete output point: %s\n", ac->tag, temp_tag);
    }
    else
    {
      ac->unit_disable_point = (do_point_t *) db_point;
    }
    ac->delay = atof(argv[6]);

    ac->last_state_at_change = true;
    ac->last_change_time = time(NULL);
    ac->last_current = 0.0;
    ac->change_started = false;
    ac->this_hour = ac->this_day = time(NULL);

    ac->hour_num_on_readings = 0;
    ac->hour_num_off_readings = 0;
    ac->hour_num_temp_readings = 0;

    ac->hour_total_cold = 0.0;
    ac->hour_total_hot = 0.0;

    ac->day_num_on_readings = 0;
    ac->day_num_off_readings = 0;
    ac->day_num_temp_readings = 0;

    ac->day_total_cold = 0.0;
    ac->day_total_hot = 0.0;

    ac->cold_detected = false;
    ac->cold_alarm = false;
    ac->cold_detect_time = 0;

    ac->small_diference_dectected = false;
    ac->diff_alarm = false;
    ac->small_difference_detect_time = 0;

    const char *html_home = ap_config.get_config("htmlhome");
    if (html_home == NULL)
    {
      logfile->vprint("Warning: htmlhome variable not set\n");
      snprintf(ac_log_home, sizeof(ac_log_home), "./log/");
    }
    else
    {
      snprintf(ac_log_home, sizeof(ac_log_home), "%s/", html_home);
    }

    if (ac_fp == NULL)
    {
      ac_fp = rt_open_day_history_file(NULL, "_ac_log.txt", ac_log_home, NULL);
    }

    char tbuf[50];
    snprintf(tbuf, sizeof(tbuf), "_%s.txt", ac->tag);
    ac->history_fp = rt_open_day_history_file(NULL, tbuf, ac_log_home, NULL);
    
    ac_points[count] = ac;
    count++;
    if (count > max_points)
    {
      max_points += 10;
      ac_points = (ac_point_t **) realloc(ac_points,
	       max_points * sizeof(ac_point_t*));
      MALLOC_CHECK(ac_points);
    }
  }

  *cnt = count;
  return ac_points;
}

/******************************************************************/

