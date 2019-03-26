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

/******

iodriver.cpp

Contains code for input/output drivers.

*************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> // for open(...)
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>

#include "rtcommon.h"
#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "iodriver.h"


/***********************************************************************/

int io_driver_factory_t::inc(void)
{
  n_drivers++;
  return n_drivers;
}

/***********************************************************************/
/********
driver tag
driver name
driver option1, option2, . . .
sim1|sim|burner,1,2,4|sin,1,5,0,1|sawtooth,1,5,0,1|triangle,1,5,0,1|
modbus1|modbus|192.168.1.99|ai,1,16,0|ao,1,2,0|di,1,32,0,0|do,1,8,0,0|
pci9111_1|pci9111|
********/
