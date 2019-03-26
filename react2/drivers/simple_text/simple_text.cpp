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

simple_text.cpp

Contains code for react simple text driver.

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

#include "simple_text.h"

/****
  Ok, this is the simple text driver. 

  All communication is ascii text. 
  
  --------------------------
  Read DIs:

  $DI\n

  response is a '#', followed by a string of ones and zeros, followed by a new line character:

  #01011110001\n

  --------------------------
  Send single DO:

  $DOXXY\n
    XX is the channel number 00 to 99
     Y is the value, must be 0 or 1

  example:

  $DO081\n

  response is:

  #OK\n or

  #XX\n
  
  where, OK indicates all OK, other wise, XX is a two digit error code.

  --------------------------
  Read Analog Inputs
  $AI\n
  
  respons is:

  '#' followd by comma separated floating point values, followed by a new line 
  
  example:

  #23.1,44.3,73.23,18\n

  --------------------------
  Send single analog output 

  $AOXXYY.Y\n

  where:
    XX is the analog output number 00 to 99

    YY.Y is a floating point number of any number of characters.
  
  response is:

  #OK\n or

  #XX\n
  
  where, OK indicates all OK, other wise, XX is a two digit error code.

****/


/***********************************************************************/

extern "C" io_driver_t *new_simple_text(react_drv_base_t *r, const char *option)
{
  printf("Creating new simple_text iodriver, option: %s\n", option);
  return new simple_text_driver_t(r, option);
}

/***********************************************************************/

simple_text_driver_t::simple_text_driver_t(react_drv_base_t *react, 
                           const char *device)
{
  char err[100];

  driver_name = "simple_text";
  di_offset = 0;
  do_offset = 0;
  ai_offset = 0;
  ao_offset = 0;


  printf("Initializing simple text\n");
  serial_fd = init_simple_text(device, err, sizeof(err));
  if (-1 == serial_fd)
  {
    printf("Error initializing the simple text interface: %s\n", err);
  }
  printf("DONE initializing simple text\n");
  last_good[0] = 55.0;
  last_good[1] = 55.0;
}

/***********************************************************************/

void simple_text_driver_t::send_ao(int ch, double val)
{
  if ((ch >= 0) && (ch < 32))
  {
  }
}

/***********************************************************************/

void simple_text_driver_t::close(void)
{
}

/***********************************************************************/

void simple_text_driver_t::send_do(int ch, bool val)
{
  if ((ch >= 0) && (ch < 16))
  {
    char error[50];
    do_vals[ch] = val;
    send_simple_text(serial_fd, ch, val,
           error, sizeof(error));
  }
}

/***********************************************************************/

double simple_text_driver_t::get_ai(int channel)
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

bool simple_text_driver_t::get_di(int channel)
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

long simple_text_driver_t::get_count(int channel)
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

void simple_text_driver_t::read(void)
{
    char error[50];
    read_simple_text(serial_fd, 
             ai_vals, 32, 
             di_vals, 32, 
             tmp_do_vals, 32, 
             error, sizeof(error));
}

/***********************************************************************/

void simple_text_driver_t::end_read(void)
{
}

/***********************************************************************/


