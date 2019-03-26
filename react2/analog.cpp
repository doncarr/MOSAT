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

/*************
Analog.c

Member functions for the analog point types.

*************************************************************************/


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "db_point.h"
#include "db.h"

/************************************************************************/

void analog_update_point_t::check_alarms(void)
{
  /* Check for alarms in an analog update point */

  //int tmpx, tmpy;
  double temp;

  last_point_state = point_state;

  if (pv == FLT_MAX)
  {
    point_state = STATE_FAILED;
    //pv_attr = FAILED_ATTR;
    return;
  }

  if (point_alarm_disable || db->global_alarm_disabled())
  {
    point_state = STATE_NORMAL;
    //pv_attr = NORMAL_PV_ATTR;
    return;
  }

  switch (last_point_state)
  {
    case STATE_HIGH_ALARM:
      temp = hi_alarm;
      hi_alarm = temp - deadband;
      break;
    case STATE_LOW_ALARM:
      temp = lo_alarm;
      lo_alarm = temp + deadband;
      break;
    case STATE_HIGH_CAUTION:
      temp = hi_caution;
      hi_caution = temp - deadband;
      break;
    case STATE_LOW_CAUTION:
      temp = lo_caution;
      lo_caution = temp + deadband;
      break;
    default:
      /* For other cases, nothing to do. */
      break;
  }

  if (lo_alarm_enable && (pv <= lo_alarm))
  {
    point_state = STATE_LOW_ALARM;
    //pv_attr = ALARM_ATTR;
    if (lo_alarm_shutdown)
    {
      char reason[40];
      //pv_attr |= (1 << 7);
      snprintf(reason, sizeof(reason), "LOW ALARM, Point Value = %0.2f", pv);
      trip_shutdown(reason);
    }
  }
  else if (lo_caution_enable && (pv <= lo_caution))
  {
    point_state = STATE_LOW_CAUTION;
    //pv_attr = CAUTION_ATTR;
  }
  else if (hi_alarm_enable && (pv >= hi_alarm))
  {
    point_state = STATE_HIGH_ALARM;
    //pv_attr = ALARM_ATTR;
    if (hi_alarm_shutdown)
    {
      char reason[40];
      //pv_attr |= (1 << 7);
      snprintf(reason, sizeof(reason), "HIGH ALARM, Point Value = %0.2f", pv);
      trip_shutdown(reason);
    }
  }
  else if (hi_caution_enable && (pv >= hi_caution))
  {
    point_state = STATE_HIGH_CAUTION;
    //pv_attr = CAUTION_ATTR;
  }
  else
  {
    point_state = STATE_NORMAL;
    //pv_attr = NORMAL_PV_ATTR;
  }

  switch (last_point_state)
  {
    case STATE_HIGH_ALARM:
      hi_alarm = temp;
      break;
    case STATE_LOW_ALARM:
      lo_alarm = temp;
      break;
    case STATE_HIGH_CAUTION:
      hi_caution = temp;
      break;
    case STATE_LOW_CAUTION:
      lo_caution = temp;
      break;
    default:
      /* For other cases, nothing to do. */
      break;
  }
  if ((!display_is_on) &&
      (point_state != STATE_NORMAL) &&
      (last_point_state == STATE_NORMAL))
  {
    enable_alarm_display(this);
  }

}

/************************************************************************/

analog_update_point_t::analog_update_point_t(void)
{
  /* Constructor for an analog update point. */

  hi_alarm_enable = true;
  hi_caution_enable = true;
  lo_alarm_enable = true;
  lo_caution_enable = true;
  hi_alarm_shutdown = false;
  lo_alarm_shutdown = false;
  pv = 0;
  point_alarm_disable = false;
  json_str = NULL;
  zero_cutoff = 0;
}

/*********************************************************************/

double round_10(double x)
{
  /* utility function to round to the nearest 10 */
  return (double) 10.0 * floor((x / 10.0) + 0.5);
}

/************************************************************************/

analog_point_t::analog_point_t(void)
{
  /* Constructor for an analog point */

  round = 0;
  decimal_places = 0;
  eu[0] = '\0';
  pv = 0.0;
  safe_strcpy(fmt, "%-6.0f", sizeof(fmt));
  display_width = 6;
  scale_lo = 0;
  scale_hi = 0;
}

/************************************************************************/

void analog_point_t::set_format(void)
{
  /* Set up display formating for an analog point */

  if (decimal_places > 4)
  {
    decimal_places = 4;
  }
  if (decimal_places < -1)
  {
    decimal_places = -1;
  }
  if (display_width < 1)
  {
    display_width = 1;
  }
  if (display_width > 9)
  {
    display_width = 9;
  }

  if (decimal_places == -1)
  {
    snprintf(fmt, sizeof(fmt), "%%-%d.0f", display_width);
  }
  else
  {
    snprintf(fmt, sizeof(fmt), "%%-%d.%df", display_width, decimal_places);
  }

}

/************************************************************************/

void analog_point_t::display(void)
{
  /* Display the given analog point. */


  if (display_is_on)
  {
    /****
    //textattr(NORMAL_ATTR);
  //char temp[40];
    gotoxy(display_x + DESCRIPTION_OFFSET +
	   sizeof(description) - strlen(description) - 1, display_y);
    cputs(description);

    //textattr(pv_attr);
    gotoxy(display_x + PV_OFFSET, display_y);
    if (round)
    {
      pv = round_10(pv);
    }
    //snprintf(fmt, sizeof(fmt), "%%-6.%df", decimal);
    snprintf(temp, sizeof(temp), fmt, pv);

    if (temp[6] == '\0')
    {
      cputs(temp);
    }
    else
    {
      cputs("******");
    }
    //textattr(NORMAL_ATTR);
    gotoxy(display_x + EU_OFFSET, display_y);
    cputs(eu);
    ***/
  }
}

/*************************************************************************/

void analog_point_t::display_pv(void)
{
  /* Display the point value for the analog point. */

  return;
    /***
  char temp[40];
  int xpos;
  //int refresh_on = screen_refresh_is_on();
  if (display_is_on)
  {
    xpos = display_x + PV_OFFSET;
    gotoxy(xpos, display_y);
    if (pv == FLT_MAX)
    {
      //textattr(FAILED_ATTR);
      cputs("FAULT");
    }
    else
    {
      //textattr((flash_on) ? (pv_attr | (1 << 7)) : pv_attr);
      if (round)
      {
	pv = round_10(pv);
      }
      //snprintf(fmt, sizeof(fmt), "%%-6.%df", decimal);
      snprintf(temp, sizeof(temp), fmt, pv);
      if (temp[display_width] == '\0')
      {
	//cputs(temp);
      }
      else
      {
	//cputs("******");
      }
    }
  }
    ****/
}

/*************************************************************************/

int analog_point_t::get_json(char *attribute, char *buf, int sz)
{
  point_lock_t l(&point_lock, tag);
  if (0 == strcmp(attribute, "pv"))
  {
    snprintf(buf, sz, "%0.*lf", this->decimal_places, this->pv);
    return 0;
  }
  buf[0] = '\0';
  return -2;
}

/*************************************************************************/

void analog_point_t::get_pv_json(char *buf, int sz)
{
  point_lock_t l(&point_lock, tag);
  snprintf(buf, sz, "%0.*lf", this->decimal_places, this->pv);
}

/*************************************************************************/

const char *analog_point_t::get_config_json(void)
{
  point_lock_t l(&this->point_lock, tag);

  if (json_str == NULL)
  {
     asprintf(&json_str, "{\"tag\":\"%s\",\"description\":\"%s\",\"eu\":\"%s\",\"decimal_places\":%d,\"zero_cutoff\":%lf,\"scale_lo\":%lf, \"scale_hi\":%lf}",
         this->tag,
         this->description,
         this->eu,
         this->decimal_places,
         this->zero_cutoff,
         this->scale_lo,
         this->scale_hi
       );
  }
  return json_str;
}


/*************************************************************************/

