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


#include "logfile.h"

#include "db.h"



/*
 * This function dynamically loads an iodriver from a shared object file.
 * The iodirver must inherit from the class io_driver_t in iodriver.h, and
 * you must also create a class factory function in this shared object that,
 * when called, returns a pointer to an io_driver_t.
 * This function loads the shared object file, then tries to load the given
 * class factory function. Finally, the class factory function is called to
 * create an iodriver.
 *
 * Also, it is very common that the manufacturer supplies a shared object that MUST
 * be loaded first. This is handled by the parameter "other_lib". If this parameter
 * is not NULL, then we load it too, using the option RTLD_GLOBAL so that the functions
 * in that shared object can be called by your driver code. With this, you do NOT have
 * to compile react with a reference to any manufacturer shared libraries.
 *
 * As written, there is no way to unload the iodriver once loaded. You typically
 * load an iodriver, and it stays loaded until the program terminates, so really no
 * reason to be able to unload it.
 *
 * As written, once you load the sharred object and create an instance, you do not
 * have any way to create a second instance without loading the shared object again.
 * This will change, since it is often very useful to load more than one copy of the
 * same driver. I am still considering how to handle this.
 *
 * Also, as written, if two drivers need to load the same manufactures library,
 * the second would have to not specify an other library. I am thinking that this
 * loader should keep track of the extra libraries loaded, and then not load a
 * duplicate.
 *
 * We should also have a way to pass parameters to a driver. I am thinking this should
 * be a simple string, that could contain numbers or about anything, or even a name of
 * a configuration file with more information. For example, to support a modbus device,
 * we would need the registers numbers and type of all of the I/O.
 * 
 * I want to eventually have the ability for each driver to have a configuration file
 * extra information about the device and the inputs/outputs located on the device. Of
 * course after we have the heirarchical functions for react fully enabled, there will
 * be methods to retrieve this information from the slave device if it exists. In other
 * cases, the driver wil be for local I/O cards directly attached to the computers 
 * runnint REACT.
 */

io_driver_factory_t *load_iodriver_factory(react_drv_base_t *drvdb, 
     const char *sofile, const char *get_fn_name, const char *other_lib, const char *option)
{
  if (other_lib != NULL)
  {
    if (0 != strlen(other_lib))
    {
    logfile->vprint("Loading other driver: %s\n", other_lib);
    void *handlex = dlopen (other_lib, RTLD_LAZY | RTLD_GLOBAL);
    if (!handlex)
    {
      logfile->vprint("%s\n", dlerror());
      exit(1);
    }
    }
  }
  logfile->vprint("Loading iodriver from: %s\n", sofile);
  void *handle = dlopen (sofile, RTLD_LAZY);
  if (!handle)
  {
    logfile->vprint("%s\n", dlerror());
    exit(1);
  }
  logfile->vprint("Class factory function is: %s\n", get_fn_name);
  get_iodriver_factory_t fn = (get_iodriver_factory_t) dlsym(handle, get_fn_name);
  char *error;
  if ((error = dlerror()) != NULL)
  {
    logfile->vprint("%s\n", error);
    exit(1);
  }

  io_driver_factory_t *iodrv_factory = (*fn)(drvdb);
  //mybasep->myprint();
  //dlclose(handle);
  return iodrv_factory;
}


io_driver_t *load_iodriver(react_drv_base_t *drvdb, 
     const char *sofile, const char *get_fn_name, const char *other_lib, const char *option)
{

  if (other_lib != NULL)
  {
    if (0 != strlen(other_lib))
    {
    logfile->vprint("Loading other driver: %s\n", other_lib);
    void *handlex = dlopen (other_lib, RTLD_LAZY | RTLD_GLOBAL);
    if (!handlex)
    {
      logfile->vprint("%s\n", dlerror());
      exit(1);
    }
    }
  }
  logfile->vprint("Loading iodriver from: %s\n", sofile);
  void *handle = dlopen (sofile, RTLD_LAZY);
  if (!handle)
  {
    logfile->vprint("%s\n", dlerror());
    exit(1);
  }
  logfile->vprint("Class factory function is: %s\n", get_fn_name);
  get_io_drv_t fn = (get_io_drv_t) dlsym(handle, get_fn_name);
  char *error;
  if ((error = dlerror()) != NULL)
  {
    logfile->vprint("%s\n", error);
    exit(1);
  }

  io_driver_t *iodrv = (*fn)(drvdb, option);
  //mybasep->myprint();
  //dlclose(handle);
  return iodrv;
}

