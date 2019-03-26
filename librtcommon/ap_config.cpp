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
#include <stdlib.h>

#include "rtcommon.h"
#include "arg.h"
#include "ap_config.h"


/**********************************************************************/

ap_config_t::ap_config_t(char a_delimiter)
{
  n_keys = 0;
  cur_key = 0;
  delimiter = a_delimiter;
  //printf("Init done, %d, %d\n", n_keys, cur_key);
}

/**********************************************************************/

ap_config_t::ap_config_t(void)
{
  n_keys = 0;
  cur_key = 0;
  delimiter = '|';
  //printf("Init done, %d, %d\n", n_keys, cur_key);
}

/**********************************************************************/

int ap_config_t::get_int(const char *aKey, int default_value)
{
  for (int i=0; i < n_keys; i++)
  {
    if (0 == strcasecmp(aKey, key[i]))
    {
      //printf("Found: %s, using %s\n", aKey, val[i]);
      return atol(val[i]);
    }
  }
  //printf("NOT found: %s, returning default: %d\n", aKey, default_value);
  return default_value;
}

/**********************************************************************/

int ap_config_t::get_int(const char *aKey)
{
  for (int i=0; i < n_keys; i++)
  {
    if (0 == strcasecmp(aKey, key[i]))
    {
      //printf("Found: %s, using %s\n", aKey, val[i]);
      return atol(val[i]);
    }
  }
  //printf("NOT found: %s, returning -1\n", aKey);
  return -1;
}

/**********************************************************************/

double ap_config_t::get_double(const char *aKey)
{
  for (int i=0; i < n_keys; i++)
  {
    if (0 == strcasecmp(aKey, key[i]))
    {
      //printf("Found: %s, using %s\n", aKey, val[i]);
      return atof(val[i]);
    }
  }
  //printf("NOT found: %s, returning 0.0\n", aKey);
  return 0.0;
}

/**********************************************************************/

double ap_config_t::get_double(const char *aKey, double default_value)
{
  for (int i=0; i < n_keys; i++)
  {
    if (0 == strcasecmp(aKey, key[i]))
    {
      //printf("Found: %s, using %s\n", aKey, val[i]);
      return atof(val[i]);
    }
  }
  //printf("NOT found: %s, using default: %lf\n", aKey, default_value);
  return default_value;
}

/**********************************************************************/

const char *ap_config_t::get_config(const char *aKey)
{
  for (int i=0; i < n_keys; i++)
  {
    if (0 == strcasecmp(aKey, key[i]))
    {
      //printf("Found: %s, using %s\n", aKey, val[i]);
      return val[i];
    }
  }
  //printf("NOT found: %s, returning NULL\n", aKey);
  return NULL;
}

/**********************************************************************/

const char *ap_config_t::get_config(const char *aKey, const char *default_value)
{
  for (int i=0; i < n_keys; i++)
  {
    if (0 == strcasecmp(aKey, key[i]))
    {
      //printf("Found: %s, using %s\n", aKey, val[i]);
      return val[i];
    }
  }
  //printf("NOT found: %s, using default: %s\n", aKey, default_value);
  return default_value;
}

/**********************************************************************/

bool ap_config_t::get_bool(const char *aKey, bool default_value)
{
  for (int i=0; i < n_keys; i++)
  {
    if (0 == strcasecmp(aKey, key[i]))
    {
      //printf("Found: %s, using %s\n", aKey, val[i]);
      return ( (val[i][0] == 'T') || 
               (val[i][0] == 'T') ||
               (val[i][0] == '1') ||
               (val[i][0] == 'y') ||
               (val[i][0] == 'Y') );
    }
  }
  //printf("NOT found: %s, using default: %s\n", aKey, default_value ? "true" : "false");
  return default_value;
}

/**********************************************************************/

void ap_config_t::insert_value(const char *aKey, const char *aVal)
{
  key[n_keys] = strdup(aKey);
  val[n_keys] = strdup(aVal);
  printf("key: %s, val: %s\n", key[n_keys], val[n_keys]);
  n_keys++;
}

/**********************************************************************/

void ap_config_t::read_file(const char *path)
{
  char fname[100];
  if (path == NULL)
  {
    safe_strcpy(fname, "config.txt", sizeof(fname));
  }
  else
  {
    snprintf(fname, sizeof(fname), "%s", path);
  }
  printf("Reading config file: %s\n", fname);
  FILE *fp = fopen(fname, "r");
  if (fp == NULL)
  {
    printf("Can't open %s\n", fname);
    return;
  }
  char line[300];

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[25];

    safe_strcpy(tmp, (const char*) line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, delimiter, 25);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }
    else if (argc != 2)
    {
      printf("Error in %s: %s\n", fname, line);
      continue;
    }
    rtrim(argv[1]);
    this->insert_value(argv[0],argv[1]);
  }
}

/**********************************************************************/
