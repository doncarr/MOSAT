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

react_rtu.cpp

Contains code for react_rtu driver 

*************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

extern "C"
{
#include <oemdefs.h>
#include <iodb.h>
}

#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "iodriver.h"

#define SX_MAX_AI 16
#define SX_MAX_DI 16
#define SX_MAX_AO 16
#define SX_MAX_DO 8 

class sixnet_rtu_driver_t : public io_driver_t
{
private:
  bool di_data[SX_MAX_DI];
  uint16_t ai_data[SX_MAX_AI];
  unsigned char do_buf;
public:
  sixnet_rtu_driver_t(react_drv_base_t *react);
  void read(void);
  bool get_di(int channel);
  double get_ai(int channel);
  long get_count(int channel);
  void send_do(int channel, bool val);
  void send_ao(int channel, double val);
  void close(void);
};

/***********************************************************************/

static void print_u8(unsigned char x)
{
  unsigned char mask = 1;
  for (int i=0; i < 8; i++)
  {
    if ((x & mask) != 0)
    {
      printf("1");
    }
    else
    {
      printf("0");
    }
    mask <<= 1;
  }
  printf(" ");
}

/***********************************************************************/

extern "C" io_driver_t *new_sixnet_rtu(react_drv_base_t *r, const char *option)
{
  printf("Creating new sixnet_rtu iodriver\n");
  return new sixnet_rtu_driver_t(r);
}

/***********************************************************************/

sixnet_rtu_driver_t::sixnet_rtu_driver_t(react_drv_base_t *react)
{
  driver_name = "Sixnet_RTU";
  driver = 1;
  card = 1;


  do_buf = 0;
  printf("Sixnet_RTU initialized\n");
}

/***********************************************************************/

void sixnet_rtu_driver_t::send_ao(int channel, double val)
{
  uint16_t uval = (uint16_t) uval;
  int ecode;
  if ((channel >= 0) && (channel < SX_MAX_AO))
  {
    ecode = IODBWrite(ANALOG_OUT, channel, 1, &uval, NULL);
    printf("sending AO, err: %d, channel: %d, val: %hu\n", ecode, channel, uval);
  }
}

/***********************************************************************/

void sixnet_rtu_driver_t::close(void)
{
}

/***********************************************************************/

void sixnet_rtu_driver_t::send_do(int channel, bool val)
{
  int ecode;
  if ((channel >= 0) && (channel < SX_MAX_DO))
  {
    int bit;
    bit = channel % 8;
    unsigned char mask;
    mask = ((unsigned char)1) << bit;
    //printf("mask = %u, ~mask = %u\n", unsigned(mask), unsigned(~mask));
    //printf("do_buf = %u\n", do_buf);

    if (val)
    {
      do_buf = do_buf | mask;
      //do_buf = 0xFF;
    }
    else
    {
      do_buf = do_buf & ~mask;
      //do_buf = 0x00;
    }
   print_u8(mask);
   print_u8(~mask);
   print_u8(do_buf);
   printf("/n"); 
    //printf("do_buf = %u\n", do_buf);
    ecode = IODBWrite(DISCRETE_OUT, 0, 8, &do_buf, NULL);
    printf("sending DO, err: %d, channel: %d, val: %s\n", ecode, channel, val ? "1" : "0");
  }
}

/***********************************************************************/

double sixnet_rtu_driver_t::get_ai(int channel)
{
  if ((channel >= 0) && (channel < SX_MAX_AI))
  {
    return double(ai_data[channel]);
  }
  else
  {
    return 0.0;
  }
}

/***********************************************************************/

bool sixnet_rtu_driver_t::get_di(int channel)
{
  if ((channel >= 0) && (channel < SX_MAX_DI))
  {
    return di_data[channel];
  }
  else
  {
    return false;
  }
}

/***********************************************************************/

long sixnet_rtu_driver_t::get_count(int channel)
{
  return 0;
}

/***********************************************************************/

void sixnet_rtu_driver_t::read(void)
{
  int ecode;
  ecode = IODBRead(ANALOG_IN, 0, SX_MAX_AI, (void *)ai_data, NULL);
  printf("err: %d, ANALOG:", ecode); 
  for (int i=0; i < SX_MAX_AI; i++)
  {
    printf(" %hu", ai_data[i]);
  }
  printf("\n");

  unsigned char read_buf[2];
  ecode = IODBRead(DISCRETE_IN, 0, SX_MAX_DI, (void *)read_buf, NULL);
  printf("err: %d, DISCRETE:", ecode); 
  for (int i=0; i < SX_MAX_DI; i++)
  {
    int byte; 
    int bit;
    bit = i % 8;
    byte = i / 9;
    unsigned char mask;
    mask = ((unsigned char)1) << bit;
    di_data[i] = ((read_buf[byte] & mask)) != 0 ;
    printf(" %s", di_data[i] ? "T" : "F");
  }
  printf("\n");
}

/***********************************************************************/

