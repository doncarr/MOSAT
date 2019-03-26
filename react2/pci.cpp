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

/*********************************************************************/

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

/*********************************************************************/

void pci_point_t::init_values(void)
{
    this->lo_alarm_shutdown = false;
    this->hi_alarm_shutdown = false;

    this->set_format();
    this->pv = 0;
    this->raw_count = 0;
    this->history_loc = 0;
    //int n = sizeof(pci->count_history) / sizeof(pci->count_history[0]);
}

/********************************************************************/

