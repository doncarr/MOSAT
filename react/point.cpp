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

/**********
Point.c

Contains database point utility procedures.

************************************************************************/

#include <stdio.h>

#include "rtcommon.h"
#include "reactpoint.h"

/************************************************************************/

pv_type_t get_pv_type(point_type_t point_type)
{
  /* Return the point value type for a point. */
  switch (point_type)
  {
    case ANALOG_INPUT:
    case VAR_POINT:
    case ARRAY_POINT:
    case CALC_POINT:
    case PID_POINT:
    case ANALOG_OUTPUT:
    case TIMER_POINT:
    case ANALOG_VALUE_POINT:
      return ANALOG_VALUE;
    case DISCRETE_INPUT:
    case DISCRETE_OUTPUT:
    case DCALC_POINT:
    case DARRAY_POINT:
    case DVAR_POINT:
    case DISCRETE_VALUE_POINT:
      return DISCRETE_VALUE;
    default:
      return UNDEFINED_VALUE;
  }
}

/***********************************************************************/

