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

/*******************************************************************

Db.c

Member functions for the real time database.

*************************************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
 
#include "db_point.h"
#include "db.h"
#include "rtcommon.h"
#include "reactpoint.h"
#include "secuencia.h"
#include "conio.h"
#include "timeaccum.h"
#include "arg.h"
#include "ap_config.h"
//#include "myconio.h"
#include "react_tag_rw.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#ifdef __REACT_MSG__
#include "reactmsg.h"
#endif
#ifdef __REACT_SHM__
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

static react_t *reactdb = NULL;

/***********************************************************************/

void react_t::send_do(int drv, int crd, int channel, bool val)
{
  /* Send a discrete output */
  if (drv < num_io_drivers)
  {
    if (io_driver[drv] != NULL)
    {
      io_driver[drv]->send_do(channel, val);
    }
  }
  else
  {
    logfile->vprint("*********************** Driver out of range: %d\n", drv);
  }
}

/***********************************************************************/

void react_t::send_ao(int drv, int crd, int channel, double val)
{
  /* Send an analog output */
  if (drv < num_io_drivers)
  {
    if (io_driver[drv] != NULL)
    {
      io_driver[drv]->send_ao(channel, val);
    }
  }
  else
  {
    logfile->vprint("*********************** Driver out of range: %d\n", drv);
  }
}

/***********************************************************************/

void react_t::exit_clean_up(void)
{
  if (ap_config.get_bool("zero_outputs_on_exit", true))
  {
    if (num_do > 0) logfile->vprint("Turning off %d discrete outputs:\n", num_do);
    for (int i=0; i < num_do; i++)
    {
      logfile->vprint("\t%s\n", do_points[i]->tag);
      do_points[i]->send(false);
    }
    if (num_ao > 0) logfile->vprint("Zeroing %d analog outputs:\n", num_ao);
    for (int i=0; i < num_ao; i++)
    {
      logfile->vprint("\t%s\n", ao_points[i]->tag);
      ao_points[i]->send(0.0);
    }
  }
  else
  {
    logfile->vprint("Zero outputs on exit is DISABLED\n");
  }

  for (int i=0; i < num_scan; i++)
  {
    scan_points[i]->write_to_file();
  }

  for (int i=0; i < num_data; i++)
  {
    data_points[i]->write_to_file();
  }

  for (int i=0; i < num_file_logger; i++)
  {
    file_logger_points[i]->write_to_file();
  }



#ifdef __REACT_MSG__
  delete_msg_queue();
#endif

#ifdef __REACT_SHM__
  delete_shared_memory();
#endif

  for (int i=0; i < num_io_drivers; i++)
  {
    if (io_driver[i] != NULL)
    {
      io_driver[i]->close();
    }
  }
}

/***********************************************************************/

void react_t::print_all_points(void)
{
  for (int i=0; i < num_di; i++)
  {
    logfile->vprint("DI: %s\n", di_points[i]->tag);
  }
  for (int i=0; i < num_do; i++)
  {
    logfile->vprint("DO: %s\n", do_points[i]->tag);
  }
  for (int i=0; i < num_calc; i++)
  {
    logfile->vprint("Calc: %s\n", calcs[i]->tag);
  }
  for (int i=0; i < num_d_calc; i++)
  {
    logfile->vprint("DiscreteCalc: %s\n", d_calcs[i]->tag);
  }
  for (int i=0; i < num_dcalc; i++)
  {
    logfile->vprint("DCalc: %s\n", dcalcs[i]->tag);
  }
  for (int i=0; i < num_timer; i++)
  {
    logfile->vprint("Timer: %s\n", timers[i]->tag);
  }
  for (int i=0; i < num_analog_val; i++)
  {
    logfile->vprint("Analog value: %s\n", analog_vals[i]->tag);
  }
  for (int i=0; i < num_discrete_val; i++)
  {
    logfile->vprint("Discrete value: %s\n", discrete_vals[i]->tag);
  }
  for (int i=0; i < num_int; i++)
  {
    logfile->vprint("Int: %s\n", ints[i]->tag);
  }
  for (int i=0; i < num_ai; i++)
  {
    logfile->vprint("AI: %s\n", ai_points[i]->tag);
  }
  for (int i=0; i < num_ao; i++)
  {
    logfile->vprint("AO: %s\n", ao_points[i]->tag);
  }
  for (int i=0; i < num_pci; i++)
  {
    logfile->vprint("PCI: %s\n", pci_points[i]->tag);
  }
  for (int i=0; i < num_pid; i++)
  {
    logfile->vprint("PID: %s\n", pid_points[i]->tag);
  }
  for (int i=0; i < num_pump; i++)
  {
    logfile->vprint("PUMP: %s\n", pump_points[i]->tag);
  }
  for (int i=0; i < num_ac; i++)
  {
    logfile->vprint("AC: %s\n", ac_points[i]->tag);
  }
  for (int i=0; i < num_level; i++)
  {
    logfile->vprint("LEVEL: %s\n", level_points[i]->tag);
  }
  for (int i=0; i < num_data; i++)
  {
    logfile->vprint("Data: %s\n", data_points[i]->tag);
  }
  for (int i=0; i < num_file_logger; i++)
  {
    logfile->vprint("FileLogger: %s\n", file_logger_points[i]->tag);
  }
  for (int i=0; i < num_web_logger; i++)
  {
    logfile->vprint("FileLogger: %s\n", web_logger_points[i]->tag);
  }
  for (int i=0; i < num_discrete_logger; i++)
  {
    logfile->vprint("DiscreteLogger: %s\n", discrete_logger_points[i]->tag);
  }
  for (int i=0; i < num_scan; i++)
  {
    logfile->vprint("Scan: %s\n", scan_points[i]->tag);
  }
  for (int i=0; i < num_web; i++)
  {
    logfile->vprint("Web: %s\n", web_points[i]->tag);
  }
}


/***********************************************************************/

void react_t::verify_drivers(void)
{
  for (int i=0; i < num_ai; i++)
  {
    int drv = ai_points[i]->get_driver();
    if ((drv < 0) || (drv >= num_io_drivers))
    {
      logfile->vprint("Driver out of range for ai point %s: %d\n", ai_points[i]->tag, drv); 
      exit(0);
    } 
  }
  for (int i=0; i < num_di; i++)
  {
    int drv = di_points[i]->get_driver();
    if ((drv < 0) || (drv >= num_io_drivers))
    {
      logfile->vprint("Driver out of range for di point %s: %d\n", di_points[i]->tag, drv); 
      exit(0);
    } 
  }
  for (int i=0; i < num_do; i++)
  {
    int drv = do_points[i]->get_driver();
    if ((drv < 0) || (drv >= num_io_drivers))
    {
      logfile->vprint("Driver out of range for do point %s: %d\n", do_points[i]->tag, drv); 
      exit(0);
    } 
  }
  for (int i=0; i < num_ao; i++)
  {
    int drv = ao_points[i]->get_driver();
    if ((drv < 0) || (drv >= num_io_drivers))
    {
      logfile->vprint("Driver out of range for ao point %s: %d\n", ao_points[i]->tag, drv); 
      exit(0);
    } 
  }
  for (int i=0; i < num_pci; i++)
  {
    int drv = pci_points[i]->get_driver();
    if ((drv < 0) || (drv >= num_io_drivers))
    {
      logfile->vprint("Driver out of range for pci point %s: %d\n", ai_points[i]->tag, drv); 
      exit(0);
    } 
  }
}

/***********************************************************************/

timeaccum_t taa[20];

void react_t::read_inputs(void)
{
  int j = 0;
  taa[j].start();
  for (int i=0; i < num_io_drivers; i++)
  {
    //printf("------------------------------- In read for driver %d\n", i);
    io_driver[i]->read();
    //printf("------------------------------- Out of read for driver %d\n", i);
  }
  taa[j].stop();

  j++;
  taa[j].start();
  for (int i=0; i < num_di; i++)
  {
    int drv = di_points[i]->driver;
    int ch = di_points[i]->channel;
    bool val = io_driver[drv]->get_di(ch);
    di_points[i]->update(val);
  }
  taa[j].stop();

  j++;
  taa[j].start();
  for (int i=0; i < num_ai; i++)
  {
    int drv = ai_points[i]->get_driver();
    int ch = ai_points[i]->get_channel();
    double val = io_driver[drv]->get_ai(ch);
    ai_points[i]->update_analog(val);
  }
  taa[j].stop();

  j++;
  taa[j].start();
  for (int i=0; i < num_pci; i++)
  {
    int drv = pci_points[i]->driver;
    int ch = pci_points[i]->channel;
    long val = io_driver[drv]->get_count(ch);
    pci_points[i]->update(val);
  }

  taa[j].stop();
  for (int i=0; i < num_io_drivers; i++)
  {
    //printf("------------------------------- In end read for driver %d\n", i);
    io_driver[i]->end_read();
    //printf("------------------------------- Finished end read for driver %d\n", i);
  }


  j++;
  taa[j].start();
  for (int i=0; i < num_pid; i++)
  {
    pid_points[i]->update();
  }
  taa[j].stop();

  for (int i=0; i < num_pump; i++)
  {
    pump_points[i]->update();
  }

  for (int i=0; i < num_ac; i++)
  {
    ac_points[i]->update();
  }

  for (int i=0; i < num_level; i++)
  {
    level_points[i]->update();
  }

  j++;
  taa[j].start();
  for (int i=0; i < num_data; i++)
  {
    data_points[i]->update();
  }
  taa[j].stop();

  for (int i=0; i < num_file_logger; i++)
  {
    file_logger_points[i]->update();
  }

  for (int i=0; i < num_web_logger; i++)
  {
    web_logger_points[i]->update();
  }

  for (int i=0; i < num_discrete_logger; i++)
  {
    discrete_logger_points[i]->update();
  }

  j++;
  taa[j].start();
  for (int i=0; i < num_ao; i++)
  {
    ao_points[i]->update_ramp();
  }
  taa[j].stop();

  j++;
  taa[j].start();
  for (int i=0; i < num_do; i++)
  {
    do_points[i]->update();
  }
  taa[j].stop();

  j++;
  taa[j].start();
  for (int i=0; i < num_calc; i++)
  {
    calcs[i]->update();
  }
  for (int i=0; i < num_d_calc; i++)
  {
    d_calcs[i]->update();
    d_calcs[i]->evaluate();
  }
  for (int i=0; i < num_dcalc; i++)
  {
    dcalcs[i]->update();
  }
  taa[j].stop();

  for (int i=0; i < num_timer; i++)
  {
    timers[i]->update();
  }

  j++;
  taa[j].start();
  for (int i=0; i < num_int; i++)
  {
    ints[i]->update();
  }
  taa[j].stop();

  j++;
  taa[j].start();
  for (int i=0; i < num_web; i++)
  {
    web_points[i]->update();
  }
  taa[j].stop();

#ifdef __REACT_SHM__
  this->update_shared_memory();
#endif

  //printf("\n");
  if (shutdown)
  {
    logfile->vprint("\nShuting down, sending all discrete outputs\n");
    for (int i=0; i < num_do; i++)
    {
      do_points[i]->send(false);
    }
    for (int i=0; i < num_ao; i++)
    {
      ao_points[i]->send(0.0);
    }
    logfile->vprint("\n");
    exit(0);
  }
}

/***********************************************************************/
timeaccum_t ta1, ta2, ta3;

bool react_t::update(double theTime, bool execute_script)
{
  bool retval;
  this->set_time(theTime);
  if (get_time() > next_sms_check)
  {
    this->check_sms();
    next_sms_check += 10.0;
  }

  ta1.start();
  this->read_inputs();
  ta1.stop();

#ifdef __REACT_MSG__
  check_msg_queue();
#endif

  if (execute_script)
  {
    ta2.start();
    retval = this->execute_secuencia();
    if (retval)
    {
      logfile->vprint("Execute returned true, time to exit\n");
    }
    ta2.stop();
  }
  else
  {
    retval = false;
  }
  execute_background_scripts();
  return retval;
}

/***********************************************************************/

point_type_t fast_get_point_type(char *tag)
{
  db_point_t *db_point;

  db_point = reactdb->get_db_point(tag);

  if (db_point == NULL)
  {
    return UNDEFINED_POINT;
  }
  return db_point->point_type();
}

/***********************************************************************/

rt_bool_ref_t *react_get_discrete_ref_fn(char *tag)
{
  db_point_t *db_point;
  db_point = reactdb->get_db_point(tag);
  if (db_point == NULL) return NULL;
  if (db_point->pv_type() != DISCRETE_VALUE)
  {
    return NULL;
  }
  else
  {
    char err[20];
    discrete_point_t *discrete_point;
    discrete_point = (discrete_point_t *) db_point;
    return discrete_point->get_bool_ref("pv", err, sizeof(err));
  }
}

/***********************************************************************/

bool *react_get_discrete_ptr_fn(char *tag)
{
  db_point_t *db_point;
  db_point = reactdb->get_db_point(tag);
  if (db_point == NULL) return NULL;
  if (db_point->pv_type() != DISCRETE_VALUE)
  {
    return NULL;
  }
  else
  {
    discrete_point_t *discrete_point;
    discrete_point = (discrete_point_t *) db_point;
    return discrete_point->pv_ptr();
  }
}

/***********************************************************************/

rt_double_ref_t *react_get_analog_ref_fn(char *tag)
{
  db_point_t *db_point;
  db_point = reactdb->get_db_point(tag);
  if (db_point == NULL) 
  {
    logfile->vprint("Tag not found: %s\n", tag);
    return NULL;
  }
  if (db_point->pv_type() != ANALOG_VALUE)
  {
    logfile->vprint("Wrong point type: %d\n", db_point->pv_type());
    return NULL;
  }
  else
  {
    char err[20];
    analog_point_t *analog_point;
    analog_point = (analog_point_t *) db_point;
    return analog_point->get_double_ref("pv", err, sizeof(err));
  }
}

/***********************************************************************/

double *react_get_analog_ptr_fn(char *tag)
{
  db_point_t *db_point;
  db_point = reactdb->get_db_point(tag);
  if (db_point == NULL) return NULL;
  if (db_point->pv_type() != ANALOG_VALUE)
  {
    return NULL;
  }
  else
  {
    analog_point_t *analog_point;
    analog_point = (analog_point_t *) db_point;
    return analog_point->pv_ptr();
  }
}

/***********************************************************************/

react_t::react_t()
{
  /* Constructor for the Real-time DataBase. */

  reactdb = this;
  num_ai = 0;
  ai_points = NULL;
  num_pci = 0;
  pci_points = NULL;
  num_ao = 0;
  ao_points = NULL;
  num_di = 0;
  di_points = NULL;
  num_do = 0;
  do_points = NULL;
  num_dcalc = 0;
  dcalcs = NULL;
  num_timer = 0;
  timers = NULL;
  num_analog_val = 0;
  analog_vals = NULL;
  num_discrete_val = 0;
  discrete_vals = NULL;
  num_calc = 0;
  calcs = NULL;
  num_d_calc = 0;
  d_calcs = NULL;
  num_int = 0;
  ints = NULL;
  num_pid = 0;
  pid_points = NULL;
  num_pump = 0;
  pump_points = NULL;
  num_ac = 0;
  ac_points = NULL;
  num_level = 0;
  level_points = NULL;
  num_data = 0;
  data_points = NULL;
  num_file_logger = 0;
  file_logger_points = NULL;
  num_web_logger = 0;
  web_logger_points = NULL;
  num_discrete_logger = 0;
  discrete_logger_points = NULL;
  num_scan = 0;
  scan_points = NULL;
  num_web = 0;
  web_points = NULL;

  num_io_drivers = 0;
  for (int i=0; i < 5; i++)
  {
    io_driver[i] = NULL;
  }
  shutdown = false;
  dinfo.adata = NULL;
  dinfo.ddata = NULL;
  dinfo.n_analog = 0;
  dinfo.n_discrete = 0;
  shmp = NULL;
  safe_strcpy(home_dir, "./", sizeof(home_dir));
  keyboard_is_on = true;

  set_get_discrete_ptr_fn(react_get_discrete_ptr_fn);
  set_get_analog_ptr_fn(react_get_analog_ptr_fn);
  set_get_point_type_fn(fast_get_point_type);
  next_sms_check = 10.0;
}

/************************************************************************/

void react_t::init_driver(void)
{
  //io_driver = new file_io_driver_t();
  //io_driver = new PCI9111_driver_t();
  //io_driver = new PCI7248_driver_t();
  //io_driver = load_iodriver(db, "./drivers/pci9111/libpci9111.so", "new_pci9111", "libpci_dask.so");
  //io_driver = load_iodriver(db, "./drivers/pci7248/libpci7248.so", "new_pci7248", "libpci_dask.so");
  //io_driver = load_iodriver(db, "./drivers/sim/libsimdriver.so", "new_simdriver", NULL);
  char path[200];

  safe_strcpy(path, this->get_home_dir(), sizeof(path));
  safe_strcat(path, "/dbfiles/drivers.dat", sizeof(path));
  logfile->vprint("Loading iodrivers from: %s\n", path);
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    logfile->perror(path);
    logfile->vprint("Can't open %s, using default sim driver\n", path);
    io_driver[0] = load_iodriver(db, "./drivers/sim/libsimdriver.so", "new_simdriver", NULL, NULL);
    num_io_drivers = 1;
    return;
  }
  char line[300];

  num_io_drivers = 0;
  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[25];

    //safe_strcpy(line, "AI1|Analog Input 1|PSI|0|0|1|0|100|0|4095|1|5|10|20|80|90|5|0|0|0|0|");
    ltrim(line);
    rtrim(line);
    safe_strcpy(tmp, (const char*) line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '|', 25);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }
    else if ((argc != 7) && (argc != 4))
    {
      logfile->vprint("Line: %s\n", line);
      logfile->vprint("%s: Wrong number of args, line %d, %d\n", path, i+1, argc);
      continue;
    }
    int offset;
    if (argc == 7)
    {
      offset = 3;
    }
    else
    {
      offset = 0;
    }

    logfile->vprint("%s\n", line);
    const char *option;
    option = argv[3+offset];


     /***
    io_driver_factory_t *iodrvf = load_iodriver_factory(db, argv[0], argv[1], argv[2]);
    io_driver =  
    ****/

    if (num_io_drivers >= int(sizeof(io_driver)/sizeof(io_driver[0])))
    {
      logfile->vprint("********** Maximum number of drivers loaded - can not load:\n\t%s\n", line);
      continue;
    }


    io_driver[num_io_drivers] = 
         load_iodriver(db, argv[0+offset], argv[1+offset], argv[2+offset], option);
    if (io_driver[num_io_drivers] == NULL)
    {
      logfile->perror(argv[0]);
      logfile->vprint("Can't load %s, using default sim driver\n", argv[0]);
      io_driver[num_io_drivers] = load_iodriver(db, "./drivers/sim/libsimdriver.so",
               "new_simdriver", NULL, NULL);
      //return;
    }
    num_io_drivers++;

    //break; // For now, only load one driver, this will change.
  }
}


/************************************************************************/

db_point_t ** react_t::read_one_point_type(db_point_factory_t *factory, 
     const char *a_home_dir, int *cnt)
{
  logfile->vprint("Reading %s ........\n", factory->abbreviation());
  int max_points = 50;
  db_point_t **db_points =
	(db_point_t **) malloc(max_points * sizeof(db_point_t*));
  MALLOC_CHECK(db_points);

  int n = 0;
  *cnt = 0;
  char path[200];
  snprintf(path, sizeof(path), "%s/dbfiles/%s", a_home_dir, factory->get_config_name());
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    logfile->perror(path);
    logfile->vprint("Can't open %s\n", path);
    return NULL;
  }
  char line[300];

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[25];

    safe_strcpy(tmp, line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '|', 25);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }

    char errbuf[100];

    db_points[n] = factory->new_point(argc, argv, errbuf, sizeof(errbuf));

    if (db_points[n] == NULL)
    {
      logfile->vprint("%s:%d %s\n", path, i+1, errbuf);
      continue;
    }
    logfile->vprint("%s", line);
    n++;
    if (n > max_points)
    {
      max_points += 10;
      db_points = (db_point_t **) realloc(db_points,
	      max_points * sizeof(db_point_t*));
      MALLOC_CHECK(db_points);
    }
  }
  *cnt = n;
  if (n == 0)
  {
    free(db_points);
    return NULL;
  }
  return db_points;
}

/************************************************************************/

void react_t::read_factory_points(const char *a_home_dir)
{
  char path[200];
  snprintf(path, sizeof(path), "%s/dbfiles/factory.txt", a_home_dir);
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    logfile->perror(path);
    logfile->vprint("Can't open %s\n", path);
    return;
  }
  char line[300];

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    ltrim(line);
    rtrim(line);
    if (0 == strlen(line)) continue;
    if (line[0] == '#') continue;
    db_point_factory_t *factory = load_db_point(line);
    if (factory == NULL)
    {
      exit(0);
    }
    ai_points = read_one_point_type(factory, a_home_dir, &num_ai);
  }
}

/************************************************************************/

void react_t::read_all_points(const char *a_home_dir)
{
  /* Read all database points from disk. */

  this->read_factory_points(a_home_dir);
  //db_point_factory_t *aif = new ai_point_factory_t;
  /**
  db_point_factory_t *aif = load_db_point("./ai.so");
  if (aif == NULL)
  {
    exit(0);
  }

  ai_points = read_one_point_type(aif, a_home_dir, &num_ai);
  ***/

  /**
  printf("Reading ai ........\n");
  //ai_points = read_ai_points(&num_ai, a_home_dir);
  ai_points = ai_point_t::read(&num_ai, a_home_dir);
  **/

  logfile->vprint("Reading ao ........\n");
  ao_points = ao_point_t::read(&num_ao, a_home_dir);

  logfile->vprint("Reading di ........\n");
  di_points = di_point_t::read(&num_di, a_home_dir);

  logfile->vprint("Reading do ........\n");
  do_points = do_point_t::read(&num_do, a_home_dir);

  logfile->vprint("Reading pci ........\n");
  pci_points = pci_point_t::read(&num_pci, a_home_dir);

  logfile->vprint("Reading calc ........\n");
  calcs = calc_point_t::read(&num_calc, a_home_dir);

  logfile->vprint("Reading discrete calc ........\n");
  d_calcs = dcalc_point_t::read(&num_d_calc, a_home_dir);
  logfile->vprint("%d discrete calcs read\n", num_d_calc);

  logfile->vprint("Reading dcalc ........\n");
  dcalcs = dcalc_t::read(&num_dcalc, a_home_dir);

  logfile->vprint("Reading timer ........\n");
  timers = timer_point_t::read(&num_timer, a_home_dir);
  logfile->vprint("num_timer = %d\n", num_timer);

  logfile->vprint("Reading analog values ........\n");
  analog_vals = analog_value_point_t::read(&num_analog_val, a_home_dir);
  logfile->vprint("num analog values = %d\n", num_analog_val);

  logfile->vprint("Reading discrete values ........\n");
  discrete_vals = discrete_value_point_t::read(&num_discrete_val, a_home_dir);
  logfile->vprint("num discrete values = %d\n", num_discrete_val);

  logfile->vprint("Reading int ........\n");
  ints = int_t::read(&num_int, a_home_dir);

  logfile->vprint("Reading pid ........\n");
  pid_points = pid_point_t::read(&num_pid, a_home_dir);

  logfile->vprint("Reading pump ........\n");
  pump_points = pump_point_t::read(&num_pump, a_home_dir);

  logfile->vprint("Reading ac ........\n");
  ac_points = ac_point_t::read(&num_ac, a_home_dir);

  logfile->vprint("Reading level ........\n");
  level_points = level_point_t::read(&num_level, a_home_dir);

  logfile->vprint("Reading data ........\n");
  data_points = data_point_t::read(&num_data, a_home_dir);

  logfile->vprint("Reading file logger ........\n");
  file_logger_points = file_logger_t::read(&num_file_logger, a_home_dir);

  logfile->vprint("Reading web logger ........\n");
  web_logger_points = web_logger_t::read(&num_web_logger, a_home_dir);

  logfile->vprint("Reading discrete logger ........\n");
  discrete_logger_points = discrete_logger_t::read(&num_discrete_logger, a_home_dir);

  logfile->vprint("Reading scan ........\n");
  scan_points = scan_point_t::read(&num_scan, a_home_dir);

  logfile->vprint("Reading web points ........\n");
  web_points = web_point_t::read(&num_web, a_home_dir);

  /* We can only parse the expressions after all points have been loaded! */
  logfile->vprint("Parsing %d Analog Expressions ......\n", num_calc);
  for (int i=0; i < num_calc; i++)
  {
    calcs[i]->parse_expr();
  }

  logfile->vprint("Parsing %d Discrete Expressions ......\n", num_d_calc);
  for (int i=0; i < num_d_calc; i++)
  {
    d_calcs[i]->parse_expr();
  }
#ifdef __REACT_SHM__
  this->init_shared_memory();
  this->fill_shared_memory();
  this->update_shared_memory();
#endif
#ifdef __REACT_MSG__
  this->init_msg_queue();
#endif
}

/**********************************************************************/

#ifdef __REACT_MSG__
void react_t::check_msg_queue(void)
{
  reactmsgbuf_t msgbuf;
  //printf("Checking queue . . \n");
  if (-1 != msgrcv(this->qid, &msgbuf, sizeof(msgbuf.data), 1, IPC_NOWAIT))
  {
    //printf("Wow, another connection: %s\n", msgbuf.data.msg);
    reactmsgbuf_t myreply;
    myreply.mtype = 1;
    myreply.data.qid = -1; // reall not used for a reply;

    secuencia_step_t *stp;
    stp = secuencia->new_script_type(msgbuf.data.msg,
              myreply.data.msg, sizeof(myreply.data.msg), false);
    if (stp == NULL)
    {
    }
    else if (!stp->check())
    {
      delete stp;
    }
    else
    {
      stp->execute(0.0);
      delete stp;
      strcpy(myreply.data.msg, "SUCCESS");
    }

    //printf("Sending reply on qid: %d\n", msgbuf.data.qid);
    int ret = msgsnd(msgbuf.data.qid, &myreply, sizeof(myreply.data), 0);
    if (ret == -1)
    {
      logfile->perror("Can't send message\n");
    }
  }
}
#endif

/**********************************************************************/

#ifdef __REACT_SHM__
void react_t::fill_shared_memory(void)
{
  if ((dinfo.adata == NULL) || (dinfo.ddata == NULL))
  {
    return;
  }
  int k=0;
  for (int i=0; i < num_ai; i++)
  {
    safe_strcpy(dinfo.adata[k].tag, ai_points[i]->tag,
                   sizeof(dinfo.adata[k].tag));
    safe_strcpy(dinfo.adata[k].description, ai_points[i]->description,
                   sizeof(dinfo.adata[k].description));
    dinfo.adata[i].min = ai_points[i]->eu_lo;
    dinfo.adata[i].max = ai_points[i]->eu_hi;
    dinfo.adata[i].dec = ai_points[i]->decimal;
    k++;
  }
  for (int i=0; i < num_ao; i++)
  {
    safe_strcpy(dinfo.adata[k].tag, ao_points[i]->tag,
                   sizeof(dinfo.adata[k].tag));
    safe_strcpy(dinfo.adata[k].description, ao_points[i]->description,
                   sizeof(dinfo.adata[k].description));
    dinfo.adata[i].min = ao_points[i]->output_limit_lo;
    dinfo.adata[i].max = ao_points[i]->output_limit_hi;
    dinfo.adata[i].dec = ao_points[i]->decimal;
    k++;
  }
  k=0;
  for (int i=0; i < num_di; i++)
  {
    safe_strcpy(dinfo.ddata[k].tag, di_points[i]->tag,
                   sizeof(dinfo.ddata[k].tag));
    safe_strcpy(dinfo.ddata[k].description, di_points[i]->description,
                   sizeof(dinfo.ddata[k].description));
    k++;
  }
  for (int i=0; i < num_do; i++)
  {
    safe_strcpy(dinfo.ddata[k].tag, do_points[i]->tag,
                   sizeof(dinfo.ddata[k].tag));
    safe_strcpy(dinfo.ddata[k].description, do_points[i]->description,
                   sizeof(dinfo.ddata[k].description));
    k++;
  }
  for (int i=0; i < num_dcalc; i++)
  {
    safe_strcpy(dinfo.ddata[k].tag, dcalcs[i]->tag,
                   sizeof(dinfo.ddata[k].tag));
    safe_strcpy(dinfo.ddata[k].description, dcalcs[i]->description,
                   sizeof(dinfo.ddata[k].description));
    k++;
  }
}

/**********************************************************************/

void react_t::update_shared_memory(void)
{
  if ((dinfo.adata == NULL) || (dinfo.ddata == NULL))
  {
    return;
  }
  int k=0;
  for (int i=0; i < num_ai; i++)
  {
    dinfo.adata[k].pv = ai_points[i]->pv;
    dinfo.adata[k].state = ai_points[i]->get_point_state();
    k++;
  }
  for (int i=0; i < num_ao; i++)
  {
    dinfo.adata[k].pv = ao_points[i]->pv;
    dinfo.adata[k].state = ao_points[i]->get_point_state();
    k++;
  }
  k=0;
  for (int i=0; i < num_di; i++)
  {
    safe_strcpy(dinfo.ddata[k].pv, di_points[i]->pv_string,
                   sizeof(dinfo.ddata[k].pv));
    dinfo.ddata[k].state = di_points[i]->get_point_state();
    k++;
  }
  for (int i=0; i < num_do; i++)
  {
    safe_strcpy(dinfo.ddata[k].pv, do_points[i]->pv_string,
                   sizeof(dinfo.ddata[k].pv));
    dinfo.ddata[k].state = do_points[i]->get_point_state();
    k++;
  }
  for (int i=0; i < num_dcalc; i++)
  {
    safe_strcpy(dinfo.ddata[k].pv, dcalcs[i]->pv_string,
                   sizeof(dinfo.ddata[k].pv));
    dinfo.ddata[k].state = dcalcs[i]->get_point_state();
    k++;
  }
}

/**********************************************************************/

void react_t::init_shared_memory(void)
{
  // First, calculate the size needed.
  int shmsize = sizeof(display_info_t) + (sizeof(display_info_t) % 8);
  int analog_offset = shmsize;
  int n_analog = num_ai + num_ao;
  int n_discrete = num_di + num_do + num_dcalc;
  shmsize +=
        (n_analog * sizeof(analog_display_data_t)) +
       ((n_analog * sizeof(analog_display_data_t)) % 8);
  int discrete_offset = shmsize;
  shmsize +=
        (n_discrete * sizeof(discrete_display_data_t)) +
       ((n_discrete * sizeof(discrete_display_data_t)) % 8);
  logfile->vprint("Analog offset = %d, discrete offset = %d\n",
        analog_offset, discrete_offset);

  // Create a shared segment, read/write for me, read only for others.
  shmid =  shmget(IPC_PRIVATE, shmsize, IPC_CREAT | 0744);
  if (shmid == -1)
  {
    logfile->perror("Could not create shared memory");
    shmp = NULL;
    return;
  }
  logfile->vprint("Created %d bytes of shared memory, id = %d\n", shmsize, shmid);

  // Now attach to the shared memory.
  shmp = shmat(shmid, NULL, 0);
  if (shmp == (void *) -1)
  {
    logfile->perror("Could not attach to shared memory");
    shmp = NULL;
    return;
  }
  logfile->vprint("Attached to shared memory, addr = %p\n", shmp);

  // Now issue a command to delete the segment. Notice we issue a delete
  // command now, and it will only really be deleted when the LAST
  // process detaches from shared memory.
  int ret = shmctl(shmid, IPC_RMID, NULL);
  if (ret == -1)
  {
    logfile->perror("Could not remove shared memory");
    shmp = NULL;
    return;
  }
  char *cp = (char *)shmp;
  *cp = '#';
  // Now put the info at address 0 and set up the pointers.
  shm_data_header_t *shmheader = (shm_data_header_t *) cp;
  shmheader->n_analog = n_analog;
  shmheader->n_discrete = n_discrete;
  shmheader->analog_offset = analog_offset;
  shmheader->discrete_offset = discrete_offset;
  get_display_pointers(shmp, &this->dinfo);

  char path[200];
  safe_strcpy(path, this->get_home_dir(), sizeof(path));
  safe_strcat(path, "/shmid.txt", sizeof(path));
  FILE *fp = fopen(path, "w");
  if (fp == NULL)
  {
    logfile->perror(path);
    return;
  }
  flogfile->vprint(fp, "%d\n", shmid);
  fclose(fp);
  logfile->vprint("%s: %d\n", path, shmid);
}

/**********************************************************************/

void react_t::delete_shared_memory(void)
{
  // Detach from shared memory.
  if (shmp == NULL)
  {
    logfile->vprint("Shared memory pointer is null . . .  can not detach\n");
    return;
  }
  int ret = shmdt(shmp);
  if (ret == -1)
  {
    logfile->perror("Could not detach from shared memory");
    return;
  }
  logfile->vprint("Detach from shared memory successful\n");
}

#endif

/**********************************************************************/

#ifdef __REACT_MSG__
void react_t::init_msg_queue(void)
{
  /* create the message que */

  logfile->vprint("initializing queue . . \n");
  int myqid = msgget(IPC_PRIVATE, IPC_CREAT | 0777);
  if (myqid == -1)
  {
    logfile->perror("Could not create message queue");
    return;
  }
  logfile->vprint("I got a message queue, id = %d\n", myqid);

  // Write the id to a file for others to use.
  char path[200];
  safe_strcpy(path, this->get_home_dir(), sizeof(path));
  safe_strcat(path, "/reactqid.txt", sizeof(path));
  FILE *fp = fopen(path, "w");
  if (fp == NULL)
  {
    logfile->perror(path);
    return;
  }
  fprintf(fp, "%d\n", myqid);
  double val;
  this->qid = myqid;
  fclose(fp);
  logfile->vprint("%s: %d\n", path, myqid);
}

/**********************************************************************/

void react_t::delete_msg_queue(void)
{
  if (qid == -1)
  {
    logfile->vprint("Message qid = -1, cannot destroy\n");
    return;
  }
  int ret = msgctl(this->qid, IPC_RMID, NULL);
  if (ret == -1)
  {
    logfile->perror("Could not destroy the message queue");
    return;
  }
  logfile->vprint("Message queue destroyed successfully\n");
}
#endif

/**********************************************************************/

/**
static const char * qname = "/adaptivertc.react.write_tag"; 
static mqd_t mq_fd = (mqd_t) -1;
 **/

static const char *wr_msg_name = "/home/artc";
//static int wr_msg_id;



void react_t::open_write_tag_queue(void)
{
  /***
  mq_fd = mq_open(qname, O_RDWR | O_CREAT, 0755, NULL);
  if (mq_fd == ((mqd_t) -1))
  {
    perror("mq_open");
  }
  ****/

}

/**********************************************************************/

void react_t::check_write_tag_queue(void)
{
  /**
  if (mq_fd == ((mqd_t) -1)) return;

  unsigned prio;
  char msgbuf[8192];
  struct timespec ts;

  ts.tv_sec = 0;
  ts.tv_nsec = 0;
  **/
  tag_rw_data_t rwd;

  key_t mykey = ftok(wr_msg_name, 1);
  if (mykey == -1)
  {
    logfile->perror("Could not get shared memory key");
    //logfile->vprint("Message queue destroyed successfully\n");
    exit(0);
  }
  logfile->vprint("message queue key is: %x\n", mykey);

  int myqid = msgget(mykey, IPC_CREAT | 0755);
  if (myqid == -1)
  {
    logfile->perror("Could not create message queue");
  }
  else
  {
    logfile->vprint("I got a message queue, id = %d\n", myqid);
  }


  //int rval = mq_receive(mq_fd, msgbuf, sizeof(msgbuf), &prio);
  //if (rval == -1)
  //{
  //  perror("mq_recieve");
  //}

  /***
  int retv = mq_timedreceive(mq_fd, msgbuf, sizeof(msgbuf), &prio, &ts);

  if (retv != sizeof(rwd)) return;

  memcpy(&rwd, msgbuf, sizeof(rwd));
  **/

  const char *tag = rwd.tag;
  db_point_t *dbp;
  analog_value_point_t *aval;
  dbp = this->get_db_point(tag);
  if (dbp == NULL) return;
  aval = dynamic_cast <analog_value_point_t *> (dbp);
  if (aval != NULL)
  {
    aval->set(atof(rwd.value));
  }
}

/**********************************************************************/


static secuencia_t *background_sequences[20];
static int n_background;

static secuencia_t *secuencia_stack[100];
static int current_secuencia;

void react_t::new_secuencia(secuencia_t *s)
{
  /* would be a good idea to make sure they are not doing recursion! */
  current_secuencia++;
  secuencia_stack[current_secuencia] = s;
  secuencia = s;
  logfile->vprint("new sequence: %s\n", secuencia->name);
  logfile->vprint("step = %s\n", secuencia->step_text());
}

/**********************************************************************/

const char *react_t::secuencia_name(void)
{
  return secuencia->name;
}

/**********************************************************************/

void react_t::execute_background_scripts(void)
{
  for (int i=0; i < n_background; i++)
  {
    background_sequences[i]->run(current_time);
  }
}

/**********************************************************************/

void react_t::read_background_sequences(const char *a_home_dir, const char *a_seq_dir)
{
  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/background.dat", sizeof(path));
  logfile->vprint("-- Reading background scripts: %s\n", path);
  n_background = 0;
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    logfile->vprint("Can't open background script file: %s\n", path);
    return;
  }
  char line[300];
  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    ltrim(line);
    rtrim(line);

    if ((line[0] == '#') || (line[0] == '\0'))
    {
      continue;
    }
    char tmp[300];
    int argc;
    char *argv[25];

    //safe_strcpy(line, "AI1|Analog Input 1|PSI|0|0|1|0|100|0|4095|1|5|10|20|80|90|5|0|0|0|0|");
    ltrim(line);
    rtrim(line);


    safe_strcpy(tmp, (const char*) line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '|', 25);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }
    else if ((argc != 1) && (argc != 4))
    {
      logfile->vprint("Line: %s\n", line);
      logfile->vprint("%s: Wrong number of args, line %d, %d\n", path, i+1, argc);
      continue;
    }
    int offset;
    if (argc == 4)
    {
      offset = 2;
    }
    else
    {
      offset = 0;
    }
    logfile->vprint("%s\n", line);

    background_sequences[n_background] = new secuencia_t(argv[offset], a_seq_dir);
    n_background++;
  }

}

/**********************************************************************/

void react_t::read_secuencia(const char *name, const char *a_home_dir)
{
  secuencia = new secuencia_t(name, a_home_dir);
  secuencia_stack[0] = secuencia;
  current_secuencia = 0;
}

/**********************************************************************/

static double quit_time = 0.0;
static bool waiting = false;
static char last_key = '\0';
static bool have_key = false;

timeaccum_t ta4;

bool react_t::execute_secuencia(void)
{
  bool done = secuencia->run(current_time);
  if (done)
  {
    if (current_secuencia != 0)
    {
      logfile->vprint("return from sequence: %s\n", secuencia->name);
      current_secuencia--;
      secuencia = secuencia_stack[current_secuencia];
      logfile->vprint("Resuming sequence: %s, line %d of %d\n", secuencia->name, secuencia->step_number, secuencia->num_steps);
      logfile->vprint("step = %s\n", secuencia->step_text());
      secuencia->advance();
      logfile->vprint("After advance: %s, line %d of %d\n", secuencia->name, secuencia->step_number, secuencia->num_steps);
      logfile->vprint("step = %s\n", secuencia->step_text());
      done = false;
    }
  }


  ta4.start();
  if (keyboard_is_on)
  {
    while (kb_hit())
    {
      printf("Starting key loop\n"); 
      char c = mygetch();
      if (c == 'q')
      {
        if (!waiting)
        {
          waiting = true;
          quit_time = current_time + 10.0;
          printf("\nTo quit hit q again within 10 seconds . . .\n\n");
        }
        else
        {
          logfile->vprint("User Terminated Program\n");
           done =  true;
        }
      }
      else if (c == '\t')
      {
        secuencia->skip_to_mark();
      }
      else if (c == 0x03)
      {
        printf("<CTRL-C> recieved\n");
        done = true;
      }
      else
      {
        last_key = c;
        have_key = true;
      }
      printf("End of key loop\n"); 
    }
  }
  ta4.stop();

  if (waiting)
  {
    if (current_time > quit_time)
    {
      waiting = false;
      printf("Quit canceled.\n");
    }
  }

  return done;
}


/**********************************************************************/

void react_t::clear_kb_buf(void)
{
  have_key = false;
}

/**********************************************************************/

bool react_t::kbhit(void)
{
  return have_key;
}

/**********************************************************************/

void react_t::set_shutdown(void)
{
  shutdown = true;
}

/**********************************************************************/

const char *react_t::get_home_dir(void)
{
  return home_dir;
}

/**********************************************************************/

void react_t::set_home_dir(const char *a_dir)
{
  safe_strcpy(home_dir, a_dir, sizeof(home_dir));
}

/**********************************************************************/

db_point_t::db_point_t(void)
{
  /* Constructor for a database point. */

  tag[0] = '\0';
  description[0] = '\0';
  display_is_on = true;
  display_x = display_y = 0;
  point_state = last_point_state = STATE_NORMAL;
}

/***********************************************************************/

rt_double_ref_t *db_point_t::get_double_ref(const char *expr, char *err, int sz) 
{
  //pv_type_t pv_type(void) {return ANALOG_VALUE;};
  if (ANALOG_VALUE == this->pv_type())
  {
    analog_point_t *ap = (analog_point_t *) this;
    return new rt_double_ptr_ref_t(&ap->pv);  
  }
  snprintf(err, sz, "Not an analog point"); 
  return NULL;
}

/***********************************************************************/

rt_bool_ref_t *db_point_t::get_bool_ref(const char *expr, char *err, int sz) 
{
  snprintf(err, sz, "Not implemented"); return NULL;
}

/***********************************************************************/

rt_long_ref_t *db_point_t::get_long_ref(const char *expr, char *err, int sz) 
{
  snprintf(err, sz, "Not implemented"); 
  return NULL;
}

/***********************************************************************/

void db_point_t::enable_display(int x, int y)
{
  /* This enables the display for a database point. */

  display_is_on = true;
  display_x = x;
  display_y = y;
}

/*************************************************************************/

void db_point_t::disable_display(void)
{
  if (display_is_on)
  {
    /***
    gotoxy(display_x, display_y);
    //textattr(NORMAL_ATTR);
    cprintf("                                   ");
    ****/
    display_is_on = false;
  }
}

/*************************************************************************/

db_point_t *react_t::get_db_point(const char *tag)
{
  /* This returns a pointer to a database point given the tag. */

  int i;

  for (i=0; i < num_ai; i++)
  {
    if (0 == strcasecmp(ai_points[i]->tag, tag))
    {
      return ai_points[i];
    }
  }

  for (i=0; i < num_pci; i++)
  {
    if (0 == strcasecmp(pci_points[i]->tag, tag))
    {
      return pci_points[i];
    }
  }


  for (i=0; i < num_ao; i++)
  {
    if (0 == strcasecmp(ao_points[i]->tag, tag))
    {
      return ao_points[i];
    }
  }

  for (i=0; i < num_di; i++)
  {
    if (0 == strcasecmp(di_points[i]->tag, tag))
    {
      return di_points[i];
    }
  }


  for (i=0; i < num_do; i++)
  {
    if (0 == strcasecmp(do_points[i]->tag, tag))
    {
      return do_points[i];
    }
  }

  for (i=0; i < num_calc; i++)
  {
    if (0 == strcasecmp(calcs[i]->tag, tag))
    {
      return calcs[i];
    }
  }

  for (i=0; i < num_d_calc; i++)
  {
    if (0 == strcasecmp(d_calcs[i]->tag, tag))
    {
      return d_calcs[i];
    }
  }

  for (i=0; i < num_dcalc; i++)
  {
    if (0 == strcasecmp(dcalcs[i]->tag, tag))
    {
      return dcalcs[i];
    }
  }

  for (i=0; i < num_timer; i++)
  {
    if (0 == strcasecmp(timers[i]->tag, tag))
    {
      return timers[i];
    }
  }

  for (i=0; i < num_analog_val; i++)
  {
    if (0 == strcasecmp(analog_vals[i]->tag, tag))
    {
      return analog_vals[i];
    }
  }

  for (i=0; i < num_discrete_val; i++)
  {
    if (0 == strcasecmp(discrete_vals[i]->tag, tag))
    {
      return discrete_vals[i];
    }
  }


  for (i=0; i < num_int; i++)
  {
    if (0 == strcasecmp(ints[i]->tag, tag))
    {
      return ints[i];
    }
  }

  for (i=0; i < num_pid; i++)
  {
    if (0 == strcasecmp(pid_points[i]->tag, tag))
    {
      return pid_points[i];
    }
  }

  for (i=0; i < num_pump; i++)
  {
    if (0 == strcasecmp(pump_points[i]->tag, tag))
    {
      return pump_points[i];
    }
  }

  for (i=0; i < num_ac; i++)
  {
    if (0 == strcasecmp(ac_points[i]->tag, tag))
    {
      return ac_points[i];
    }
  }


  for (i=0; i < num_level; i++)
  {
    if (0 == strcasecmp(level_points[i]->tag, tag))
    {
      return level_points[i];
    }
  }


  for (i=0; i < num_data; i++)
  {
    if (0 == strcasecmp(data_points[i]->tag, tag))
    {
      return data_points[i];
    }
  }

  for (i=0; i < num_file_logger; i++)
  {
    if (0 == strcasecmp(file_logger_points[i]->tag, tag))
    {
      return file_logger_points[i];
    }
  }

  for (i=0; i < num_web_logger; i++)
  {
    if (0 == strcasecmp(web_logger_points[i]->tag, tag))
    {
      return web_logger_points[i];
    }
  }

  for (i=0; i < num_discrete_logger; i++)
  {
    if (0 == strcasecmp(discrete_logger_points[i]->tag, tag))
    {
      return discrete_logger_points[i];
    }
  }

  for (i=0; i < num_scan; i++)
  {
    if (0 == strcasecmp(scan_points[i]->tag, tag))
    {
      return scan_points[i];
    }
  }

  for (i=0; i < num_web; i++)
  {
    if (0 == strcasecmp(web_points[i]->tag, tag))
    {
      return web_points[i];
    }
  }

  return NULL;
}
/***********************************************************************/

