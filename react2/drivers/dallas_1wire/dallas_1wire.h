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

#include <semaphore.h>
#include <pthread.h>

#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "iodriver.h"

#include "iodriver.h"

class dallas_1wire_driver_t : public io_driver_t
{
private:
  bool read_values;
  bool wake_him_up;
  double ai_vals[32];
  double tmp_ai_vals[32];
  bool di_vals[32];
  bool tmp_di_vals[32];
  int di_offset;
  int do_offset;
  int ai_offset;
  int ao_offset;
  sem_t read_mutex_sem; 
  sem_t read_wait_sem; 
public:
  dallas_1wire_driver_t(react_drv_base_t *react);
  void read(void);
  void end_read(void);
  void read_thread(void);
  bool get_di(int channel);
  double get_ai(int channel);
  long get_count(int channel);
  void send_do(int channel, bool val);
  void send_ao(int channel, double val);
  void close(void);
};

int init_dallas_wire_temperatures(char *error, int size);

int read_dallas_wire_temperatures(double *temperatures, int max, 
           char *error, int size);


