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

pci7248.cpp

Contains code for pci7248 io card

*************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> // for open(...)
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>

#include "dask.h"
#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "iodriver.h"
#include "pci7248.h"

static const U16 ports[6] = {Channel_P1A, Channel_P1B, Channel_P1C,
                  Channel_P2A, Channel_P2B, Channel_P2C};

/***********************************************************************/

extern "C" io_driver_t *new_pci7248(react_drv_base_t *react, const char *option)
{
  return new PCI7248_driver_t(react);
}


/***********************************************************************/

void PCI7248_driver_t::read_config_file(void)
{
  /* ok, set the defaults first */

  FILE *fp = fopen("dbfiles/pci7248.dat", "r");
  if (fp == NULL)
  {
    printf("Can't open dbfiles/pci7248.dat, using defaults\n");
    return;
  }

  char line[100];
  for (int i=0; NULL != fgets(line, sizeof(line) , fp); i++)
  {
    ltrim(line);
    rtrim(line);
   // insert code here for any configuration.
  }
}


/***********************************************************************/

PCI7248_driver_t::PCI7248_driver_t(react_drv_base_t *react)
{
  driver_name = "PCI7248";
  driver = 1;
  card = 1;

  //#define Max_Chan_7248 (8)
  //I16 cardnum;
  //I16 chan_data[Max_Chan];
  //F32 chan_voltage[Max_Chan];
  //I16 range=AD_B_10_V;


  cardnum = 0;
  int err  = Register_Card(PCI_7248, cardnum);
  if (err != 0)
  {
     printf("PCI7248 Register_Card error = %d\n", err);
     exit(1);
  }
  printf ("PCI7248: registered card successfully!!\n");

  read_config_file();

  err = DIO_PortConfig(cardnum,Channel_P1A, INPUT_PORT);
  if (err != 0)
  {
    printf("PCI7248: Error Configuring port P1A: %d\n", err);
  }
  err = DIO_PortConfig(cardnum,Channel_P2A, OUTPUT_PORT);
  if (err != 0)
  {
    printf("PCI7248: Error Configuring port P2A: %d\n", err);
  }
  err = DIO_PortConfig(cardnum,Channel_P1B, INPUT_PORT);
  if (err != 0)
  {
    printf("PCI7248: Error Configuring port P1B: %d\n", err);
  }
  err = DIO_PortConfig(cardnum,Channel_P2B, OUTPUT_PORT);
  if (err != 0)
  {
    printf("PCI7248: Error Configuring port P2B: %d\n", err);
  }
  err = DIO_PortConfig(cardnum,Channel_P1C, INPUT_PORT);
  if (err != 0)
  {
    printf("PCI7248: Error Configuring port P1C: %d\n", err);
  }
  err = DIO_PortConfig(cardnum,Channel_P2C, OUTPUT_PORT);
  if (err != 0)
  {
    printf("PCI7248: Error Configuring port P2C: %d\n", err);
  }

  is_input[0] = true;
  is_input[1] = true;
  is_input[2] = true;
  is_input[3] = false;
  is_input[4] = false;
  is_input[5] = false;

  memset(dobuf, 0, sizeof(dobuf));
  printf("PCI7248 initialized\n");
}

/***********************************************************************/

void PCI7248_driver_t::send_ao(int ch, double val)
{
}

/***********************************************************************/

void PCI7248_driver_t::close(void)
{
  Release_Card(cardnum);
}

/***********************************************************************/

void PCI7248_driver_t::send_do(int channel, bool val)
{
  U32 tbuf;
  U32 mask = ((U32) 1) << channel;
  U16 port;
  int port_num;

  if ((channel < 0) || (channel > 47))
  {
    return;
  }

  printf("sending do, ch: %d, v: %c\n", channel, val?'1':'0');
  for (int i=0; i < 6; i++)
  {
    if (channel < ((i+1)*8) )
    {
      if (is_input[i])
      {
	printf("Error, not an output\n");
        return;
      }
      port_num = i;
      int temp_ch = channel - (i*8);
      mask = ((U32) 1) << temp_ch;
      if (val)
      {
        dobuf[i] |= mask;
      }
      else
      {
        dobuf[i] &= ~mask;
      }
      port = ports[i];
      tbuf = dobuf[i];
      break;
    }
  }
  int ret = DO_WritePort(cardnum, port, tbuf);
  printf("Writting, ch: %d, v: %c,  port_num: %d, port: %hu, tbuf: %02lx\n",
		  channel, val?'1':'0', port_num, port, tbuf);
  if (ret != 0)
  {
    printf("PCI7248: Error writing port %d: %d\n", port, ret);
  }
}

/***********************************************************************/

double PCI7248_driver_t::get_ai(int channel)
{
  return 0.0;
}

/***********************************************************************/

bool PCI7248_driver_t::get_di(int channel)
{
  if ((channel >= 0) && (channel < 48))
  {
    return di_data[channel];
  }
  else
  {
    return false;
  }
}

/***********************************************************************/

void PCI7248_driver_t::read(void)
{
  U32 input, mask;
  I16 err;
  for (int i=0; i < 6; i++)
  {
    if (is_input[i])
    {
      int ret = DI_ReadPort(cardnum,  ports[i], &input);
      if (ret != 0)
      {
        printf("PCI7248: Error reading port %d: %d\n", ports[i], ret);
      }
      input = ~input;
      input &= 0xFF;
      printf("Value read from PORT %d: %02lx\n", ports[i], input);
      for (int j=0; j < 8; j++)
      {
        mask = ((U32) 1) << j;
	int n = j+(i*8);
	//printf("masking out chan %d\n", n);
        di_data[n] = (input & mask) != 0;
        printf("%d:%c, ", n, di_data[n]? '1':'0');
      }
      printf("\n");
    }
  }
}

/***********************************************************************/


