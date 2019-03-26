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
#include <unistd.h>
#include <stdlib.h>

#include "react_io.h"


void msgio_t::print_to(char *fmt, ...)
{
  char myline[80];
  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  vsnprintf(myline, sizeof(myline), fmt, arg_ptr);
  va_end(arg_ptr);

}


