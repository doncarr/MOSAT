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
#include "arg.h"


/**************************************************************************/

void secuencia_t::reset(void)
{
  step_number = 0;
  for (int i=0; i < num_steps; i++)
  {
    steps[i]->reset();
  }
}

/**************************************************************************/
int secuencia_t::params_required(void)
{
  return max_params;
}
/**************************************************************************/

void secuencia_t::advance(void)
{
  step_number++;
}

/**************************************************************************/

void secuencia_t::skip_to_endif(void)
{
  int count = 0;
  while (step_number < num_steps)
  {
    if(steps[step_number]->get_type() == STEP_IF)
    {
      count++;
    }
    if(steps[step_number]->get_type() == STEP_ENDIF)
    {
      if (count == 0)
      {
        break;
      }
      else
      {
        count--;
      }
    }
    step_number++;
  }
}

/**************************************************************************/


void secuencia_t::skip_to_mark(void)
{
  while (step_number < num_steps)
  {
    if(steps[step_number]->get_type() == STEP_MARK)
    {
      break;
    }
    step_number++;
  }
}

/**************************************************************************/

bool secuencia_t::run(double t)
{
  int n;
  current_time = t;
  bool done = false;

  if ((step_number < 0) || (step_number >= num_steps))
  {
    return true;
  }

  run_data_collect(t);
  while (!done)
  {
    if (first_time)
    {
    }
    if (steps[step_number] == NULL)
    {
      logfile->vprint("NULL pointer\n");
      n = 1;
    }
    else if (steps[step_number]->check())
    {
      n = steps[step_number]->execute(t);
    }
    else
    {
      n = 1;
    }

    if (n == 0)
    {
      done = true; // done for now.
      first_time = false;
    }
    else if (n == SECUENCIA_LOOP)
    {
      step_number = 0;
    }
    else if (n == SECUENCIA_SKIP_TO_ENDIF)
    {
      skip_to_endif();
    }
    else
    {
      first_time = true;
      step_number += n; // jump to the next instruction.
      if ((step_number < 0) || (step_number >= num_steps))
      {
        return true;
      }
    }
  }
  return false;
}

/**************************************************************************/

void secuencia_t::set_params(int argc, char *argv[])
{
  if (argc != this->max_params)
  {
    logfile->vprint("Parameter miss-match in %s: %d %d\n\r",
		    name, argc, max_params);
  }
  secuencia_param_loc_t *spl;
  spl = this->param_locs;
  while (spl != NULL)
  {
     if (spl->call_param_number < argc)
     {
       steps[spl->step_number]->set_param(spl->step_param_number,
		    argv[spl->call_param_number]);
    }
    spl = spl->next;
  }
}

/**************************************************************************/

secuencia_step_t *secuencia_t::new_script_type(char *str,
     char *error, int esize, bool script_mode)
{
  secuencia_step_t *stp;
  char *argv[25];
  int argc;
  char line[500];
  safe_strcpy(line, (const char*) str, sizeof(line));
  logfile->vprint("%s\n", line);
  if (!parse_function_call(str, &argc, argv, 25, error, esize))
  {
    return NULL;
  }

  stp = create_step(argc, argv, error, esize);
  bool have_error = false;
  if (stp == NULL)
  {
    logfile->vprint("Error creating instruction: %s\n", error);
    have_error = true;
  }
  else if (!stp->check())
  {
    logfile->vprint("Error creating instruction: %s\n", error);
    have_error = true;
  }
  if (!have_error && script_mode)
  {
    for (int i=2; i < argc; i++)
    {
      if (argv[i][0] == '$')
      {
	//printf("$$$$ Found a param: %s\n\r", argv[i]);
	secuencia_param_loc_t *spl = new secuencia_param_loc_t;
	spl->step_param_number = i - 2;
	spl->step_number = this->num_steps;
	spl->call_param_number = atol(&argv[i][1]);
	if (spl->call_param_number > max_params)
	{
	  max_params = spl->call_param_number;
	}
	spl->call_param_number--; // users use 1 - N for params;
	spl->next = this->param_locs;
	this->param_locs = spl;
      }
    }
  }
  return stp;
}

/**************************************************************************/

secuencia_t::secuencia_t(const char *fname, const char *home_dir)
{
  FILE *fp;
  char path[200];
  this->name = strdup(fname);
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/", sizeof(path));
  safe_strcat(path, fname, sizeof(path));
  fp = fopen(path, "r");
  if (fp == NULL)
  {
    logfile->vprint("Can not open file: %s\n", path);
    exit(0);
  }
  step_number = 0;
  num_steps = 0;
  this->param_locs = NULL;
  this->max_params = 0;
  first_time = true;
  char line[200];
  char tmp[200];
  char error[500];
  int line_number = 0;
  descriptions = new char *[max_steps];

  /**/
  logfile->vprint("----- reading %s -----\n", path);
  while (NULL != fgets(line, sizeof(line) - 1, fp))
  {
    line[sizeof(line)-1] = '\0';
    line_number++;
    if (line[0] == '#')
    {
      continue;
    }
    safe_strcpy(tmp, (const char*) line, sizeof(tmp));
    logfile->vprint("%s", line);
    secuencia_step_t *stp;
    error[0] = '\0';
    stp = new_script_type(line, error, sizeof(error), true);

    if (stp != NULL)
    {
      steps[num_steps] = stp;
      num_steps++;
    }
    else
    {
      logfile->vprint("***** %s\n", error);
    }

    if (num_steps >= max_steps)
    {
       logfile->vprint("Too many steps: %d\n", num_steps);
       //printf("%s %d\n", __FILE__, __LINE__);
       exit(0);
    }
  }
  logfile->vprint("----- done reading %s -----\n", path);

}

/**************************************************************************/

