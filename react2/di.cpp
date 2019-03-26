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

Di.c

Member functions for discrete input points.

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

/**********************************************************************/

void di_point_t::update(bool value)
{
  point_lock_t l(&point_lock, tag);
  //printf("entering update for %s\n", tag);
  /* Update the discrete input point. */

  //printf("@@ tag: %s, value: %s, invert: %s\n", tag, value ? "T":"F", invert_pv ? "T":"F");
  
  if (invert_pv) value = not value;
  pv_last = pv;
  pv  = value;

  display_pv();
  check_alarms();
  //printf("exiting update for %s\n", tag);

}

/**********************************************************************/

void di_point_t::init_values(void)
{
  pv_string = lo_desc;
  pv = false;
  json_str = NULL;
  switch (this->alarm_state_str[0])
  {
      case 'H':
      case 'h':
	this->alarm_state = DISCRETE_HI;
	break;
      case 'L':
      case 'l':
	this->alarm_state = DISCRETE_LO;
	break;
      case 'N':
      case 'n':
	this->alarm_state = DISCRETE_NONE;
	break;
      default:
	this->alarm_state = DISCRETE_NONE;
        logfile->vprint("  %s Bad value for alarm state: %s\n", 
                  tag, alarm_state_str);
	break;
  }
  switch (this->shutdown_state_str[0])
  {
      case 'H':
      case 'h':
	this->shutdown_state = DISCRETE_HI;
	break;
      case 'L':
      case 'l':
	this->shutdown_state = DISCRETE_LO;
	break;
      case 'N':
      case 'n':
	this->shutdown_state = DISCRETE_NONE;
	break;
      default:
	this->shutdown_state = DISCRETE_NONE;
        logfile->vprint("  %s Bad value for shutdown state: %s\n", 
                  tag, shutdown_state_str);
	break;
  }
}

/**********************************************************************/
static const char *get_json_dstate(int state)
{
  switch (state) 
  {
    case DISCRETE_LO:
      return "LOW";
      break;
    case DISCRETE_HI:
      return "HIGH";
      break;
    case DISCRETE_NONE:
    default:
      return "NONE";
      break;
  }
}

/**********************************************************************/

const char *di_point_t::get_config_json(void)
{
  point_lock_t l(&this->point_lock, tag);
  if (json_str == NULL)
  {
     asprintf(&json_str, "{\"tag\":\"%s\",\"description\":\"%s\",\"driver\":%d,\"card\":%d,\"channel\":%d, \"lo_desc\":\"%s\",\"hi_desc\":\"%s\",\"alarm_state\":\"%s\",\"shutdown_state\":\"%s\"}",
         this->tag,
         this->description,
         this->driver,
         this->card,
         this->channel,
         this->lo_desc,
         this->hi_desc,
         get_json_dstate(this->alarm_state),
         get_json_dstate(this->shutdown_state)
      );
  }
  return json_str;
}


/*********************************************************************/

