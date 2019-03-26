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

function pump_init_f(val)
{
  this.lo_desc = val.lo_desc;
  this.hi_desc = val.hi_desc;
}
pump_t.prototype.init=pump_init_f;

function pid_t(the_in_tag, the_out_tag)
{
  this.in_tag = the_in_tag;
  this.out_tag = the_out_tag;
  this.integrated_error = 0; 
  this.last_error = 0;
  this.last_input = 0;
  this.p_gain = 0;
  this.i_time = 0;
  this.d_time = 0;
  this.output_limit_hi = 0;
  this.output_limit_lo = 0;
  this.control_enabled = true;
}

/******************************************************************/

function pid_update_f(now)
{
  /* Update the given control point. */

  if (!control_enabled)
  {
    return;
  }

  /* PID algorithm starts here. */
  var current_time = now;
  var delta_t = current_time - this.last_time;

    /* Read the process variable. */
  var current_input = ai_point->get_pv();

    /* Calculate error */
  var current_error = setpoint - current_input;

    /* Do a simple straight line approximation of the derivitave. */
  var current_derivative = (current_input - this.last_input) / delta_t;

    /* Integrate only if we're NOT saturated! */
  if ((this.last_output < this.output_limit_hi) &&
       (this.last_output > this.output_limit_lo))
  {
      /* Integrate using trapeziodal method. */
    this.integrated_error += delta_t * (this.last_error + current_error) * 0.5;
  }

    /* Calculate the output response using the standard PID algorithm */
  double current_output = this.p_gain * ( current_error +
			     ((1/this.i_time) * this.integrated_error) -
			     (this.d_time * current_derivative) );

  ao_point->send(current_output);

  /* Save current values for the next update interval */
  this.last_time = current_time;
  this.last_input = current_input;
  this.last_output = current_output;
  this.last_error = current_error;
}:
pid_t.prototype.update=pid_update_f;

/******************************************************************/

function pid_init_f()
{
  this.integrated_error = 0;
  this.last_output = this.last_input = this.last_error = this.last_time = 0;
  this.control_enabled = true;
}
pid_t.prototype.init=pid_init_f;

/********************************************************************/

pid_point_t *pid_point_t::create_one(int argc, char *argv[], char *err, int esz)
{
  pid_point_t *objp;
  objp = new pid_point_t;
  if (objp == NULL)
  {
    perror("new pid_point_t");
    snprintf(err, esz, "call to 'new' failed");
    return NULL;
  }
  if (argc != 19)
  {
    snprintf(err, esz, "Wrong number of args for pid: %d, should be 19", argc);
    return NULL;
  }
  snprintf(objp->tag, sizeof(objp->tag), "%s", argv[0]);
  snprintf(objp->description, sizeof(objp->description), "%s", argv[1]);
  snprintf(objp->eu, sizeof(objp->eu), "%s", argv[2]);
  objp->decimal_places = atoi(argv[3]);
  objp->setpoint_limit_hi = atof(argv[4]);
  objp->setpoint_limit_lo = atof(argv[5]);
  objp->p_gain = atof(argv[6]);
  objp->i_time = atof(argv[7]);
  objp->d_time = atof(argv[8]);
  snprintf(objp->ai_tag, sizeof(objp->ai_tag), "%s", argv[9]);
  snprintf(objp->ao_tag, sizeof(objp->ao_tag), "%s", argv[10]);
  objp->deviation_alarm = atof(argv[11]);
  objp->deviation_caution = atof(argv[12]);
  objp->deviation_delay = atof(argv[13]);
  objp->dev_alarm_enable = (argv[14][0] == '1') ||
             (argv[14][0] == 'T') ||
             (argv[14][0] == 't');
  objp->dev_alarm_shutdown = (argv[15][0] == '1') ||
             (argv[15][0] == 'T') ||
             (argv[15][0] == 't');
  objp->dev_caution_enable = (argv[16][0] == '1') ||
             (argv[16][0] == 'T') ||
             (argv[16][0] == 't');
  objp->scale_lo = atof(argv[17]);
  objp->scale_hi = atof(argv[18]);
  objp->init_values();
  return objp;
}


/***************************/
