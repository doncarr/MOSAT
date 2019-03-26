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

/***

valve.cpp

Member functions for a valve monitor.

*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "arg.h"

/********************************************************************/

void valve_point_t::open(void)
{
  point_lock_t l(&this->point_lock, tag);
  this->in_transition = true; 
  this->start_time = db->get_time();
  this->pv = true;
  this->do_point->send(true);
  this->pv_string = this->hi_desc;
}

/********************************************************************/

void valve_point_t::close(void)
{
  point_lock_t l(&this->point_lock, tag);
  this->in_transition = true; 
  this->start_time = db->get_time();
  this->pv = false;
  this->do_point->send(false);
  this->pv_string = this->lo_desc;
}

/********************************************************************/

void valve_point_t::update(void)
{
  point_lock_t l(&this->point_lock, tag);

  bool is_open = open_point->get_pv();
  bool is_closed = closed_point->get_pv();
  double tnow = db->get_time();
  bool valid = (!pv && !is_open && is_closed) || (pv && is_open && !is_closed);
  
  if (this->in_transition)
  {
    if ((tnow - this->start_time) > this->delay)
    {
      this->in_transition = false; 
    }
    else
    {
      return; 
    }
  }

  if (!valid)
  {
  }
}


/********************************************************************/

void valve_point_t::init_values(void)
{
  pv = false;
  snprintf(lo_desc, sizeof(lo_desc), "CLOSED");
  snprintf(hi_desc, sizeof(hi_desc), "OPEN");
  pv_string = lo_desc;

  db_point_t *db_point;

  ltrim(do_tag);
  rtrim(do_tag);
  db_point = db->get_db_point(do_tag);

  if (db_point == NULL)
  {
    logfile->vprint("%s - bad TAGNAME: %s\n", tag, do_tag);
    do_point = NULL;
  }
  else
  {
    do_point = dynamic_cast <do_point_t *> (db_point);
    if (do_point == NULL)
    {
      logfile->vprint("%s - bad discrete output point: %s\n", tag, do_tag);
    }
  }

  ltrim(closed_tag);
  rtrim(closed_tag);
  db_point = db->get_db_point(closed_tag);

  if (db_point == NULL)
  {
    logfile->vprint("%s - bad TAGNAME: %s\n", tag, closed_tag);
    closed_point = NULL;
  }
  else
  {
    closed_point = dynamic_cast <discrete_point_t *> (db_point);
    if (closed_point == NULL)
    {
      logfile->vprint("%s - bad discrete tag for closed: %s\n", tag, closed_tag);
    }
  }

  ltrim(open_tag);
  rtrim(open_tag);
  db_point = db->get_db_point(open_tag);

  if (db_point == NULL)
  {
    logfile->vprint("%s - bad TAGNAME: %s\n", tag, open_tag);
    open_point = NULL;
  }
  else
  {
    open_point = dynamic_cast <discrete_point_t *> (db_point);
    if (open_point == NULL)
    {
      logfile->vprint("%s - bad discrete tag for open: %s\n", tag, open_tag);
    }
  }
}

/********************************************************************/
