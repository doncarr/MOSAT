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

/************
Iodriver.h

Header file for input/output driver definitions.

**************************************************************************/

#ifndef __IODRIVER_INC__
#define __IODRIVER_INC__

#include "db_point.h"

class io_driver_t;

class io_driver_factory_t
{
public:
  int n_drivers;
  int inc(void);
  virtual io_driver_t *new_driver(const char *config_data, char *err, int esz);
  virtual const char *long_description(void);
  virtual const char *short_discription(void);
  virtual const char *abbreviation(void);
  virtual ~io_driver_factory_t(void) {n_drivers = 0;};
  
};

class io_driver_t
{
public:
  const char *driver_name;
  int driver;
  int card;
  int base_addr;

  io_driver_t(void){};
  virtual void read(void){};
  virtual void end_read(void){};
  virtual bool get_di(int channel){return false;};
  virtual double get_ai(int channel){return 0.0;};
  virtual void get_ai_range(int start, int end, double vals[]){for (int i=0; i < (end - start); i++) vals[i] = 0.0;};
  virtual long get_count(int channel){return 0;};
  virtual void send_do(int channel, bool val){};
  virtual void send_ao(int channel, double val){};
  virtual void close(void){};
  virtual ~io_driver_t(void) {};
};

typedef io_driver_t *(*get_io_drv_t)(react_drv_base_t *r, const char *option);

typedef io_driver_factory_t *(*get_iodriver_factory_t)(react_drv_base_t *r);

typedef void (*del_io_drv_t)(io_driver_t *);

io_driver_t *load_iodriver(react_drv_base_t *drvdb, 
     const char *sofile, const char *get_fn_name, const char *other_lib, const char *option);

io_driver_factory_t *load_iodriver_factory(react_drv_base_t *drvdb, 
  const char *sofile, const char *get_fn_name, const char *other_lib);

class parallel_driver_t : public io_driver_t
{
private:
  bool di_data[0];
  short chan_data[8];
  double chan_voltage[8];
  unsigned long dobuf;
public:
  parallel_driver_t(react_drv_base_t *react);
  void read(void);
  bool get_di(int channel);
  double get_ai(int channel);
  void send_do(int channel, bool val);
  void send_ao(int channel, double val);
  void close(void);
};

/*********

class dummy_io_driver_t : public io_driver_t
{
public:
  dummy_io_driver_t(int drv, int crd, int b_addr);
  int read_ai(void);
  int read_di(void);
  int read_string(void);
  void send_do(int channel, bool val);
  void send_do(int channel, bool val);
  bool card_installed(void) {return TRUE;};
};

*******/

#endif

