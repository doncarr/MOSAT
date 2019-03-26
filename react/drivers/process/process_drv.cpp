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

process_drv.cpp

Contains code for react simple ascii driver.

*************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>

#include <mqueue.h>

#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "iodriver.h"

#include "process_drv.h"

/****
  Ok, this is the process driver. 

****/

/***********************************************************************/

extern "C" io_driver_t *new_process_drv(react_drv_base_t *r, const char *option)
{
  printf("Creating new process_drv iodriver\n");
  return new process_driver_t(r, option);
}

/***********************************************************************/

process_driver_t::process_driver_t(react_drv_base_t *react, 
                           const char *device)
{
  driver_name = "process_drv";
  di_offset = 0;
  do_offset = 0;
  ai_offset = 0;
  ao_offset = 0;

  read_values = false;
  wake_him_up = true;

  printf("Initializing process driver\n");

  mq_fd_1 = mq_open("/com.adaptivertc.react.p1", O_RDWR | O_CREAT | O_NONBLOCK, 0755, NULL);
  if (mq_fd_1 == ((mqd_t) -1))
  {
    perror("mq_open");
  }
  mq_fd_2 = mq_open("/com.adaptivertc.react.p2", O_RDWR | O_CREAT | O_NONBLOCK, 0755, NULL);
  if (mq_fd_2 == ((mqd_t) -1))
  {
    perror("mq_open");
  }

}

/***********************************************************************/

void process_driver_t::send_ao(int ch, double val)
{
  if ((ch >= 0) && (ch < 32))
  {
    process_mq_send_t write_buf;
    write_buf.type = SEND_AO; 
    write_buf.channel = ch; 
    write_buf.ao_val = val; 
    int rval = mq_send(mq_fd_1, (char *) &write_buf, sizeof(write_buf), 0);
    if (rval == -1)
    {
      perror("mq_send");
      return;
    }
    printf("Send Success\n");
  }
}

/***********************************************************************/

void process_driver_t::close(void)
{
}

/***********************************************************************/

void process_driver_t::send_do(int ch, bool val)
{
  if ((ch >= 0) && (ch < 64))
  {
    process_mq_send_t write_buf;
    write_buf.type = SEND_DO;
    write_buf.channel = ch;
    write_buf.do_val = val;
    int rval = mq_send(mq_fd_1, (char *) &write_buf, sizeof(write_buf), 0);
    if (rval == -1)
    {
      perror("mq_send");
      return;
    }
    printf("Send Success\n");

  }
}

/***********************************************************************/

double process_driver_t::get_ai(int channel)
{
  double read_val;
  if ((channel >= 0) && (channel < 64))
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

bool process_driver_t::get_di(int channel)
{
  bool read_val;
  if ((channel >= 0) && (channel < 64))
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

long process_driver_t::get_count(int channel)
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

void process_driver_t::read(void)
{
  unsigned prio;
  char msgbuf[8192];
  process_mq_read_t read_buf;
  printf("Waiting for a message . . . . \n");
  int rval = mq_receive(mq_fd_2, msgbuf, sizeof(msgbuf), &prio);
  if (rval == -1)
  {
    perror("mq_recieve");
    return;
  }
  memcpy(&read_buf, msgbuf, sizeof(read_buf));

  printf("Got a message of %d bytes\n", rval);
}

/***********************************************************************/

void process_driver_t::end_read(void)
{
  process_mq_send_t write_buf;
  write_buf.type = READ_INPUTS;
  int rval = mq_send(mq_fd_1, (char *) &write_buf, sizeof(write_buf), 0);
  if (rval == -1)
  {
    perror("mq_send");
    return;
  }
  printf("Send Success\n");
}

/***********************************************************************/
