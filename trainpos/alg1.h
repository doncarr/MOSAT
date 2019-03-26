/************************************************************************
This is software to monitor and/or control trains.
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

#ifndef __ALG1_INC__
#define __ALG1_INC__

#define DIR_SUR 0
#define DIR_NORTE 1

#define A1MAXT 12

struct alg1_tdata_t
{
  int last_section;
  time_t last_time;
};

class alg1_t : public base_alg_t
{
private:
  int n_north;
  int n_south;
  alg1_tdata_t north_data[A1MAXT];
  alg1_tdata_t south_data[A1MAXT];
public:
  void reset(void);
  void train_inserted(int section);
  void train_exited(int section);
  void initialize(config_info_t cinfo);
  void process_event(crossing_event_t event);
};

#endif


