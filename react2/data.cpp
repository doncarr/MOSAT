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

/**********************************************************************/

void data_point_t::write_to_file(void)
{
  if (count == 0)
  {
    return;
  }
  char fname[200];
  snprintf(fname, sizeof(fname), "%s/dbout/%s", db->get_home_dir(), file_name);
  FILE *fp = fopen(fname, "w");
  if (fp == NULL)
  {
    logfile->vprint("Can not open output file: %s\n", fname);
    return;
  }
  logfile->vprint("Writing: %s . . .\n", fname);
  for (int i=0; i < count; i++)
  {
    fprintf(fp, "%lf", double(i) / db->get_sample_rate());
    for (int j=0; j < num_points; j++)
    {
      fprintf(fp, "\t%lf", data[j][i]);
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}

/**********************************************************************/

void data_point_t::update(void)
{
  if (!collecting)
  {
    return;
  }

  if (count >= max_samples)
  {
    collecting = false;
    return;
  }

  for (int i=0; i < num_points; i++)
  {
    if (analog_points[i] != NULL)
    {
      data[i][count] = analog_points[i]->get_pv();
    }
    else
    {
      data[count][i] = 0.0;
    }
  }
  count++;
}

/**********************************************************************/

void data_point_t::start(void)
{
  collecting = true;
  count = 0;
}

/**********************************************************************/

void data_point_t::stop(void)
{
  collecting = false;
}

/**********************************************************************/

void data_point_t::init_values(void)
{
  char tmp[500];
  int argc;
  char *argv[50];

  rtrim(tags);
  ltrim(tags);
  safe_strcpy(tmp, (const char*) tags, sizeof(tmp));
  argc = get_delim_args(tmp, argv, ',', 25);
  this->num_points = argc;

  this->max_samples = int(this->max_time * db->get_sample_rate());
  logfile->vprint("  MaxSamples = %d MaxTime = %d\n", this->max_samples, this->max_time);
  this->analog_points = new analog_point_t *[this->num_points + 1];
  this->data = new double *[this->num_points + 1];

  for (int i=0; i < argc; i++)
  {
    rtrim(argv[i]);
    ltrim(argv[i]);
    db_point_t *db_point;
    db_point = db->get_db_point(argv[i]);
    if ((db_point == NULL) || (db_point->pv_type() != ANALOG_VALUE))
    {
      this->analog_points[i] = NULL;
      logfile->vprint("%s - Bad analog point: %s\n", this->tag, argv[i]);
    }
    else
    {
      this->analog_points[i] = (analog_point_t *) db_point;
    }
    logfile->vprint("  %s - allocating space for  %d samples for tag %s\n", this->tag, this->max_samples, argv[i]);
    this->data[i] = new double[this->max_samples + 1];
    MALLOC_CHECK(this->data[i]);
  }
  this->collecting = false;
  this->count = 0;
}

/*********************************************************************/

