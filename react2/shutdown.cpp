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

/********

Shutdown.c

Procedures for trigering shutdown.

*************************************************************************/


#include <stdio.h>
#include <stdlib.h>
//#include <ncurses.h>

#include "rtcommon.h"
#include "db.h"

static int shutdown_tripped = false;

/*************************************************************************/

void db_point_t::trip_shutdown(char *reason)
{
  /* Trip a shutdown with the given reason. */

  if (shutdown_tripped)
  {
    return;
  }
  shutdown_tripped = true;
  printf("\nShutdown ......\n");
  printf("%s, %s\n", description, reason);
  db->set_shutdown();
  /****
  char temp[100];
  gotoxy(3, 25);
  textattr((RED << 4) | YELLOW | (1 << 7));
  cprintf(" Shutdown -- %s, %s ", description, reason);
  snprintf(temp, sizeof(temp), "%s, %s", description, reason);
  script->do_script("SHUTDOWN");
  ****/
}

