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

data.cpp

Member functions for data collection points.

*************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "rtcommon.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"
#include "ap_config.h"

#define FL_MIN_ARGS (11)

/**********************************************************************/

void file_logger_t::write_to_file(void)
{
  //char fname[200];
  //snprintf(fname, sizeof(fname), "%s/dbout/%s", db->get_home_dir(), file_name);
  //FILE *fp = fopen(fname, "w");
  //if (fp == NULL)
  //{
  //  printf("Can not open output file: %s\n", fname);
  //  return;
  //}
  //printf("Writing: %s . . .\n", fname);
  //fclose(fp);
}

/**********************************************************************/

void file_logger_t::delete_old_files(time_t now)
{
  char fname[500];
  rt_make_day_file_name(now, fname, sizeof(fname), base_name, "_del_log.txt", ap_config.get_config("LogHome"));
  FILE *fp = fopen(fname, "w");
  if (fp != NULL) fprintf(fp, "Delete Log\n");
  if (fp != NULL) fprintf(fp, "keeping %d days of history\n", n_days_of_history);
  if (n_days_of_history <= 0)
  {
    if (fp != NULL) fprintf(fp, "Nothing to do, exiting . . . \n");
    if (fp != NULL) fclose(fp);
    return;
  }
  for (int i = n_days_of_history; i < (n_days_of_history + 10); i++)
  {
    int ret;
    time_t day_time = now - ((i+1) * (60*60*24));
    rt_make_day_file_name(day_time, fname, sizeof(fname), base_name, ".txt", ap_config.get_config("LogHome"));
    if (fp != NULL) fprintf(fp, "Checking file: %s\n", fname);
    if (file_exists(fname))
    {
      ret = unlink(fname);
      if (fp != NULL) fprintf(fp, "Deleted file: %s, ret = %d\n\n", fname, ret);
    }
    else
    {
      if (fp != NULL) fprintf(fp, " DOES NOT EXIST\n\n");
    }
    rt_make_day_file_name(day_time, fname, sizeof(fname), base_name, "_hour.txt", ap_config.get_config("LogHome"));
    if (fp != NULL) fprintf(fp, "Checking file: %s\n", fname);
    if (file_exists(fname))
    {
      ret = unlink(fname);
      if (fp != NULL) fprintf(fp, "Deleted file: %s, ret = %d\n\n", fname, ret);
    }
    else
    {
      if (fp != NULL) fprintf(fp, " DOES NOT EXIST\n\n");
    }
  }
  if (fp != NULL) fclose(fp);
}

/**********************************************************************/ 

void file_logger_t::update(void)
{
  //fprintf(instantaneous_fp, "data:");
  if (!collecting)
  {
    logfile->vprint("  **** NOT collecting\n");
    return;
  }
  time_t now = time(NULL);
  if ((now - last_log_time) < sample_interval)
  {
    return;
  }

  char buf[40];
  struct tm mytm;
  localtime_r(&now, &mytm);
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S%z", &mytm); // Changed to use the ISO standard for timestamp.
  bool day_change = rt_day_changed(now, last_log_time);

  if (rt_hour_changed(now, last_log_time))
  {
    if ((hour_fp != NULL) && (hour_averages != NULL))
    {
      fprintf(hour_fp, "%s", buf);
      for (int i=0; i < num_points; i++)
      {
        fprintf(hour_fp, "\t%lf", hour_averages[i] / n_hour_samples);
        hour_averages[i] = 0;
      }
      fprintf(hour_fp, "\n");
      fflush(hour_fp);
    }
  }
  if (day_change)
  {
    instantaneous_fp = rt_open_day_history_file(base_name, 
           ".txt", ap_config.get_config("LogHome"), instantaneous_fp);
    if (this->hour_enable)
    {
      hour_fp = rt_open_day_history_file(base_name, 
          "_hour.txt", ap_config.get_config("LogHome"), hour_fp);

    }
    delete_old_files(now);
  }
  if (rt_week_changed(now, last_log_time))
  {
  }
  if (rt_month_changed(now, last_log_time))
  {
  }
  if (rt_year_changed(now, last_log_time))
  {
  }

  fprintf(instantaneous_fp, "%s", buf);

  //printf("Hr avgs so far: %s", buf);
  for (int i=0; i < num_points; i++)
  {
    double val = 0.0;
    if (analog_points[i] != NULL)
    {
      val = analog_points[i]->get_pv();
    }
    fprintf(instantaneous_fp, "\t%lf", val);
    if (hour_averages != NULL) hour_averages[i] += val;
    //printf(" %lf", hour_averages[i] / (n_hour_samples + 1));
  }
  n_hour_samples++;
  //printf("\n");

  fprintf(instantaneous_fp, "\n");
  fflush(instantaneous_fp);
  last_log_time = now;
}

/**********************************************************************/

void file_logger_t::start(void)
{
  collecting = true;
}

/**********************************************************************/

void file_logger_t::stop(void)
{
  collecting = false;
}

/*********************************************************************/

void file_logger_t::init_values(void)
{
  char tmp[500];
  int argc;
  char *argv[50];

  rtrim(tags);
  ltrim(tags);
  safe_strcpy(tmp, (const char*) tags, sizeof(tmp));
  argc = get_delim_args(tmp, argv, ',', 25);
  this->num_points = argc;

  collecting = true;
  instantaneous_fp = rt_open_day_history_file(base_name, ".txt", ap_config.get_config("LogHome"), NULL);
  if (instantaneous_fp == NULL)
  {
    perror(base_name);
    collecting = false;
  } 
  if (collecting)
  {
    logfile->vprint("  collection is on for %s\n", tag);
  }

  analog_points = new analog_point_t *[num_points + 1];

  for (int j=0; j < argc; j++)
  {
    char temp_tag[50];
    db_point_t *db_point;
    safe_strcpy(temp_tag, (const char*) argv[j], sizeof(temp_tag));
    ltrim(temp_tag);
    rtrim(temp_tag);
    strip_quotes(temp_tag);
    logfile->vprint("  Tag = %s\n", temp_tag);
    db_point = db->get_db_point(temp_tag);
    if ((db_point == NULL) || (db_point->pv_type() != ANALOG_VALUE))
    {
      analog_points[j] = NULL;
      logfile->vprint("  Bad analog point: %s\n", temp_tag);
    }
    else
    {
      analog_points[j] = (analog_point_t *) db_point;
    }
    logfile->vprint("  analog point[%d]: %s\n", j, temp_tag);
  }
  last_log_time = time(NULL);
  n_hour_samples = 0;
  if (hour_enable)
  {
    hour_averages = new double[num_points];
    hour_fp = rt_open_day_history_file(base_name, "_hour.txt", ap_config.get_config("LogHome"), NULL);
    if (hour_fp == NULL)
    {
      perror("Opening Hour File");
    } 
    for (int i=0; i < num_points; i++)
    {
      hour_averages[i] = 0.0;
    }
  }
  else
  {
    hour_fp = NULL;
    hour_averages = NULL;
  }
}

/*********************************************************************/
