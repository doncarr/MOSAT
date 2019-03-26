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
#include "arg.h"

#include "simfiledriver.h"

/***********************************************************************/

class sim_file_driver_factory_t : public io_driver_factory_t
{
private:
  react_drv_base_t *drvb;
  
public:
  sim_file_driver_factory_t(react_drv_base_t *r) {drvb = r;};
  io_driver_t *new_driver(const char *config_data, char *err, int esz)
     {return new sim_file_driver_t(drvb, NULL);};
  const char *long_description(void) {return "Simulator";};
  const char *short_discription(void) {return "Simulator";}; 
  const char *abbreviation(void) {return "SIM";};
  ~sim_file_driver_factory_t(void) {};
};


/***********************************************************************

extern "C" io_driver_factory_t *new_simdriver_factory_1_0(react_drv_base_t *r, const char *option)
{
  return new sim_file_driver_factory_t(r);
}

***********************************************************************/

extern "C" io_driver_t *new_simfiledriver(react_drv_base_t *r, const char *option)
{
  return new sim_file_driver_t(r, option);
}

/***********************************************************************/

void sim_file_driver_t::read_sim_file(const char *fname)
{
  printf("Opening: %s\n" , fname);
  delim_file_t df(300, 10, '\t', '#');
  char **argv;
  int argc, line_num;
  argv = df.first(fname, &argc, &line_num);
  while (argv != NULL)
  {
    printf("Line %d, %d args", line_num, argc);
    for (int i=0; i < argc; i++)
    {
      printf(", \"%s\"", argv[i]);
    }
    printf("\n");
    argv = df.next(&argc, &line_num);
  }
}

/***********************************************************************/


sim_file_driver_t::sim_file_driver_t(react_drv_base_t *r, const char *option)
{
  printf("Creating sim_file_driver . . . .\n");
  driver_name = "File";
  driver = 1;
  card = 1;
  base_addr = 0;
  fp = NULL;
  react = r;

  this->read_sim_file(option);

  memset(di_data, '\0', sizeof(di_data));
  memset(do_data, '\0', sizeof(do_data));
  memset(ai_data, '\0', sizeof(ai_data));
  memset(ao_data, '\0', sizeof(ao_data));
}

/***********************************************************************/

void sim_file_driver_t::send_ao(int aChannel, double aVal)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return;
  }
  ao_data[aChannel] = aVal;
}

/***********************************************************************/

void sim_file_driver_t::send_do(int aChannel, bool aVal)
{
}

/***********************************************************************/

double sim_file_driver_t::get_ai(int aChannel)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return 0.0;
  }
  return ai_data[aChannel];
}

/***********************************************************************/

void sim_file_driver_t::close(void)
{
}

/***********************************************************************/

bool sim_file_driver_t::get_di(int channel)
{
  if ((channel > 0) && (channel < 64))
  {
    return di_data[channel];
  }
  else
  {
    return false;
  }
}

/***********************************************************************/

void sim_file_driver_t::read(void)
{

}

/***********************************************************************/


