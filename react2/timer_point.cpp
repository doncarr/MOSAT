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

/***********
timer_point.cpp

Member functions for timer points.

*************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "exp.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"
#include "error.h"

/************************************************************************/

void timer_point_t::stop(void)
{
  running = false;
}

/************************************************************************/

void timer_point_t::start(void)
{
  running = true;
  last_time = db->get_time();
}

/************************************************************************/

void timer_point_t::count_down(void)
{
  mode_count_up = false;
}

/************************************************************************/

void timer_point_t::count_up(void)
{
  mode_count_up = true;
}

/************************************************************************/

void timer_point_t::update()
{
  /* update the timer */

  double now = db->get_time();
  if (not running)
  {
    return;
  }
  if (mode_count_up)
  {
    pv += now - last_time;
  }
  else 
  {
    pv += last_time - now;
  }
  last_time = now;
  display_pv();
}

/*************************************************************************/

void timer_point_t::init_values()
{
  this->pv = 0.0;
  this->running = false;
  this->last_time = 0.0;
}

/************************************************************************/
