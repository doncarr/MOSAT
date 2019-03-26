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
#include <string.h>
#include <time.h>
#include <sys/timeb.h>

#include "db.h"
#include "secuencia.h"
#include "gen_script.h"
#include "arg.h"
#include "logfile.h"
#include "conio.h"
#include "error.h"


/*****************************************************************/

double split_time(char *str)
{
  /* This takes a time string in the format MM:SS and converts to
   * seconds
   */
  char temp[20];
  safe_strcpy(temp, (const char*) str, sizeof(temp));
  int argc;
  char *argv[2];
  argc = get_delim_args(temp, argv,  ':', 2);
  //printf("str '%s', n %d", str, argc);
  //for (int i=0; i < argc; i++) printf(", %s", argv[i]);
  //printf("\n");
  if (argc == 1)
  {
    return atof(argv[0]);
  }
  else if (argc == 2)
  {
    return (60.0 * atof(argv[0])) + atof(argv[1]);
  }
  else if (argc == 3)
  {
    return (3600.0 * atof(argv[0])) + (60.0 * atof(argv[1]) + atof(argv[3]));
  }
  else
  {
    return 0.0;
  }
}

/*****************************************************************/

step_mark_t::step_mark_t(int argc, char *argv[], char *error, int esize)
{
  if (argc != 2)
  {
    logfile->vprint("Wrong number of args for 'mark', %d\n", argc);
  }
}

/*****************************************************************/

int step_mark_t::execute(double current_time)
{
return 1;
}

/*****************************************************************/

void step_mark_t::set_param(int n, char *val)
{
}

/*****************************************************************/

bool step_mark_t::check(void)
{
  return true;
}

/****************************************************************/

step_get_key_t::step_get_key_t(int argc, char *argv[], char *error, int esize)
{
  if (argc != 2)
  {
    logfile->vprint("Wrong number of args for 'get_key', %d\n", argc);
  }
  first_time = true;
}

/*****************************************************************/

int step_get_key_t::execute(double current_time)
{
  if (first_time)
  {
    db->clear_kb_buf();
    printf("Hit any key to continue . . . \n");
    first_time = false;
    return 0;
  }
  else
  {
    if (db->kbhit())
    {
      first_time = true;
      db->clear_kb_buf();
      return 1;
    }
    else
    {
      return 0;
    }
  }
}

/*****************************************************************/

void step_get_key_t::set_param(int n, char *val)
{
}

/*****************************************************************/

bool step_get_key_t::check(void)
{
  return true;
}

/****************************************************************/

step_wait_t::step_wait_t(int argc, char *argv[], char *error, int esize)
{
  if (argc != 3)
  {
    wait_time = 0;
    logfile->vprint("Wrong number of args for 'wait', %d\n", argc);
  }
  else
  {
    wait_time = split_time(argv[2]);
    //printf("Wait, %02d:%02d\n",
    //	    int(wait_time) / 60, int(wait_time) % 60);
  }
  first_time = true;
}

/*****************************************************************/

int step_wait_t::execute(double current_time)
{
  if (first_time)
  {
    end_time = current_time + wait_time;
    time_t tnow = time(NULL);
    logfile->vprint("%s", ctime(&tnow));
    //logfile->vprint("Start wait at: %lf\n", current_time);
    logfile->vprint("sys.wait(%02d:%02d)\n",
		    int(wait_time) / 60, int(wait_time) % 60);
    if (wait_time > 0)
    {
      first_time = false;
      return 0;
    }
    else
    {
      first_time = true;
      return 1; // if zero time, advance right away.
    }
  }
  else
  {
    //printf("waiting: %lf, %lf\n", current_time, end_time);
    if (current_time >= end_time)
    {
      first_time = true;
      time_t tnow = time(NULL);
      //logfile->vprint("End wait at: %lf\n", current_time);
      logfile->vprint("End wait: %s", ctime(&tnow));
      return 1;
    }
    else
    {
      return 0;
    }
  }
}

/*****************************************************************/

void step_wait_t::set_param(int n, char *val)
{
  if (n == 0)
  {
    wait_time = split_time(val);
  }
}

/*****************************************************************/

bool step_wait_t::check(void)
{
  return true;
}

/*****************************************************************/

static char collect_file_name[100];
static bool collect_is_on = false;
static ai_point_t *ai_point = NULL;
static FILE *collect_fp= NULL;
double collect_total = 0.0;
int collect_cnt = 0;
double collect_start = 0.0;
static const int collect_win_size = 10;

void run_data_collect(double t)
{
  if ((collect_fp != NULL) || (ai_point != NULL) || (collect_is_on != false))
  {
    if ((collect_fp == NULL) || (ai_point == NULL) || (collect_is_on == false))
    {
      logfile->vprint("Logic Error in run_data_collect\n");
    }
  }
  if ((collect_fp == NULL) || (ai_point == NULL) || (collect_is_on == false))
  {
    return;
  }
  /******
  // Write the average every 10 readings.
  collect_total += ai_point->get_pv();
  collect_cnt++;
  if (collect_cnt >= collect_win_size)
  {
    fprintf(collect_fp, "%0.1lf\t%f\n",
      t - collect_start, collect_total / double(collect_win_size));
    fflush(collect_fp);
    collect_total = 0.0;
    collect_cnt = 0;
  }
  *****/

  /******/
  fprintf(collect_fp, "%0.1lf\t%f\n", t - collect_start, ai_point->get_pv());
  fflush(collect_fp);
  /*****/
}

/*****************************************************************/

step_start_acq_t::step_start_acq_t(int argc, char *argv[], char *error, int esize)
{
  if (argc != 4)
  {
    ai_tag[0] = '\0';
    file_name[0] = '\0';
    safe_strcpy(collect_file_name, "", sizeof(collect_file_name));
    logfile->vprint("Wrong number of args for 'start': %d\n", argc);
  }
  else
  {
    safe_strcpy(file_name, (const char*) argv[2], sizeof(file_name));
    safe_strcpy(ai_tag, (const char*) argv[3], sizeof(ai_tag));
    strip_quotes(file_name);
    safe_strcpy(collect_file_name, (const char*) file_name,
       sizeof(collect_file_name));
    //printf("Start Acq, %s, %s\n", file_name, ai_tag);
  }
}

/*****************************************************************/

int step_start_acq_t::execute(double time)
{
  //safe_strcpy(temp_tag, argv[10], sizeof(temp_tag));
  //rtrim(temp_tag);
  char *temp_tag = ai_tag;
  collect_is_on = false;
  db_point_t *db_point;
  db_point = db->get_db_point(temp_tag);
  if ((db_point == NULL) || (db_point->point_type() != ANALOG_INPUT))
  {
    ai_point = NULL;
    logfile->vprint("Bad analog input point: %s\n", temp_tag);
  }
  else
  {
    ai_point = (ai_point_t *) db_point;
    collect_is_on = true;
  }
  //collect_fp = fopen("dataout.txt", "w");
  if (strlen(collect_file_name) == 0)
  {
  }
  collect_fp = logfile->open_file_in_log_dir(file_name);
  if (collect_fp == NULL)
  {
    logfile->vprint("Error opening logfile %s\n", file_name);
    exit(0);
    collect_is_on = false;
  }
  //fprintf(collect_fp, "start of collection\n");
  collect_total = 0.0;
  collect_cnt = 0;
  collect_start = time;
  logfile->vprint("Start data collection, %s, %s, at %0.1lf\n",
      file_name, ai_point->tag, time);
  return 1;
}

/*****************************************************************/

void step_start_acq_t::set_param(int n, char *val)
{
}

/*****************************************************************/

bool step_start_acq_t::check(void)
{
  return true;
}

/*****************************************************************/

step_stop_acq_t::step_stop_acq_t(int argc, char *argv[], char *error, int esize)
{
  if (argc != 3)
  {
    file_name[0] = '\0';
    logfile->vprint("Wrong number of args for 'stop': %d\n", argc);
  }
  else
  {
    safe_strcpy(file_name, (const char*) argv[2], sizeof(file_name));
    snprintf(text, sizeof(text),"sys.stopdaq()");
    //printf("Stop Acq, %s\n", collect_file_name);
  }
}

/*****************************************************************/

int step_stop_acq_t::execute(double time)
{

  if (collect_fp != NULL)
  {
    fclose(collect_fp);
  }
  collect_is_on = false;
  collect_fp = NULL;
  ai_point = NULL;
  logfile->vprint("Stop data collection, %s\n", collect_file_name);
  return 1;
}

/*****************************************************************/

void step_stop_acq_t::set_param(int n, char *val)
{
}

/*****************************************************************/

bool step_stop_acq_t::check(void)
{
  return true;
}

/*****************************************************************/

step_new_test_t::step_new_test_t(int argc, char *argv[], char *error, int esize)
{
  if (argc != 2)
  {
    logfile->vprint("Wrong number of args for 'new test': %d\n", argc);
  }
  else
  {
  }
}

/*****************************************************************/

static int test_number = 0;

int step_new_test_t::execute(double time)
{
  test_number++;
  char init_name[100];
  snprintf(init_name, sizeof(init_name), "inicial%d.txt", test_number);
  //logfile = new logfile_t(true, true);
  logfile->next_test();
  logfile->insert_in_index(init_name, db->secuencia_name());
  logfile->copy_file_to_log_dir(init_name, "inicial.txt");

  printf("-------- Starting new test --------\n");
  return 1;
}

/*****************************************************************/

void step_new_test_t::set_param(int n, char *val)
{
}

/*****************************************************************/

bool step_new_test_t::check(void)
{
  return true;
}

/*****************************************************************/

step_close_test_t::step_close_test_t(int argc, char *argv[], char *error, int esize)
{
  if (argc != 2)
  {
    logfile->vprint("Wrong number of args for 'new test': %d\n", argc);
  }
  else
  {
  }
}

/*****************************************************************/


int step_close_test_t::execute(double time)
{
  logfile->close_test();
  printf("-------- closing test --------\n");
  return 1;
}

/*****************************************************************/

void step_close_test_t::set_param(int n, char *val)
{
}

/*****************************************************************/

bool step_close_test_t::check(void)
{
  return true;
}

/*****************************************************************/

step_print_t::step_print_t(int argc, char *argv[], char *error, int esize)
{
  if (argc != 3)
  {
    msg[0] = '\0';
    text[0] = '\0';
    logfile->vprint("Wrong number of args for 'print': %d\n", argc);
  }
  else
  {
    safe_strcpy(msg, (const char*) argv[2], sizeof(msg));
    snprintf(text,sizeof(text), "sys.print(%s)", msg);
    strip_quotes(msg);
    //printf("Print: %s\n", msg);
  }
}

/*****************************************************************/

int step_print_t::execute(double time)
{
  logfile->vprint("%s\n", msg);
  return 1;
}

/*****************************************************************/

void step_print_t::set_param(int n, char *val)
{
}

/*****************************************************************/

bool step_print_t::check(void)
{
  return true;
}

/*****************************************************************/

step_run_t::step_run_t(secuencia_t *s, int argc, char *argv[])
{
  secuencia = s;
  snprintf(text, sizeof(text), "run.%s()", s->name); 
  if (argc > 2)
  {
    pargc = argc - 2;
    pargv = new char*[pargc];
    for (int i=0; i < pargc; i++)
    {
      pargv[i] = strdup(argv[i+2]);
    }
  }
  else
  {
    pargc = 0;
    pargv = NULL;
  }
}

/*****************************************************************/

step_run_t::step_run_t(int argc, char *argv[], char *error, int esize)
{
  secuencia = NULL;
}

int step_run_t::execute(double time)
{
  if (secuencia != NULL)
  {
    secuencia->reset();
    secuencia->set_params(pargc, pargv);
    db->new_secuencia(secuencia);
    logfile->vprint("run.%s(", secuencia->name);
    if (pargc > 0)
    {
      for (int i=0; i < pargc; i++)
      {
        if (i!= 0) logfile->vprint(" ,");
        logfile->vprint("%s", pargv[i]);
      }
    }
    logfile->vprint(")\n");
    return 0;
  }
  return 1;
}

/*****************************************************************/

void step_run_t::set_param(int n, char *val)
{
  if ((n >= 0) && (n < pargc))
  {
    pargv[n] = val;
  }
}

int step_run_t::num_params_in_call(void)
{
  return pargc;
}

bool step_run_t::check(void)
{
  return secuencia != NULL;
}

/*****************************************************************/

step_loop_t::step_loop_t(int argc, char *argv[], char *error, int esize)
{
  if (argc != 2)
  {
    logfile->vprint("Wrong number of args for 'new test': %d\n", argc);
  }
  else
  {
  }
}

/*****************************************************************/


int step_loop_t::execute(double time)
{
  //logfile->close_test();
  printf("-------- Loop --------\n");
  return SECUENCIA_LOOP;
}

/*****************************************************************/

void step_loop_t::set_param(int n, char *val)
{
}

/*****************************************************************/

bool step_loop_t::check(void)
{
  return true;
}

/*****************************************************************/


/*****************************************************************

step_example_t::step_example_t(int argc, char *argv[], char *error, int esize)
{
  if (argc != 4)
  {
     // report error if arg count is incorrect
  }
  else
  {
     // read and configure
  }
}
int step_example_t::execute(double time)
{
  if (1) // all ok?
  {
    // do something here.
  }
  else // error
  {
    // report error
  }
  return 1;
}
bool step_example_t::check(void)
{
  return true; // return true if all is ok
}

*****************************************************************/

/****************************************************************/

step_wait_until_t::step_wait_until_t(int argc, char *argv[], char *the_error, int esize)
{
  first_time = true;
  expr_string = "Error";
  if (argc != 4)
  {
    timeout = 0.0;
    expression.expr = new expr_op_t[2];
    expression.expr[0].token_type = LOGICAL_VAL;
    expression.expr[0].val.logical_val = true;
    expression.expr[1].token_type = END_EXPR;
    logfile->vprint("Wrong number of args for 'wait_until', %d\n", argc);
    return;
  }

  printf("expr: '%s'\n", argv[2]);
  expression.expr = make_expr(argv[2]);
  if (expression.expr == NULL)
  {
    logfile->vprint("Bad expression for 'wait_until': %s\n", rtexperror.str());
    expression.expr = new expr_op_t[2];
    expression.expr[0].token_type = LOGICAL_VAL;
    expression.expr[0].val.logical_val = true;
    expression.expr[1].token_type = END_EXPR;
    return;
  }
  expr_string = strdup(argv[2]);
  MALLOC_CHECK(expr_string);
  timeout = split_time(argv[3]);
  return;
}

/*****************************************************************/

int step_wait_until_t::execute(double current_time)
{
  if (first_time)
  {
    end_time = current_time + timeout;
    first_time = false;
    time_t tnow = time(NULL);
    logfile->vprint("%s", ctime(&tnow));
    //logfile->vprint("Start wait until at: %lf\n", current_time);
    logfile->vprint("sys.wait_until(%s, %0.2lf)\n", expr_string, timeout);
    if (!expression.evaluate())
    {
      //logfile->vprint("FALSE: %s", ctime(&tnow));
      return 0;
    }
    else
    {
      first_time = true;
      //time_t tnow = time(NULL);
      //logfile->vprint("TRUE: %s", ctime(&tnow));
      return 1; // if the expresion is true, advance right away.
    }
  }
  else
  {
    //printf("waiting: %lf, %lf\n", current_time, end_time);
    if (current_time >= end_time)
    {
      first_time = true;
      time_t tnow = time(NULL);
      logfile->vprint("**TIMEOUT**: %s", ctime(&tnow));
      return 1;
    }
    else if (expression.evaluate())
    {
      first_time = true;
      //time_t tnow = time(NULL);
      //logfile->vprint("wait_until TRUE: %lf\n", current_time);
      //logfile->vprint("wait_until TRUE: %s", ctime(&tnow));
      return 1;
    }
    else
    {
      //time_t tnow = time(NULL);
      //printf("****************************************** FALSE: %s, %s", expr_string, ctime(&tnow));
      return 0;
    }
  }
}

/*****************************************************************/

void step_wait_until_t::set_param(int n, char *val)
{
  if (n == 0)
  {

  }
}

/*****************************************************************/

bool step_wait_until_t::check(void)
{
  return true;
}

/*****************************************************************/

/****************************************************************/

step_wait_until_min_t::step_wait_until_min_t(int argc, char *argv[], char *the_error, int esize)
{
  first_time = true;
  expr_string = "Error";
  if (argc != 5)
  {
    timeout = 0.0;
    expression.expr = new expr_op_t[2];
    expression.expr[0].token_type = LOGICAL_VAL;
    expression.expr[0].val.logical_val = true;
    expression.expr[1].token_type = END_EXPR;
    logfile->vprint("Wrong number of args for 'wait_until_min', %d\n", argc);
    return;
  }

  printf("expr: '%s'\n", argv[2]);
  expression.expr = make_expr(argv[2]);
  if (expression.expr == NULL)
  {
    logfile->vprint("Bad expression for 'wait_until_min': %s\n", rtexperror.str());
    expression.expr = new expr_op_t[2];
    expression.expr[0].token_type = LOGICAL_VAL;
    expression.expr[0].val.logical_val = true;
    expression.expr[1].token_type = END_EXPR;
    return;
  }
  expr_string = strdup(argv[2]);
  MALLOC_CHECK(expr_string);
  timeout = split_time(argv[3]);
  min_true = split_time(argv[4]);
  return;
}

/*****************************************************************/

int step_wait_until_min_t::execute(double current_time)
{
  if (first_time)
  {
    end_time = current_time + timeout;
    first_time = false;
    time_t tnow = time(NULL);
    logfile->vprint("%s", ctime(&tnow));
    //logfile->vprint("Start wait until at: %lf\n", current_time);
    logfile->vprint("sys.wait_until_min(%s, %0.2lf)\n", expr_string, timeout);
    if (!expression.evaluate())
    {
      //logfile->vprint("FALSE: %s", ctime(&tnow));
      return 0;
    }
    else
    {
      first_time = true;
      //time_t tnow = time(NULL);
      //logfile->vprint("wait_until_min condition TRUE at start: %lf\n", current_time);
      //logfile->vprint("TRUE: %s", ctime(&tnow));
      // Important trick for tina, if true to start, return right away.
      return 1; // if the expresion is true, advance right away.
    }
  }
  else
  {
    //printf("waiting: %lf, %lf\n", current_time, end_time);
    if (current_time >= end_time)
    {
      first_time = true;
      time_t tnow = time(NULL);
      logfile->vprint("**TIMEOUT**: %lf\n", current_time);
      logfile->vprint("**TIMEOUT**: %s", ctime(&tnow));
      return 1;
    }
    else if (expression.evaluate())
    {
      time_t tnow = time(NULL);
      if (not detected)
      {
        detected = true;
        detect_time = tnow;
      }
      else if (double(tnow - detect_time) > min_true)
      {
        first_time = true;
        //logfile->vprint("wait_until_min TRUE: %lf\n", current_time);
        //logfile->vprint("wait_until_min TRUE: %s", ctime(&tnow));
        return 1;
      }
    }
    else
    {
      detected = false;
      return 0;
    }
  }
  return 0;
}

/*****************************************************************/

void step_wait_until_min_t::set_param(int n, char *val)
{
  if (n == 0)
  {

  }
}

/*****************************************************************/

bool step_wait_until_min_t::check(void)
{
  return true;
}

/*****************************************************************/
/*****************************************************************/
/****************************************************************/

step_if_t::step_if_t(int argc, char *argv[], char *the_error, int esize)
{
  expr_string = "Error";
  if (argc != 3)
  {
    expression.expr = new expr_op_t[2];
    expression.expr[0].token_type = LOGICAL_VAL;
    expression.expr[0].val.logical_val = true;
    expression.expr[1].token_type = END_EXPR;
    logfile->vprint("Wrong number of args for 'if', %d\n", argc);
    return;
  }

  expression.expr = make_expr(argv[2]);
  if (expression.expr == NULL)
  {
    logfile->vprint("Bad expression for 'if': %s\n", rtexperror.str());
    expression.expr = new expr_op_t[2];
    expression.expr[0].token_type = LOGICAL_VAL;
    expression.expr[0].val.logical_val = true;
    expression.expr[1].token_type = END_EXPR;
    return;
  }
  expr_string = strdup(argv[2]);
  MALLOC_CHECK(expr_string);
  return;
}

/*****************************************************************/

int step_if_t::execute(double current_time)
{
  logfile->vprint("sys.if(%s)\n", expr_string);
  if (expression.evaluate())
  {
    return 1;
  }
  else
  {
    return SECUENCIA_SKIP_TO_ENDIF;
  }
}

/*****************************************************************/

void step_if_t::set_param(int n, char *val)
{
  if (n == 0)
  {

  }
}

/*****************************************************************/

bool step_if_t::check(void)
{
  return true;
}

/*****************************************************************/

int step_yield_t::execute(double time)
{
  if (first_time)
  {
    logfile->vprint("sys.yield()\n");
    first_time = false;
    return 0;
  }
  first_time = true;
  return 1;
}

/*****************************************************************/

