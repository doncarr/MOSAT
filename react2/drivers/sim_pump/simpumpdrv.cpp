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

#include "simpumpdrv.h"

/***********************************************************************

class simpumpdrv_factory_t : public io_driver_factory_t
{
private:
  react_drv_base_t *drvb;
  
public:
  simpumpdrv_factory_t(react_drv_base_t *r) {drvb = r;};
  io_driver_t *new_driver(const char *config_data, char *err, int esz)
     {return new simpumpdrv_t(drvb);};
  const char *long_description(void) {return "PumpSimulator";};
  const char *short_discription(void) {return "pumpSim";}; 
  const char *abbreviation(void) {return "PSIM";};
  ~simpumpdrv_factory_t(void) {};
};


***********************************************************************/
/*

extern "C" io_driver_factory_t *new_simpumpdrv_factory_1_0(react_drv_base_t *r, const char *option)
{
  return new simpumpdrv_factory_t(r);
}

***********************************************************************/

double add_rand_noise(double val, double full_scale, double fraction_noise)
{
  double rand_mult = (((double) random() / (double) RAND_MAX) 
        * 2.0 * fraction_noise) - fraction_noise;
  return val + (full_scale * rand_mult);
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

  di_data[LO_LEVEL_CH] = false;
  di_data[HI_LEVEL_CH] = true;

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
  di_data[aChannel] = aVal;
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
  if (!filling) // reached top, pumps on;
  {
    di_data[HI_LEVEL_CH] = true;
    di_data[LO_LEVEL_CH] = true;
    double level;
    if (now > (start + 5))
    {
      level = 325 - ((325 - 65) * ((now - start - 5.0) / (PUMP_ON_TIME - 5.0)));
    }
    else
    {
      level = 325;
    }
    level = add_rand_noise(level, 500, 0.02);
    ai_data[LEVEL_CH] = 32000 - ((32000 - 6400) * (level/500.0));
    if (now > (start + 1.0))
    {
      di_data[P1_DI_CH] = false;
      ai_data[P1_AI_CH] = 20480.0;
      ai_data[P1_AI_CH] = add_rand_noise(ai_data[P1_AI_CH], 25000, 0.015);
    }
    if (now > (start + 5.0))
    {
      di_data[P2_DI_CH] = false;
      ai_data[P2_AI_CH] = 20480.0;
      ai_data[P2_AI_CH] = add_rand_noise(ai_data[P2_AI_CH], 25000, 0.015);
    }
    if (now > (start + 9.0))
    {
      di_data[P3_DI_CH] = false;
      ai_data[P3_AI_CH] = 20480.0;
      ai_data[P3_AI_CH] = add_rand_noise(ai_data[P3_AI_CH], 25000, 0.015);
    }
    if (now > (start + 7.0))
    {
      di_data[HI_LEVEL_CH] = false;
    }
    if (now > (start + PUMP_ON_TIME))
    {
      filling = true;
      start = now;
    }
  }
  else // reached the bottom, pumps off.
  {
    di_data[LO_LEVEL_CH] = false;
    di_data[HI_LEVEL_CH] = false;
    double level;
    if (now > (start + 5))
    {
      level = 65 + ((325 - 65) * ((now - start - 5.0) / (PUMP_OFF_TIME - 5.0)));
    }
    else
    {
      level = 65;
    }
    level = add_rand_noise(level, 500, 0.02);
    ai_data[LEVEL_CH] = 32000 - ((32000 - 6400) * (level/500.0));

    if (now > (start + 1.0))
    {
      di_data[P1_DI_CH] = true;
      ai_data[P1_AI_CH] = 6433.0;
      ai_data[P1_AI_CH] = add_rand_noise(ai_data[P1_AI_CH], 25000, 0.015);
    }
    if (now > (start + 5.0))
    {
      di_data[P2_DI_CH] = true;
      ai_data[P2_AI_CH] = 6422.0;
      ai_data[P2_AI_CH] = add_rand_noise(ai_data[P2_AI_CH], 25000, 0.015);
    }
    if (now > (start + 9.0))
    {
      di_data[P3_DI_CH] = true;
      ai_data[P3_AI_CH] = 6445.0;
      ai_data[P3_AI_CH] = add_rand_noise(ai_data[P3_AI_CH], 25000, 0.015);
    }
    if (now > (start + 8.0))
    {
      di_data[LO_LEVEL_CH] = true;
    }
    if (now > (start + PUMP_OFF_TIME))
    {
      filling = false;
      start = now;
    }
  }

}

/***********************************************************************/
