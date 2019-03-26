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
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <ctype.h>

#include "db.h"
#include "secuencia.h"
#include "logfile.h"

#include "rtcommon.h"


/*************************************************************************/

bool checkdouble(char *param, char *error, int esize)
{
  return true;
  int dec_cnt = 0;
  for (char *p = param; *p != '\0'; p++)
  {
    if (*p == '.')
    {
      dec_cnt++;
      if (dec_cnt > 1)
      {
        snprintf(error, esize, "Too many decimal points in number: %s", param);
        return false;
      }
    }
    else if(!isdigit(*p))
    {
      snprintf(error, esize, "Bad floating point number: %s", param);
      return false;
    }
  }
  return true;
}

/************************************************************/

bool checkbool(char *param, char *error, int esize)
{
  return true;
  if (strlen(param) > 1)
  {
    snprintf(error, esize, "Bad boolean value: %s", param);
    return false;
  }
  else if ( (*param != 'T') && (*param != 't') &&
            (*param != 'F') && (*param != 'f') &&
            (*param != '1') && (*param != '0') )
  {
    snprintf(error, esize, "Bad boolean value: %s", param);
    return false;
  }
  return true;
}

/************************************************************/

bool checkint(char *param, char *error, int esize)
{
  char *p = param;
  // The first char can  be a minus sign.
  if (*p == '-')
  {
    p++;
  }
  for ( ; p != '\0'; p++)
  {
    if (!isdigit(*p))
    {
      snprintf(error, esize, "Bad integer value: %s", param);
      return false;
    }
  }
  return true;
}

/************************************************************/

bool check_param(char *param, script_param_t tp, char *error, int esize)
{
   bool retval;
   switch (tp)
   {
     case PARAM_DOUBLE:
	     retval = checkdouble(param, error, esize);
	     break;
     case PARAM_BOOL:
	     retval = checkbool(param, error, esize);
	     break;
     case PARAM_INT:
	     retval = checkint(param, error, esize);
	     break;
     case PARAM_STRING:
	     retval = true;
	     break;
     case PARAM_VOID:
             safe_strcpy(error, "Void not allowed here", esize);
	     retval = false;
	     break;
     default:
             safe_strcpy(error, "Unknown type", esize);
	     retval = false;
	     break;
   }
   return retval;
}

/************************************************************/

void log_double(double val, bool first)
{
  //logfile->vprint("\n(log_double)\n");
  if (first)
  {
    logfile->vprint("%.2lf", val);
  }
  else
  {
    logfile->vprint(" ,%.2lf", val);
  }
}

/************************************************************/

void log_int(int val, bool first)
{
  //logfile->vprint("\n(log_int)\n");
  if (first)
  {
    logfile->vprint("%d", val);
  }
  else
  {
    logfile->vprint(" ,%d", val);
  }
}

/************************************************************/

void log_bool(bool val, bool first)
{
  //logfile->vprint("\n(log_bool)\n");
  const char *s;
  if (val)
  {
    s = "T";
  }
  else
  {
    s = "F";
  }
  if (first)
  {
    logfile->vprint("%s", s);
  }
  else
  {
    logfile->vprint(" ,%s", s);
  }
}

/************************************************************/

void log_string(char *val, bool first)
{
  if (first)
  {
    logfile->vprint("%s", val);
  }
  else
  {
    logfile->vprint(" ,%s", val);
  }
}

/************************************************************/

void log_start(char *str)
{
  //logfile->vprint("\n(log_start)\n");
  logfile->vprint("%s(", str);
}

/************************************************************/

char *find_str_ignore_strings(char *place_to_look, 
                                    const char *string_to_find)
{
  // Here, we look for a certain string, but not in quoted strings.
  char *p;
  bool in_string = false;
  int n = strlen(string_to_find);
  for (p = place_to_look; *p != '\0'; p++)
  {
    if (in_string) 
    {
      if  (0 == strncmp(p, "\"", 1))
      {
        //printf("Out of string at %s\n", p);
        in_string = false;
      }
      continue;
    } 
    //if (*p == '\"')
    if  (0 == strncmp(p, "\"", 1))
    {
      //printf("In to string at %s\n", p);
      in_string = true;
      continue;
    } 
    if  (0 == strncmp(p, string_to_find, n))
    {
      return p;
    }
  }
  return NULL;
}




/************************************************************/

void log_end(void)
{
  //logfile->vprint("\n(log_end)\n");
  logfile->vprint(")\n");
}

/************************************************************/

bool parse_function_call(char *str, int *argc, char *argv[], int max,
          char *error, int esize)
{
  /* This function takes the a function call of the following form:
   * my_obj.do_in_to_it(p1, p2, . . . ,pN)
   * and returns an array of each argument, with object name first,
   * then function name, then the parameters.
   */
  char *params;
  char *p;
  char *tag;
  char *fn;

  ltrim(str);
  rtrim(str);
  if (strlen(str) == 0)
  {
    error[0] = '\0';
    return false;
  }
  tag = str;
  //int n;
  //n = strcspn(str, "(),.");
  //p = &str[n];
  fn = find_str(str, ".");
  if (fn == NULL)
  {
    snprintf(error, esize, "No '.' found in function call");
    return false;
  }
  *fn = '\0';
  rtrim(tag);
  argv[0] = tag;
  fn++;
  ltrim(fn);
  argv[1] = fn;
  params = find_str(fn, "(");
  if (params == NULL)
  {
    *argc = 2;
    return true;
  }
  *params = '\0';
  rtrim(tag);
  params++;
  p = find_str_ignore_strings(params, ")");
  if (p == NULL)
  {
    snprintf(error, esize, "No ending ')' for function");
    return false;
  }
  *p = '\0';
  ltrim(params);
  rtrim(params);
  char *start;
  start = params;
  bool done = false;
  *argc = 2;
  if (strlen(params) != 0)
  {
    while (!done)
    {
      //int n = strcspn(str, "(),.");
      p = find_str_ignore_strings(start, ",");
      //char *p;
      //for (p = start; (*p != '\0') && (*p != ','); p++);
      ////if (*p == '\0')
      //{
      //  done = true;
      //}
      if (p == NULL)
      {
        done = true;
      }
      else
      {
        *p = '\0';
        p++;
      }
      ltrim(start);
      rtrim(start);
      strip_quotes(start);
      argv[*argc] = start;
      (*argc)++;
      start = p;
      if (*argc >= max)
      {
        done = true;
      }
    }
  }
  return true;
}

static secuencia_t *scripts[100];
static int n_scripts;

/************************************************************/

void add_script_to_list(secuencia_t *a_script)
{
  scripts[n_scripts] = a_script;
  n_scripts++;
}

/************************************************************/

secuencia_step_t *create_run_step(int argc, char *argv[],
             char *error, int esize)
{
  char *name = argv[1];
  bool found = false;
  secuencia_t *the_seq;
  for (int i = 0; i < n_scripts; i++)
  {
    if (0 == strcmp(scripts[i]->name, name))
    {
      printf("script %s alread read, using existing script\n", name);
      found = true;
      the_seq = scripts[i];
    }
  }
  if (!found)
  {
    char path[200];
    safe_strcpy(path, db->get_home_dir(), sizeof(path));
    safe_strcat(path, "/corridas/", sizeof(path));
    the_seq = new secuencia_t(name, path);
    add_script_to_list(the_seq);
  }

  step_run_t *stp = new step_run_t(the_seq, argc, argv);
  if (the_seq->params_required() != stp->num_params_in_call())
  {
    printf("Wrong number of params in call to: %s\n", name);
    snprintf(error, esize, "Wrong number of params in call to: %s\n", name);
  }
  return stp;
}

/************************************************************/

secuencia_step_t *create_system_step(int argc, char *argv[],
          char *error, int esize)
{
  if (0 == strcasecmp(argv[1], "wait"))
  {
    return new step_wait_t(argc, argv, error, esize);
  }
  if (0 == strcasecmp(argv[1], "wait_until"))
  {
    return new step_wait_until_t(argc, argv, error, esize);
  }
  if (0 == strcasecmp(argv[1], "wait_until_min"))
  {
    return new step_wait_until_min_t(argc, argv, error, esize);
  }
  if (0 == strcasecmp(argv[1], "if"))
  {
    return new step_if_t(argc, argv, error, esize);
  }
  if (0 == strcasecmp(argv[1], "endif"))
  {
    return new step_endif_t(argc, argv, error, esize);
  }
  else if (0 == strcasecmp(argv[1], "startdaq"))
  {
    return new step_start_acq_t(argc, argv, error, esize);
  }
  else if (0 == strcasecmp(argv[1], "stopdaq"))
  {
    return new step_stop_acq_t(argc, argv, error, esize);
  }
  else if (0 == strcasecmp(argv[1], "print"))
  {
    return new step_print_t(argc, argv, error, esize);
  }
  else if (0 == strcasecmp(argv[1], "new_test"))
  {
    return new step_new_test_t(argc, argv, error, esize);
  }
  else if (0 == strcasecmp(argv[1], "close_test"))
  {
    return new step_close_test_t(argc, argv, error, esize);
  }
  else if (0 == strcasecmp(argv[1], "loop"))
  {
    return new step_loop_t(argc, argv, error, esize);
  }
  else if (0 == strcasecmp(argv[1], "get_key"))
  {
    return new step_get_key_t(argc, argv, error, esize);
  }
  else if (0 == strcasecmp(argv[1], "mark"))
  {
    return new step_mark_t(argc, argv, error, esize);
  }
  else if (0 == strcasecmp(argv[1], "yield"))
  {
    return new step_yield_t(argc, argv, error, esize);
  }
  else
  {
    snprintf(error, esize, "No such system call: %s", argv[1]);
    return NULL;
  }
}

/************************************************************/

secuencia_step_t *create_step(int argc, char *argv[],
          char *error, int esize)
{
  secuencia_step_t *stp;

  if (0 == strncasecmp(argv[0], "sys", 3))
  {
    stp = create_system_step(argc, argv, error, esize);
  }
  else if (0 == strncasecmp(argv[0], "run", 3))
  {
    stp = create_run_step(argc, argv, error, esize);
  }
  else
  {
    stp = create_object_step(argc, argv, error, esize);
  }
  return stp;
}


/************************************************************/

/************************************************************

int main(int argc, char *argv[])
{
  char *tmp = "tag.doittoit(   10  ,  x     , y   , 3   )  junk";
  char fncall[200];
  safe_strcpy(fncall, tmp, sizeof(fncall));
  char *myargv[25];
  int myargc;
  parse_function_call(fncall, &myargc, myargv, 25);
  for (int i=0; i < myargc; i++)
  {
    printf("myargv[%d] = '%s'\n",i, myargv[i]);
  }
}

************************************************************/
