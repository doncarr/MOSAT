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

/*************************************************************************/

extern "C" db_point_factory_t *get_db_factory(react_base_t *r)
{
  return new ai_point_factory_t;
}

/*************************************************************************/

void ai_point_t::update(double new_raw_value)
{
  /* This routine updates the analog input point using the given raw
     value. */

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
  printf("%s, raw val = %lf, pv = %lf\n", tag, new_raw_value, pv);
  check_alarms();
  display_pv();
  //printf("%s.pv = %lf\n", tag, pv);
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

db_point_t *ai_point_t::read_one(int argc, char *argv[], char *err, int esz)
{
  if (argc != 21)
  {
    snprintf(err, esz, "Wrong number of args for ai");
    return NULL;
  }
  ai_point_t *ai = new ai_point_t;
  safe_strcpy(ai->tag, argv[0], sizeof(ai->tag));
  //printf("Tag = %s\n", ai->tag);
  safe_strcpy(ai->description, argv[1], sizeof(ai->description));
  safe_strcpy(ai->eu, argv[2], sizeof(ai->eu));
  ai->driver = atol(argv[3]);
  ai->card = atol(argv[4]);
  ai->channel = atol(argv[5]);
  ai->eu_lo = atof(argv[6]);
  ai->eu_hi = atof(argv[7]);
  ai->raw_lo = atof(argv[8]);
  ai->raw_hi = atof(argv[9]);
  ai->decimal_places = atol(argv[10]);
  ai->zero_cutoff = atof(argv[11]);
  ai->lo_alarm = atof(argv[12]);
  ai->lo_caution = atof(argv[13]);
  ai->hi_caution = atof(argv[14]);
  ai->hi_alarm = atof(argv[15]);
  ai->deadband = atof(argv[16]);
  ai->lo_alarm_enable = (argv[17][0] == '1');
  ai->lo_caution_enable = (argv[18][0] == '1');
  ai->hi_caution_enable = (argv[19][0] == '1');
  ai->hi_alarm_enable = (argv[20][0] == '1');

  ai->lo_alarm_shutdown = false;
  ai->hi_alarm_shutdown = false;

  ai->set_conversion();
  ai->set_format();
  ai->pv = 0;
  return ai;
}

/************************************************************************

//ai_point_t **read_ai_points(int *cnt, const char *home_dir)
ai_point_t **ai_point_t::read(int *cnt, const char *home_dir)
{
  int max_points = 50;
  ai_point_t **ai_points =
	(ai_point_t **) malloc(max_points * sizeof(ai_point_t*));
  MALLOC_CHECK(ai_points);

  *cnt = 0;
  char path[200];
  safe_strcpy(path, home_dir, sizeof(path));
  safe_strcat(path, "/dbfiles/ai.dat", sizeof(path));
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
  {
    printf("Can't open %s", path);
    return NULL;
  }
  char line[300];

  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[25];

    //safe_strcpy(line, "AI1|Analog Input 1|PSI|0|0|1|0|100|0|4095|1|5|10|20|80|90|5|0|0|0|0|");
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
    //else if (argc != 21)
    //{
     // printf("%s: Wrong number of args, line %d", path, i+1);
      //continue;
    //}
    //printf(line);
    //ai_point_t *ai = new ai_point_t;
    //(*cnt)++;

    //AI1|Analog Input 1|PSI|0|0|0|0|100|0|4095|5|0|10|20|80|90|5|1|1|1|1|
    //Tag
    //Description
    //Engineering Units Label
    //Driver Number
    //Card
    //Channel
    //Engineering Units Low
    //Engineering Units High
    //Raw Value Low
    //Raw Value High
    //Decimal Places
    //Zero Cutoff
    //Alarm Low
    //Caution Low
    //Caution High
    //Alarm High
    //Alarm Deadband
    //Alarm Low Enable
    //Caution Low Enable
    //Caution High Enable
    //Alarm High Enable

    char errbuf[100];
    ai_points[*cnt] = ai_point_t::read_one(argc, argv, errbuf, sizeof(errbuf));
    if (ai_points[*cnt] == NULL)
    {
      printf("%s:%d %s\n", path, i+1, errbuf);
      continue;
    }
    printf("AI[%d].tag = '%s'\n", *cnt, ai_points[*cnt]->tag);
    printf(line);
    (*cnt)++;
    if (*cnt > max_points)
    {
      max_points += 10;
      ai_points = (ai_point_t **) realloc(ai_points,
	      max_points * sizeof(ai_point_t*));
      MALLOC_CHECK(ai_points);
    }
    //printf("AI[%d].tag = '%s'\n", i, ai->tag);
  }
  if (*cnt == 0)
  {
    free(ai_points);
    return NULL;
  }
  return ai_points;
}

************************************************************************/

