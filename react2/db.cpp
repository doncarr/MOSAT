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
#include <pthread.h>
 
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
      logfile->vprint("\t%s\n", do_points[i]->get_tag());
      do_points[i]->send(false);
    }
    if (num_ao > 0) logfile->vprint("Zeroing %d analog outputs:\n", num_ao);
    for (int i=0; i < num_ao; i++)
    {
      logfile->vprint("\t%s\n", ao_points[i]->get_tag());
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
    logfile->vprint("DI: %s\n", di_points[i]->get_tag());
  }
  for (int i=0; i < num_do; i++)
  {
    logfile->vprint("DO: %s\n", do_points[i]->get_tag());
  }
  for (int i=0; i < num_calc; i++)
  {
    logfile->vprint("Calc: %s\n", calcs[i]->get_tag());
  }
  for (int i=0; i < num_d_calc; i++)
  {
    logfile->vprint("DiscreteCalc: %s\n", d_calcs[i]->get_tag());
  }
  /**
  for (int i=0; i < num_dcalc; i++)
  {
    logfile->vprint("DCalc: %s\n", dcalcs[i]->get_tag());
  }
  **/
  for (int i=0; i < num_timer; i++)
  {
    logfile->vprint("Timer: %s\n", timers[i]->get_tag());
  }
  for (int i=0; i < num_analog_val; i++)
  {
    logfile->vprint("Analog value: %s\n", analog_vals[i]->get_tag());
  }
  for (int i=0; i < num_discrete_val; i++)
  {
    logfile->vprint("Discrete value: %s\n", discrete_vals[i]->get_tag());
  }
  for (int i=0; i < num_int; i++)
  {
    logfile->vprint("Int: %s\n", ints[i]->get_tag());
  }
  for (int i=0; i < num_ai; i++)
  {
    logfile->vprint("AI: %s\n", ai_points[i]->get_tag());
  }
  for (int i=0; i < num_ao; i++)
  {
    logfile->vprint("AO: %s\n", ao_points[i]->get_tag());
  }
  for (int i=0; i < num_pci; i++)
  {
    logfile->vprint("PCI: %s\n", pci_points[i]->get_tag());
  }
  for (int i=0; i < num_pid; i++)
  {
    logfile->vprint("PID: %s\n", pid_points[i]->get_tag());
  }
  for (int i=0; i < num_rpid; i++)
  {
    logfile->vprint("PID: %s\n", rpid_points[i]->get_tag());
  }
  for (int i=0; i < num_pump; i++)
  {
    logfile->vprint("PUMP: %s\n", pump_points[i]->get_tag());
  }
  for (int i=0; i < num_ac; i++)
  {
    logfile->vprint("AC: %s\n", ac_points[i]->get_tag());
  }
  for (int i=0; i < num_level; i++)
  {
    logfile->vprint("LEVEL: %s\n", level_points[i]->get_tag());
  }
  for (int i=0; i < num_data; i++)
  {
    logfile->vprint("Data: %s\n", data_points[i]->get_tag());
  }
  for (int i=0; i < num_file_logger; i++)
  {
    logfile->vprint("FileLogger: %s\n", file_logger_points[i]->get_tag());
  }
  for (int i=0; i < num_web_logger; i++)
  {
    logfile->vprint("FileLogger: %s\n", web_logger_points[i]->get_tag());
  }
  for (int i=0; i < num_discrete_logger; i++)
  {
    logfile->vprint("DiscreteLogger: %s\n", discrete_logger_points[i]->get_tag());
  }
  for (int i=0; i < num_scan; i++)
  {
    logfile->vprint("Scan: %s\n", scan_points[i]->get_tag());
  }
  for (int i=0; i < num_web; i++)
  {
    logfile->vprint("Web: %s\n", web_points[i]->get_tag());
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
      logfile->vprint("Driver out of range for ai point %s: %d\n", ai_points[i]->get_tag(), drv); 
      exit(0);
    } 
  }
  for (int i=0; i < num_di; i++)
  {
    int drv = di_points[i]->get_driver();
    if ((drv < 0) || (drv >= num_io_drivers))
    {
      logfile->vprint("Driver out of range for di point %s: %d\n", di_points[i]->get_tag(), drv); 
      exit(0);
    } 
  }
  for (int i=0; i < num_do; i++)
  {
    int drv = do_points[i]->get_driver();
    if ((drv < 0) || (drv >= num_io_drivers))
    {
      logfile->vprint("Driver out of range for do point %s: %d\n", do_points[i]->get_tag(), drv); 
      exit(0);
    } 
  }
  for (int i=0; i < num_ao; i++)
  {
    int drv = ao_points[i]->get_driver();
    if ((drv < 0) || (drv >= num_io_drivers))
    {
      logfile->vprint("Driver out of range for ao point %s: %d\n", ao_points[i]->get_tag(), drv); 
      exit(0);
    } 
  }
  for (int i=0; i < num_pci; i++)
  {
    int drv = pci_points[i]->get_driver();
    if ((drv < 0) || (drv >= num_io_drivers))
    {
      logfile->vprint("Driver out of range for pci point %s: %d\n", ai_points[i]->get_tag(), drv); 
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
  //printf("%d di_points updated\n", num_di);
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
  //printf("%d ai_points updated\n", num_ai);
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
  for (int i=0; i < num_rpid; i++)
  {
    rpid_points[i]->update();
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
    ao_points[i]->update();
  }
  //printf("%d ao_points updated\n", num_ao);
  taa[j].stop();

  j++;
  taa[j].start();
  for (int i=0; i < num_do; i++)
  {
    do_points[i]->update();
  }
  //printf("%d do_points updated\n", num_do);
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
  /**
  for (int i=0; i < num_dcalc; i++)
  {
    dcalcs[i]->update();
  }
  **/
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

const bool *react_get_discrete_ptr_fn(char *tag)
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

const double *react_get_analog_ptr_fn(char *tag)
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
  //num_dcalc = 0;
  //dcalcs = NULL;
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
  num_rpid = 0;
  rpid_points = NULL;
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
  int max_points = 100;
  db_point_t **dbps =
	(db_point_t **) malloc(max_points * sizeof(db_point_t*));
  MALLOC_CHECK(dbps);

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

    dbps[n] = factory->new_point(argc, argv, errbuf, sizeof(errbuf));

    if (dbps[n] == NULL)
    {
      logfile->vprint("%s:%d %s\n", path, i+1, errbuf);
      continue;
    }
    logfile->vprint("%s", line);
    n++;
    if (n >= max_points)
    {
      max_points += 50;
      dbps = (db_point_t **) realloc(dbps,
	      max_points * sizeof(db_point_t*));
      MALLOC_CHECK(dbps);
    }
  }
  *cnt = n;
  if (n == 0)
  {
    free(dbps);
    return NULL;
  }
  return dbps;
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
    //ai_points = read_one_point_type(factory, a_home_dir, &num_ai);
  }
}

/************************************************************************/

void react_t::read_all_points(const char *a_home_dir)
{
  /* Read all database points from disk. */

  /* Take it out for now to make it work for all types
    this->read_factory_points(a_home_dir);
  */

  //db_point_factory_t *aif = new ai_point_factory_t;
  /**
  db_point_factory_t *aif = load_db_point("./ai.so");
  if (aif == NULL)
  {
    exit(0);
  }

  ai_points = read_one_point_type(aif, a_home_dir, &num_ai);
  ***/
  if (0 != read_fns_start_hook(a_home_dir))
  {
    logfile->vprint("Error initalizing read functions, exiting  ........\n");
    exit(0);
  }

  logfile->vprint("Reading ai ........\n");
  ai_points = ai_point_t::read(&num_ai, a_home_dir);
  for (int i=0; i < num_ai; i++)
  {
    index_db_point(ai_points[i]);
  }

  /**
  printf("Reading ai ........\n");
  //ai_points = read_ai_points(&num_ai, a_home_dir);
  ai_points = ai_point_t::read(&num_ai, a_home_dir);
  **/

  logfile->vprint("Reading ao ........\n");
  ao_points = ao_point_t::read(&num_ao, a_home_dir);
  for (int i=0; i < num_ao; i++)
  {
    logfile->vprint("Indexing ao: %s\n", ao_points[i]->get_tag());
    index_db_point(ao_points[i]);
  }

  logfile->vprint("Reading di ........\n");
  di_points = di_point_t::read(&num_di, a_home_dir);
  for (int i=0; i < num_di; i++)
  {
    index_db_point(di_points[i]);
  }

  logfile->vprint("Reading do ........\n");
  do_points = do_point_t::read(&num_do, a_home_dir);
  for (int i=0; i < num_do; i++)
  {
    printf("****************** adding do: %s\n", do_points[i]->get_tag());
    index_db_point(do_points[i]);
  }

  logfile->vprint("Reading pci ........\n");
  pci_points = pci_point_t::read(&num_pci, a_home_dir);
  for (int i=0; i < num_pci; i++)
  {
    index_db_point(pci_points[i]);
  }

  logfile->vprint("Reading calc ........\n");
  calcs = calc_point_t::read(&num_calc, a_home_dir);
  for (int i=0; i < num_calc; i++)
  {
    index_db_point(calcs[i]);
  }


  logfile->vprint("Reading discrete calc ........\n");
  d_calcs = dcalc_point_t::read(&num_d_calc, a_home_dir);
  for (int i=0; i < num_d_calc; i++)
  {
    index_db_point(d_calcs[i]);
  }
  logfile->vprint("%d discrete calcs read\n", num_d_calc);

  /**
  logfile->vprint("Reading dcalc ........\n");
  dcalcs = dcalc_t::read(&num_dcalc, a_home_dir);
  for (int i=0; i < num_dcalc; i++)
  {
    index_db_point(dcalcs[i]);
  }
  **/

  logfile->vprint("Reading timer ........\n");
  timers = timer_point_t::read(&num_timer, a_home_dir);
  for (int i=0; i < num_timer; i++)
  {
    index_db_point(timers[i]);
  }
  logfile->vprint("num_timer = %d\n", num_timer);

  logfile->vprint("Reading analog values ........\n");
  analog_vals = analog_value_point_t::read(&num_analog_val, a_home_dir);
  for (int i=0; i < num_analog_val; i++)
  {
    index_db_point(analog_vals[i]);
  }
  logfile->vprint("num analog values = %d\n", num_analog_val);

  logfile->vprint("Reading discrete values ........\n");
  discrete_vals = discrete_value_point_t::read(&num_discrete_val, a_home_dir);
  for (int i=0; i < num_discrete_val; i++)
  {
    index_db_point(discrete_vals[i]);
  }
  logfile->vprint("num discrete values = %d\n", num_discrete_val);

  logfile->vprint("Reading int ........\n");
  ints = int_t::read(&num_int, a_home_dir);
  for (int i=0; i < num_int; i++)
  {
    index_db_point(ints[i]);
  }

  logfile->vprint("Reading pid ........\n");
  pid_points = pid_point_t::read(&num_pid, a_home_dir);
  for (int i=0; i < num_pid; i++)
  {
    index_db_point(pid_points[i]);
  }

  logfile->vprint("Reading remote pid ........\n");
  rpid_points = remote_pid_t::read(&num_rpid, a_home_dir);
  for (int i=0; i < num_rpid; i++)
  {
    index_db_point(rpid_points[i]);
  }

  logfile->vprint("Reading pump ........\n");
  pump_points = pump_point_t::read(&num_pump, a_home_dir);
  for (int i=0; i < num_pump; i++)
  {
    index_db_point(pump_points[i]);
  }

  logfile->vprint("Reading ac ........\n");
  ac_points = ac_point_t::read(&num_ac, a_home_dir);
  for (int i=0; i < num_ac; i++)
  {
    index_db_point(ac_points[i]);
  }

  logfile->vprint("Reading level ........\n");
  level_points = level_point_t::read(&num_level, a_home_dir);
  for (int i=0; i < num_level; i++)
  {
    index_db_point(level_points[i]);
  }

  logfile->vprint("Reading data ........\n");
  data_points = data_point_t::read(&num_data, a_home_dir);
  for (int i=0; i < num_data; i++)
  {
    index_db_point(data_points[i]);
  }

  logfile->vprint("Reading file logger ........\n");
  file_logger_points = file_logger_t::read(&num_file_logger, a_home_dir);
  for (int i=0; i < num_file_logger; i++)
  {
    index_db_point(file_logger_points[i]);
  }

  logfile->vprint("Reading web logger ........\n");
  web_logger_points = web_logger_t::read(&num_web_logger, a_home_dir);
  for (int i=0; i < num_web_logger; i++)
  {
    index_db_point(web_logger_points[i]);
  }

  logfile->vprint("Reading discrete logger ........\n");
  discrete_logger_points = discrete_logger_t::read(&num_discrete_logger, a_home_dir);
  for (int i=0; i < num_discrete_logger; i++)
  {
    index_db_point(discrete_logger_points[i]);
  }

  logfile->vprint("Reading scan ........\n");
  scan_points = scan_point_t::read(&num_scan, a_home_dir);
  for (int i=0; i < num_scan; i++)
  {
    index_db_point(scan_points[i]);
  }

  logfile->vprint("Reading web points ........\n");
  web_points = web_point_t::read(&num_web, a_home_dir);
  for (int i=0; i < num_web; i++)
  {
    index_db_point(web_points[i]);
  }

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
  read_fns_end_hook();

  this->setup_connection_handler();
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
  sem_init(&point_lock, 0, 1); // Initilize the point lock
}

/***********************************************************************/

rt_double_ref_t *db_point_t::get_double_ref(const char *expr, char *err, int sz) 
{
  //pv_type_t pv_type(void) {return ANALOG_VALUE;};
  if (ANALOG_VALUE == this->pv_type())
  {
    analog_point_t *ap = (analog_point_t *) this;
    return new rt_double_ptr_ref_t(ap->pv_ptr());  
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

void react_t::index_db_point(db_point_t *dbp)
{
  char tag[50];

  snprintf(tag, sizeof(tag), "%s", dbp->get_tag());
  for (char *p=tag; *p != '\0'; p++) 
  {
    *p = tolower(*p);
  }
  if (NULL != map[tag])
  {
    logfile->vprint("*** Error, duplicate tag: %s\n", tag);
    return;
  }
  printf("@@@@@@@@@@@@@@@@@@@@@@@@@ Indexing: %s, map size = %d\n", tag, int(map.size()));
  map[tag] = dbp; 
}

/*************************************************************************/

db_point_t *react_t::get_db_point(const char *the_tag)
{
  /* This returns a pointer to a database point given the tag. */

  char tag[50];
  snprintf(tag, sizeof(tag), "%s", the_tag);
  for (char *p=tag; *p != '\0'; p++) 
  {
    *p = tolower(*p);
  }
  return  map[tag];
}

/***********************************************************************/
static FILE *sock_log_fp = NULL;

static const char *get_face(react_t *react, const char *tag)
{
  const char *val;
  db_point_t *db_point;
  fprintf(sock_log_fp, "Faceplate call !! Tag  = %s\n", tag);
  fflush(sock_log_fp);
  db_point = react->get_db_point(tag);
  if (db_point == NULL)
  {
    fprintf(sock_log_fp, "Tagname NOT found: = %s\n", tag);
    fflush(sock_log_fp);
    return "<br><h1>Tagname NOT Found</h1><br>\n";
  }
  val = db_point->get_faceplate();
  fprintf(sock_log_fp, "db_point->get_faceplate returned:: %s\n", val);
  fflush(sock_log_fp);
  return val;
}

/*************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>

void react_t::serve_client(serve_thread_data_t *st)
{
  int sock_fd = st->sock_fd;
  int nc = st->n;
  delete st; 
  char buf[2000];
  char buf_out[2000];
  fprintf(sock_log_fp, "In react_t::serve_client(), sock_fd = %d, n = %d\n", sock_fd, nc);
  fflush(sock_log_fp);
  int n_req = 0;
  //analog_point_t *analog_point = NULL;
  db_point_t *db_point = NULL;
  delim_separator_t ds_tag(2000, 200, '+');
  while (true)
  {
    const char *msg = "React responded!!!!";
    int n = read(sock_fd, buf, sizeof(buf)); 
    n_req++;
    if (n <= 0)
    {
      fprintf(sock_log_fp, "react_t::serve_client(), req # %d, error on READ socket %d, n = %d\n", n_req, sock_fd, nc);
      fprintf(sock_log_fp, "react_t::serve_client(), closing socket and exiting thread\n");
      fflush(sock_log_fp);
      close(sock_fd);
      pthread_exit(NULL);
    }
    buf[n] = '\0';
    char *bstart = buf; 
    char *bnext = NULL; 
    int n_so_far = 0;
    int k=0;
    for (bstart = buf; bstart != NULL; bstart = bnext)
    {
      //fprintf(sock_log_fp, "Got message of %d bytes on fd %d, n %d: %s, req # %d\n", n, sock_fd, nc, buf, n_req);
      bool bfound = false;
      for (; k < n; k++)
      {
        if (buf[k] == '\0')
        {
          bfound = true;
          n_so_far += k;
          if (k == (n-1))  
          {
            bnext = NULL;
            break;
          }
          else
          {
            bnext = buf + k + 1;
            k++;
            fprintf(sock_log_fp, "at least one more message: '%s'\n", bnext);
            break;
          }
        }

      }
      if (!bfound)
      {
        fprintf(sock_log_fp, "NO terminating char: '%s'\n", bstart);
        bnext = NULL;
      }
      fflush(sock_log_fp);
      if (0 == strncmp(bstart, "faceplate", 9))
      {
        msg = get_face(this, bstart + 10); 
      }
      else if (0 == strncasecmp(bstart, "config", 6))
      {
        char *tag = bstart + 7;
        ltrim(tag); 
        rtrim(tag); 

        //analog_point = NULL;
        db_point = db->get_db_point(tag);

        if (db_point == NULL)
        {
          fprintf(sock_log_fp, "NOT a database point: %s\n", tag);
          snprintf(buf_out, sizeof(buf_out), "{\"pv\":99.99,\"eulabel\":\"not found: %s\"}", tag);
          msg = buf_out;
        }
        else
        {
          msg = db_point->get_config_json();
        }
      }
      else if (0 == strncasecmp(bstart, "tag", 3))
      {
        printf("msg: %s\n", bstart);
        char *tag_string = bstart + 4;
        ltrim(tag_string); 
        rtrim(tag_string); 
        char **tag_argv;
        int tag_argc;
        tag_argv = ds_tag.separate(&tag_argc, tag_string);

        snprintf(buf_out, sizeof(buf_out), "[");
        printf("Tags[%d]: ", tag_argc);
        for (int i=0; i < tag_argc; i++)
        {
          if (i != 0) safe_strcat(buf_out, ",", sizeof(buf_out));
          printf("%s ", tag_argv[i]);
          db_point = db->get_db_point(tag_argv[i]);
          if (db_point == NULL)
          {
            safe_strcat(buf_out, "0", sizeof(buf_out));
            fprintf(sock_log_fp, "Tag not found: %s\n", tag_argv[i]);
          }
          else
          {
            char val_buf[30];
            db_point->get_pv_json(val_buf, sizeof(val_buf));
            safe_strcat(buf_out, val_buf, sizeof(buf_out));
            //printf("tag: %s, val: %s\n", tag_argv[i], val_buf);
          }
        } 
        printf("\n");
        safe_strcat(buf_out, "]", sizeof(buf_out));
        msg = buf_out;
      }
      else if (0 == strncasecmp(bstart, "output", 6))
      {
        printf("msg: %s\n", bstart);
        char *output_string = bstart + 7;
        ltrim(output_string);
        rtrim(output_string);
        char **output_argv;
        int output_argc;
        output_argv = ds_tag.separate(&output_argc, output_string);
        if (output_argc != 2)
        {
          msg = "Wrong number of args";
        }
        else
        {
          db_point = db->get_db_point(output_argv[0]);
          if (db_point == NULL)
          {
            msg = "Tag not found";
          }
          else
          {
            db_point->set_json("pv", output_argv[1]); 
            msg = "OK";
          }
        }
      }
      else
      {
        snprintf(buf_out, sizeof(buf_out), "Unknown Command: '%s'\n", buf);
        msg = buf_out;
      }
      printf("reply: %s\n", msg);
      n = write(sock_fd, msg, strlen(msg)+1); // include a null 
      if (n <= 0)
      {
        fprintf(sock_log_fp, "react_t::serve_client(), error on WRITE socket %d, n = %d\n", sock_fd, nc);
        fprintf(sock_log_fp, "react_t::serve_client(), closing socket and exiting thread\n");
        fflush(sock_log_fp);
        close(sock_fd);
        pthread_exit(NULL);
      }
    }
  }
}

/*************************************************************************/


static void *serve_cl(void *p)
{
  serve_thread_data_t *st = (serve_thread_data_t *) p;
  fprintf(sock_log_fp, "In serve_cl(), sock_fd = %d, n = %d\n", st->sock_fd, st->n);
  fflush(sock_log_fp);
  st->react->serve_client(st);
  return NULL; // serve_client should never return, but we must return a value from non void functions. 
}

/*************************************************************************/

void react_t::wait_connections(void)
{
  const short the_port = 5707;
  const int the_backlog = 100;
  int sockfd, new_fd; 
  int n_connect = 0;
  struct sockaddr_in my_addr;  
  struct sockaddr_in their_addr; 
  socklen_t sin_size;
  int retv;
  char errline[100];

  logfile->vprint("In react_t::wait_connections()\n");
  fprintf(sock_log_fp, "In react_t::wait_connections()\n");
  fflush(sock_log_fp);

  sockfd = socket(AF_INET, SOCK_STREAM, 0); 

  my_addr.sin_family = AF_INET; 
  my_addr.sin_port = htons(the_port);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(my_addr.sin_zero), 0, 8); 

  retv = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));
  if (retv != 0)
  {
    errline[0] = '\0';
    strerror_r(errno, errline, sizeof(errline));
    fprintf(sock_log_fp, "bind error: %s\n", errline);
    logfile->vprint("react_t::wait_connections(), bind error: %s\n", errline);
    fflush(sock_log_fp);
    return;
  }

  retv = listen(sockfd, the_backlog);
  if (retv != 0)
  {
    errline[0] = '\0';
    strerror_r(errno, errline, sizeof(errline));
    fprintf(sock_log_fp, "listen error: %s\n", errline);
    logfile->vprint("react_t::wait_connections(), listen error: %s\n", errline);
    fflush(sock_log_fp);
    return;
  }


  sin_size = sizeof(struct sockaddr_in);

  logfile->vprint("react_t::wait_connections(), All OK, waiting for connections on port %d\n", the_port);
  while (true)
  {
    fprintf(sock_log_fp, "Waiting for a connection!!\n");
    fflush(sock_log_fp);
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1)
    {
      errline[0] = '\0';
      strerror_r(errno, errline, sizeof(errline));
      fprintf(sock_log_fp, "accept: %s\n", errline);
      fflush(sock_log_fp);
      return;
    }
    fprintf(sock_log_fp, "New connection: %d\n", new_fd);
    fflush(sock_log_fp);
    n_connect++;
    pthread_t thr;
    serve_thread_data_t *st = new serve_thread_data_t;
    st->sock_fd = new_fd;
    st->n = n_connect;
    st->react = this;
    int retval = pthread_create(&thr, NULL, serve_cl, st);
    if (retval != 0)
    {
      perror("can't create thread");
    }
    pthread_detach(thr);
  }
}

/*************************************************************************/

static void *wait_conn(void *p)
{
  fprintf(sock_log_fp, "In wait_conn()\n");
  fflush(sock_log_fp);
  react_t *rp = (react_t *) p;
  rp->wait_connections();
  return NULL;
}

/*************************************************************************/

void react_t::setup_connection_handler(void)
{
  sock_log_fp = fopen("sock_log.txt", "a");
  if (sock_log_fp == NULL)
  {
    perror("sock_log.txt");
    printf("Can not open sock log file\n");
    exit(0);
  }
  fprintf(sock_log_fp, "file opened\n");
  fflush(sock_log_fp);

  pthread_t thr;
  logfile->vprint("Creating thread to wait on connections . . . \n");
  int retval = pthread_create(&thr, NULL, wait_conn, this);
  if (retval != 0)
  {
    perror("can't create thread");
    logfile->vprint("************* Cound not create thread to wait on connections\n");
    exit(0);
  }
  pthread_detach(thr);
}

/*************************************************************************/
//static int xsemcnt=0;
/****************
The followig is used to lock an object during method execution. This
helps eliminate bugs, by assuring that the post will automatically
be called on exit, as, C++ automatically calls the destructor of
objects allowcated on the stack. In other words, the constructor
calls "sem_wait", and, the destructor calls "sem_post";

However, be VERY careful. You must never call another member function that
also has a lock (instant deadlock). The best policy, is, to put this in all public
member functions, but, NOT in private member functions. Then, public member
functions are also prohibited from calling other public member functions.
****************/

point_lock_t::point_lock_t(sem_t *s, const char *t)
{
  //xsemcnt--;
  //n=0;
  //tag=t;
  sem=s;
  sem_wait(sem);
  //printf("'%s' locked\n",t);
}

point_lock_t::~point_lock_t(void)
{
  //xsemcnt++;
  //n++;
  //printf("'%s' un-locked %d\n",tag, n);
  //int sv;
  //sem_getvalue(sem, &sv);
  /*int r=*/sem_post(sem);
  //printf("sem ret: %d, p: %p, sv: %d, cnt: %d\n",r,sem, sv, xsemcnt);
}

/*************************************************************************/

