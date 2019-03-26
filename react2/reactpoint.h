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
reactpoint.h

Header file for database point routines.

*************************************************************************/


#ifndef __REACTPOINT_INC__
#define __REACTPOINT_INC__


typedef char tag_t[26];

enum discrete_value_t
{
  DISCRETE_LO = 0,
  DISCRETE_HI = 1,
  DISCRETE_FAILED = 2,
  DISCRETE_NONE = 3
};

enum point_type_t
{
  ANALOG_INPUT,
  DISCRETE_INPUT,
  DISCRETE_OUTPUT,
  VAR_POINT,
  ARRAY_POINT,
  CALC_POINT,
  UNDEFINED_POINT,
  DCALC_POINT,
  DARRAY_POINT,
  DVAR_POINT,
  ANALOG_OUTPUT,
  PID_POINT,
  REMOTE_PID,
  VALVE_POINT,
  FUZZY_POINT,
  PENDULUM_POINT,
  ENUM_POINT,
  STRING_INPUT,
  DATA_POINT,
  FILE_LOGGER,
  SCAN_POINT,
  SYSTEM_POINT,
  WEB_POINT,
  WEB_LOGGER,
  INT_POINT,
  PUMP_POINT,
  AC_POINT,
  LEVEL_POINT,
  PCI_POINT,
  ANALOG_VALUE_POINT,
  DISCRETE_VALUE_POINT, 
  INTEGER_VALUE_POINT, 
  SCRIPT_TRIGGER, 
  DRIVER_POINT, 
  TIMER_POINT
};

enum pv_type_t
{
  ANALOG_VALUE,
  DISCRETE_VALUE,
  UNDEFINED_VALUE,
  INTEGER_VALUE,
  STRING_VALUE
};

enum point_state_t
{
  STATE_NORMAL = 0,
  STATE_CAUTION = 1,
  STATE_LOW_CAUTION = 2,
  STATE_HIGH_CAUTION = 3,
  STATE_DEV_CAUTION = 4,
  STATE_ALARM = 5,
  STATE_LOW_ALARM = 6,
  STATE_HIGH_ALARM = 7,
  STATE_DEV_ALARM = 8,
  STATE_FAILED = 9
};

point_state_t get_basic_state(point_state_t);
point_type_t get_point_type(char *tag);
pv_type_t get_pv_type(point_type_t point_type);
bool point_exists(char *tag, char *file_name);

#endif



