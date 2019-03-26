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
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "msg_queue.h"
#include "tcontrol.h"
#include "event_alg.h"
#include "section_reader.h"
#include "trainpos.h"
#include "dc_evalg.h"

#include "rtcommon.h"
#include "arg.h"


/*************************************************************************************************/

void display_alg_t::read_sections(const char *fname)
{
  n_trains = 0;
  train_number = 1;
  when_last_train_entered = time(NULL); 

  sections.read_section_file();
  n_sections = sections.get_n_sections();

  gen_displays.init();

  time_table.read_day();
}

/*********************************************************/

void display_alg_t::add_train(time_t time_now, const char *train_id)
{
  for (int i=(n_trains - 1); i >= 0; i--)
  {
    trains[i+1] = trains[i];
    calcs[i+1] = calcs[i]; // must copy both calcs and trains up one space in the array.
  }
  trains[0].section = 0;
  trains[0].num = train_number;
  safe_strcpy(trains[0].train_id, train_id, sizeof(trains[0].train_id));
  calcs[0].arival_time = time_now - sections.get_departure_sensor_loc(0); // really irrelavent, but should be assigned.
  calcs[0].section_entry_time = time_now - 
                    sections.get_departure_sensor_loc(0);
  calcs[0].departed = true;
  trains[0].service_entry_time = time_now - 
                   sections.get_departure_sensor_loc(0);
  calcs[0].seconds_in_section = time_now - calcs[0].arival_time;
  calcs[0].seconds_late = 0;
  calcs[0].unexpected = false;
  calcs[0].switched_direction = false;
  trains[0].line_location = calcs[0].seconds_in_section;

  n_trains++;
  train_number++;
  when_last_train_entered = time_now;
  printf("********** Added train, n_trains = %d\n", n_trains);
}

/*********************************************************/

void display_alg_t::update_train(unsigned n, time_t time_now)
{
  double fr;
  int section = trains[n].section;
  if (!calcs[n].departed)
  {
    fr = 0.0;
    calcs[n].seconds_in_section = 0;
    int time_to_start = sections.get_time_to_start(section); 
    int line_location = time_to_start + (time_now - calcs[n].arival_time);
    int global_departure_sensor_loc = 
        time_to_start + sections.get_departure_sensor_loc(section);
    if (line_location > global_departure_sensor_loc)  
    {
      // do NOT calculate a position past the departure sensor 
      // untill they cross it.
      trains[n].line_location = global_departure_sensor_loc;
    } 
    trains[n].line_location = line_location;
//sections.get_time_to_start(section) + (time_now - calcs[n].arival_time);
    //printf("in station\n");
  }
  else
  {
    calcs[n].seconds_in_section = time_now - calcs[n].section_entry_time;
    int extra = calcs[n].seconds_in_section;
    if (calcs[n].seconds_in_section > sections.get_section_time(section))
    {
      extra = sections.get_section_time(section);
    }
    trains[n].line_location = sections.get_time_to_start(section) + 
                extra + RT_DWELL_TIME;
    fr = double(extra) / double(sections.get_section_time(section));
  }
  calcs[n].fraction_of_section_traveled = fr;
  if (n < (n_trains - 1))
  {
    calcs[n].seconds_to_next_train = trains[n+1].line_location - trains[n].line_location;
  }
  else
  {
    calcs[n].seconds_to_next_train = 0;
  }
  //printf("trx = %d, try = %d\n", trains[n].x, trains[n].y);
}

/*********************************************************/

void display_alg_t::update(time_t time_now)
{
  //printf("--- update %d trains\n", n_trains);
  if ((time_now - when_last_train_entered) > (60 * 60 * 2))
  {
    if (train_number > 10)
    {
      when_last_train_entered = time_now; 
      train_number = 1;
      time_table.next_day();
    }
  }

  for (unsigned i=0; i < n_trains; i++)
  {
    update_train(i, time_now);
  }
  gen_displays.gen(time_now, trains, calcs, n_trains);
}

/*********************************************************/

void display_alg_t::process_departure(unsigned section, time_t now)
{
  bool found = false;
  int tn = 0;
  for (unsigned i=0; i < n_trains; i++)
  {
    if (trains[i].section == section)
    {
      found = true;
      tn = i;
      break;
    }
  }
  if (!found)
  {
    printf("***** horrible error\n");
    return;
  }
  calcs[tn].departed = true;
  calcs[tn].section_entry_time = now - 
                 sections.get_departure_sensor_loc(section);
  calcs[tn].seconds_late = (now - trains[tn].service_entry_time) -
            (sections.get_time_to_start(section) + 
            sections.get_departure_sensor_loc(section) + RT_DWELL_TIME);

}

/*********************************************************/

void display_alg_t::process_arival(unsigned section, time_t now)
{
  bool found = false;
  int tn = 0;
  for (unsigned i=0; i < n_trains; i++)
  {
    if (trains[i].section == (section-1))
    {
      found = true;
      tn = i;
      break;
    }
  }
  if (!found)
  {
    printf("***** horrible error\n");
    return;
  }

  calcs[tn].arival_time = now +
     (sections.get_section_time(trains[tn].section) -  
        sections.get_arival_sensor_loc(trains[tn].section));
  calcs[tn].departed = false;
  calcs[tn].seconds_late = (calcs[tn].arival_time - 
          trains[tn].service_entry_time) - sections.get_time_to_start(section);
  trains[tn].section = section;
}


/*********************************************************/

void display_alg_t::process_event(crossing_event_t ev)
{
  if (ev.section == 0)
  {
    const char *tid = time_table.match_departure(ev.time_stamp);
    add_train(ev.time_stamp, tid);
  }
  else if (ev.section == n_sections)
  {
    if (trains[n_trains-1].section != (n_sections-1))
    {
      printf("***** horrible error\n");
    }
    printf("----------- train left service\n");
    n_trains--;
  }
  else
  {
    if (ev.departure)
    {
      process_departure(ev.section, ev.time_stamp);
    }
    else
    {
      process_arival(ev.section, ev.time_stamp);
    }
  }
}

/*********************************************************/

