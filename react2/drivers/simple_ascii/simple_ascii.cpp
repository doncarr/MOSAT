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

All lines must terminate with \n\r so this can be tested with minicom, so
this means that each line will be 2 characters longer than stated.

04
+12345.123
-12345.123
+12345.123
-12345.123
101010101010
010001000101

The first line, is always the number of analog intputs (N) 2 ascii digits.

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


  printf("Initializing simple ascii\n");
  serial_fd = init_simple_ascii(device, err, sizeof(err));
  if (-1 == serial_fd)
  {
    printf("Error initializing the simple ascii interface: %s\n", err);
  }
  printf("DONE initializing simple ascii\n");
  last_good[0] = 55.0;
  last_good[1] = 55.0;
}

/***********************************************************************/

void simple_ascii_driver_t::send_ao(int ch, double val)
{
  if ((ch >= 0) && (ch < 32))
  {
  }
}

/***********************************************************************/

void simple_ascii_driver_t::close(void)
{
}

/***********************************************************************/

void simple_ascii_driver_t::resend_dos(void)
{
  for (int i=0; i < 12; i++)
  {
    if (tmp_do_vals[i] != do_vals[i])
    {
      this->send_do(i, do_vals[i]);
    }
  }
}

/***********************************************************************/

void simple_ascii_driver_t::send_do(int ch, bool val)
{
  if ((ch >= 0) && (ch < 12))
  {
    char error[50];
    do_vals[ch] = val;
    send_simple_ascii(serial_fd, ch, val,
           error, sizeof(error));
  }
}

/***********************************************************************/

double simple_ascii_driver_t::get_ai(int channel)
{
  double read_val;
  if ((channel >= 0) && (channel < 32))
  {
    read_val = (double) ai_vals[channel];
    //printf("AI %d, read %lf\n", channel, read_val);
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
  if ((channel >= 0) && (channel < 32))
  {
    read_val = di_vals[channel];
    //printf("DI %d, read %s\n", channel, read_val ? "T" : "F");
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
  if ((channel >= 0) && (channel < 32))
  {
    return 0;
  }
  else
  {
    return 0;
  }
}

/***********************************************************************/

void simple_ascii_driver_t::read(void)
{
    char error[50];
    read_simple_ascii(serial_fd, 
             ai_vals, 32, 
             di_vals, 32, 
             tmp_do_vals, 32, 
             error, sizeof(error));
    this->resend_dos();
    for (int i=0; i < 2; i++)
    {
      ai_vals[i+10] = ai_vals[i];
      if (ai_vals[i] < -99.0)
      {
        ai_vals[i] = last_good[i];
      }
      else
      {
        last_good[i] = ai_vals[i];
      }
    }
}

/***********************************************************************/

void simple_ascii_driver_t::end_read(void)
{
}

/***********************************************************************/


