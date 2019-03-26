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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>


#include "utimer.h"
#include "db.h"
#include "iodriver.h"
#include "reactshmio.h"
#include "ap_config.h"
#include "logfile.h"

logfile_t *logfile;

bool signal_recieved = false;
//react_base_t *db = NULL;

static reactshmio_t *shm;

const int hist_window_size = 10;
double ai_history[10][16];
int ai_history_ctr = 0;

static io_driver_t *io_driver;

static double time_now;

class dummy_db_t : public react_drv_base_t
{
public:
  double get_time(void);
};

ap_config_t ap_config;

/*****************************************************************/

double dummy_db_t::get_time(void)
{
  //printf("Wow, they requested the time: %0.1lf\n", time_now);
  return time_now;
}

/*****************************************************************/

void react_signal_handler(int sig_num)
{
  signal_recieved = true;
}

/*****************************************************************/

void read_io(void)
{

  io_driver->read();
  for (int i=0; i < 16; i++)
  {
    ai_history[ai_history_ctr][i] = io_driver->get_ai(i);
    double total = 0;
    for (int j=0; j < hist_window_size; j++)
    {
      total +=  ai_history[j][i];
    }
    shm->ai_val[i] = total / hist_window_size;
  }
  ai_history_ctr = (ai_history_ctr + 1) % hist_window_size;

  for (int i=0; i < 16; i++)
  {
    shm->di_val[i] = io_driver->get_di(i);
  }

  for (int i=0; i < 16; i++)
  {
    shm->count_val[i] = io_driver->get_count(i);
  }

  for (int i=0; i < 16; i++)
  {
    if (shm->do_val[i] != shm->do_save[i])
    {
      io_driver->send_do(i, shm->do_val[i]);
      shm->do_save[i] = shm->do_val[i];
      printf("*** do[%d].send(%c)\n", i, shm->do_val[i] ? 'T' : 'F');
    }
  }

  for (int i=0; i < 16; i++)
  {
    if (shm->ao_val[i] != shm->ao_save[i])
    {
      io_driver->send_ao(i, shm->ao_val[i]);
      shm->ao_save[i] = shm->ao_val[i];
      printf("--- ao[%d].send(%0.1lf)\n", i, shm->ao_val[i]);
    }
  }

}

/**
struct reactshmio_t
{
  double ai_val[64];
  double ao_val[64];
  double ao_last[64];
  bool di_val[64];
  bool do_val[64];
  bool do_last[64];
  unsigned long count_val[64];
};
**/


//  io_driver->send_do(channel, val);
//  io_driver->send_ao(channel, val);
//    bool val = io_driver->get_di(ch);
//    double val = io_driver->get_ai(ch);
//    long val = io_driver->get_count(ch);

/*****************************************************************/

void exit_clean_up(void)
{
}

/*****************************************************************/


int main(int argc, char *argv[])
{
  dummy_db_t dummy_db;

  logfile = new logfile_t(false, true);

  //printf("%s:%d\n", __FILE__, __LINE__);
  if (SIG_ERR == signal(SIGHUP, react_signal_handler))
  {
    perror("Error setting signal SIGHUP");
    exit(1);
  }

  atexit(exit_clean_up);

  long sample_rate = 10;;
  if (sample_rate <= 0) sample_rate =  10;
  long usecs_per_sample =  (long) 1000000 / sample_rate;
//  ./drivers/pci9111/libpci9111.so|new_pci9111|libpci_dask.so|

  shm = reactshmio_attach();

  io_driver = load_iodriver(&dummy_db, "./drivers/modbusdrv/libreactmodbusdrv.so", "new_reactmodbus", "librtmodbus.so", NULL);
  //io_driver = load_iodriver(&dummy_db, "./drivers/sim/libsimdriver.so", "new_simdriver", NULL);
  //io_driver = load_iodriver(&dummy_db, "./drivers/pci9111/libpci9111.so", "new_pci9111", "libpci_dask.so");

  utimer_t utimer;
  utimer.set_busy_wait(false);
  utimer.set_interval(usecs_per_sample);
  utimer.set_start_time();
  bool done = false;
  for (int i=0; !done; i++)
  {
    time_now = double(i) / double(sample_rate);
    utimer.wait_next();
    read_io();
    if ((i % 10) == 0)
    {
      printf(
          "shmmain, %0.1lf, ai(%0.1lf, %0.1lf, %0.1lf, %0.1lf) ao(%0.1lf, %0.1lf, %0.1lf, %0.1lf)\n",
          time_now,
          shm->ai_val[0], shm->ai_val[1], shm->ai_val[2], shm->ai_val[4],
          shm->ao_val[0], shm->ao_val[1], shm->ao_val[2], shm->ao_val[4]);
    }
  }
}

/**************************************************************************/

