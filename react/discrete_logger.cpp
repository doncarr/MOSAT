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
    logfile->vprint(" **** NOT collecting\n");
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
      struct tm mytm;
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
        fprintf(hour_fp, "\t%s", discrete_points[i]->tag);
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
        snow = discrete_points[i]->hi_desc;
        slast = discrete_points[i]->lo_desc;
      }
      else
      {
        snow = discrete_points[i]->lo_desc;
        slast = discrete_points[i]->hi_desc;
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

bool discrete_logger_t::read_json(char *json_buffer, discrete_logger_t *p)
{
  char *argv[100];
  int argc;
  int type;
  char *trailing_chars;
  char err[100];

  //char *rt_json_next_value(char *buf, char *argv[], int max_argc, int *actual_argc, int *type, char *err, int esz)  
  trailing_chars = rt_json_next_value(json_buffer, argv, sizeof(argv)/sizeof(argv[0]), 
                                               &argc, &type, err, sizeof(err)/sizeof(err[0]));  

  ltrim(trailing_chars);
  if (strlen(trailing_chars) != 0)
  {
    printf("Unexpected characters at end: %s\n", trailing_chars);
    return false;
  }

  if (type == RT_JSON_ERROR)
  {
    p->num_points = 0; 
    p->discrete_points = NULL;
    p->bool_refs = NULL;
    p->last_discrete_vals = NULL;
    p->log_rising = NULL;
    p->log_falling = NULL;
    p->last_detect = NULL;
    p->n_hour_detects = NULL;
    p->n_day_detects = NULL;
    logfile->vprint("Error parsing JSON for %s: %s\n", p->tag, err);
    return false;
  }
  else if (type != RT_JSON_ARRAY)
  {
    p->num_points = 0; 
    p->discrete_points = NULL;
    p->bool_refs = NULL;
    p->last_discrete_vals = NULL;
    p->log_rising = NULL;
    p->log_falling = NULL;
    p->last_detect = NULL;
    p->n_hour_detects = NULL;
    p->n_day_detects = NULL;
    logfile->vprint("Error parsing for %s, expected JSON ARRAY, found: %s\n", p->tag, rt_json_type_string(type));
    return false;
  }

  p->num_points = argc; 
  p->discrete_points = new discrete_point_t *[p->num_points + 1];
  p->bool_refs = new rt_bool_ref_t *[p->num_points + 1];
  p->last_discrete_vals = new bool[p->num_points + 1];
  p->log_rising = new bool[p->num_points + 1];
  p->log_falling = new bool[p->num_points + 1];
  p->last_detect = new time_t[p->num_points + 1];
  p->n_hour_detects = new int[p->num_points + 1];
  p->n_day_detects = new int[p->num_points + 1];

  char *item_argv[2];
  int item_argc;
  int item_type;

  for (int i=0; i < argc; i++)
  {
    trailing_chars = rt_json_next_value(argv[i], item_argv, sizeof(item_argv)/sizeof(item_argv[0]), 
                            &item_argc, &item_type, err, sizeof(err)/sizeof(err[0]));  

    ltrim(trailing_chars);
    if (strlen(trailing_chars) != 0)
    {
      printf("Unexpected characters at end: %s\n", trailing_chars);
      return false;
    }

    if (item_type == RT_JSON_ERROR)
    {
      logfile->vprint("Error parsing JSON for %s: %s\n", p->tag, err);
      p->bool_refs[i] = NULL;
      p->log_rising[i] = false;
      p->log_falling[i] = false;
      continue;
    }
    else if (item_type != RT_JSON_ARRAY)
    {
      logfile->vprint("Error parsing for %s, expected JSON ARRAY, found: %s\n", p->tag, rt_json_type_string(item_type));
      p->bool_refs[i] = NULL;
      p->log_rising[i] = false;
      p->log_falling[i] = false;
      continue;
    }
    
    if (item_argc != 2)
    {
      printf("Error, must have exactly two arguments for every tag / log type\n");
      return false;
    }

     
    char *tag = item_argv[0];
    char *log_type = item_argv[1];
    ltrim(tag);
    rtrim(tag);
    strip_quotes(tag);
    ltrim(log_type);
    rtrim(log_type);
    strip_quotes(log_type);
    rt_bool_ref_t *bref;
    bref = react_get_discrete_ref_fn(tag);
    if (bref == NULL)
    {
      logfile->vprint("BAD discrete tag: %s\n", tag);
      p->bool_refs[i] = NULL;
      continue;
    }

    switch (log_type[0])
    {
      case 'R':
      case 'r':
        logfile->vprint("rising only\n");
        p->log_rising[i] = true;
        p->log_falling[i] = false;
        break;
      case 'F':
      case 'f':
        logfile->vprint("falling only\n");
        p->log_rising[i] = false;
        p->log_falling[i] = true;
        break;
      case 'B':
      case 'b':
        logfile->vprint("both rising and falling\n");
        p->log_rising[i] = true;
        p->log_falling[i] = true;
        break;
      default:
        logfile->vprint("******* Error %s: discrete %d, bad param: '%s' log type must be R, F, or B (rising, falling, or both)\n",
                   p->tag, i, log_type); 
        logfile->vprint("defaulting to both rising and falling\n");
        p->log_rising[i] = true;
        p->log_falling[i] = true;
        break;
    }
  }

  return true;
}

/**********************************************************************/

discrete_logger_t **discrete_logger_t::read(int *cnt, const char *home_dir)
{
  int max_points = 50;
  discrete_logger_t **logger_points =
	(discrete_logger_t **) malloc((max_points + 1) * sizeof(discrete_logger_t*));
  MALLOC_CHECK(logger_points);

  *cnt = 0;
  int count = 0;

  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/discrete_logger.dat", sizeof(path));
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    logfile->perror(path);
    logfile->vprint("Can't open: %s\n", path);
    return NULL;
  }

  time_t now = time(NULL);
  char line[300];

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[50];
    //DI1|Discrete Input 1|0|0|1|HI|LO|N|N|
    ltrim(line);
    rtrim(line);
    safe_strcpy(tmp, (const char*) line, sizeof(tmp));
    logfile->vprint("Line: %s\n", line);

    argc = get_delim_args(tmp, argv, '|', 50);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }
    else if (argc < 6)
    {
      logfile->vprint("%s: Wrong number of args (minimum 5), line %d\n", path, i+1);
      continue;
    }
    for (int i=0; i < argc; i++) logfile->vprint("argv[%d]: %s\n", i, argv[i]);
    logfile->vprint("\n");


    logfile->vprint("%s\n", line);
    discrete_logger_t *p = new discrete_logger_t;

    safe_strcpy(p->tag, (const char*) argv[0], sizeof(p->tag));
    safe_strcpy(p->description, (const char*) argv[1], sizeof(p->description));
    safe_strcpy(p->base_name, (const char*) argv[2], sizeof(p->base_name));
    p->log_hour_totals = (argv[3][0] == '1') || 
                         (argv[3][0] == 'T') || 
                         (argv[3][0] == 't');
    if (p->log_hour_totals) logfile->vprint("Logging hour totals\n");
    p->collecting = true;

    p->instantaneous_fp = rt_open_day_history_file(p->base_name, ".txt", ap_config.get_config("LogHome"), NULL);
    if (p->instantaneous_fp == NULL)
    {
      perror(p->base_name);
      p->collecting = false;
    } 

    if (p->log_hour_totals)
    {
      p->hour_fp = rt_open_day_history_file(p->base_name, "_hour.txt", ap_config.get_config("LogHome"), NULL);

      if (p->hour_fp == NULL)
      {
        perror(p->base_name);
        p->collecting = false;
      } 
    }

    if (p->collecting)
    {
      logfile->vprint("collection is on for %s\n", p->tag);
    }

    int n_extra = argc - 4;
    if ((n_extra % 2) != 0)
    {
      logfile->vprint("%s: There must be an even number of extra parameters - each with tag, and log type (rise, fall, or both (RFB))\n", p->tag);
      logfile->vprint("    There were %d parameters, and %d extra parameters\n", argc, n_extra);
    }
    p->num_points = n_extra / 2; 
    p->discrete_points = new discrete_point_t *[p->num_points + 1];
    p->last_discrete_vals = new bool[p->num_points + 1];
    p->log_rising = new bool[p->num_points + 1];
    p->log_falling = new bool[p->num_points + 1];
    p->last_detect = new time_t[p->num_points + 1];
    p->n_hour_detects = new int[p->num_points + 1];
    p->n_day_detects = new int[p->num_points + 1];

     
    for (int nd=0, j=4; (j + 1) < argc; j += 2, nd++)
    {
      char temp_tag[50];
      db_point_t *db_point;
      safe_strcpy(temp_tag, (const char*) argv[j], sizeof(temp_tag));
      rtrim(temp_tag);
      db_point = db->get_db_point(temp_tag);
      if ((db_point == NULL) || (db_point->pv_type() != DISCRETE_VALUE))
      {
        p->discrete_points[nd] = NULL;
        logfile->vprint("Bad discrete point: %s\n", temp_tag);
      }
      else
      {
        p->discrete_points[nd] = (discrete_point_t *) db_point;
      }
      logfile->vprint("discrete point [%d]: %s, ", nd, temp_tag);
      p->last_discrete_vals[nd] = false;
      p->last_detect[nd] = now; 
      p->n_day_detects[nd] = 0;
      p->n_hour_detects[nd] = 0;
      switch (argv[j+1][0])
      {
        case 'R':
        case 'r':
          logfile->vprint("rising only\n");
          p->log_rising[nd] = true;
          p->log_falling[nd] = false;
          break;
        case 'F':
        case 'f':
          logfile->vprint("falling only\n");
          p->log_rising[nd] = false;
          p->log_falling[nd] = true;
          break;
        case 'B':
        case 'b':
          logfile->vprint("both rising and falling\n");
          p->log_rising[nd] = true;
          p->log_falling[nd] = true;
          break;
        default:
          logfile->vprint("******* Error %s: discrete %d, bad param: '%s' log type must be R, F, or B (rising, falling, or both)\n", p->tag, nd, argv[j+1]); 
          logfile->vprint("defaulting to both rising and falling\n");
          p->log_rising[nd] = true;
          p->log_falling[nd] = true;
          break;
      }
    }

    if ((p->log_hour_totals) && (p->hour_fp != NULL))
    {
      for (int i=0; i < p->num_points; i++)
      {
        if (p->discrete_points[i] == NULL) continue;
        fprintf(p->hour_fp, "\t%s", p->discrete_points[i]->tag);
      }
      fprintf(p->hour_fp, "\n");
    }

    p->last_log_time = time(NULL);

    logger_points[count] = p;
    count++;
    if (count > max_points)
    {
      max_points += 10;
      logger_points = (discrete_logger_t **) realloc(logger_points,
		  max_points * sizeof(discrete_logger_t*));
      MALLOC_CHECK(logger_points);
    }
  }

  *cnt = count;
  if (count == 0)
  {
    free(logger_points);
    return NULL;
  }
  return logger_points;
}

/*************************************************************************/

