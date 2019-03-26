/************************************************************************
This software is part of React, a control engine
Copyright (C) 2012 Donald Wayne Carr 

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


/************************************************************************

Iodriver.c

Contains code for input/output drivers.

*************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>

#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "iodriver.h"

class spraydrv_t : public io_driver_t
{
public:
  FILE *fp;
  FILE *fpout;
  bool di_data[64];
  bool do_data[64];
  double ao_data[32];
  double ai_data[32];
  react_drv_base_t *react;
  double last_time;

  spraydrv_t(react_drv_base_t *r);
  void read(void);
  bool get_di(int channel);
  double get_ai(int channel);
  void send_do(int channel, bool val);
  void send_ao(int channel, double val);
  void close(void);
};


/***********************************************************************/

extern "C" io_driver_t *new_spray(react_drv_base_t *r, const char *option)
{
  return new spraydrv_t(r);
}

/***********************************************************************/


spraydrv_t::spraydrv_t(react_drv_base_t *r)
{
  //printf("Read: ");
  printf("Creating  spraydrv_t . . . .\n");
  const char *fname = "didata.txt";
  driver_name = "File";
  driver = 1;
  card = 1;
  base_addr = 0;
  fp = NULL;
  react = r;

  memset(di_data, '\0', sizeof(di_data));
  memset(do_data, '\0', sizeof(do_data));
  memset(ai_data, '\0', sizeof(ai_data));
  memset(ao_data, '\0', sizeof(ao_data));
  last_time = react->get_time();
}

/***********************************************************************/

void spraydrv_t::send_ao(int aChannel, double aVal)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return;
  }
  //printf("New AO[%d] = %f\n", aChannel, aVal);
  ao_data[aChannel] = aVal;
}

/***********************************************************************/

void spraydrv_t::send_do(int aChannel, bool aVal)
{
  /* Use DOs to change DIs */
  if ((aChannel <=8) && aVal && !do_data[aChannel])
  {
  }
  di_data[aChannel] = aVal;
  do_data[aChannel] = aVal;
}

/***********************************************************************/

double spraydrv_t::get_ai(int aChannel)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return 0.0;
  }
  return ai_data[aChannel];
}

/***********************************************************************/

void spraydrv_t::close(void)
{
}

/***********************************************************************/

bool spraydrv_t::get_di(int channel)
{
  if ((channel >= 0) && (channel < 64))
  {
    return di_data[channel];
  }
  else
  {
    return false;
  }
}

/***********************************************************************/

double next_pressure(double tau, double steady_state_value,
                     double last_value, double delta_t)
{
  double fraction = 1.0 - exp(-delta_t/tau);
  return last_value + ((steady_state_value - last_value) * fraction);
}

/***********************************************************************/

void spraydrv_t::read(void)
{
  bool pump_on = di_data[0];
  bool pv_25_75 = di_data[1];
//  double pvalve_proportion = pv_25_75 ? 0.75 : 0.25; 
  double pvalve_proportion = ao_data[1]; 
  double last_pressure = ai_data[1]; 
  double now = react->get_time();
  double delta_t = now - last_time; 
  double pressure;
  if (pump_on)
  {
    double steady_state = 162.5 * pvalve_proportion;
    pressure = next_pressure(1.0, steady_state, last_pressure, delta_t); 
  }
  else
  {
    pressure = next_pressure(0.5, 0.0, last_pressure, delta_t); 
  }
  ai_data[0] = pvalve_proportion; 
  ai_data[1] = pressure; 
  last_time = now;
}

/*************************************************************e**********/
