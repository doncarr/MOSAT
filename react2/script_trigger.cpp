/************************************************************************
This software is part of React, a control engine
Copyright (C) 2009 Donald Wayne Carr 

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

/*********
script_trigger.cpp

Code for the script_trigger member functions.

This object triggers scripts to run based on a lo-to-hi transition 
or hi-to-lo transition.

It also features a timeout to prevent repeated triggers, one after another.

*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <ncurses.h>
#include <math.h>
#include <malloc.h>
#include <float.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"

class script_trigger_factory_t : public db_point_factory_t
{
public:
  db_point_t *new_point (int argc, char *argv[], char *err, int esz)
    {return script_trigger_t::read_one(argc, argv, err, esz);};
  void delete_point(db_point_t *dbp) {delete (script_trigger_t *)dbp;};
  const char *get_config_name(void) {return "script_trigger.dat";};
  virtual const char * long_description(void) {return "Script Trigger";};
  virtual const char * short_description(void) {return "Script Trigger";};
  virtual const char * abbreviation(void) {return "ST";};
  virtual bool is_analog_input() {return false;};
  virtual bool is_discrete_input() {return false;};
  virtual bool is_integer_input() {return false;};
  virtual bool is_analog_output() {return false;};
  virtual bool is_discrete_output() {return false;};
  virtual bool is_integer_output() {return false;};
  virtual bool needs_update() {return true;};
  virtual bool has_analog_val() {return false;};
  virtual bool has_discrete_val() {return true;};
  virtual bool has_int_val() {return false;};
};

/*************************************************************************/

extern "C" db_point_factory_t *get_db_factory(react_base_t *r)
{
  return new script_trigger_factory_t;
}

/*************************************************************************/

void script_trigger_t::update(void)
{
}

/*************************************************************************/

db_point_t *script_trigger_t::read_one(int argc, char *argv[], char *err, int esz)
{
  /**
  discrete_point_t *trigger_point;
  double lo_hi_timeout;
  double hi_lo_timeout;
  tag_t lo_hi_script_name;
  tag_t hi_lo_script_name;
  //secuencia_t *lo_hi_script;
  //secuencia_t *hi_lo_script;
  bool running_lo_hi;
  bool running_hi_lo;

# Tag (string max 25)
# Description (string max 50)
# Discrete Trigger Point 
# Script to run (low to high)  
# Delay in seconds (after changes in DI) 
# Script to run (high to low) 
# Delay in seconds (after changes in DI) 

  **/

  if (argc != 7)
  {
    snprintf(err, esz, "Wrong number of args for ST");
    return NULL;
  }
  script_trigger_t *st = new script_trigger_t;
  safe_strcpy(st->tag, (const char*) argv[0], sizeof(st->tag));
  //printf("Tag = %s\n", st->tag);
  safe_strcpy(st->description, (const char*) argv[1], sizeof(st->description));


  tag_t temp_tag;
  safe_strcpy(temp_tag, (const char*) argv[2], sizeof(temp_tag));
  rtrim(temp_tag);
  ltrim(temp_tag);
  db_point_t *db_point;
  db_point = db->get_db_point(temp_tag);

  if (db_point == NULL)
  {
    logfile->vprint("%s - bad TAGNAME for trigger point: %s\n", st->tag, temp_tag);
    st->trigger_point = NULL;
  }
  else
  {
    st->trigger_point = dynamic_cast <discrete_point_t *> (db_point);
    if (st->trigger_point == NULL)
    {
      logfile->vprint("%s - NOT a discrete point: %s\n", st->tag, temp_tag);
    }
  }

  safe_strcpy(st->lo_hi_script_name, (const char*) argv[3], sizeof(st->lo_hi_script_name));
  st->lo_hi_timeout = atof(argv[4]);
  safe_strcpy(st->hi_lo_script_name, (const char*) argv[5], sizeof(st->hi_lo_script_name));
  st->hi_lo_timeout = atof(argv[6]);

  st->enabled_lo_hi = false;
  st->enabled_hi_lo = false;
  st->running_lo_hi = false;
  st->running_hi_lo = false;
  st->last_state = false;

  return st;
}

/************************************************************************/


