/************************************************************************
This software is part of React, a control engine
Copyright (C) 2010 Donald Wayne Carr 

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

/*******************************************************************

get_point_fns.cpp

Functions to get a pointer to a specific point type based only on the "tag".

These functions will return a NULL pointer and fill the error string IF: 
1) The tag does not refer to a database point, OR,
2) The database point referred to by the tag is not the point type specified.

*************************************************************************/

#include <stdio.h>

#include "db_point.h"
#include "db.h"

/********************************************************************/

discrete_point_t *get_discrete_point(char *tag, char *err, int sz)
{
  // Get a discrete_point
  db_point_t *db_point;
  discrete_point_t *discrete_point;

  db_point = db->get_db_point(tag);

  if (db_point == NULL)
  {
    snprintf(err, sz, "NOT a database point: %s\n", tag);
    return NULL;
  }
  else
  {
    discrete_point = dynamic_cast <discrete_point_t *> (db_point);
    if (discrete_point == NULL)
    {
      snprintf(err, sz, "NOT a discrete point: %s\n", tag);
      return NULL;
    }
  }
  return discrete_point;
}

/********************************************************************/

analog_point_t *get_analog_point(char *tag, char *err, int sz)
{
  db_point_t *db_point;
  analog_point_t *analog_point;

  db_point = db->get_db_point(tag);

  if (db_point == NULL)
  {
    snprintf(err, sz, "NOT a database point: %s\n", tag);
    return NULL;
  }
  else
  {
    analog_point = dynamic_cast <analog_point_t *> (db_point);
    if (analog_point == NULL)
    {
      snprintf(err, sz, "NOT a analog point: %s\n", tag);
      return NULL;
    }
  }
  return analog_point;
}

/********************************************************************/

do_point_t *get_do_point(char *tag, char *err, int sz)
{
  db_point_t *db_point;
  do_point_t *do_point;

  db_point = db->get_db_point(tag);

  if (db_point == NULL)
  {
    snprintf(err, sz, "NOT a database point: %s\n", tag);
    return NULL;
  }
  else
  {
    do_point = dynamic_cast <do_point_t *> (db_point);
    if (do_point == NULL)
    {
      snprintf(err, sz, "NOT a discrete output point: %s\n", tag);
      return NULL;
    }
  }
  return do_point;
}

/********************************************************************/

ao_point_t *get_ao_point(char *tag, char *err, int sz)
{
  db_point_t *db_point;
  ao_point_t *ao_point;

  db_point = db->get_db_point(tag);

  if (db_point == NULL)
  {
    snprintf(err, sz, "NOT a database point: %s\n", tag);
    return NULL;
  }
  else
  {
    ao_point = dynamic_cast <ao_point_t *> (db_point);
    if (ao_point == NULL)
    {
      snprintf(err, sz, "NOT a analog output point: %s\n", tag);
      return NULL;
    }
  }
  return ao_point;
}

