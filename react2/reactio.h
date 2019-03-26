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

#ifndef __REACTIO_INC__
#define __REACTIO_INC__

class io_object_t
{
public:
  virtual int print_to(char *fmt, ...) = 0;
#ifdef __GNUC__
          __attribute__ (( format( printf, 2, 3 ) ));
#endif
  virtual ~io_object_t(void) {};
};

class msgio_t
{
private:
  int qid;
public:
  int print_to(char *fmt, ...);
};

class stdio_t
{
public:
  int print_to(char *fmt, ...);
};

#endif
