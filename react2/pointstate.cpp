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
#include "reactpoint.h"

/*************************************************************************/

point_state_t get_basic_state(point_state_t aState)
{
  switch (aState)
  {
    case STATE_NORMAL:
      return STATE_NORMAL;
    case STATE_CAUTION:
    case STATE_LOW_CAUTION:
    case STATE_HIGH_CAUTION:
    case STATE_DEV_CAUTION:
      return STATE_CAUTION;
    case STATE_ALARM:
    case STATE_LOW_ALARM:
    case STATE_HIGH_ALARM:
    case STATE_DEV_ALARM:
      return STATE_ALARM;
    case STATE_FAILED:
      return STATE_FAILED;
  }
  return STATE_FAILED;
}


