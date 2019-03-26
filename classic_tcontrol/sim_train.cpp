/***************************************************************************
Copyright (c) 2002,2003,2004 Donald Wayne Carr 

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS IN THE SOFTWARE.
*************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "rtcommon.h"
#include "section.h"
#include "sim.h"
#include "event.h"

extern sections_t *sect;

/*********************************************************************/

sim_train_t::sim_train_t(void)
{
  init(0, 0, 0, 0.0, false);
}

/*********************************************************************/

double sim_train_t::get_location(double t)
{
  int next = sect->get_next_section(section);
  if (at_station)
  {
    return sect->get_section_start(next);
  }
  else
  {
    double fract = (t - time_entered) / sim_time_to_station;
    if (fract < 0.0) fract = 0.0;
    if (fract > 1.0) fract = 1.0;
    return sect->get_location(section, fract);
  }

}

/*********************************************************************/

bool sim_train_t::is_forward()
{
  bool fwd = sect->is_forward(section);
  return fwd;
}

/*********************************************************************/


void sim_train_t::update(double t)
{
  if (!active)
  {
    return;
  }
  int next = sect->get_next_section(section);
  double time_in_section = t - time_entered;
  event_t event;
  // Ok, if you have been selected to exit, AND this is an exit section, AND, you are at the exit, get out.
  if (sect->has_entrance(section) && exiting && (time_in_section >= sect->get_time_to_entrance(section)))
  {
    event.type = SYSTEM_EXIT_EVENT;
    event.section = section;
    trip_event(&event);
    //trip_exit_system(section);
    active = false;
  }
  if (!past_detector && (time_in_section > sect->get_time_to_detector()))
  {
    // Ok, you are past the dector. Send the detecor crossed event.
    past_detector = true;
    //printf("Detector crossed at section %d\n");
    event.type = CROSSING_EVENT;
    event.section = section;
    trip_event(&event);
    //trip_detector_crossed(section);
  }
  if (!arrived_at_station && (time_in_section > sim_time_to_station))
  {
    // Ok, you have arrived at the station.
    // Send the station entered event.
    at_station = true;
    arrived_at_station = true;
    event.type = STATION_ENTER_EVENT;
    event.section = section;
    trip_event(&event);
    //trip_entered_station(section);
  }
  else if (at_station &&
     (time_in_section > (sim_time_to_station + sect->get_dwell_time())) &&
      actual_stop_light[next])
  {
    // Ok, passengers have boarded and you have closed the doors.
    // If the light is green GO!!
    // Send the left station event.
    event.type = STATION_EXIT_EVENT;
    event.section = section;
    trip_event(&event);
    //trip_left_station(section);
    at_station = false;
    arrived_at_station = false;
    past_detector = false;
    time_entered = t;
    just_entered = false;
    section = next;
    sim_time_to_station = sect->get_sim_time(section, driver_type) - sect->get_dwell_time();
  }
}

/*********************************************************************/

void sim_train_t::select_for_exit(void)
{
  exiting = true;
}

/*********************************************************************/

void sim_train_t::init_at_entry(int n, int s, double time, double dtype)
{
  at_station = false;
  exiting = false;
  past_detector = true;
  active = true;
  just_entered = true;
  section = s;
  driver_type = dtype;
  sim_time_to_station = sect->get_section_time(s) - sect->get_dwell_time();

  time_entered = time - sim_time_to_station + sect->get_time_to_entrance(s);
  number = n;
}

/*********************************************************************/

void sim_train_t::init(int n, int s, double time, double dtype, bool a)
{
  active = a;
  number = n;
  just_entered = false;
  exiting = false;
  if (!active)
  {
    //printf("Inactive train\n");
    section = -1;
    at_station = false;
    past_detector = false;
    time_entered = 0.0;
    driver_type = 0.0;
  }
  section = s;
  driver_type = dtype;
  at_station = true;
  past_detector = true;
  sim_time_to_station = sect->get_sim_time(s, driver_type) - sect->get_dwell_time();
  time_entered = time - sim_time_to_station;
  //printf("New train %d, section %d, drv type: %0.1lf\n", n, s, dtype);
  //printf("Activated train at section %d\n", s);
}


