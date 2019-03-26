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

/*****

scan.cpp

Member functions for scan collection points.

The scan object is for periodically taking a scan of a group of points.
This can be triggered by the "scan" member function.

*************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "rtcommon.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"

/**********************************************************************/

void scan_point_t::write_to_file(void)
{
  if (count == 0)
  {
    return;
  }
  char path[200];
  snprintf(path, sizeof(path), "%s/dbout/%s", db->get_home_dir(), file_name);
  FILE *fp = fopen(path, "w");
  if (fp == NULL)
  {
    logfile->vprint("Can not open output file: %s\n", path);
    return;
  }
  logfile->vprint("Writing: %s . . .\n", path);
  for (int i=0; i < count; i++)
  {
    fprintf(fp, "%f", data[0][i]);
    for (int j=1; j <= num_points; j++)
    {
      fprintf(fp, "\t%f", data[j][i]);
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}

/**********************************************************************/

void scan_point_t::scan(void)
{
  if (count >= max_samples)
  {
    collecting = false;
    return;
  }

  data[0][count] = db->get_time();
  for (int i=0; i < num_points; i++)
  {
    if (analog_points[i] != NULL)
    {
      data[i+1][count] = analog_points[i]->get_pv();
    }
    else
    {
      data[i+1][count] = 0.0;
    }
  }
  for (int i=0; i <= num_points; i++)
  {
    logfile->vprint("%lf ", data[i][count]);
  }
  logfile->vprint("\n");
  count++;
}

/**********************************************************************/


scan_point_t **scan_point_t::read(int *cnt, const char *home_dir)
{
  int max_points = 50;
  scan_point_t **scan_points =
	(scan_point_t **) malloc(max_points * sizeof(scan_point_t*));
  MALLOC_CHECK(scan_points);

  *cnt = 0;
  int count = 0;

  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/scan.dat", sizeof(path));
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
    char *argv[25];
    //DI1|Discrete Input 1|0|0|1|HI|LO|N|N|
    ltrim(line);
    rtrim(line);

    safe_strcpy(tmp, (const char *) line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '|', 25);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }
    else if (argc < 5)
    {
      logfile->vprint("%s: Wrong number of args, line %d", path, i+1);
      continue;
    }

    logfile->vprint("%s\n", line);
    scan_point_t *p = new scan_point_t;

    /*****
    Tag
    Description
    File Name
    Max Time
    Tag 1
    Tag 2
    . . .
    Tag N
    ******/

    safe_strcpy(p->tag, (const char *) argv[0], sizeof(p->tag));
    safe_strcpy(p->description, (const char *) argv[1], sizeof(p->description));
    safe_strcpy(p->file_name, (const char *) argv[2], sizeof(p->file_name));
    p->max_samples = atol(argv[3]);
    p->num_points = argc - 4;

    p->analog_points = new analog_point_t *[p->num_points];
    //(analog_point_t **) malloc(p->num_points * sizeof(analog_point_t*));
    p->data = new float *[p->num_points];
    // Ok, the first is for the time of each scan.
    p->data[0] = new float[p->max_samples];
    MALLOC_CHECK(p->data);

    for (int j=4; j < argc; j++)
    {
      char temp_tag[50];
      db_point_t *db_point;
      safe_strcpy(temp_tag, (const char *) argv[j], sizeof(temp_tag));
      rtrim(temp_tag);
      db_point = db->get_db_point(temp_tag);
      if ((db_point == NULL) || (db_point->pv_type() != ANALOG_VALUE))
      {
        p->analog_points[j-4] = NULL;
        logfile->vprint("Bad analog point: %s\n", temp_tag);
      }
      else
      {
        p->analog_points[j-4] = (analog_point_t *) db_point;
      }
      p->data[j-3] = new float[p->max_samples];
      MALLOC_CHECK(p->data[j-3]);
    }


    p->count = 0;
    p->pv = false;

    scan_points[count] = p;
    count++;
    if (count > max_points)
    {
      max_points += 10;
      scan_points = (scan_point_t **) realloc(scan_points,
		  max_points * sizeof(scan_point_t*));
      MALLOC_CHECK(scan_points);
    }
  }

  if (count == 0)
  {
    free(scan_points);
    return NULL;
  }
  *cnt = count;
  return scan_points;

}

/*************************************************************************/

