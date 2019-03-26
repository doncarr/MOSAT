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

enum tune_type_t {TUNE_PI, TUNE_PID};
struct tune_data_t
{
  double A0;
  double A1;
  double k;
  double L;
  double T;
  double a;
  double Kp;
  double Ti;
  double Td;
};

/******************************************************************/

double find_k(double data[], int n)
{
  /* This is the steady state at the end - the stead state value
   * at the start.
   */
  return data[n-1] - data[0];
}

/******************************************************************/

double integrate_above(double data[], int n, double dt)
{
  /* This is the area ABOVE the curve, from start to end
   */
  double hi = data[n];
  double total = 0.0;
  for (int i=1; i < n; i++)
  {
    total += (((hi - data[i]) + (hi - data[i-1])) / 2.0) * dt;
  }
  return total;
}

/******************************************************************/

double integrate_below(double data[], int n, double dt, double LplusT)
{
  /* This is the area BELOW the curve, from start to (L + T) */
  double lo = data[0];
  double total = 0.0;
  for (int i=1; i < n; i++)
  {
    total += (((data[i] - lo) + (data[i-1] - lo)) / 2.0) * dt;
    if ((i * dt) > LplusT)
    {
      break;
    }
  }
  return total;
}

/******************************************************************/

void calc_tune_coef(double data[], int n, double dt, tune_type_t tt)
{
  double LplusT;
  tune_data_t td;

  td.k = find_k(data, n);
  td.A0 = integrate_above(data, n, dt);
  LplusT = td.A0 / td.k;
  td.A1 = integrate_below(data, n, dt, LplusT);
  td.T = (td.A1 / td.k) * M_E;
  td.L = LplusT - td.T;
  td.a = td.L * (td.k / td.T);
  switch (tt)
  {
    case TUNE_PID:
      td.Kp = 1.2 / td.a;
      td.Ti = 2 * td.L;
      td.Td = td.L / 2.0;
      break;
    case TUNE_PI:
      td.Kp = 0.9 / td.a;
      td.Ti = 3 * td.L;
      td.Td = 0;
      break;
  }
  logfile->vprint("Kp = %lf, Ti = %lf, Td = %lf\n", td.Kp, td.Ti, td.Td);
  logfile->vprint("A0 = %lf, A1 = %lf, L = %lf, T = %lf, k = %lf, a = %lf\n",
		  td.A0, td.A1, td.L, td.T, td.k, td.a);
}

/******************************************************************/

void pid_point_t::auto_tune(double initial, double final, double time)
{
  tune = true;
  tune_delay = time;
  tune_initial_output = initial;
  tune_final_output = final;
  ao_point->send(initial);
  logfile->vprint("sending %lf\n", initial);
  tune_start_time = db->get_time();
  tune_total_samples = (int)(time * db->get_sample_rate()) + 1;
  tune_data = new double[tune_total_samples];
  tune_n_samples = 0;
  tune_phase_2 = false;
  MALLOC_CHECK(tune_data);
  logfile->vprint("Tune phase 1, %lf\n", initial);
  logfile->vprint("sample rate: %lf\n", db->get_sample_rate());
  logfile->vprint("Total Samples: %d, initial: %lf, final: %lf, time: %lf\n",
		  tune_total_samples, initial, final, time);
}

/******************************************************************/

void pid_point_t::tune_update(void)
{
  double elapsed_time = db->get_time() - tune_start_time;
  if (tune_n_samples >= tune_total_samples)
  {
    tune = false;
    logfile->vprint("Tune finished, calculating gains\n");
    calc_tune_coef(tune_data, tune_n_samples,
             1.0 / db->get_sample_rate(), TUNE_PID);
    const char *fname = "tuneout.txt";
    FILE *fp = fopen(fname, "w");
    logfile->vprint("Writing tune data to %s\n", fname);
    for (int i=0; i < tune_n_samples; i++)
    {
      fprintf(fp, "%d\t%lf\n", i, tune_data[i]);
    }
    fclose(fp);
    return;
  }
  else if (tune_phase_2)
  {
    tune_data[tune_n_samples] = ai_point->get_pv();
    tune_n_samples++;
  }
  else if (elapsed_time > tune_delay)
  {
    ao_point->send(tune_final_output);
    logfile->vprint("sending %lf\n", tune_final_output);
    tune_data[0] = ai_point->get_pv();
    tune_n_samples++;
    logfile->vprint("Tune phase 2, starting data collection, %lf\n", tune_final_output);
    tune_phase_2 = true;
  }
}

/******************************************************************/

void pid_point_t::update(void)
{
  /* Update the given control point. */

  if (this->tune)
  {
    tune_update();
    return;
  }
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

/********************************************************************/

pid_point_t **pid_point_t::read(int *cnt, const char *home_dir)
{
  /* Read the pid point from the database. */
  db_point_t *db_point;

  int max_points = 20;
  pid_point_t **pid_points =
	(pid_point_t **) malloc(max_points * sizeof(pid_point_t*));
  MALLOC_CHECK(pid_points);

  int count = 0;


  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/pid.dat", sizeof(path));
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    logfile->perror(path);
    logfile->vprint("Can't open %s\n", path);
    *cnt = 0;
    return NULL;
  }
  char line[300];

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[25];
    ltrim(line);
    rtrim(line);

    safe_strcpy(tmp, (const char*)line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '|', 25);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }
    else if (argc != 17)
    {
      logfile->vprint("%s: Wrong number of args, line %d\n", path, i+1);
      continue;
    }
    logfile->vprint("%s\n", line);
    pid_point_t *pid = new pid_point_t;

    safe_strcpy(pid->tag, (const char*) argv[0], sizeof(pid->tag));
    safe_strcpy(pid->description, (const char*) argv[1], sizeof(pid->description));
    safe_strcpy(pid->eu, (const char*) argv[2], sizeof(pid->eu));
    pid->decimal_places = atol(argv[3]);


    pid->setpoint_limit_hi = atof(argv[4]);

    pid->setpoint_limit_lo = atof(argv[5]);

    pid->p_gain = atof(argv[6]);
    pid->i_time = atof(argv[7]);
    pid->d_time = atof(argv[8]);
    char temp_tag[30];
    safe_strcpy(temp_tag, (const char*) argv[9], sizeof(temp_tag));
    rtrim(temp_tag);

    db_point = db->get_db_point(temp_tag);

    if (db_point == NULL)
    {
      logfile->vprint("%s - bad TAGNAME: %s\n", pid->tag, temp_tag);
      pid->ai_point = NULL;
    }
    else
    {
      pid->ai_point = dynamic_cast <analog_point_t *> (db_point);
      if (pid->ai_point == NULL)
      {
        logfile->vprint("%s - bad analog input point: %s\n", pid->tag, temp_tag);
      }
    }

    /**
    if ((db_point == NULL) || (db_point->point_type() != ANALOG_INPUT))
    {
      pid->ai_point = NULL;
      printf("%s - bad analog input point: %s\n", pid->tag, temp_tag);
    }
    else
    {
      pid->ai_point = (ai_point_t *) db_point;
    }
    ***/

    safe_strcpy(temp_tag, (const char*) argv[10], sizeof(temp_tag));
    rtrim(temp_tag);
    db_point = db->get_db_point(temp_tag);

    if (db_point == NULL)
    {
      logfile->vprint("%s - bad TAGNAME: %s\n", pid->tag, temp_tag);
      pid->ao_point = NULL;
    }
    else
    {
      pid->ao_point = dynamic_cast <ao_point_t *> (db_point);
      if (pid->ai_point == NULL)
      {
        logfile->vprint("%s - bad analog output point: %s\n", pid->tag, temp_tag);
      }
    }

    /********
    if ((db_point == NULL) || (db_point->point_type() != ANALOG_OUTPUT))
    {
      pid->ao_point = NULL;
      printf("%s - bad analog output point: %s\n", pid->tag, temp_tag);
    }
    else
    {
      pid->ao_point = (ao_point_t *) db_point;
    }
    ****/

    pid->deviation_alarm = atof(argv[11]);
    pid->deviation_caution = atof(argv[12]);
    pid->deviation_delay = atof(argv[13]);
    pid->dev_alarm_enable = (argv[14][0] == '1');
    pid->dev_alarm_shutdown = (argv[15][0] == '1');
    pid->dev_caution_enable = (argv[16][0] == '1');

    pid->set_format();
    pid->integrated_error = 0;
    pid->last_output = pid->last_input = pid->last_error = pid->last_time = 0;
    pid->pv = pid->setpoint = 22.0;
    pid->point_state = STATE_NORMAL;
    pid->dev_alm_detect_time = -1;
    pid->dev_caut_detect_time = -1;
    //pid->pv_attr = NORMAL_PV_ATTR;
    pid->control_enabled = false;
    pid->ramp_is_on = false;
    pid->updates_per_evaluation = 1;
    pid->delay_counter = pid->updates_per_evaluation - 1;
    pid->tune = false;
    pid->tune_data = NULL;
    pid_points[count] = pid;
    count++;
    if (count > max_points)
    {
      max_points += 10;
      pid_points = (pid_point_t **) realloc(pid_points,
	       max_points * sizeof(pid_point_t*));
      MALLOC_CHECK(pid_points);
    }
  }

  *cnt = count;
  return pid_points;
}

/******************************************************************/

