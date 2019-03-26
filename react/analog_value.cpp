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

/***********
analog_value.cpp

Member functions for analog value points.

*************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "exp.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"
#include "error.h"

/************************************************************************/

analog_value_point_t *analog_value_point_t::read_one(int argc, char *argv[], char *err, int esz)
{
  if (argc != 5)
  {
    snprintf(err, esz, "Should be 5 args for analog_value, found: %d\n", argc);
    return NULL;
  }
  analog_value_point_t *p = new analog_value_point_t;

  safe_strcpy(p->tag, argv[0], sizeof(p->tag));
  safe_strcpy(p->description, argv[1], sizeof(p->description));
  safe_strcpy(p->eu, argv[2], sizeof(p->eu));

  p->decimal_places = atoi(argv[3]);
  p->pv = atof(argv[4]);

  return p;
}

/*************************************************************************/

analog_value_point_t **analog_value_point_t::read(int *cnt, const char *home_dir)
{

  int max_points = 20;
  analog_value_point_t **analog_values =
	(analog_value_point_t **) malloc(max_points * sizeof(analog_value_point_t*));
  MALLOC_CHECK(analog_values);

  *cnt = 0;
  int count = 0;

  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/analog_value.dat", sizeof(path));
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    logfile->perror(path);
    logfile->vprint("Can't open file: %s\n", path);

    return NULL;
  }
  char line[300];

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    int argc;
    char *argv[25];
    char tmp[300];
    ltrim(line);
    rtrim(line);
    safe_strcpy(tmp, line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '|', 25);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }
    else if (argc != 5)
    {
      logfile->vprint("analog_value.dat, line %d: Should be 5 args, found %d\n", i+1, argc);
      continue;
    }
    logfile->vprint("%s\n", line);
    /*****
    tag
    description
    engineering units
    decimals
    initial_value 
    ****/
    if (count > max_points)
    {
      max_points += 10;
      analog_values = (analog_value_point_t **) realloc(analog_values,
	       max_points * sizeof(analog_value_point_t*));
      MALLOC_CHECK(analog_values);
    }

    char errbuf[100];
    analog_values[count] = analog_value_point_t::read_one(argc, argv, errbuf, sizeof(errbuf));
    if (analog_values[count] == NULL)
    {
      logfile->vprint("%s:%d %s\n", path, i+1, errbuf);
      continue;
    }
    count++;
  }

  fclose(fp);
  if (count == 0)
  {
    free(analog_values);
    return NULL;
  }
  *cnt = count;
  return analog_values;
}

/************************************************************************/

