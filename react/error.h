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
Error.h

Header file for error routines in error.lib.

*************************************************************************/

#ifndef __ERROR_INC__
#define __ERROR_INC__

typedef enum
{
  EXP_ANALOG_EXPR_EXPECTED,
  EXP_DISCRETE_EXPR_EXPECTED,
  EXP_UNEXPECTED_SYMBOL,
  EXP_EMPTY_EXPR,
  EXP_UNEXPECTED_CHAR,
  EXP_POINT_DOESNT_EXIST,
  EXP_MATCHING_PAREN,
  EXP_BAD_POINT_TYPE,
  EXP_FUNCTION_NOT_ASSIGNED,
  EXP_BAD_TOKEN,
  EXP_NO_ERROR
}  error_num_t;

class rtexperror_t
{
  char error_str[256];
  int error_num;
public:
  rtexperror_t(void);
  void set(int err_num, const char *fmt, ... );
#ifdef __GNUC__
          __attribute__ (( format( printf, 3, 4 ) ));
#endif
  char *str() {return error_str;}
  int number() {return error_num;}
};

extern rtexperror_t rtexperror;

#endif


