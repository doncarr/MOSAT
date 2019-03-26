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

#ifndef __REACTSHMIO_INC__
#define __REACTSHMIO_INC__

struct reactshmio_t
{
  double ai_val[64];
  double ao_val[64];
  double ao_save[64];
  bool di_val[64];
  bool do_val[64];
  bool do_save[64];
  unsigned long count_val[64];
};

reactshmio_t *reactshmio_attach(void);

#endif


