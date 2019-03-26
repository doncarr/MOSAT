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

reactmodbusdrv.cpp

Contains code for react modbus driver.

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

#include "rtmodbus.h"

#include "arg.h"

#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "iodriver.h"

#include "reactmodbusdrv.h"

#include "logfile.h"

/****
  Ok, this is a first stab at a modbus driver. There are many issues
  to deal with, such as what is the round trip delay, and how many
  round trips to you need to read all the data. For a LAN, the round
  trips should be very short, and typically you would only read modbus
  devices that were local. You could have a modbus bridge with a
  slow serial device on the other end though. Also, this same driver
  will be adapted to talk directly to serial devices.

  Also, remember that you need to send outputs. If you have a script
  that sends lots of outputs, that can add up. You might want a back-
  ground thread to send the outputs so the main thread does not have
  to wait while the outputs are being sent, and you might also be able
  to send more than one output in the same message. This would mean we
  would need a hook to tell the driver to flush the outputs.

  I am thinking we need a hook so that all device can be told a poll
  is starting, so they could all start polling the devices at the same
  time. This would be ignored by drivers that did not have a background
  thread.

  This uses the main thread
  to read the remote device. It assumes a single start address for
  ai, ao, di, do, counters. It assumes a single modbus device number.

  In the future, we would like the driver to possible read more than one
  offset, and more than one modbus device number.

  1) The main thread will block waiting for the modbus data when the
     data is read. This would work just fine for sample periods that
     are sufficiently less than the summ of the round trip delays.

  2) A back ground thread the continually reads the modbus data and puts
     it in memory protected by a semaphore. This could be free running, or
     at a given sample rate. Then when the main thread
     wants to read the data, a recent value will be in memory.

  3) Further, especially for each slow serial modbus device, we MUST have a 
     seperate thread polling it, rather than polling them all in serial. 
     With this in mind, it might be a good idea to have a thread for each device number,
     since each might be a different serial port on a bridge, and could thus be
     polled at the same time. Should this be configurable?? Thinking more, it would
     have to be configurable, since often multiple devices are on the SAME RS-485 serial 
     port, and thus MUST be polled in serial!!
     
  4) So what is the design? One single modbus driver with different threads, a modbus
     driver for each ip address, or a modbus driver for each modbus device number??

  5) How do we do the configuration? It might be that IP addresses and modbus device 
     numbers could change when the network is re-configured. How about if the modbus
     ip address and device number are part of the driver configuration, and then you
     would select modbus1, modbus2, etc, (which would include the ip address and device number),
     and then, if the ip address or device number changes for a particular device, we
     just need to change it in one place. It would also be VERY cumbersome to be specifying
     the IP address and device number for EACH point, then sorting these to create groups.
     Better to explicitly create the groups bye ip address and device number, or serial device
     and device number. Then, the card will be the offsett, and the channel will be the
     channel from there. 

     Given this, we might then need a way to group drivers when they are all on the same
     serial port and must be polled by the same thread. And, since each modbus bridge could
     have more than one serial port, we can not necessarily do this by ip address.

  6) On startup, it should be configurable driver by driver if the outputs should be zeroed
     when starting React and when stopping React. Since modbus allows us to read the current
     state of the outputs, this is possible! With other cards, you MUST start with the outputs
     in a known state.

****/

static FILE *dfp = NULL;
/***********************************************************************/

int xx_printf(const char *format, ...)
{
  return 0;
}

static void *rtmodbus_start_read_thread(void *driver_ptr)
{
  reactmodbus_driver_t *p = (reactmodbus_driver_t *)driver_ptr;
  printf("Calling read_thread method\n");
  p->read_thread();
  printf("*** Error read_thread returned\n");
  return NULL; // Should never return, but without a return, gives warning.
}

/***********************************************************************/

extern "C" io_driver_t *new_reactmodbus(react_drv_base_t *r, const char *option)
{
  logfile->vprint("Creating new reactmodbus iodriver\n");
   dfp = fopen("log/outlog.txt", "w");
   //dfp = stdout;
  return new reactmodbus_driver_t(r, option);
}

/***********************************************************************/

void reactmodbus_driver_t::check_overlap(int a_modbus_id, const char *io_type, int opcode, int n_io, int modbus_offset, int channel_offset)
{
}

/***********************************************************************/

void reactmodbus_driver_t::add_io(int a_modbus_id, const char *io_type, int opcode, int n_io, int modbus_offset, int channel_offset)
{
  if ((0 == strcasecmp("DO", io_type)) && (a_modbus_id == -1))
  {
    // This needs more thought, but, for now, for now, just switch opcode.
    logfile->vprint("Will use alternate opcode for DO");
    alt_do_opcode = true;
    return;
  }
  if ((0 == strcasecmp("AO", io_type)) && (a_modbus_id == -1))
  {
    logfile->vprint("Will use alternate opcode for AO");
    alt_ao_opcode = true;
    return;
  }

  if (n_mod_io == REACT_MAX_MOD_IO)
  {
    logfile->vprint("******* Too many modbus io definitions\n");
    return;
  }

  if ((channel_offset + n_io) > REACT_MAX_MOD_AI)
  {
    logfile->vprint("******* can not map past %de for %s definitions, chan offset: %d, n: %d, total: %d\n", 
           REACT_MAX_MOD_AI, io_type, channel_offset, n_io, channel_offset + n_io);
    return;
  }

  mod_io_def_t tmp_mod_io;

  if (0 == strcasecmp("DI", io_type))
  {
    tmp_mod_io.type = REACT_MOD_DI; 
    logfile->vprint("Type DI, ");
  }
  else if (0 == strcasecmp("DO", io_type))
  {
    tmp_mod_io.type = REACT_MOD_DO; 
    logfile->vprint("Type DO, ");
  }
  else if (0 == strcasecmp("AI", io_type))
  {
    tmp_mod_io.type = REACT_MOD_AI; 
    logfile->vprint("Type AI, ");
  }
  else if (0 == strcasecmp("AO", io_type))
  {
    tmp_mod_io.type = REACT_MOD_AO; 
    logfile->vprint("Type AO, ");
  }
  else if (0 == strcasecmp("ERROR_MAP", io_type))
  {
    tmp_mod_io.type = REACT_MOD_ERROR_MAP; 
    logfile->vprint("Modbus Error Map, ");
  }
  else
  {
    logfile->vprint("******* invalid io type: %s\n", io_type);
    return;
  }
  
  tmp_mod_io.modbus_id = a_modbus_id; 
  tmp_mod_io.opcode = opcode; 
  tmp_mod_io.n = n_io; 
  tmp_mod_io.modbus_offset = modbus_offset; 
  tmp_mod_io.channel_offset = channel_offset; 
  logfile->vprint("unit id: %d, opcode: %d, n: %d, mod off: %d, ch off: %d\n", 
    a_modbus_id, opcode, n_io, modbus_offset, channel_offset);
  switch (tmp_mod_io.type)
  {
    case REACT_MOD_DI:
      switch (tmp_mod_io.opcode)
      {
        case 1:
          logfile->vprint("Will read DIs using read_do()\n");
          break;
        case 2:
          logfile->vprint("Will read DIs using read_di()\n");
          break;
        case 3:
          logfile->vprint("Will read DIs using read_di_register()\n");
          break;
        default:
          logfile->vprint("Invalid opcode for DI: %d\n", tmp_mod_io.opcode);
          break;
      }
      mod_io[n_mod_io] = tmp_mod_io;
      n_mod_io++;
      break;
    case REACT_MOD_AI:
      switch (tmp_mod_io.opcode)
      {
        case 3:
          logfile->vprint("Will read AIs using read_reg()\n");
          break;
        case 4:
          logfile->vprint("Will read AIs using read_ai()\n");
          break;
        default:
          logfile->vprint("Invalid opcode for AI: %d\n", tmp_mod_io.opcode);
          break;
      }
      mod_io[n_mod_io] = tmp_mod_io;
      n_mod_io++;
      break;
    case REACT_MOD_AO:
      logfile->vprint("Will send AOs using write_reg()\n");
      ao_map[n_ao_map] = tmp_mod_io;
      n_ao_map++;
      break;
    case REACT_MOD_DO:
      logfile->vprint("Will send DOs using send_do()\n");
      do_map[n_do_map] = tmp_mod_io;
      n_do_map++;
      break;
    case REACT_MOD_ERROR_MAP:
      modbus_error_map_start = tmp_mod_io.channel_offset;
      modbus_error_map_n = tmp_mod_io.n;
      logfile->vprint("Mapping errors starting at %d, n = %d\n",
          modbus_error_map_start, modbus_error_map_n);
      break;
    default:
      logfile->vprint("Invalid type: %d\n", tmp_mod_io.type);
      break;
  }

}

/***********************************************************************/

reactmodbus_driver_t::reactmodbus_driver_t(react_drv_base_t *react, const char *option)
{

/* Ok, we really need a configuration file to configure modbus, to start
 * it will be all hard-coded 
 */

  driver_name = "reactmodbus";
  di_offset = 0;
  do_offset = 0;
  ai_offset = 0;
  ao_offset = 0;

  read_values = false;
  wake_him_up = true;
  n_dos_to_send = 0;
  n_aos_to_send = 0;
  alt_do_opcode = false;
  alt_ao_opcode = false;
  logfile->vprint("---- initializing modbus %s ---------------------------\n", option);
  //modbus = rt_create_modbus("127.0.0.1:502");
  //modbus = rt_create_modbus("192.168.1.104:502");
  modbus_error_map_start = -1; 
  modbus_error_map_n = 0; 
  if (option == NULL)
  {
    modbus = rt_create_modbus("127.0.0.1:502");
  }
  else if (option[0] == '\0')
  {
    modbus = rt_create_modbus("127.0.0.1:502");
  }
  else
  {
    logfile->vprint("Using ip passed to driver: %s\n", option);
    modbus = rt_create_modbus(option);
    //logfile->vprint("modptr = %p\n", modbus);
    //modbus->read_ai(0, 16, tmp_ai_vals);
  }
  if (modbus == NULL)
  {
    exit(0);
  }
  //modbus->set_debug_level(3);
  //modbus->set_debug_level(0);
  //modbus->set_address(0);
  logfile->vprint("DONE initializing modbus\n");

  default_modbus_id = modbus->get_address();
  logfile->vprint("Default modbus address is: %d\n", default_modbus_id);

  logfile->vprint("Initializing semaphores\n");
  if (0 != sem_init(&read_mutex_sem, 0, 1))
  {
    perror("sem_init");
  }
  if (0 != sem_init(&output_mutex_sem, 0, 1))
  {
    perror("sem_init");
  }
  if (0 != sem_init(&read_wait_sem, 0, 0))
  {
    perror("sem_init");
  }
  logfile->vprint("DONE Initializing semaphores\n");
  
  delim_file_t *df = new delim_file_t(200, 10, '|', '#');
  char **argv;
  int argc;
  int line_num;
  
  // <DI or AI>|<opcode>|<number of io>|<modbus offset|<channel offset>|

/****** Example file contents
1|DI|1|16|0|0|
1|AI|3|16|0|0
2|DI|2|16|0|16|
2|AI|4|16|0|16|
3|DI|3|16|0|32|
3|AI|3|16|0|32|
# modbus id
# io type (DI or AI>)
# opcode 1=DO, 2=DI, 3=Register, 4=AI
# number of io points
# modbus offset
# channel offset
********/

 
  argv = df->first("./dbfiles/modbus.dat", &argc, &line_num);

  n_mod_io = 0;
  while (argv != NULL)
  {
    if (argc != 6)
    {
      logfile->vprint("**** modbus.dat: wrong number of args: %d, on line %d\n", argc, line_num);
      argv = df->next(&argc, &line_num);
      continue;
    }
    for (int i=0; i < 6; i++)
    {
      logfile->vprint("'%s', ", argv[i]);
    } 
    logfile->vprint("\n");

    logfile->vprint("Adding I/O\n");

    this->add_io(atoi(argv[0]), argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));

    argv = df->next(&argc, &line_num);
  }
  logfile->vprint("Done with modbus.dat\n");

  logfile->vprint("Creating thread\n");
  pthread_t thr;
  int retval;
  retval = pthread_create(&thr, NULL, rtmodbus_start_read_thread, this);
  if (retval != 0)
  {
    perror("can't create thread");
    exit(0);
  }
  retval = pthread_detach(thr);
  if (retval != 0)
  {
    perror("can't detach thread");
    exit(0);
  }
  logfile->vprint("DONE Creating thread\n");

  logfile->vprint("---- DONE initializing modbus ---------------------------\n");
}

/***********************************************************************/

void reactmodbus_driver_t::send_ao(int ch, double val)
{
  if ((ch >= 0) && (ch < REACT_MAX_MOD_AO))
  {
    //shm->do_val[ch] = val;
    //modbus->send_do(ch, val);

    if (n_aos_to_send >= 64) return;

    sem_wait(&output_mutex_sem);
    ao_vals_to_send[n_aos_to_send].ch = ch;
    ao_vals_to_send[n_aos_to_send].val = (unsigned short) val;
    n_aos_to_send++;
    sem_post(&output_mutex_sem);
  }
  //printf("Send AO ch = %d, val = %lf\n", ch, val);
}

/***********************************************************************/

void reactmodbus_driver_t::close(void)
{
}

/***********************************************************************/

void reactmodbus_driver_t::send_do(int ch, bool val)
{
  if ((ch >= 0) && (ch < REACT_MAX_MOD_DO))
  {
    //shm->do_val[ch] = val;
    //modbus->send_do(ch, val);

    if (n_dos_to_send >= 64) return;

    sem_wait(&output_mutex_sem);
    do_vals_to_send[n_dos_to_send].ch = ch;
    do_vals_to_send[n_dos_to_send].val = val;
    n_dos_to_send++;
    //modbus->send_do(ch, val);
    sem_post(&output_mutex_sem);
  }

}

/***********************************************************************/

void reactmodbus_driver_t::get_ai_range(int start, int end, double vals[])
{
  for (int i=0; i < (end - start); i++)
  {
    int channel = i + start;
    if ((channel >= 0) && (channel < REACT_MAX_MOD_AI))
    {
      vals[i] = (double) ai_vals[channel];
    }
    else
    {
      vals[i] = 0.0;
    }
  } 
}

/***********************************************************************/

double reactmodbus_driver_t::get_ai(int channel)
{
  double read_val;
  if ((channel >= 0) && (channel < REACT_MAX_MOD_AI))
  {
    read_val = (double) ai_vals[channel];
    xx_printf("AI %d, read %lf\n", channel, read_val);
    return read_val;
  }
  else
  {
    return 0.0;
  }
}

/***********************************************************************/

bool reactmodbus_driver_t::get_di(int channel)
{
  bool read_val;
  if ((channel >= 0) && (channel < REACT_MAX_MOD_DI))
  {
    read_val = di_vals[channel];
    xx_printf("DI %d, read %s\n", channel, read_val ? "T" : "F");
    //if (channel == 82)
      //fprintf(dfp, "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& DI %d, read %s\n", channel, read_val ? "T" : "F");
    return read_val;
  }
  else
  {
    return false;
  }
}

/***********************************************************************/

long reactmodbus_driver_t::get_count(int channel)
{
  if ((channel >= 0) && (channel < 32))
  {
    return 0;//shm->count_val[channel];
  }
  else
  {
    return 0;
  }
}

/***********************************************************************/

void reactmodbus_driver_t::set_mod_error(int retval, int unit_id)
{
  if (unit_id >= modbus_error_map_n) return;
  int ch = modbus_error_map_start + unit_id; 
  bool is_err = (retval < 0); 
  if (is_err) 
  {
    tmp_di_vals[ch] = true;
    fprintf(dfp, "Com error: %s, unit id: %d, channel: %d, retval: %d, wrote %s to channel %d\n", 
           (retval < 0) ? "YES" : "NO", unit_id, ch, retval, 
              is_err? "ERROR": "OK", ch); 
  }
  for (int i=0; i < modbus_error_map_n; i++)
  {
    int ach = modbus_error_map_start + i;
    //fprintf(dfp, "tmp_di_vals[%d] = %s\n", ach, tmp_di_vals[ach] ? "TRUE":"FALSE");
  }
  fflush(dfp);
}

/***********************************************************************/

void reactmodbus_driver_t::read_mod_io(void)
{
  // First clear all the errors
  // Later, if there is an error in any read for a unit id, we set an error
  for (int i=0; i < modbus_error_map_n; i++)
  {
    tmp_di_vals[modbus_error_map_start + i] = false;
  }
  for (int i=0; i < n_mod_io; i++)
  {
    int retval;
    modbus->set_address(mod_io[i].modbus_id);
    switch (mod_io[i].type)
    {
      case REACT_MOD_DI:
        //fprintf(dfp, "DI: ");
        switch (mod_io[i].opcode)
        {
          case 1:
            retval = modbus->read_do(mod_io[i].modbus_offset, mod_io[i].n, tmp_di_vals + mod_io[i].channel_offset);
            set_mod_error(retval, mod_io[i].modbus_id);
            break;
          case 2:
            retval = modbus->read_di(mod_io[i].modbus_offset, mod_io[i].n, tmp_di_vals + mod_io[i].channel_offset);
            set_mod_error(retval, mod_io[i].modbus_id);
            break;
          case 3:
            retval = modbus->read_di_register(mod_io[i].modbus_offset, mod_io[i].n, tmp_di_vals + mod_io[i].channel_offset);
            set_mod_error(retval, mod_io[i].modbus_id);
            break;
          default:
            xx_printf("Invalid opcode for DI: %d\n", mod_io[i].opcode);
            break;
        }
        break;
      case REACT_MOD_AI:
        //fprintf(dfp, "AI: ");
        switch (mod_io[i].opcode)
        {
          case 3:
            retval = modbus->read_reg(mod_io[i].modbus_offset, mod_io[i].n, tmp_ai_vals + mod_io[i].channel_offset);
            set_mod_error(retval, mod_io[i].modbus_id);
            break;
          case 4:
            retval = modbus->read_ai(mod_io[i].modbus_offset, mod_io[i].n, tmp_ai_vals + mod_io[i].channel_offset);
            //if ((mod_io[i].modbus_id == 4) && (mod_io[i].modbus_offset == 100))
            {
              fprintf(dfp, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
              fprintf(dfp, "unit id: %d, mod offset: %d\n ", 
                        mod_io[i].modbus_id, mod_io[i].modbus_offset);
              for (int j=0; j < 12; j++) 
              {
               fprintf(dfp, "%hu ", tmp_ai_vals[36 + j]);
              }
              fprintf(dfp, "\n");
            }
            set_mod_error(retval, mod_io[i].modbus_id);
            break;
          default:
            xx_printf("Invalid opcode for AI: %d\n", mod_io[i].opcode);
            break;
        }
        break;
      default:
        xx_printf("Invalid type: %d\n", mod_io[i].type);
        break;
    }
  }
  //fprintf(dfp, "tmp_di_vals[82] = %s\n", tmp_di_vals[82] ? "TRUE":"FALSE");
}

/***********************************************************************/

void reactmodbus_driver_t::map_do(int ch, int *modbus_ch, int *modbus_id, int *opcode)
{
  for (int i=0; i < n_do_map; i++)
  {
    int min = do_map[i].channel_offset;
    int max = do_map[i].channel_offset + do_map[i].n - 1;
    if ((ch >= min) && (ch <= max))
    {
      *modbus_ch = do_map[i].modbus_offset + (ch - do_map[i].channel_offset);
      *modbus_id = do_map[i].modbus_id;
      *opcode = do_map[i].opcode;
      //fprintf(dfp, "map DO ch %d to unitid %d, opcode %d, mod ch %d\n", 
       //  ch, *modbus_id, *opcode, *modbus_ch); 
      //fflush(dfp);
      return;
    }
  }
  fprintf(dfp, "***** NO map for DO Channel: %d, in %d maps\n", ch, n_do_map);
  fflush(dfp);
  *modbus_ch = -1;
  *modbus_id = -1;
  *opcode = -1;
}

/***********************************************************************/

void reactmodbus_driver_t::map_ao(int ch, int *modbus_ch, int *modbus_id, int *opcode)
{
/**
struct mod_io_def_t
{
  int modbus_id; // With a radio, or RS485, you can have multiple modbus ids for a single serial port 
  int type;
  int opcode;
  int n;
  int modbus_offset;
  int channel_offset;
};
**/

  for (int i=0; i < n_ao_map; i++)
  {
    int min = ao_map[i].channel_offset;
    int max = ao_map[i].channel_offset + ao_map[i].n - 1;    
    if ((ch >= min) && (ch <= max))
    {
      *modbus_ch = ao_map[i].modbus_offset + (ch - ao_map[i].channel_offset);
      *modbus_id = ao_map[i].modbus_id;
      *opcode = ao_map[i].opcode;
      fprintf(dfp, "map AO ch %d to unitid %d, opcode %d, mod ch %d\n", 
         ch, *modbus_id, *opcode, *modbus_ch); 
      fflush(dfp);
      return;
    }
  }
  fprintf(dfp, "***** NO map for AO Channel: %d, in %d maps\n", ch, n_ao_map);
  fflush(dfp);
  *modbus_ch = -1;
  *modbus_id = -1;
  *opcode = -1;
}

/***********************************************************************/

static int mycounter = 0;

void reactmodbus_driver_t::read_thread(void)
{
  usleep(1000);
  while(true)
  {
    fprintf(dfp, "----- Sending %d AOs ---------\n", n_aos_to_send);
    fflush(dfp);
    while (n_aos_to_send > 0)
    {
      int ch;
      unsigned short val;

      sem_wait(&output_mutex_sem);
      n_aos_to_send--;
      ch = ao_vals_to_send[n_aos_to_send].ch;
      val = ao_vals_to_send[n_aos_to_send].val;
      sem_post(&output_mutex_sem);

      if (n_ao_map > 0)
      {
        int modbus_opcode, modbus_id, modbus_ch;
        this->map_ao(ch, &modbus_ch, &modbus_id, &modbus_opcode);
        if (modbus_ch == -1)
        {
          printf("No mapping for channel\n");
          continue;
        }
        modbus->set_address(modbus_id);
        switch (modbus_opcode)
        {
          case 6: // Use the send single register opcode. 
           fprintf(dfp, "sending AO to %d, ch %d\n", modbus_id, modbus_ch);
           fflush(dfp);
            modbus->write_reg(modbus_ch, val);
            break;
          case 9: // Use the send multipl register opcode. 
            modbus->write_multiple_regs(modbus_ch, 1, &val);
            break;
          default:
            printf("Bad AO opcode: %d, should be 6 or 9\n", modbus_opcode);
            break;
        }
      }
      else
      {

        // Be extremely careful, NEVER do a modbus transmit when a semaphore
        // is held that could hold up the main thread!!!
        // Also, ONLY do modbus trainsmits from a background thread.
        // You must NEVER block react.
        modbus->set_address(default_modbus_id);
        if (alt_ao_opcode)
        {
          // SOME do NOT support write single register!!
          modbus->write_multiple_regs(ch, 1, &val);

        }
        else
        {
          modbus->write_reg(ch, val);
        }
      }
    }

    fprintf(dfp, "----- Sending %d DOs ---------\n", n_dos_to_send);
    fflush(dfp);
    while (n_dos_to_send > 0)
    {
      unsigned short ch;
      unsigned char val;

      sem_wait(&output_mutex_sem);
      n_dos_to_send--;
      ch = do_vals_to_send[n_dos_to_send].ch;
      val = do_vals_to_send[n_dos_to_send].val;
      sem_post(&output_mutex_sem);

      if (n_do_map > 0)
      {
        int modbus_opcode, modbus_id, modbus_ch;
        this->map_do(ch, &modbus_ch, &modbus_id, &modbus_opcode);
        if (modbus_ch == -1)
        {
          printf("No mapping for channel\n");
          continue;
        }
        modbus->set_address(modbus_id);
        switch (modbus_opcode)
        {
          case 5: // Use the send single DO. 
           fprintf(dfp, "sending DO to %d, ch %d\n", modbus_id, modbus_ch);
            fflush(dfp);
            modbus->send_do(modbus_ch, val);
            break;
          case 8: // Use the send multipl register opcode. 
            modbus->send_multiple_dos(modbus_ch, 1, &val);
            break;
          default:
            printf("Bad DO opcode: %d, should be 5 or 8\n", modbus_opcode);
            break;
        }
      }
      else
      {

        // Be extremely careful, NEVER do a modbus transmit when a semaphore
        // is held that could hold up the main thread!!!
        // Also, ONLY do modbus trainsmits from a background thread.
        // You must NEVER block react.
        //modbus->set_address(1);
        modbus->set_address(default_modbus_id);
        if (alt_do_opcode)
        {
          // SOME do NOT support send single DO!!
          modbus->send_multiple_dos(ch, 1, &val);
        }
        else
        {
          modbus->send_do(ch, val);
        }
        //modbus->set_address(2);
      }
    }

    //printf("read thread reading modbus values . . .\n");
    if (n_mod_io == 0)
    {
      modbus->set_address(default_modbus_id);
      printf("read thread before read_ai . . .\n");
      printf("modptr = %p\n", modbus);
      modbus->read_ai(0, 16, tmp_ai_vals);
      printf("read thread after read_ai . . .\n");
      modbus->read_di(0, 16, tmp_di_vals);
      printf("read thread after read_di . . .\n");
    }
    else
    {
      read_mod_io();
    }
    //printf("read thread DONE reading modbus values . . .\n");

    xx_printf("read thread copying values . . .\n");
    sem_wait(&read_mutex_sem);
      xx_printf("  -- read thread in critical section . . .\n");
      memcpy(ai_vals, tmp_ai_vals, sizeof(ai_vals));
      //fprintf(dfp, "tmp_di_vals[82] = %s\n", tmp_di_vals[82] ? "TRUE":"FALSE");
      //fprintf(dfp, "di_vals[82] = %s\n", di_vals[82] ? "TRUE":"FALSE");
      memcpy(di_vals, tmp_di_vals, sizeof(di_vals));
      //fprintf(dfp, "tmp_di_vals[82] = %s\n", tmp_di_vals[82] ? "TRUE":"FALSE");
      //fprintf(dfp, "di_vals[82] = %s\n", di_vals[82] ? "TRUE":"FALSE");
      read_values = true;
      xx_printf("  -- read thread leaving critical section . . .\n");
    sem_post(&read_mutex_sem);
    xx_printf("read thread DONE copying values . . .\n");

    xx_printf("read thread waiting %d\n", mycounter++);
    sem_wait(&read_wait_sem);
    xx_printf("thread woke up\n");
  }
}

/***********************************************************************/

void reactmodbus_driver_t::read(void)
{
    sem_wait(&read_mutex_sem);
    xx_printf("  -- main thread in critical section . . .\n");
    if (read_values)
    {
      wake_him_up = true;
      //di_vals[16] = false;
    }
    else
    {
      // Either the reader is slow or the link is down
      wake_him_up = false;
      //di_vals[16] = true;
    }
  //modbus->read_ai(0, 16, ai_vals);
  //modbus->read_di(0, 16, di_vals);
}

/***********************************************************************/

void reactmodbus_driver_t::end_read(void)
{
  read_values = false;
  xx_printf("  -- main thread leaving critical section . . .\n");
  sem_post(&read_mutex_sem);
  if (wake_him_up)
  {
    xx_printf("waking up reader\n");
    sem_post(&read_wait_sem); // wake up the read thread to read the next values.
    xx_printf("DONE waking up reader\n");
  }
  else
  {
    xx_printf("The read thread is stalled, no reason to keep incrementing . . .\n");
  } 
}

/***********************************************************************/
