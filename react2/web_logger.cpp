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

web_logger.cpp

Member functions for web logger.

*************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>

#include <fcntl.h> 
#include <sys/stat.h>
#include <mqueue.h>



#include "rtcommon.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"
#include "ap_config.h"

#include "silodata.h"

#define WL_MIN_ARGS (8)

FILE *fpl = NULL;

/*************************************************************************/

static int write_float_to_mq(long mq_fd, const char *tag, float value, 
             time_t the_time, const char *key)
{
  silodata_t sdata;
  sdata.the_time = the_time;
  sdata.ctype = SILO_DATA_TAG;
  snprintf(sdata.key, sizeof(sdata.key), "%s", key);

  snprintf(sdata.td.tag, sizeof(sdata.td.tag), "%s", tag);
  snprintf(sdata.td.type, sizeof(sdata.td.type), "float");
  snprintf(sdata.td.value, sizeof(sdata.td.value), "%0.1f", value);

  int rval = mq_send(mq_fd, (char *) &sdata, sizeof(sdata), 0);
  if (rval == -1)
  {
    char myline[300];
    myline[0] = '\0';
    const char *sx = strerror_r(errno, myline, sizeof(myline));
    if (fpl != NULL)
    {
      fprintf(fpl, "mq_send: %s\n", sx);
      fflush(fpl);
    }
    perror("************************************ mq_send");
  }
  printf("Send Float Success\n");
  return 0;
}

/*************************************************************************/

int write_bool_to_mq(long mq_fd, const char *tag, bool value, 
     time_t the_time, const char *key)
{
  silodata_t sdata;

  snprintf(sdata.key, sizeof(sdata.key), "%s", key);
  sdata.the_time = the_time;
  sdata.ctype = SILO_DATA_TAG;

  snprintf(sdata.td.tag, sizeof(sdata.td.tag), "%s", tag);
  snprintf(sdata.td.type, sizeof(sdata.td.type), "bool");
  snprintf(sdata.td.value, sizeof(sdata.td.value), "%c", value ? '1' : '0');

  int rval = mq_send(mq_fd, (char *) &sdata, sizeof(sdata), 0);
  if (rval == -1)
  {
    perror("************************************ mq_send");
  }
  printf("Send Bool Success\n");
  return 0;
}



/**********************************************************************/

void web_logger_t::update(void)
{
  time_t now = time(NULL);
  const char *wtag;
  // floats get written at regular intervals
  /***
  printf("Update %s, time = %ld, next = %ld, interval %d\n", tag, now, next_time, sample_interval);
  if (fpl != NULL)
  {
    fprintf(fpl, "Update %s, time = %ld, next = %ld, interval %d\n", tag, now, next_time, sample_interval);
    fflush(fpl);
  }
  logfile->vprint("web tags: %p\n", discrete_web_tags);
  ***/
  if (now >= next_time)
  {
    for (int i=0; i < num_analog; i++)
    {
      if (analog_points[i] == NULL) continue;
      wtag = analog_web_tags[i]; 
      float val = analog_points[i]->get_pv();
      write_float_to_mq(mq_fd, wtag, val, next_time, key);
      /****
      if (fpl != NULL)
      {
         fprintf(fpl, "float,%s,%0.1lf\n", wtag, val);
         fflush(fpl);
      }
      *******/
    }
    next_time += sample_interval; 
  }

  // discretes get written when they change 
  for (int i=0; i < num_discrete; i++)
  {
    printf("discrete point %d\n", i);
    if (discrete_points[i] == NULL) 
    {
      printf("*** bad discrete point\n");
      continue;
    }
    bool val = discrete_points[i]->get_pv();
    printf("%d: val now: %c, last: %c\n", i, val ? 'T' : 'F',
          this->discrete_last_vals[i] ? 'T' : 'F');
    wtag = discrete_web_tags[i]; 
    if (val != this->discrete_last_vals[i])
    {
      // Ok, this discrete point changed
     write_bool_to_mq(mq_fd, wtag, val, now, key);
     this->discrete_last_vals[i] = val;
    }
  }

}

/**********************************************************************/

void web_logger_t::init_values(void)
{
}

/**********************************************************************/

web_logger_t **web_logger_t::read(int *cnt, const char *home_dir)
{
  int max_points = 50;
  web_logger_t **logger_points =
	(web_logger_t **) malloc((max_points + 1) * sizeof(web_logger_t*));
  MALLOC_CHECK(logger_points);

  *cnt = 0;
  int count = 0;

  fpl = fopen("/tmp/web_logger.txt", "a+");

  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/web_logger.dat", sizeof(path));
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    logfile->perror(path);
    logfile->vprint("Can't open: %s\n", path);
    return NULL;
  }
  char line[300];

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[50];
    ltrim(line);
    rtrim(line);

    safe_strcpy(tmp, (const char*) line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '|', 50);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }
    else if (argc < WL_MIN_ARGS)
    {
      logfile->vprint("%s: Wrong number of args (minimum %d), line %d\n", 
          path, WL_MIN_ARGS, i+1);
      continue;
    }
    if (0 != ((argc - 5) % 3)) 
    {
      logfile->vprint(
       "%s: Wrong number of args (%d) (must be 5 + multiple of 3 ), line %d\n", 
         path, argc, i+1);
      continue;
    }

    logfile->vprint("%s\n", line);
    web_logger_t *p = new web_logger_t;

    safe_strcpy(p->tag, (const char*) argv[0], sizeof(p->tag));
    safe_strcpy(p->description, (const char*) argv[1], sizeof(p->description));
    p->sample_interval = atol(argv[2]) * 60;
    p->url = strdup(argv[3]);
    p->key = strdup(argv[4]);

    p->num_analog = 0;
    p->num_discrete = 0;
    int num_points = (argc - (WL_MIN_ARGS - 5)) / 3;
    int start;
    for (int j=0; j < num_points; j++)
    {
      start = 5 + (j * 3);
      if (0 == strcasecmp(argv[start+2], "bool"))
      {
        p->num_discrete++;
      }
      else if (0 == strcasecmp(argv[start+2], "float"))
      {
        p->num_analog++;
      }
      else
      {
        logfile->vprint(
         "%s: Unknown value type: %s, must be 'bool' or 'float', line %d\n", 
          path, argv[start+2], i+1);
      }
    } 

    logfile->vprint("%d analog points\n", p->num_analog);
    logfile->vprint("%d discrete points\n", p->num_discrete);

    // Allocate space
    p->analog_points = new analog_point_t *[p->num_analog + 1];
    p->discrete_points = new discrete_point_t *[p->num_discrete + 1];
    p->analog_web_tags = new char *[p->num_analog + 1];
    p->discrete_web_tags = new char *[p->num_discrete + 1];
    logfile->vprint("web tags: %p\n", p->discrete_web_tags);
    p->discrete_last_vals = new bool [p->num_discrete + 1];

    p->num_analog = 0;
    p->num_discrete = 0;
    for (int j=0; j < num_points; j++)
    {
      start = 5 + (j * 3);
      char temp_tag[50];
      db_point_t *db_point;
      safe_strcpy(temp_tag, (const char*) argv[start], sizeof(temp_tag));
      ltrim(temp_tag);
      rtrim(temp_tag);
      logfile->vprint("Tag = %s\n", temp_tag);

      if (0 == strcasecmp(argv[start+2], "float"))
      {
        p->analog_web_tags[p->num_analog] = strdup(argv[start+1]);
        db_point = db->get_db_point(temp_tag);
        if ((db_point == NULL) || (db_point->pv_type() != ANALOG_VALUE))
        {
          p->analog_points[p->num_analog] = NULL;
          logfile->vprint("Bad analog point: %s\n", temp_tag);
        }
        else
	{
          p->analog_points[p->num_analog] = (analog_point_t *) db_point;
          logfile->vprint("analog point %d: %s\n", p->num_analog, temp_tag);
        }
        logfile->vprint("Web Tag: %s\n", p->analog_web_tags[p->num_analog]);
        p->num_analog++;
      }
      else if (0 == strcasecmp(argv[start+2], "bool"))
      {
        p->discrete_web_tags[p->num_discrete] = strdup(argv[start+1]);
        p->discrete_last_vals[p->num_discrete] = false;
        db_point = db->get_db_point(temp_tag);
        if ((db_point == NULL) || (db_point->pv_type() != DISCRETE_VALUE))
        {
          p->discrete_points[p->num_discrete] = NULL;
          logfile->vprint("Bad discrete point: %s\n", temp_tag);
        }
        else
	{
          p->discrete_points[p->num_discrete] = (discrete_point_t *) db_point;
          logfile->vprint("discrete point %d: %s\n", p->num_discrete, temp_tag);
        }
        logfile->vprint("Web Tag: %s\n", p->discrete_web_tags[p->num_discrete]);
        p->num_discrete++;
      }
      else
      {
      }
      p->mq_fd = mq_open("/adaptivertc.react.weblog1", 
                   O_RDWR | O_CREAT, 0755, NULL);
      if (p->mq_fd == ((mqd_t) -1))
      {
        perror("mq_open");
        logfile->vprint("Can not open queue\n");
      }
    } 

    //p->hour_enable = (argv[6][0] == '1');

    logger_points[count] = p;
    count++;
    if (count >= max_points)
    {
      max_points += 25;
      logger_points = (web_logger_t **) realloc(logger_points,
		  max_points * sizeof(web_logger_t*));
      MALLOC_CHECK(logger_points);
    }

    struct tm nowtm;
    time_t now = time(NULL);
    p->next_secs = 0;
  
    localtime_r(&now, &nowtm);
    printf("%s%ld secs since the Epoch\n", asctime(&nowtm), now);
    int secs_after_hour = nowtm.tm_sec + (nowtm.tm_min * 60);
    printf("Secs after hour: %d\n", secs_after_hour);
     
    for (int j=0; 
       ((j * p->sample_interval) < (secs_after_hour)) && (j < 500); i++)
    {
      printf("%d: %d\n", j, j * p->sample_interval);
      p->next_secs = (j + 1) * p->sample_interval;
      printf("next: %ld\n", p->next_secs);
    }
    nowtm.tm_min = 0;
    nowtm.tm_sec = 0;
    p->next_time =  mktime(&nowtm);
    p->next_time += p->next_secs;
    localtime_r(&p->next_time, &nowtm);

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

time_t calc_next_time(time_t now, int sample_interval)
{
  struct tm nowtm;
  long next_secs = 0;
 
  localtime_r(&now, &nowtm);
  printf("%s %ld secs since the Epoch\n", asctime(&nowtm), now);
  int secs_after_hour = nowtm.tm_sec + (nowtm.tm_min * 60);
  printf("Secs after hour: %d\n", secs_after_hour);
     
  for (int i=0; 
       ((i * sample_interval) < (secs_after_hour)) && (i < 500); i++)
  {
    printf("%d: %d\n", i, i * sample_interval);
    next_secs = (i + 1) * sample_interval;
    printf("next: %ld\n", next_secs);
  }
  nowtm.tm_min = 0;
  nowtm.tm_sec = 0;
  time_t next_time =  mktime(&nowtm);
  next_time += next_secs;

  localtime_r(&next_time, &nowtm);
  printf("%s\n", asctime(&nowtm));
  return next_time;
}

/*************************************************************************/

