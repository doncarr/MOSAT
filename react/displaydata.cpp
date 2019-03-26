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

#include "reactpoint.h"
#include "displaydata.h"

void get_display_pointers(void *shmp, display_info_t *dinfo)
{
  char *cp = (char *) shmp;
  shm_data_header_t *sheader = (shm_data_header_t *) shmp;
  dinfo->n_analog = sheader->n_analog;
  dinfo->n_discrete = sheader->n_discrete;
  dinfo->adata = (analog_display_data_t*) &(cp[sheader->analog_offset]);
  dinfo->ddata = (discrete_display_data_t*) &(cp[sheader->discrete_offset]);
}
