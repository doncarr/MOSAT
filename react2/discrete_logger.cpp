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

discrete_logger.cpp

Member functions for discrete logger point. This logger will log all
changes to discrete points given in the list.


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
#include "rt_json.h"
#include "dbref.h"

/**********************************************************************/

void discrete_logger_t::update(void)
{
  //fprintf(instantaneous_fp, "data:");
  if (!collecting)
  {
    logfile->vprint("  **** NOT collecting\n");
    return;
  }
  time_t now = time(NULL);

  char buf[40];
  struct tm mytm;
  localtime_r(&now, &mytm);
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S%z", &mytm); // Changed to use the ISO standard for timestamp.

  bool day_change = rt_day_changed(now, last_log_time);

  if (log_hour_totals && (hour_fp != NULL))
  {
    bool hour_change = rt_hour_changed(now, last_log_time);
   
    if (hour_change)
    {
      localtime_r(&last_log_time, &mytm);
      fprintf(hour_fp, "%d", mytm.tm_hour);
      for (int i=0; i < num_points; i++)
      {
        if (discrete_points[i] == NULL) continue;
        fprintf(hour_fp, "\t%d", n_hour_detects[i]);
        n_hour_detects[i] = 0;
      }
      fprintf(hour_fp, "\n");
      fflush(hour_fp);
    }
  }

  if (day_change)
  {
    instantaneous_fp = rt_open_day_history_file(base_name, 
            ".txt", ap_config.get_config("LogHome"), instantaneous_fp);
    
    if (log_hour_totals)
    {
      fprintf(hour_fp, "--------------\n");
      fprintf(hour_fp, "total");
      for (int i=0; i < num_points; i++)
      {
        if (discrete_points[i] == NULL) continue;
        fprintf(hour_fp, "\t%d", n_day_detects[i]);
        n_day_detects[i] = 0;
      }
      fprintf(hour_fp, "\n");

      hour_fp = rt_open_day_history_file(base_name, "_hour.txt", ap_config.get_config("LogHome"), hour_fp);

      for (int i=0; i < num_points; i++)
      {
        if (discrete_points[i] == NULL) continue;
        fprintf(hour_fp, "\t%s", discrete_points[i]->get_tag());
      }
      fprintf(hour_fp, "\n");
    }
  }

  int n_changes = 0;
  for (int i=0; i < num_points; i++)
  {
    if (discrete_points[i] == NULL) continue;
    bool vnow = discrete_points[i]->get_pv();
    if (vnow != last_discrete_vals[i])
    {
      const char *tg = discrete_points[i]->get_tag();
      const char *snow;
      const char *slast;
      //printf("Tag changed: %s\n", tg);

      if (vnow) 
      {
        snow = discrete_points[i]->get_hi_desc_ptr();
        slast = discrete_points[i]->get_lo_desc_ptr();
      }
      else
      {
        snow = discrete_points[i]->get_lo_desc_ptr();
        slast = discrete_points[i]->get_hi_desc_ptr();
      }

      if ((log_falling[i] && !vnow) || (log_rising[i] && vnow))
      {
        int elapsed_secs = now - last_detect[i];
        int n_mins = elapsed_secs / 60;
        int n_secs = elapsed_secs % 60;
        fprintf(instantaneous_fp, "%s\t%s\t%s --> %s\t%02d:%02d\n", 
           //log_falling[i] ? 'T' : 'F', 
           //log_rising[i] ? 'T' : 'F', 
           buf, tg, slast, snow, n_mins, n_secs);
        last_detect[i] = now; 
        if (log_hour_totals) 
        {
          n_hour_detects[i]++;
          n_day_detects[i]++;
        }
        n_changes++;
      }
      last_discrete_vals[i] = vnow;
    }
  }
  if (n_changes > 0)
  {
    fflush(instantaneous_fp);
  }
  last_log_time = now;
}

/**********************************************************************/

void discrete_logger_t::init_values(void)
{ 
  if (this->log_hour_totals) logfile->vprint("  Logging hour totals\n");
  this->collecting = true;

  this->instantaneous_fp = rt_open_day_history_file(this->base_name, ".txt", ap_config.get_config("LogHome"), NULL);
  if (this->instantaneous_fp == NULL)
  {
    perror(this->base_name);
    this->collecting = false;
  } 

  if (this->log_hour_totals)
  {
    this->hour_fp = rt_open_day_history_file(this->base_name, "_hour.txt", ap_config.get_config("LogHome"), NULL);

    if (this->hour_fp == NULL)
    {
      perror(this->base_name);
      this->collecting = false;
    } 
  }

  if (this->collecting)
  {
    logfile->vprint("  Collection is on for %s\n", this->get_tag());
  }

  char tmp[500];
  int argc;
  char *argv[50];

  rtrim(tags);
  ltrim(tags);
  safe_strcpy(tmp, (const char*) tags, sizeof(tmp));
  logfile->vprint("  Tags: %s\n", tmp);
  argc = get_delim_args(tmp, argv, ',', 25);

  if ((argc % 2) != 0)
  {
    logfile->vprint("  %s: There must be an even number of tag/type parameters - each group of two with first tag, then log type (RFB)\n", this->get_tag());
    logfile->vprint("      R-Rising, F-Falling, B-Both\n");
    logfile->vprint("      There were %d tag/type parameters\n", argc);
  }
  this->num_points = argc/2;
  this->discrete_points = new discrete_point_t *[this->num_points + 1];
  this->last_discrete_vals = new bool[this->num_points + 1];
  this->log_rising = new bool[this->num_points + 1];
  this->log_falling = new bool[this->num_points + 1];
  this->last_detect = new time_t[this->num_points + 1];
  this->n_hour_detects = new int[this->num_points + 1];
  this->n_day_detects = new int[this->num_points + 1];

  //for (int nd=0, j=4; (j + 1) < argc; j += 2, nd++)
  for (int i=0; i < argc/2; i++)
  {
    int p1 = i * 2;
    int p2 = (i * 2) + 1;

    logfile->vprint("  tag[%d]: %s\n", i, argv[p1]);
    logfile->vprint("  type[%d]: %s\n", i, argv[p2]);
    char temp_tag[50];
    db_point_t *db_point;
    safe_strcpy(temp_tag, (const char*) argv[p1], sizeof(temp_tag));
    rtrim(temp_tag);
    db_point = db->get_db_point(temp_tag);
    if ((db_point == NULL) || (db_point->pv_type() != DISCRETE_VALUE))
    {
      this->discrete_points[i] = NULL;
      logfile->vprint("  Bad discrete point: %s\n", temp_tag);
    }
    else
    {
      this->discrete_points[i] = (discrete_point_t *) db_point;
    }
    logfile->vprint("  discrete point [%d]: %s, ", i, temp_tag);
    this->last_discrete_vals[i] = false;
    this->last_detect[i] = time(NULL); 
    this->n_day_detects[i] = 0;
    this->n_hour_detects[i] = 0;
    switch (argv[p2][0])
    {
      case 'R':
      case 'r':
        logfile->vprint("  rising only\n");
        this->log_rising[i] = true;
        this->log_falling[i] = false;
        break;
      case 'F':
      case 'f':
        logfile->vprint("  falling only\n");
        this->log_rising[i] = false;
        this->log_falling[i] = true;
        break;
      case 'B':
      case 'b':
        logfile->vprint("  both rising and falling\n");
        this->log_rising[i] = true;
        this->log_falling[i] = true;
        break;
      default:
        logfile->vprint("  **** Error %s: discrete %d, bad param: '%s' log type must be R, F, or B (rising, falling, or both)\n", this->get_tag(), i, argv[p2]); 
        logfile->vprint("       defaulting to both rising and falling\n");
        this->log_rising[i] = true;
        this->log_falling[i] = true;
        break;
    }
  }

  if ((this->log_hour_totals) && (this->hour_fp != NULL))
  {
    for (int i=0; i < this->num_points; i++)
    {
      if (this->discrete_points[i] == NULL) continue;
      fprintf(this->hour_fp, "\t%s", this->discrete_points[i]->get_tag());
    }
    fprintf(this->hour_fp, "\n");
  }
  this->last_log_time = time(NULL);

}

/**********************************************************************/
