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
error.c

Contains error member functions.

**************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include "error.h"

rtexperror_t rtexperror;

/*******************************************************************/

rtexperror_t::rtexperror_t(void)
{
  error_str[0] = '\0';
  error_num = EXP_NO_ERROR;
}


void rtexperror_t::set(int error_n, const char *fmt, ... )
{
  /* Set the current error. */
  va_list arg_pointer;

  error_num = error_n;
  va_start(arg_pointer, fmt);
  vsnprintf(error_str, sizeof(error_str), fmt, arg_pointer);
  va_end(arg_pointer);
}

/********************************************************************/

