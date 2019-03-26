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

dallas_1wire.cpp

Contains code for react Dallas 1Wire driver.

*************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>

#include <semaphore.h>
#include <pthread.h>

#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "iodriver.h"

#include "dallas_1wire.h"

/****
  Ok, this is a first stab at a Dallas 1Wire driver. 

  It assumes a serial port that is connected to a serial to Dallas 1Wire chip 
 
  There are a number of issues, such as identifying all of the devices
  on the Dallas 1-Wire bus, and them assigning each one of them to a 
  channel. To start, we will have a configuration file that reads
  a configuration file that has all of the serial numbers of the
  devices on the bus, one serial number per line. With this, we will 
  just assign the channel number starting with zero. 

  This works for the temperature device we will start with, but,
  for other devices, there are multiple channels per device, such
  as the 4 channel analog input device. There could be devices with 
  a mix if analog I/O and discrete I/O

  Furthermore, we might have to have special code to write to extract
  the values from each device type. 
****/

/***********************************************************************/

static void *rt_dallas_1wire_start_read_thread(void *driver_ptr)
{
  dallas_1wire_driver_t *p = (dallas_1wire_driver_t *)driver_ptr;
  p->read_thread();
  return NULL; // Should never return, but, this takes away the warning.
}

extern "C" io_driver_t *new_dallas_1wire(react_drv_base_t *r, const char *option)
{
  printf("Creating new dallas_1wire iodriver\n");
  return new dallas_1wire_driver_t(r);
}

/***********************************************************************/

dallas_1wire_driver_t::dallas_1wire_driver_t(react_drv_base_t *react)
{
  char err[100];

  driver_name = "dallas_1wire";
  di_offset = 0;
  do_offset = 0;
  ai_offset = 0;
  ao_offset = 0;

  read_values = false;
  wake_him_up = true;

  printf("Initializing Dallas 1Wire\n");
  if (-1 == init_dallas_wire_temperatures(err, sizeof(err)))
  {
    printf("Error initializing the Dallas 1-Wire interface: %s\n", err);
  }
  printf("DONE initializing Dallas 1Wire\n");

  printf("Initializing semaphores\n");
  if (0 != sem_init(&read_mutex_sem, 0, 1))
  {
    perror("sem_init");
  }
  if (0 != sem_init(&read_wait_sem, 0, 0))
  {
    perror("sem_init");
  }
  printf("DONE Initializing semaphores\n");
  
  printf("Creating thread\n");
  pthread_t thr;
  int retval;
  retval = pthread_create(&thr, NULL, rt_dallas_1wire_start_read_thread, this);
  if (retval != 0)
  {
    perror("can't create thread");
    exit(0);
  }
  retval = pthread_detach(thr);
  if (retval != 0)
  {
    perror("can't detach thread");
    exit(0);
  }
  printf("DONE Creating thread\n");

}

/***********************************************************************/

void dallas_1wire_driver_t::send_ao(int ch, double val)
{
  if ((ch >= 0) && (ch < 64))
  {
  }
}

/***********************************************************************/

void dallas_1wire_driver_t::close(void)
{
}

/***********************************************************************/

void dallas_1wire_driver_t::send_do(int ch, bool val)
{
  if ((ch >= 0) && (ch < 64))
  {
  }
}

/***********************************************************************/

double dallas_1wire_driver_t::get_ai(int channel)
{
  double read_val;
  if ((channel >= 0) && (channel < 64))
  {
    read_val = (double) ai_vals[channel];
    printf("AI %d, read %lf\n", channel, read_val);
    return read_val;
  }
  else
  {
    return 0.0;
  }
}

/***********************************************************************/

bool dallas_1wire_driver_t::get_di(int channel)
{
  bool read_val;
  if ((channel >= 0) && (channel < 64))
  {
    read_val = di_vals[channel];
    printf("DI %d, read %s\n", channel, read_val ? "T" : "F");
    return read_val;
  }
  else
  {
    return false;
  }
}

/***********************************************************************/

long dallas_1wire_driver_t::get_count(int channel)
{
  if ((channel >= 0) && (channel < 64))
  {
    return 0;
  }
  else
  {
    return 0;
  }
}

/***********************************************************************/
static int mycounter = 0;

void dallas_1wire_driver_t::read_thread(void)
{
  char err[100];
  while(true)
  {
    printf("read thread reading Dallas 1Wire values . . .\n");
    read_dallas_wire_temperatures(tmp_ai_vals, 16, err, sizeof(err));
    printf("read thread DONE reading Dallas 1Wire values . . .\n");

    printf("read thread copying values . . .\n");
    sem_wait(&read_mutex_sem);
      printf("  -- read thread in critical section . . .\n");
      memcpy(ai_vals, tmp_ai_vals, sizeof(ai_vals));
      memcpy(di_vals, tmp_di_vals, sizeof(di_vals));
      read_values = true;
      printf("  -- read thread leaving critical section . . .\n");
    sem_post(&read_mutex_sem);
    printf("read thread DONE copying values . . .\n");

    printf("read thread waiting %d\n", mycounter++);
    sem_wait(&read_wait_sem);
    printf("thread woke up\n");
  }
}

/***********************************************************************/

void dallas_1wire_driver_t::read(void)
{
    sem_wait(&read_mutex_sem);
    printf("  -- main thread in critical section . . .\n");
    if (read_values)
    {
      wake_him_up = true;
    }
    else
    {
      // Either the reader is very slow or the link is down
      wake_him_up = false;
    }
}

/***********************************************************************/

void dallas_1wire_driver_t::end_read(void)
{
  read_values = false;
  printf("  -- main thread leaving critical section . . .\n");
  sem_post(&read_mutex_sem);
  if (wake_him_up)
  {
    printf("waking up reader\n");
    sem_post(&read_wait_sem); // wake up the read thread to read the next values.
    printf("DONE waking up reader\n");
  }
  else
  {
    printf("The read thread is hosed, no reason to keep incrementing . . .\n");
  } 
}

/***********************************************************************/
