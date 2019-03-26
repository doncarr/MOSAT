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
Discrete.c

Member functions for discrete point types and discrete update points.

*************************************************************************/


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "db_point.h"
#include "db.h"


/************************************************************************/

void discrete_point_t::display(void)
{
  /* Display the discrete input point. */

  if (display_is_on)
  {
    /****
    //textattr(NORMAL_ATTR);
    gotoxy(display_x + DESCRIPTION_OFFSET +
	   sizeof(description) - strlen(description) - 1, display_y);
    cputs(description);

    //textattr(pv_attr);
    gotoxy(display_x + PV_OFFSET, display_y);
    cprintf("%-12s", pv_string);
    ****/
  }
}

/*************************************************************************/

void discrete_point_t::display_pv(void)
{
  /* Display the point value for the discrete input point. */

  if (display_is_on /* && screen_refresh_is_on()*/)
  {
    if (pv)
    {
      pv_string = hi_desc;
    }
    else
    {
      pv_string = lo_desc;
    }
    //printf("%s:%s,", tag, pv_string);
    /***
    //textattr((flash_on) ? (pv_attr | (1 << 7)) : pv_attr);
    //textattr(pv_attr);
    gotoxy(display_x + PV_OFFSET, display_y);
    cprintf("%-12s", pv_string);
    ****/
  }
}

/*************************************************************************/

void discrete_update_point_t::check_alarms(void)
{

  /* Check the alarms for the given discrete update point. */

  last_point_state = point_state;

  if (point_alarm_disable || db->global_alarm_disabled())
  {
    point_state = STATE_NORMAL;
    //pv_attr = NORMAL_PV_ATTR;
    return;
  }

  discrete_value_t dvalue;

  dvalue = pv ? DISCRETE_HI : DISCRETE_LO;

  if (dvalue == alarm_state)
  {
    point_state = STATE_ALARM;
    //pv_attr = ALARM_ATTR;
    if ((!display_is_on) && (last_point_state == STATE_NORMAL))
    {
      enable_alarm_display(this);
    }
  }
  else
  {
    point_state = STATE_NORMAL;
    //pv_attr = NORMAL_PV_ATTR;
  }
  if (dvalue == shutdown_state)
  {
    //pv_attr |= (1 << 7);
    char reason[40];
    snprintf(reason, sizeof(reason), "DISCRETE ALARM, state = %s", pv_string);
    trip_shutdown(reason);
  }

}

/*************************************************************************/

discrete_point_t::discrete_point_t(void)
{
  invert_pv = false;
  point_alarm_disable = false;
  json_str = NULL;
}

/*************************************************************************/

discrete_update_point_t::discrete_update_point_t(void)
{
  /* Constructor for discrete update point. */

  shutdown_state = DISCRETE_NONE;
  alarm_state = DISCRETE_NONE;
}

/*************************************************************************/

int discrete_point_t::get_json(char *attribute, char *buf, int sz)
{
  point_lock_t l(&point_lock, tag);
  if (0 == strcmp(attribute, "pv"))
  {
    snprintf(buf, sz, "%s", this->pv?"true":"false");
    return 0;
  }
  else if (0 == strcmp(attribute, "pv_string"))
  {
    snprintf(buf, sz, "\"%s\"", this->pv_string);
    return 0;
  }
  else if (0 == strcmp(attribute, "lo_desc"))
  {
    snprintf(buf, sz, "\"%s\"", this->lo_desc);
    return 0;
  }
  else if (0 == strcmp(attribute, "hi_desc"))
  {
    snprintf(buf, sz, "\"%s\"", this->hi_desc);
    return 0;
  }
  buf[0] = '\0';
  return -2;
}

/*************************************************************************/

void discrete_point_t::get_pv_json(char *buf, int sz)
{
  point_lock_t l(&point_lock, tag);
  snprintf(buf, sz, "%s", this->pv?"true":"false");
  //printf("#### tag: %s, json value: %s, invert: %s\n", tag, buf, invert_pv ? "T":"F");

}

/*************************************************************************/

const char *discrete_point_t::get_config_json(void)
{
  point_lock_t l(&this->point_lock, tag);

  if (json_str == NULL)
  {
     asprintf(&json_str, "{\"tag\":\"%s\",\"description\":\"%s\", \"lo_desc\":\"%s\",\"hi_desc\":\"%s\"}",
         this->tag,
         this->description,
         this->lo_desc,
         this->hi_desc
      );
  }
  return json_str;
}

/*************************************************************************/
