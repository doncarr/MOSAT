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
  pid control

*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <string.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"

/********************************************************************/

void pid_point_t::change_setpoint(double val, double ramp_time)
{
  /* change the set point for the given control point. */

  if ((ai_point == NULL) || (ao_point == NULL))
  {
    return;
  }

  if (!control_enabled)
  {
    this->start_control();
  }

  set_ramp(val, ramp_time);
  pv = setpoint;
  display_pv();
  control_enabled = true;
}

/******************************************************************/

void pid_point_t::start_control(void)
{
  /* Start control for the given control point. */

  if ((ai_point == NULL) || (ao_point == NULL))
  {
    return;
  }

  pv = setpoint = ai_point->get_pv();

    /* Use the current output for a starting point. */
  integrated_error = ao_point->get_pv() * i_time / p_gain;

  last_time = db->get_time();
  last_error = 0;
  last_output = ao_point->get_pv();
  last_input = ai_point->get_pv();
  display_pv();
  control_enabled = true;
  point_state = STATE_NORMAL;
  delay_counter = updates_per_evaluation - 1;
  reset_alarms();
}

/******************************************************************/

void pid_point_t::stop_control(void)
{
  /* Stop control for the given control point. */

  control_enabled = false;
  point_state = STATE_NORMAL;
  reset_alarms();
}

/******************************************************************/

void pid_point_t::update(void)
{
  /* Update the given control point. */

  if (!control_enabled)
  {
    return;
  }
  update_ramp();

  if (delay_counter == 0)
  {
    delay_counter = updates_per_evaluation - 1;
  }
  else
  {
    delay_counter--;
    return;
  }

  /* PID algorithm starts here. */
  double current_time = db->get_time();
  double delta_t = current_time - last_time;

    /* Read the process variable. */
  double current_input = ai_point->get_pv();

    /* Calculate error */
  double current_error = setpoint - current_input;

    /* Do a simple straight line approximation of the derivitave. */
  double current_derivative = (current_input - last_input) / delta_t;

    /* Integrate only if we're NOT saturated! */
  if ((last_output < ao_point->output_limit_hi) &&
       (last_output > ao_point->output_limit_lo))
  {
      /* Integrate using trapeziodal method. */
    integrated_error += delta_t * (last_error + current_error) * 0.5;
  }

    /* Calculate the output response using the standard PID algorithm */
  double current_output = p_gain * ( current_error +
			     ((1/i_time) * integrated_error) -
			     (d_time * current_derivative) );

  //printf("err: %0.1lf, i_err: %0.1lf, d_err: %0.1lf, ao: %lf\n",
  // current_error, integrated_error, current_derivative, current_output);
    /* Update the manipulated variable. */
   //printf("%lf %lf %lf %lf %lf\n",
   //delta_t, last_input, current_input, integrated_error, current_derivative);
 // printf("%lf %lf %lf %lf\n",
  //  current_time, current_error, setpoint, current_output);
  ao_point->send(current_output);

    /* Now do some alarm detection */
  check_alarms();

  /* Save current values for the next update interval */
  last_time = current_time;
  last_input = current_input;
  last_output = current_output;
  last_error = current_error;
  last_point_state = point_state;

  display_pv();
}

/******************************************************************/

void pid_point_t::init_values(void)
{
  db_point_t *db_point;

  ltrim(ai_tag);
  rtrim(ai_tag);
  db_point = db->get_db_point(ai_tag);

  if (db_point == NULL)
  {
    logfile->vprint("  %s - bad TAGNAME: %s\n", tag, ai_tag);
    ai_point = NULL;
  }
  else
  {
    ai_point = dynamic_cast <analog_point_t *> (db_point);
    if (ai_point == NULL)
    {
      logfile->vprint("  %s - bad analog input point: %s\n", tag, ai_tag);
    }
  }

  ltrim(ao_tag);
  rtrim(ao_tag);
  db_point = db->get_db_point(ao_tag);

  if (db_point == NULL)
  {
    logfile->vprint("  %s - bad TAGNAME: %s\n", tag, ao_tag);
    ao_point = NULL;
  }
  else
  {
    ao_point = dynamic_cast <ao_point_t *> (db_point);
    if (ao_point == NULL)
    {
      logfile->vprint("  %s - bad analog output point: %s\n", tag, ao_tag);
    }
  }

  set_format();
  integrated_error = 0;
  last_output = last_input = last_error = last_time = 0;
  pv = setpoint = 22.0;
  point_state = STATE_NORMAL;
  dev_alm_detect_time = -1;
  dev_caut_detect_time = -1;
  //pv_attr = NORMAL_PV_ATTR;
  control_enabled = false;
  ramp_is_on = false;
  updates_per_evaluation = 1;
  delay_counter = updates_per_evaluation - 1;
}

/********************************************************************/

