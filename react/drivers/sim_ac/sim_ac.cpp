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


/************************************************************************

Iodriver.c

Contains code for input/output drivers.

*************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> // for open(...)
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>

#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "iodriver.h"


#include "random_tools.h"
#include "exp_sim.h"
#include "sim_ac.h"

/**********************************************************************/

static double calc_level_raw(double level_eu)
{
  //double pct = level_eu / 500.0;
  return 32000.0 - ((32000.0 - 6400.0) * (level_eu / 500.0));
}

/**********************************************************************/

extern "C" io_driver_t *new_sim_ac_drv(react_drv_base_t *r, const char *option)
{
  return new sim_ac_drv_t(r);
}

/***********************************************************************/


sim_ac_drv_t::sim_ac_drv_t(react_drv_base_t *r)
{
  //printf("Read: ");
  printf("Creating sim A/C driver . . . .\n");
  driver_name = "sim_ac_drv";
  driver = 1;
  card = 1;
  base_addr = 0;
  fp = NULL;
  react = r;

  memset(di_data, '\0', sizeof(di_data));
  memset(do_data, '\0', sizeof(do_data));
  memset(ai_data, '\0', sizeof(ai_data));
  memset(ao_data, '\0', sizeof(ao_data));

  double now = react->get_time();
  last_time = now;

  ac_sim[0] = new ac_unit_sim_t(64, 0.35, 22, now);
  ac_sim[1] = new ac_unit_sim_t(55, 0.38, 42, now);
  ac_sim[2] = new ac_unit_sim_t(88, 0.31, 55, now);
  ac_sim[3] = new ac_unit_sim_t(99, 0.25, 52, now);
  ac_sim[4] = new ac_unit_sim_t(45, 0.33, 12, now);
  ac_sim[5] = new ac_unit_sim_t(44, 0.22, 19, now);
  ac_sim[6] = new ac_unit_sim_t(77, 0.44, 48, now);
  ac_sim[7] = new ac_unit_sim_t(88, 0.39, 39, now);
  ac_sim[8] = new ac_unit_sim_t(121, 0.41, 122, now);
  ac_sim[9] = new ac_unit_sim_t(77, 0.28, 155, now);
  ac_sim[10] = new ac_unit_sim_t(92, 0.26, 66, now);
  ac_sim[11] = new ac_unit_sim_t(81, 0.28, 60, now);
}

/***********************************************************************/

void sim_ac_drv_t::send_ao(int aChannel, double aVal)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return;
  }
  //printf("New AO[%d] = %f\n", aChannel, aVal);
  ao_data[aChannel] = aVal;
  ai_data[aChannel] = aVal;
}

/***********************************************************************/

void sim_ac_drv_t::send_do(int aChannel, bool aVal)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return;
  }
  //di_data[aChannel] = aVal;
  do_data[aChannel] = aVal;
  if (aChannel == 30)
  {
     if (aVal)
     {
       printf("***************** Creating performance error on ac unit 1\n");
       ac_sim[0]-> set_steady_state_hot(35.3);
       ac_sim[0]-> set_steady_state_cold(28.1);
     } 
     else
     {
       printf("-- Canceling performance error\n");
       ac_sim[0]-> set_steady_state_hot(55.8);
       ac_sim[0]-> set_steady_state_cold(4.5);
     }
  }
  else if (aChannel == 31)
  {
     if (aVal)
     {
       printf("############## Creating too cold error on ac unit 1\n");
       ac_sim[0]-> set_steady_state_cold(-8.0);
     } 
     else
     {
       printf("-- Canceling cold error\n");
       ac_sim[0]-> set_steady_state_cold(4.5);
     }
  }
}


/***********************************************************************/

double sim_ac_drv_t::get_ai(int aChannel)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return 0.0;
  }
  return ai_data[aChannel];
}

/***********************************************************************/

void sim_ac_drv_t::close(void)
{
}

/***********************************************************************/

bool sim_ac_drv_t::get_di(int aChannel)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return false;
  }
  return di_data[aChannel];
}

/***********************************************************************/

void sim_ac_drv_t::read(void)
{
  double now = react->get_time();
  double elapsed_time = now - last_time;
  last_time = now;

  if (elapsed_time < 0.05) return;

  //printf("------ %0.1lf\n", now);
  for (int i=0; i < 12; i++)
  {
    di_data[i] = ac_sim[i]->update(now, &ai_data[i], &ai_data[i+12]);
  }
}

/***********************************************************************/
