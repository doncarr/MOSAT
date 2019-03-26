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

#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>


#include "db.h"

/*
 * This function dynamically loads a database point from a shared
 * object file.
 */

db_point_factory_t *load_db_point(const char *sofile)
{
  logfile->vprint("Loading db point factory from: %s\n", sofile);
  void *handle = dlopen (sofile, RTLD_LAZY | RTLD_GLOBAL);
  if (!handle)
  {
    logfile->vprint("dlopen: %s\n", dlerror());
    return NULL;
  }
  //db_point_factory_t *(*get_db_factory_t)(react_base_t *r);

  get_db_factory_t fn = (get_db_factory_t) dlsym(handle, "get_db_factory");
  char *error;
  if ((error = dlerror()) != NULL)
  {
    logfile->vprint("dlsym: %s\n", error);
    dlclose(handle);
    return NULL;
  }

  db_point_factory_t *db_factory = (*fn)(db);
  //dlclose(handle);
  return db_factory;
}

