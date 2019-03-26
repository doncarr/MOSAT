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

class simple_ascii_driver_t : public io_driver_t
{
private:
  double last_good[2];
  double ai_vals[32];
  double tmp_ai_vals[32];
  double ao_vals[32];
  double tmp_ao_vals[32];
  bool di_vals[32];
  bool tmp_di_vals[32];
  bool do_vals[32];
  bool tmp_do_vals[32];
  int di_offset;
  int do_offset;
  int ai_offset;
  int ao_offset;
  int serial_fd;
public:
  simple_ascii_driver_t(react_drv_base_t *react, const char *device);
  void read(void);
  void end_read(void);
  bool get_di(int channel);
  double get_ai(int channel);
  long get_count(int channel);
  void send_do(int channel, bool val);
  void send_ao(int channel, double val);
  void resend_dos(void);
  void close(void);
};

int init_simple_ascii(const char *device, char *error, int size);

int read_simple_ascii(int fd,
           double *ai_vals, int max_ai,
           bool *di_vals, int max_di,  
           bool *do_vals, int max_do,  
           char *error, int size);

int send_simple_ascii(int fd, int channel, bool val, 
           char *error, int size);

int ao_send_simple_ascii(int fd, int channel, int val,
           char *error, int size);

