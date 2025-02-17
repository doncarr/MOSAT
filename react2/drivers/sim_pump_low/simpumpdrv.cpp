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
#include <fcntl.h> 
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

/***/

#define LO_LEVEL_CH 0
#define HI_LEVEL_CH 1
#define HI_HI_LEVEL_CH 2

#define P1V_DO_CH 4
#define P2V_DO_CH 5

#define P1_DO_CH 1
#define P2_DO_CH 2
#define P3_DO_CH 3 

#define P1V_DI_CH 6
#define P2V_DI_CH 7

#define P1_DI_CH 8
#define P2_DI_CH 9
#define P3_DI_CH 10 

#define MIXER_DI_CH 5 

#define P1_AI_CH 5
#define P2_AI_CH 4
#define P3_AI_CH 3

#define LEVEL_CH 2

#define PUMP_ON_TIME (30.0)
#define PUMP_OFF_TIME (35.0)
#define LO_SWITCH_CM (100.0)
#define HI_SWITCH_CM (275.0)
#define HI_HI_SWITCH_CM (450.0)

#define SWITCH_DEADBAND (12.0)

class simpumpdrv_t : public io_driver_t
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
  double pump_rate;
  double last_time;
  double the_level;

  simpumpdrv_t(react_drv_base_t *r);
  void read(void);
  bool get_di(int channel);
  double get_ai(int channel);
  void send_do(int channel, bool val);
  void send_ao(int channel, double val);
  void close(void);
};

/********/

/**********************************************************************/

static double calc_level_raw(double level_eu)
{
  //double pct = level_eu / 500.0;
  return 32000.0 - ((32000.0 - 6400.0) * (level_eu / 500.0));
}

/**********************************************************************/

extern "C" io_driver_t *new_simpumpdrv(react_drv_base_t *r, const char *option)
{
  return new simpumpdrv_t(r);
}

/***********************************************************************/


simpumpdrv_t::simpumpdrv_t(react_drv_base_t *r)
{
  //printf("Read: ");
  printf("Creating sim pump driver . . . .\n");
  driver_name = "simpumpdrv";
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
  filling = true;

  di_data[P1V_DI_CH] = true;
  di_data[P2V_DI_CH] = true;

  di_data[P1_DI_CH] = true;
  ai_data[P1_AI_CH] = 6400.0;

  di_data[P2_DI_CH] = true;
  ai_data[P2_AI_CH] = 6400.0;

  di_data[P3_DI_CH] = true;
  ai_data[P3_AI_CH] = 6400.0;

  di_data[LO_LEVEL_CH] = true;
  di_data[HI_LEVEL_CH] = false;

  di_data[MIXER_DI_CH] = true;

  fill_rate = (HI_SWITCH_CM - LO_SWITCH_CM) / PUMP_OFF_TIME; 
  pump_rate = (HI_SWITCH_CM - LO_SWITCH_CM) / PUMP_ON_TIME; 
  pump_rate += fill_rate; // fills while pumping 
  pump_rate /= 3.0; // 3 pumps

   // Start the level at the middle.
  the_level = (HI_SWITCH_CM + LO_SWITCH_CM) / 2.0; 
  ai_data[LEVEL_CH] = calc_level_raw(add_rand_noise(the_level, 500, 0.015));

  printf("fill rate: %lf, pump_rate %lf, level %lf\n", 
       fill_rate, pump_rate,  the_level);
  do_data[P1V_DO_CH] = false;
  do_data[P2V_DO_CH] = false;

  do_data[P1_DO_CH] = false;
  do_data[P2_DO_CH] = false;
  do_data[P3_DO_CH] = false;
}

/***********************************************************************/

void simpumpdrv_t::send_ao(int aChannel, double aVal)
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

void simpumpdrv_t::send_do(int aChannel, bool aVal)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return;
  }
  //di_data[aChannel] = aVal;
  do_data[aChannel] = aVal;
}


/***********************************************************************/

double simpumpdrv_t::get_ai(int aChannel)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return 0.0;
  }
  return ai_data[aChannel];
}

/***********************************************************************/

void simpumpdrv_t::close(void)
{
}

/***********************************************************************/

bool simpumpdrv_t::get_di(int aChannel)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return false;
  }
  return di_data[aChannel];
}

/***********************************************************************/

void simpumpdrv_t::read(void)
{
  double now = react->get_time();
  double elapsed_time = now - last_time;
  last_time = now;
  if (elapsed_time < 0.05) return;

  int n_pumps = 0;
  if (do_data[P1_DO_CH]) n_pumps++;
  if (do_data[P2_DO_CH]) n_pumps++;
  if (do_data[P3_DO_CH]) n_pumps++;

  // outputs are positive logic, inputs negative logic
  di_data[P1_DI_CH] = ! do_data[P1_DO_CH];
  di_data[P2_DI_CH] = ! do_data[P2_DO_CH];
  di_data[P3_DI_CH] = ! do_data[P3_DO_CH];

  di_data[P1V_DI_CH] = ! do_data[P1V_DO_CH];
  di_data[P2V_DI_CH] = ! do_data[P2V_DO_CH];

  double level_change_rate = fill_rate - (double(n_pumps) * fill_rate); 
  double level_change = level_change_rate * elapsed_time;

  the_level += level_change; 
  the_level = add_rand_noise(the_level, 500, 0.005);

  ai_data[LEVEL_CH] = calc_level_raw(the_level);

  //printf("%d pumps, lchange %lf, level %lf\n", 
  //      n_pumps, level_change,  the_level); 

  if (di_data[HI_LEVEL_CH]) // Set the high switch acording to the level
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

  if (di_data[HI_HI_LEVEL_CH]) // Set the very high switch acording to the level
  {
    if ( the_level < (HI_HI_SWITCH_CM - SWITCH_DEADBAND)) 
    {
      di_data[HI_HI_LEVEL_CH] = false;
    }
  }
  else
  {
    if ( the_level > HI_HI_SWITCH_CM) 
    {
      di_data[HI_HI_LEVEL_CH] = true;
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

  if (do_data[P1_DO_CH])
  {
    ai_data[P1_AI_CH] = 20480.0;
    ai_data[P1_AI_CH] = add_rand_noise(ai_data[P1_AI_CH], 25000, 0.015);
  }
  else
  {
    ai_data[P1_AI_CH] = 6455.0;
    ai_data[P1_AI_CH] = add_rand_noise(ai_data[P1_AI_CH], 25000, 0.015);
  }

  if (do_data[P2_DO_CH])
  {
    ai_data[P2_AI_CH] = 20492.0;
    ai_data[P2_AI_CH] = add_rand_noise(ai_data[P2_AI_CH], 25000, 0.015);
  }
  else
  {
    ai_data[P2_AI_CH] = 6465.0;
    ai_data[P2_AI_CH] = add_rand_noise(ai_data[P2_AI_CH], 25000, 0.015);
  }

  if (do_data[P3_DO_CH])
  {
    ai_data[P3_AI_CH] = 20451.0;
    ai_data[P3_AI_CH] = add_rand_noise(ai_data[P3_AI_CH], 25000, 0.015);
  }
  else
  {
    ai_data[P3_AI_CH] = 6445.0;
    ai_data[P3_AI_CH] = add_rand_noise(ai_data[P3_AI_CH], 25000, 0.015);
  }

}

/***********************************************************************/
