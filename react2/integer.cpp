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
integer.cpp

Member functions for integer point types.

*************************************************************************/


#include <stdio.h>
#include <string.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "db_point.h"
#include "db.h"

/************************************************************************/

integer_point_t::integer_point_t(void)
{
  /* Constructor for discrete update point. */

  pv = 0;
  json_str = NULL;
}

/*************************************************************************/

const char *integer_point_t::get_config_json(void)
{
 int asprintf(char **strp, const char *fmt, ...);

  if (json_str == NULL)
  {
     asprintf(&json_str, "{\"tag\":\"%s\",\"description\":\"%s\"}",
         this->tag,
         this->description
      );
  }
  return json_str;
}

/*************************************************************************/
