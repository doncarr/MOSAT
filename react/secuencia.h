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

#ifndef __SECUENCIA_INC__
#define __SECUENCIA_INC__

#include <limits.h>
#include "logfile.h"
#include "reactio.h"

#define SECUENCIA_LOOP (INT_MAX - 1)
#define SECUENCIA_SKIP_TO_ENDIF (INT_MAX - 2)

enum script_param_t
{
  PARAM_DOUBLE, PARAM_BOOL, PARAM_INT, PARAM_BAD, PARAM_STRING, PARAM_VOID
};


enum step_type_t
{
  STEP_SENDDO, STEP_WAIT, STEP_WAITKEY, STEP_STARTDAQ,
  STEP_STOPDAQ, STEP_PRINTMSG, STEP_MARK, STEP_LOOP, 
  STEP_IF, STEP_ENDIF, STEP_UNDEFINED, STEP_YIELD
};

/***/

class secuencia_step_t
{
private:
protected:
public:
  char text[50];
  virtual void reset(void) = 0;
  virtual bool check(void) = 0;
  virtual int execute(double time) = 0;
  virtual void set_param(int n, char *val) = 0;
  virtual step_type_t get_type() = 0;
  virtual ~secuencia_step_t(void) {};
};

/*****************
class step_sendDO_t : public secuencia_step_t
{
private:
  char *do_tag;
  do_point_t *do_point;
  bool value_to_send;
public:
  step_sendDO_t(int argc, char *argv[], char *error, int esize);
  bool check(void);
  int execute(double time);
};
******************/

class step_get_key_t : public secuencia_step_t
{
private:
  bool first_time;
public:
  step_get_key_t(int argc, char *argv[], char *error, int esize);
  bool check(void);
  void reset(void) {first_time = true;};
  void set_param(int n, char *val);
  int execute(double time);
  step_type_t get_type() {return STEP_UNDEFINED;};
};

class step_mark_t : public secuencia_step_t
{
private:
  bool first_time;
public:
  step_mark_t(int argc, char *argv[], char *error, int esize);
  bool check(void);
  void reset(void) {first_time = true;};
  void set_param(int n, char *val);
  int execute(double time);
  step_type_t get_type() {return STEP_MARK;};
};

class step_endif_t : public secuencia_step_t
{
private:
  bool first_time;
public:
  step_endif_t(int argc, char *argv[], char *error, int esize){};
  bool check(void) {return true;};
  void reset(void) {first_time = true;};
  void set_param(int n, char *val) {};
  int execute(double time) 
    {logfile->vprint("sys.endif()\n"); return 1;};
  step_type_t get_type() {return STEP_ENDIF;};
};

class step_wait_t : public secuencia_step_t
{
private:
  double wait_time;
  double end_time;
  bool first_time;
public:
  step_wait_t(int argc, char *argv[], char *error, int esize);
  bool check(void);
  void reset(void) {first_time = true;};
  void set_param(int n, char *val);
  int execute(double time);
  step_type_t get_type() {return STEP_UNDEFINED;};
};


class step_if_t : public secuencia_step_t
{
private:
  const char *expr_string;
  discrete_expr_t expression;
public:
  step_if_t(int argc, char *argv[], char *error, int esize);
  bool check(void);
  void reset(void) {};
  void set_param(int n, char *val);
  int execute(double time);
  step_type_t get_type() {return STEP_UNDEFINED;};
};

class step_wait_until_t : public secuencia_step_t
{
private:
  double timeout;
  double end_time;
  bool first_time;
  const char *expr_string;
  discrete_expr_t expression;
public:
  step_wait_until_t(int argc, char *argv[], char *error, int esize);
  bool check(void);
  void reset(void) {first_time = true;};
  void set_param(int n, char *val);
  int execute(double time);
  step_type_t get_type() {return STEP_UNDEFINED;};
};

class step_wait_until_min_t : public secuencia_step_t
{
private:
  double timeout;
  double min_true;
  double end_time;
  bool first_time;
  bool detected;
  time_t detect_time;
  const char *expr_string;
  discrete_expr_t expression;
public:
  step_wait_until_min_t(int argc, char *argv[], char *error, int esize);
  bool check(void);
  void reset(void) {first_time = true;};
  void set_param(int n, char *val);
  int execute(double time);
  step_type_t get_type() {return STEP_UNDEFINED;};
};


class step_start_acq_t : public secuencia_step_t
{
private:
  tag_t ai_tag;
  char file_name[80];
public:
  void reset(void) {};
  step_start_acq_t(int argc, char *argv[], char *error, int esize);
  bool check(void);
  void set_param(int n, char *val);
  int execute(double time);
  step_type_t get_type() {return STEP_UNDEFINED;};
};

class step_stop_acq_t : public secuencia_step_t
{
private:
  char file_name[80];
public:
  void reset(void) {};
  void set_param(int n, char *val);
  step_stop_acq_t(int argc, char *argv[], char *error, int esize);
  bool check(void);
  int execute(double time);
  step_type_t get_type() {return STEP_UNDEFINED;};
};

class step_print_t : public secuencia_step_t
{
private:
  char msg[80];
public:
  void reset(void) {};
  void set_param(int n, char *val);
  step_print_t(int argc, char *argv[], char *error, int esize);
  bool check(void);
  int execute(double time);
  step_type_t get_type() {return STEP_UNDEFINED;};
};

class step_new_test_t : public secuencia_step_t
{
private:

public:
  void reset(void) {};
  void set_param(int n, char *val);
  step_new_test_t(int argc, char *argv[], char *error, int esize);
  bool check(void);
  int execute(double time);
  step_type_t get_type() {return STEP_UNDEFINED;};
};

class step_close_test_t : public secuencia_step_t
{
private:

public:
  void reset(void) {};
  void set_param(int n, char *val);
  step_close_test_t(int argc, char *argv[], char *error, int esize);
  bool check(void);
  int execute(double time);
  step_type_t get_type() {return STEP_UNDEFINED;};
};

class step_loop_t : public secuencia_step_t
{
private:
public:
  void reset(void) {};
  void set_param(int n, char *val);
  step_loop_t(int argc, char *argv[], char *error, int esize);
  bool check(void);
  int execute(double time);
  step_type_t get_type() {return STEP_UNDEFINED;};
};


class secuencia_t;
class step_run_t : public secuencia_step_t
{
private:
  secuencia_t *secuencia;
  int pargc;
  char **pargv;
public:
  void reset(void) {};
  step_run_t(int argc, char *argv[], char *error, int esize);
  step_run_t(secuencia_t *s, int argc, char *argv[]);
  bool check(void);
  void set_param(int n, char *val);
  int num_params_in_call(void);
  int execute(double time);
  step_type_t get_type() {return STEP_UNDEFINED;};
};

class step_yield_t : public secuencia_step_t
{
private:
  bool first_time;
public:
  step_yield_t(int argc, char *argv[], char *error, int esize)
            {first_time = true;};
  bool check(void) {return true;};
  void reset(void) {first_time = true;};
  void set_param(int n, char *val) {};
  int execute(double time);
  step_type_t get_type() {return STEP_YIELD;};
};


struct secuencia_param_loc_t
{
  int step_number;
  int step_param_number;
  int call_param_number;
  class secuencia_param_loc_t *next;
};

class secuencia_t
{
private:
  double current_time;
  static const int max_steps = 200;
  secuencia_step_t *steps[max_steps];
  char **descriptions;
  bool first_time;
  secuencia_param_loc_t *param_locs;
  int max_params;
  bool in_if_statement;
  io_object_t *io_obj;
public:
  secuencia_step_t *new_script_type(char *str,
              char *error, int esize, bool script_mode);
  char *name;
  int step_number;
  int num_steps;
  bool run(double tiempo);
  void advance(void);
  void set_params(int argc, char *argv[]);
  int params_required(void);
  void reset(void);
  secuencia_t(const char *fname, const char *home_dir);
  void skip_to_mark(void);
  void skip_to_endif(void);
  const char *step_text(void){printf("debug: %d, %s\n", step_number, steps[step_number]->text); return steps[step_number]->text;};
};


/**************************************************************************/

void run_data_collect(double time);
bool checkdouble(char *param);
bool checkbool(char *param);
bool checkint(char *param);
bool check_param(char *param, script_param_t tp, char *error, int esize);
secuencia_step_t *create_object_step(int argc, char *argv[],
             char *error, int esize);
secuencia_step_t *create_system_step(int argc, char *argv[],
             char *error, int esize);
secuencia_step_t *create_run_step(int argc, char *argv[],
             char *error, int esize);
secuencia_step_t *create_step(int argc, char *argv[],
             char *error, int esize);
bool parse_function_call(char *str, int *argc, char *argv[], int max,
             char *error, int esize);
void log_start(char *str);
void log_end(void);
void log_double(double val, bool first);
void log_int(int val, bool first);
void log_bool(bool val, bool first);
void log_string(char *val, bool first);

#endif
