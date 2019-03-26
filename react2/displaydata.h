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

struct display_data_t
{
  char tag[21];
  char description[21];
  point_state_t state;
};

struct analog_display_data_t : public display_data_t
{
  double min;
  double max;
  double pv;
  int dec;
};

struct discrete_display_data_t : public display_data_t
{
  char pv[12];
};

struct display_info_t
{
  int n_analog;
  analog_display_data_t *adata;
  int n_discrete;
  discrete_display_data_t *ddata;
};

struct shm_data_header_t
{
  int n_analog;
  int analog_offset;
  int n_discrete;
  int discrete_offset;
};

void get_display_pointers(void *shmp, display_info_t *dinfo);


