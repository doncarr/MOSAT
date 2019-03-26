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

/*******

alarmwin.c

Code to handler the alarm window for point values.

*************************************************************************/


#include <stdio.h>
#include <malloc.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "db_point.h"

struct alarm_win_entry_t
{
  int x, y;
  db_point_t *db_point;
};

static int num_alarm_entrys = 0;
static struct alarm_win_entry_t *alarm_win = NULL;

/*******************************************************************/

void set_alarm_window(int top, int num_lines)
{
  /* set up a window where points that are not on the display will pop
     up when the go into alarm. */

	/****
  num_alarm_entrys = 2 * num_lines;
  alarm_win = (alarm_win_entry_t *)
		 malloc(num_alarm_entrys * sizeof(alarm_win_entry_t));
  for (int i=0; i < num_lines; i++)
  {
    alarm_win[i * 2].x = 3;
    alarm_win[i * 2].y = top + i;
    alarm_win[i * 2].db_point = NULL;

    alarm_win[i * 2 + 1].x = 42;
    alarm_win[i * 2 + 1].y = top + i;
    alarm_win[i * 2 + 1].db_point = NULL;
  }
  *****/
}

/*******************************************************************/

static int current = 0;

void enable_alarm_display(db_point_t *db_point)
{
  /* Enable the display of alarms for all points in the alarm list */

  if (num_alarm_entrys == 0)
  {
    return;
  }
  if (alarm_win[current].db_point != NULL)
  {
    alarm_win[current].db_point->disable_display();
  }
  //window(1, 1, 80, 25);
  alarm_win[current].db_point  = db_point;
  db_point->enable_display(alarm_win[current].x, alarm_win[current].y);
  db_point->display();
  current++;
  if (current == num_alarm_entrys)
  {
    current = 0;
  }
}
