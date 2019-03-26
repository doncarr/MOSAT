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

/*********
pci.c

Code for the pulse count input member functions.

*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <ncurses.h>
#include <math.h>
#include <malloc.h>
#include <float.h>

#include "rtcommon.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"

/*************************************************************************/

void pci_point_t::update(long new_raw_count)
{
  /* This routine updates the analog input point using the given raw
     value. */

  long counts;
  //printf("raw val = %ld\n", new_raw_count);
  long current_count = new_raw_count;
  if (current_count > last_count)
  {
    counts = current_count - last_count;
  }
  else
  {
    counts = current_count - last_count + rollover_count;
  }
  double now = db->get_time();
  if (counts > min_counts_for_calc)
  {
    pv = ((double) counts * eu_per_count * time_factor) / (now - last_time);
    last_count = current_count;
    last_time = now;
  }
  check_alarms();
  display_pv();
}

/*************************************************************************/

pci_point_t **pci_point_t::read(int *cnt, const char *home_dir)
{
  int max_points = 50;
  pci_point_t **pci_points =
	(pci_point_t **) malloc(max_points * sizeof(pci_point_t*));
  MALLOC_CHECK(pci_points);

  *cnt = 0;
  int count = 0;
  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/pci.dat", sizeof(path));
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    logfile->perror(path);
    logfile->vprint("Can't open %s", path);
    return NULL;
  }
  char line[300];

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[25];

    //safe_strcpy(line, "AI1|Analog Input 1|PSI|0|0|1|0|100|0|4095|1|5|10|20|80|90|5|0|0|0|0|");
    ltrim(line);
    rtrim(line);
    safe_strcpy(tmp, (const char*) line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '|', 25);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }
    else if (argc != 21)
    {
      logfile->vprint("%s: Wrong number of args, line %d", path, i+1);
      continue;
    }
    logfile->vprint("%s\n", line);
    pci_point_t *pci = new pci_point_t;
    /*****/

    /******
    PCI|Pulse Count 1|RPM|0|0|0|0.0083333333|20|65535|60.0|5|0|10|20|80|90|5|1|1|1|1|
    Tag
    Description
    Engineering Units Label
    Driver Number
    Card
    Channel
    EU per count
    minimum counts for calculation
    rollover count
    time factor (time is in seconds, to convert to minutes, use 60)
    Decimal Places
    Zero Cutoff
    Alarm Low
    Caution Low
    Caution High
    Alarm High
    Alarm Deadband
    Alarm Low Enable
    Caution Low Enable
    Caution High Enable
    Alarm High Enable
    ******/
    safe_strcpy(pci->tag, (const char*) argv[0], sizeof(pci->tag));
    safe_strcpy(pci->description, (const char*) argv[1], sizeof(pci->description));
    safe_strcpy(pci->eu, (const char*) argv[2], sizeof(pci->eu));
    pci->driver = atol(argv[3]);
    pci->card = atol(argv[4]);
    pci->channel = atol(argv[5]);
    pci->eu_per_count = atof(argv[6]);
    pci->min_counts_for_calc = atol(argv[7]);
    pci->rollover_count = atol(argv[8]);
    pci->time_factor = atof(argv[9]);
    pci->decimal_places = atol(argv[10]);
    pci->zero_cutoff = atof(argv[11]);
    pci->lo_alarm = atof(argv[12]);
    pci->lo_caution = atof(argv[13]);
    pci->hi_caution = atof(argv[14]);
    pci->hi_alarm = atof(argv[15]);
    pci->deadband = atof(argv[16]);
    pci->lo_alarm_enable = (argv[17][0] == '1');
    pci->lo_caution_enable = (argv[18][0] == '1');
    pci->hi_caution_enable = (argv[19][0] == '1');
    pci->hi_alarm_enable = (argv[20][0] == '1');

    pci->lo_alarm_shutdown = false;
    pci->hi_alarm_shutdown = false;

    pci->set_format();
    pci->pv = 0;
    pci->raw_count = 0;
    pci->history_loc = 0;
    //int n = sizeof(pci->count_history) / sizeof(pci->count_history[0]);

    /*****/
    pci_points[i] = pci;
    count++;
    if (count > max_points)
    {
      max_points += 10;
      pci_points = (pci_point_t **) realloc(pci_points,
	      max_points * sizeof(pci_point_t*));
      MALLOC_CHECK(pci_points);
    }
    logfile->vprint("PCI[%d].tag = '%s'\n", i, pci->tag);
  }
  if (count == 0)
  {
    free(pci_points);
    return NULL;
  }
  *cnt = count;
  return pci_points;
}

/************************************************************************/

