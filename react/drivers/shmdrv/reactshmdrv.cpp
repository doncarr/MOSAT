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

reactshmdrv.cpp

Contains code for shared memory driver.

*************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>

#include "reactpoint.h"
#include "reactshmio.h"
#include "db_point.h"
#include "db.h"
#include "iodriver.h"

#include "reactshmdrv.h"

/***********************************************************************/

extern "C" io_driver_t *new_reactshm(react_drv_base_t *r, const char *option)
{
  printf("Creating new reactshm iodriver\n");
  return new reactshm_driver_t(r);
}


/***********************************************************************/

reactshm_driver_t::reactshm_driver_t(react_drv_base_t *react)
{
  driver_name = "reactshm";
  shm = reactshmio_attach();
  if (shm == NULL)
  {
    printf("Error attaching to sharred memory\n");
  }
  printf("reactshm initialized\n");
}

/***********************************************************************/

void reactshm_driver_t::send_ao(int ch, double val)
{
  if ((ch >= 0) && (ch < 64))
  {
    shm->ao_val[ch] = val;
  }
  //printf("Send AO ch = %d, val = %lf\n", ch, val);
}

/***********************************************************************/

void reactshm_driver_t::close(void)
{
}

/***********************************************************************/

void reactshm_driver_t::send_do(int ch, bool val)
{
  if ((ch >= 0) && (ch < 64))
  {
    shm->do_val[ch] = val;
  }
}

/***********************************************************************/

double reactshm_driver_t::get_ai(int channel)
{
  if ((channel >= 0) && (channel < 64))
  {
    return shm->ai_val[channel];
  }
  else
  {
    return 0.0;
  }
}

/***********************************************************************/

bool reactshm_driver_t::get_di(int channel)
{
  if ((channel >= 0) && (channel < 64))
  {
    return shm->di_val[channel];
  }
  else
  {
    return false;
  }
}

/***********************************************************************/

long reactshm_driver_t::get_count(int channel)
{
  if ((channel >= 0) && (channel < 64))
  {
    return shm->count_val[channel];
  }
  else
  {
    return 0;
  }
}

/***********************************************************************/

void reactshm_driver_t::read(void)
{
}

/***********************************************************************/

