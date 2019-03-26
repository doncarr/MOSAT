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

/********
Db.h

Header file containing main real time database structure.

**************************************************************************/

#ifndef __DB_INC__
#define __DB_INC__

class react_base_t;
class react_drv_base_t;

#include <string>
#include <unordered_map>

#include "db_point.h"
#include "dbref.h"
#include "secuencia.h"
#include "iodriver.h"
#include "displaydata.h"

typedef std::unordered_map <std::string, db_point_t *> react_map_t;

#define MAX_POINT_TYPES (100)
class react_drv_base_t
{
public:
  virtual double get_time(void) = 0;
  virtual ~react_drv_base_t(void) {};
};

class react_base_t : public react_drv_base_t
{
public:
  //virtual double get_time(void) = 0;
  virtual double get_sample_rate(void) = 0;
  virtual const char *get_home_dir(void) = 0;
  virtual void send_do(int drv, int crd, int chnl, bool val) = 0;
  virtual void send_ao(int drv, int crd, int chnl, double val) = 0;
  virtual db_point_t *get_db_point(const char *tag) = 0;
  //rt_bool_ref_t *get_bool_ref(char *tag);
  //rt_double_ref_t *get_double_ref(char *tag);
  virtual bool global_alarm_disabled(void) = 0;
  virtual void set_shutdown(void) = 0;
  virtual void clear_kb_buf(void) = 0;
  virtual bool kbhit(void) = 0;
  virtual void new_secuencia(secuencia_t *s) = 0;
  virtual const char *secuencia_name(void) = 0;
  virtual rt_double_ref_t *get_double_ref(const char *expr, char *err, int sz) = 0;
  virtual rt_bool_ref_t *get_bool_ref(const char *expr, char *err, int sz) = 0;
  virtual rt_long_ref_t *get_long_ref(const char *expr, char *err, int sz) = 0;
  virtual ~react_base_t(void) {};
};

typedef db_point_factory_t *(*get_db_factory_t)(react_drv_base_t *r);
db_point_factory_t *load_db_point(const char *sofile);

class react_t;
struct serve_thread_data_t
{
  int sock_fd;
  int n;
  react_t *react;
};


class react_t : public react_base_t
{
private:
  int qid;
  int shmid;
  void *shmp;
  display_info_t dinfo;
  char home_dir[200];
  double current_time;
  double next_sms_check;
  double sample_rate;
  bool keyboard_is_on;
  react_map_t map;
  db_point_factory_t *point_factory[MAX_POINT_TYPES];
  db_point_t **db_points[MAX_POINT_TYPES];
  db_point_t **read_one_point_type(db_point_factory_t *factory, const char *a_home_dir, int *n);
  void read_factory_points(const char *a_home_dir);
  DISALLOW_COPY_AND_ASSIGN(react_t);
public:
  bool global_alarm_disable;
  ai_point_t **ai_points;
  int num_ai;
  pci_point_t **pci_points;
  int num_pci;
  ao_point_t **ao_points;
  int num_ao;
  di_point_t **di_points;
  int num_di;
  do_point_t **do_points;
  int num_do;
  calc_point_t **calcs;
  int num_calc;
  timer_point_t **timers;
  int num_timer;
  analog_value_point_t **analog_vals;
  int num_analog_val;
  discrete_value_point_t **discrete_vals;
  int num_discrete_val;
  dcalc_point_t **d_calcs;
  int num_d_calc;
  //dcalc_t **dcalcs;
  //int num_dcalc;
  int_t **ints;
  int num_int;
  pid_point_t **pid_points;
  int num_pid;
  remote_pid_t **rpid_points;
  int num_rpid;
  pump_point_t **pump_points;
  int num_pump;
  ac_point_t **ac_points;
  int num_ac;
  level_point_t **level_points;
  int num_level;
  data_point_t **data_points;
  int num_data;
  file_logger_t **file_logger_points;
  int num_file_logger;
  web_logger_t **web_logger_points;
  int num_web_logger;
  discrete_logger_t **discrete_logger_points;
  int num_discrete_logger;
  scan_point_t **scan_points;
  int num_scan;
  web_point_t **web_points;
  int num_web;
  io_driver_t *io_driver[10];
  int num_io_drivers;
  secuencia_t *secuencia;
  bool shutdown;
  void read_inputs(void);
  void verify_drivers(void);
  void read_secuencia(const char *name, const char *a_home_dir);
  void read_background_sequences(const char *a_home_dir, const char *a_seq_dir);
  void execute_background_scripts(void);
  void new_secuencia(secuencia_t *s);
  const char *secuencia_name(void);
  bool execute_secuencia(void);
  bool update(double theTime, bool execute_script);
  void read_io_cards(void);
  react_t();
  void read_all_points(const char *a_home_dir);
  void print_all_points(void);
  void send_do(int drv, int crd, int chnl, bool val);
  void send_ao(int drv, int crd, int chnl, double val);
  void init_driver(void);
  void set_time(double tnow) {current_time = tnow;};
  void set_sample_rate(double rate) {sample_rate = rate;};
  double get_sample_rate(void) {return sample_rate;};
  double get_time(void) {return current_time;};
  void set_shutdown(void);
  db_point_t *get_db_point(const char *tag);
  void index_db_point(db_point_t *dbp);
  void clear_kb_buf(void);
  void exit_clean_up(void);
  bool kbhit(void);

  /**
  void check_msg_queue(void);
  void init_msg_queue(void);
  void delete_msg_queue(void);
  void init_shared_memory(void);
  void fill_shared_memory(void);
  void update_shared_memory(void);
  void delete_shared_memory(void);
  **/

  void set_home_dir(const char *dir);
  const char *get_home_dir(void);
  void keyboard_on(bool v){keyboard_is_on = v;};
  bool global_alarm_disabled(void) {return global_alarm_disable;};
  void init_sms(void);
  void check_sms(void);
  rt_double_ref_t *get_double_ref(const char *expr, char *err, int sz) {snprintf(err, sz, "Not implemented"); return NULL;};
  rt_bool_ref_t *get_bool_ref(const char *expr, char *err, int sz) {snprintf(err, sz, "Not implemented"); return NULL;};
  rt_long_ref_t *get_long_ref(const char *expr, char *err, int sz) {snprintf(err, sz, "Not implemented"); return NULL;};
  void check_write_tag_queue(void);
  void open_write_tag_queue(void);
  void serve_client(serve_thread_data_t *st);
  void wait_connections(void);
  void setup_connection_handler(void); 
};

extern react_base_t *db;
extern bool signal_recieved;
int read_fns_start_hook(const char *path);
void read_fns_end_hook(void);


#endif
