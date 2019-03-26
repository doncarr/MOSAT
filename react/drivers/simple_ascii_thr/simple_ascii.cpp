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

simple_ascii.cpp

Contains code for react simple ascii driver.

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

#include "simple_ascii.h"

/****
  Ok, this is the simple ascii driver. 

  All communication is ascii. 
  
  Implementes 'R' and 'W'

  'R' will return the following type of message:

4
+12345.123
-12345.123
+12345.123
-12345.123
101010101010
010001000101

The first line, is always the number of analog intputs (N).

Next, are N lines of analog inputs, EXACTLY 10 characters per line,
	formated how you like, but must be valid floating point.

Next, one line of exactly 12 DI

Next, one line of exactly 12 DO

----

  'W' has the following format:

WNNV

where: NN is two digits for channel, with leading zero if less than 10.
        V is the value to send, and MUST be either '1', or '0' 

Example:

W081

This will turn on output number 8.

You just return two characters on error, either OK to indicate
everything was ok, or exactly two digits error code.

****/

/***********************************************************************/

static void *rt_simple_ascii_start_read_thread(void *driver_ptr)
{
  simple_ascii_driver_t *p = (simple_ascii_driver_t *)driver_ptr;
  p->read_thread();
  return NULL; // Should never return, but, this takes away the warning.
}

extern "C" io_driver_t *new_simple_ascii(react_drv_base_t *r, const char *option)
{
  printf("Creating new simple_ascii iodriver\n");
  return new simple_ascii_driver_t(r, option);
}

/***********************************************************************/

simple_ascii_driver_t::simple_ascii_driver_t(react_drv_base_t *react, 
                           const char *device)
{
  char err[100];

  driver_name = "simple_ascii";
  di_offset = 0;
  do_offset = 0;
  ai_offset = 0;
  ao_offset = 0;

  read_values = false;
  wake_him_up = true;

  printf("Initializing simple ascii\n");
  serial_fd = init_simple_ascii(device, err, sizeof(err));
  if (-1 == serial_fd)
  {
    printf("Error initializing the simple ascii interface: %s\n", err);
  }
  printf("DONE initializing simple ascii\n");

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
  retval = pthread_create(&thr, NULL, rt_simple_ascii_start_read_thread, this);
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

void simple_ascii_driver_t::send_ao(int ch, double val)
{
  if ((ch >= 0) && (ch < 32))
  {
    char error[50];
    send_simple_ascii(serial_fd, ch, val,
           error, sizeof(error));

  }
}

/***********************************************************************/

void simple_ascii_driver_t::close(void)
{
}

/***********************************************************************/

void simple_ascii_driver_t::send_do(int ch, bool val)
{
  if ((ch >= 0) && (ch < 64))
  {
  }
}

/***********************************************************************/

double simple_ascii_driver_t::get_ai(int channel)
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

bool simple_ascii_driver_t::get_di(int channel)
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

long simple_ascii_driver_t::get_count(int channel)
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

void simple_ascii_driver_t::read_thread(void)
{
  char err[100];
  while(true)
  {
    printf("read thread reading simple ascii values . . .\n");
    read_simple_ascii(serial_fd, 
             tmp_ai_vals, 16, 
             tmp_di_vals, 16, 
             tmp_do_vals, 16, 
             err, sizeof(err));
    printf("read thread simple ascii DONE reading simple ascii values . . .\n");

    printf("read thread simple ascii copying values . . .\n");
    sem_wait(&read_mutex_sem);
      printf("  -- read thread simple ascii in critical section . . .\n");
      memcpy(ai_vals, tmp_ai_vals, sizeof(ai_vals));
      memcpy(di_vals, tmp_di_vals, sizeof(di_vals));
      read_values = true;
      printf("  -- read thread simple ascii leaving critical section . . .\n");
    sem_post(&read_mutex_sem);
    printf("read thread simple ascii DONE copying values . . .\n");

    printf("read thread simple ascii waiting %d\n", mycounter++);
    sem_wait(&read_wait_sem);
    printf("thread woke up\n");
  }
}

/***********************************************************************/

void simple_ascii_driver_t::read(void)
{
    time_t tstart = time(NULL);
    sem_wait(&read_mutex_sem);
    time_t tend = time(NULL);
    printf("  -- simple ascii main thread in critical section %ld. . .\n", tend - tstart);
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

void simple_ascii_driver_t::end_read(void)
{
  read_values = false;
  printf("  -- simple ascii main thread leaving critical section . . .\n");
  sem_post(&read_mutex_sem);
  if (wake_him_up)
  {
    printf("simple ascii waking up reader\n");
    sem_post(&read_wait_sem); // wake up the read thread to read the next values.
    printf("simple ascii DONE waking up reader\n");
  }
  else
  {
    printf("Simple Ascii read thread is hosed, no reason to keep incrementing . . .\n");
  } 
}

/***********************************************************************/
