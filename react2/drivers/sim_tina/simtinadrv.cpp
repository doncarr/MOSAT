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

simtinadrv.cpp

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

/***********************************************************************/

#define COLD_VALVE_DO_CH 8
#define DRAIN_VALVE_DO_CH 9

#define LO_LEVEL_CH 8
#define HI_LEVEL_CH 9

#define TEMP1_CH 0
#define TEMP2_CH 1
#define LEVEL_CH 2


#define FILL_TIME (35.0)
#define DRAIN_TIME (20.0)

#define PUMP_ON_TIME (30.0)
#define PUMP_OFF_TIME (35.0)


#define LO_SWITCH_CM (33.0)
#define HI_SWITCH_CM (38.0)

#define SWITCH_DEADBAND (1.0)


class simtinadrv_t : public io_driver_t
{
public:
  FILE *fp;
  FILE *fpout;
  bool di_data[64];
  bool do_data[64];
  double ao_data[32];
  double ai_data[32];
  react_drv_base_t *react;


  bool filling;
  double start;

  double fill_rate;
  double drain_rate;
  double water_temp;

  double actual_fill_rate;

  bool cooling;
  bool draining;

  double last_time;
  double the_level;

  simtinadrv_t(react_drv_base_t *r);
  void read(void);
  bool get_di(int channel);
  double get_ai(int channel);
  void send_do(int channel, bool val);
  void send_ao(int channel, double val);
  void close(void);
};


/**********************************************************************/

extern "C" io_driver_t *new_simtinadrv(react_drv_base_t *r, const char *option)
{
  return new simtinadrv_t(r);
}

/***********************************************************************/


simtinadrv_t::simtinadrv_t(react_drv_base_t *r)
{
  //printf("Read: ");
  printf("Creating sim tina driver . . . .\n");
  driver_name = "simtinadrv";
  driver = 1;
  card = 1;
  base_addr = 0;
  fp = NULL;
  react = r;

  memset(di_data, '\0', sizeof(di_data));
  memset(do_data, '\0', sizeof(do_data));
  memset(ai_data, '\0', sizeof(ai_data));
  memset(ao_data, '\0', sizeof(ao_data));

  start = react->get_time();

  di_data[LO_LEVEL_CH] = true;
  di_data[HI_LEVEL_CH] = false;

  fill_rate = (HI_SWITCH_CM - LO_SWITCH_CM) / FILL_TIME;
  drain_rate = (HI_SWITCH_CM - LO_SWITCH_CM) / DRAIN_TIME;
  water_temp = 50.0;

   // Start the level at the middle.
  the_level = (HI_SWITCH_CM + LO_SWITCH_CM) / 2.0; 

  ai_data[TEMP1_CH] = water_temp;
  ai_data[TEMP2_CH] = water_temp;
  ai_data[LEVEL_CH] = the_level;
}

/***********************************************************************/

void simtinadrv_t::send_ao(int aChannel, double aVal)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return;
  }
  //printf("New AO[%d] = %f\n", aChannel, aVal);
  ao_data[aChannel] = aVal;
  //ai_data[aChannel] = aVal;
}

/***********************************************************************/

void simtinadrv_t::send_do(int aChannel, bool aVal)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return;
  }
  //di_data[aChannel] = aVal;
  do_data[aChannel] = aVal;
}


/***********************************************************************/

double simtinadrv_t::get_ai(int aChannel)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return 0.0;
  }
  return ai_data[aChannel];
}

/***********************************************************************/

void simtinadrv_t::close(void)
{
}

/***********************************************************************/

bool simtinadrv_t::get_di(int aChannel)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return false;
  }
  return di_data[aChannel];
}

/***********************************************************************/

void simtinadrv_t::read(void)
{
  double now = react->get_time();
  double elapsed_time = now - last_time;
  last_time = now;
  if (elapsed_time < 0.05) return;

  filling = do_data[COLD_VALVE_DO_CH]; 
  draining = do_data[DRAIN_VALVE_DO_CH];

  actual_fill_rate = 0.0;
  
  if (filling)
  {
    actual_fill_rate += fill_rate;
    water_temp -= elapsed_time * 0.15;
  }
  else
  {
    water_temp += elapsed_time * 0.1;
  }

  if (draining)
  {
    actual_fill_rate -= drain_rate;
  }

  the_level += (actual_fill_rate * elapsed_time);
  //the_level = add_rand_noise(the_level, 38, 0.011);

  //di_data[P1_DI_CH] = do_data[P1_DO_CH];

 if (di_data[HI_LEVEL_CH]) // Set the very high switch acording to the level
  {
    if ( the_level < (HI_SWITCH_CM - SWITCH_DEADBAND))
    {
      di_data[HI_LEVEL_CH] = false;
    }
  }
  else
  {
    if ( the_level > HI_SWITCH_CM)
    {
      di_data[HI_LEVEL_CH] = true;
    }
  }

  if (di_data[LO_LEVEL_CH]) // Set the low switch acording to the level
  {
    if ( the_level < (LO_SWITCH_CM - SWITCH_DEADBAND))
    {
      di_data[LO_LEVEL_CH] = false;
    }
  }
  else
  {
    if ( the_level > LO_SWITCH_CM)
    {
      di_data[LO_LEVEL_CH] = true;
    }
  }


  ai_data[TEMP1_CH] = water_temp;
  //ai_data[TEMP1_CH] = add_rand_noise(ai_data[TEMP1_CH], 25000, 0.015);
  ai_data[TEMP2_CH] = ai_data[TEMP1_CH];

  ai_data[LEVEL_CH] = the_level;

  printf("------- TEMP: %0.1lf (%d,%d), LEVEL: %0.1lf (%d)\n", water_temp, TEMP1_CH, TEMP2_CH, the_level, LEVEL_CH);

  /********/

}

/***********************************************************************/
