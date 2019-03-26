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

Control.c

Base functions for Control point types, such as PID.

*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <string.h>

#include "rtcommon.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"

/********************************************************************/

void control_point_t::set_ramp(double val, double ramp_time)
{
  /* This sets the ramp up  */

  //printf("new spt: %lf, ramp time: %lf\n",
   // (double) val, (double) ramp_time);
  double update_interval = 1.0 / db->get_sample_rate();
  if (ramp_time > 0.0)
  {
    ramp_is_on = true;
    ramp_value = val;
    ramp_counter = (int) ((double) ramp_time / (double) update_interval);
    ramp_increment = (val - setpoint) / (double) ramp_counter;
    //printf("new spt: %lf, last spt: %lf, ramp_ctr: %d, ramp inc: %lf\n",
    //(double) val, (double) setpoint,
    //(int) ramp_counter, (double) ramp_increment);
  }
  else
  {
    setpoint = val;
  }
  pv = setpoint;
}

/******************************************************************/

void control_point_t::update_ramp(void)
{
  if (ramp_is_on)
  {
    pv = setpoint += ramp_increment;
    ramp_counter--;
    if (ramp_counter <= 0)
    {
      pv = setpoint = ramp_value;
      ramp_is_on = false;
    }
    //printf("Ramp up: %lf\n", (double) pv);
  }
}

/******************************************************************/

void control_point_t::reset_alarms(void)
{
  point_state = STATE_NORMAL;
  dev_alm_detect_time = -1;
  dev_caut_detect_time = -1;
}

/******************************************************************/

void control_point_t::check_alarms(void)
{
    /* Now do some alarm detection */
  double current_time = db->get_time();
  double current_input = ai_point->get_pv();
  double current_error = setpoint - current_input;

  point_state = STATE_NORMAL;
  if (point_alarm_disable || db->global_alarm_disabled())
  {
    dev_caut_detect_time = -1;
    dev_alm_detect_time = -1;
  }
  else
  {
    if (dev_caution_enable && (fabs(current_error) > deviation_caution))
    {
      switch (last_point_state)
      {
	case STATE_NORMAL:
	  if (dev_caut_detect_time < 0)
	  {
	    dev_caut_detect_time = current_time;
	  }
	  else if ((current_time - dev_caut_detect_time) > deviation_delay)
	  {
	    point_state = STATE_DEV_CAUTION;
	  }
	  break;
	default:
	  point_state = STATE_DEV_CAUTION;
	  break;
      }
    }
    else
    {
      dev_caut_detect_time = -1;
    }

    if (dev_alarm_enable && (fabs(current_error) > deviation_alarm))
    {
      switch (last_point_state)
      {
	case STATE_NORMAL:
	case STATE_DEV_CAUTION:
	  if (dev_alm_detect_time < 0)
	  {
	    dev_alm_detect_time = current_time;
	  }
	  else if ((current_time - dev_alm_detect_time) > deviation_delay)
	  {
	    point_state = STATE_DEV_ALARM;

	  }
	  break;
	case STATE_DEV_ALARM:
	  point_state = STATE_DEV_ALARM;
	  break;
        default:
	  break;
      }
    }
    else
    {
      dev_alm_detect_time = -1;
    }
  }

  switch (point_state)
  {
    case STATE_NORMAL:
      //pv_attr = NORMAL_PV_ATTR;
      break;
    case STATE_DEV_CAUTION:
      //pv_attr = CAUTION_ATTR;
      break;
    case STATE_DEV_ALARM:
      //pv_attr = ALARM_ATTR;
      if (dev_alarm_shutdown)
      {
	//pv_attr |= (1 << 7);
	char reason[100];
	snprintf(reason, sizeof(reason),
             "DEVIATION ALARM, SPT = %0.2f, PV = %0.2f",
	     setpoint, current_input);
	trip_shutdown(reason);
      }
      break;
    default:
      break;
  }
}

/******************************************************************/

