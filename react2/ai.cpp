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

/*********
Ai.c

Code for the analog input member functions.

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

/***
class ai_point_factory_t : public db_point_factory_t
{
public:
  db_point_t *new_point (int argc, char *argv[], char *err, int esz)
    {return ai_point_t::read_one(argc, argv, err, esz);};
  void delete_point(db_point_t *dbp) {delete (ai_point_t *)dbp;};
  const char *get_config_name(void) {return "ai.dat";};
  virtual const char * long_description(void) {return "Analog Input";};
  virtual const char * short_description(void) {return "Analog Input";};
  virtual const char * abbreviation(void) {return "AI";};
  virtual bool is_analog_input() {return true;};
  virtual bool is_discrete_input() {return false;};
  virtual bool is_integer_input() {return false;};
  virtual bool is_analog_output() {return false;};
  virtual bool is_discrete_output() {return false;};
  virtual bool is_integer_output() {return false;};
  virtual bool needs_update() {return false;};
  virtual bool has_analog_val() {return true;};
  virtual bool has_discrete_val() {return false;};
  virtual bool has_int_val() {return false;};
};
**/

/********************************************************************/

/**
extern "C" db_point_factory_t *get_db_factory(react_base_t *r)
{
  return new ai_point_factory_t;
}
**/

/*********************************************************************/

void ai_point_t::update(double new_raw_value)
{
  /* This routine updates the analog input point using the given raw
     value. */
  point_lock_t l(&this->point_lock, tag);
  //printf("Entering ai update\n");

  raw_value = new_raw_value;
  pv_last = pv;

  if (raw_value == FLT_MAX)
  {
    pv = FLT_MAX;
  }
  else
  {
    pv = ((raw_value - raw_lo) * conversion_factor) + eu_lo;

    //printf("%s, pv = %lf\n", tag, pv);
    if (pv <= zero_cutoff)
    {
      pv = 0.0;
    }
  //printf("pv = %lf\n", pv);
  }
  //printf("%s, raw val = %lf, pv = %lf\n", tag, new_raw_value, pv);
  check_alarms();
  display_pv();
  //printf("%s.pv = %lf\n", tag, pv);
  //printf("leaving ai update\n");
}

/*************************************************************************/

void ai_point_t::set_conversion(double rl, double rh, double eul, double euh)
{

  /* This sets up the conversion for the given analog input point
     using the parameters passed */

  raw_lo = rl;
  raw_hi = rh;
  eu_lo = eul;
  eu_hi = euh;
  set_conversion();
}

/*************************************************************************/

void ai_point_t::set_conversion(void)
{
  /* This procedure sets up the conversion for the given analog input
     point using the internal conversion values. */

  if ((raw_hi - raw_lo) == 0)
  {
    conversion_factor = 0.0;
  }
  else
  {
    conversion_factor = (eu_hi - eu_lo) / (raw_hi - raw_lo);
  }
}

/************************************************************************/

void ai_point_t::init_values(void)
{
  //logfile->vprint("ai_point_t::init(): %s\n", this->tag);
  lo_alarm_shutdown = false;
  hi_alarm_shutdown = false;
  set_conversion();
  set_format();
  pv = 0;
  json_str = NULL;
}

/************************************************************************/

const char *ai_point_t::get_config_json(void)
{
  point_lock_t l(&this->point_lock, tag);

  if (json_str == NULL)
  {
     asprintf(&json_str, "{\"tag\":\"%s\",\"description\":\"%s\",\"eu\":\"%s\",\"driver\":%d,\"card\":%d,\"channel\":%d,\"eu_lo\":%lf,\"eu_hi\":%lf,\"raw_lo\":%lf,\"raw_hi\":%lf,\"decimal_places\":%d,\"zero_cutoff\":%lf,\"lo_alarm\":%lf,\"lo_caution\":%lf,\"hi_caution\":%lf,\"hi_alarm\":%lf,\"deadband\":%lf,\"lo_alarm_enable\":%s,\"lo_caution_enable\":%s,\"hi_caution_enable\":%s,\"hi_alarm_enable\":%s,\"scale_lo\":%lf,\"scale_hi\":%lf}",
         this->tag, 
         this->description, 
         this->eu, 
         this->driver,
         this->card,
         this->channel,
         this->eu_lo,
         this->eu_hi,
         this->raw_lo,
         this->raw_hi, 
         this->decimal_places,
         this->zero_cutoff,
         this->lo_alarm,
         this->lo_caution,
         this->hi_caution,
         this->hi_alarm,
         this->deadband,
         this->lo_alarm_enable?"true":"false",
         this->lo_caution_enable?"true":"false",
         this->hi_caution_enable?"true":"false",
         this->hi_alarm_enable?"true":"false",
         this->scale_lo,
         this->scale_hi
      );  
           /**
Tag: LevelDescription: Level of first tankEU Label: cm.Driver: 1Card: 0Channel: 231EU Low: 0EU High: 600Raw Low: 0Raw High: 10Decimal Places: 0Zero Cutoff: 2
***/
  }
  return json_str;
}

/************************************************************************/

