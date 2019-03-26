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

/***

Do.c

Member functions for discrete output points.

*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"

/********************************************************************/

void do_point_t::send(bool val)
{
  point_lock_t l(&this->point_lock, tag);
  if (invert_pv) val = not val;
  tdo_on = false; // if tdo is on, turn it off;
  blink_on = false; // if blink is on, turn it off;
  send_it(val);
}

/********************************************************************/

void do_point_t::send_it(bool val)
{
  /* Send the discrete output. */

  db->send_do(driver, card, channel, val);
  pv = val;
  if (pv)
  {
    pv_string = hi_desc;
  }
  else
  {
    pv_string = lo_desc;
  }
  display_pv();
}

/********************************************************************/

void do_point_t::blink(double val)
{
  point_lock_t l(&this->point_lock, tag);
  //printf("%s: Turning on blink, %0.1lf\n", tag, val);
  blink_on = true;
  tdo_on = false;
  blink_time = val;
  double tnow = db->get_time();
  this->send_it(not pv);
  operation_end_time = tnow + val;
}

/********************************************************************/

void do_point_t::tdo(double val)
{
  point_lock_t l(&this->point_lock, tag);
  this->send_it(true);
  double tnow = db->get_time();
  operation_end_time = tnow + val;
  tdo_on = true;
  blink_on = false;
}


/********************************************************************/

void do_point_t::update(void)
{
  point_lock_t l(&this->point_lock, tag);
  double tnow = db->get_time();
  //printf("///////// %s: Now: %0.1lf, Next%0.1lf, %s, %s\n", 
  //     tag, tnow, operation_end_time, tdo_on ? "T":"F", blink_on ? "T":"F");
  if (tdo_on)
  {
    if (tnow > operation_end_time)
    {
      this->send_it(false);
      tdo_on = false;
    }
  }

  if (blink_on)
  {
    //printf("//////////////////// %s: Now: %0.1lf, Next%0.1lf\n", 
    //      tag, tnow, operation_end_time);
    if (tnow > operation_end_time)
    {
      operation_end_time += blink_time;
      this->send_it(not pv);
    }
  }
}

/********************************************************************/

int do_point_t::get_json(char *attribute, char *buf, int sz)
{
  return discrete_point_t::get_json(attribute, buf, sz);
}

/********************************************************************/

int do_point_t::set_json(const char *attribute, const char *value)
{
  printf("do_point_t::set_json(%s, %s\n", attribute, value);
  if (0 == strcmp(attribute, "pv"))
  {
    this->send(0 == strcmp(value, "true"));
    return 0;
  }
  else if (0 == strcmp(attribute, "send"))
  {
    this->send(0 == strcmp(value, "true"));
    return 0;
  }
  else if (0 == strcmp(attribute, "blink"))
  {
    this->blink(atof(value));
    return 0;
  }
  else if (0 == strcmp(attribute, "tdo"))
  {
    this->tdo(atof(value));
    return 0;
  }
  return -2;

}

/********************************************************************/

void do_point_t::init_values(void)
{
  pv = false;
  pv_string = lo_desc;
  //pv_attr = NORMAL_PV_ATTR;
  tdo_on = false;
  blink_on = false;
  operation_end_time = 0.0;
}

/********************************************************************/

