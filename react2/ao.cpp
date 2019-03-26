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

/****

Ao.c

Code for the analog output member functions.

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

void ao_point_t::send(double val)
{
  point_lock_t l(&this->point_lock, tag);
  ramp_on = false;
  this->sendit(val);
}

/********************************************************************/

int ao_point_t::get_json(char *attribute, char *buf, int sz)
{
  return analog_point_t::get_json(attribute, buf, sz);
}

/********************************************************************/

int ao_point_t::set_json(const char *attribute, const char *value)
{
  if (0 == strcmp(attribute, "pv"))
  {
    this->send(atof(value));
    return 0;
  }
  else if (0 == strcmp(attribute, "send"))
  {
    this->send(atof(value));
    return 0;
  }
  else if (0 == strcmp(attribute, "ramp"))
  {
    if (value[0] != '[') return -3; // must be an array
    double ramp_v = atof(value + 1);
    double ramp_time = 0;
    bool found = false;
    for (unsigned int i=0; i < strlen(value); i++)
    {
      if (value[i] == ',')
      {
        found = true;
        ramp_time = atof(&value[i + 1]);
      }
    }
    if (found) 
    {
      this->ramp(ramp_v, ramp_time);
      return 0;
    }
    return -3;
  }
  return -2;
}

/********************************************************************/

void ao_point_t::sendit(double val)
{
  /* Send an analog output. */

  double raw;

  if (val > output_limit_hi)
  {
    val = output_limit_hi;
  }
  if (val < output_limit_lo)
  {
    val = output_limit_lo;
  }
  raw = ((raw_hi - raw_lo) * (val - eu_lo) / (eu_hi - eu_lo)) + raw_lo;
  db->send_ao(driver, card, channel, raw);
  //printf("%s.send(%lf)\n", tag, val);
  pv = val;
  display_pv();
}

/********************************************************************/

void ao_point_t::update(void)
{
  point_lock_t l(&this->point_lock, tag);
  if (!ramp_on)
  {
    return;
  }
  double sendval = pv + ramp_inc;
  if (ramp_inc > 0)
  {
    if (sendval > ramp_val)
    {
      sendval = ramp_val;
      ramp_on = false;
    }
  }
  else
  {
    if (sendval < ramp_val)
    {
      sendval = ramp_val;
      ramp_on = false;
    }
  }
  //printf("%s: ramping: %lf\n", this->tag, sendval);
  this->sendit(sendval);
}

/********************************************************************/

void ao_point_t::ramp(double val, double time)
{
  point_lock_t l(&this->point_lock, tag);
  ramp_on = true;
  ramp_val = val;
  double dif = val - pv;
  int increments = int(time * db->get_sample_rate());
  ramp_inc = dif / increments;
  //printf("ramping to: %lf, ramp increment: %lf", val, ramp_inc);
}

/********************************************************************/

void ao_point_t::init_values(void)
{
    set_format();
    pv = 0.0;
    //pv_attr = NORMAL_PV_ATTR;
}

/********************************************************************/
