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

void level_point_t::init_values(void)
{
/**
table|Level|level|level_point_t|
separator|Basic Configuration|
string|Tag|tag|25|
string|Description|description|50|
string|DI Tag High|di_hi_tag|25|
string|DI Tag Low|di_lo_tag|25|
string|AI Tag Level|level_tag|25|
double|Volume Low to High|volume_lo_hi|
**/

  char temp_tag[30];
  db_point_t *db_point;

  safe_strcpy(temp_tag, (const char*) this->di_lo_tag, sizeof(temp_tag));
  rtrim(temp_tag);
  db_point = db->get_db_point(temp_tag);
  if ((db_point == NULL) || (db_point->point_type() != DISCRETE_INPUT))
  {
    this->di_lo = NULL;
    logfile->vprint("%s - bad discrete input point: %s\n", this->tag, temp_tag);
  }
  else
  {
    this->di_lo = (di_point_t *) db_point;
  }

  safe_strcpy(temp_tag, (const char*) this->di_hi_tag, sizeof(temp_tag));
  rtrim(temp_tag);
  db_point = db->get_db_point(temp_tag);
  if ((db_point == NULL) || (db_point->point_type() != DISCRETE_INPUT))
  {
    this->di_hi = NULL;
    logfile->vprint("%s - bad discrete input point: %s\n", this->tag, temp_tag);
  }
  else
  {
    this->di_hi = (di_point_t *) db_point;
  }

  safe_strcpy(temp_tag, (const char*) level_tag, sizeof(temp_tag));
  rtrim(temp_tag);
  db_point = db->get_db_point(temp_tag);
  if ((db_point == NULL) || (db_point->point_type() != ANALOG_INPUT))
  {
    this->level_point = NULL;
    logfile->vprint("%s - bad analog input point: %s\n", this->tag, temp_tag);
  }
  else
  {
    this->level_point = (ai_point_t *) db_point;
  }

  this->total_hour_fill_rates = 0.0;
  this->total_hour_samples = 0;
  this->total_day_fill_rates = 0.0;
  this->total_day_samples = 0;
  this->pumping = true;
  this->this_hour = this->this_day = time(NULL);
  this->time_at_bottom = time(NULL);

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

  this->level_fp = rt_open_day_history_file(NULL, "_agua.txt", plog_home, NULL);

}

/********************************************************************/

