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

#include "time_table.h"
#include "gen_displays.h"

class display_alg_t : public event_alg_t
{
private:
  unsigned train_number;
  unsigned n_trains;
  unsigned n_sections;
  time_t when_last_train_entered;

  train_data_t trains[50];
  train_calcs_t calcs[50];
  //section_reader_t sections;
  time_table_t time_table;
  gen_displays_t gen_displays;
  void update_train(unsigned int n, time_t now);

  alarm_entry_t actual_alarms[RT_MAX_ALARMS]; 
  alarm_entry_t alarm_history[RT_MAX_ALARMS]; 

  void add_train(time_t ts, const char *train_id);
public:
  void read_sections(const char *fname);
  void update(time_t time);
  void process_event(crossing_event_t ev);
  void process_departure(unsigned section, time_t now);
  void process_arival(unsigned section, time_t now);
  void process_arrival(crossing_event_t ev);
  //void gen_display(time_t now);
  //void gen_html(time_t now);
  //void gen_table(time_t now);
  //void gen_performance(time_t now);
  void gen_alarms(time_t now);
};


