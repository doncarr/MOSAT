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


/*****

This is the implementation of a control object to maintain the distance
between the trains. It is implemented based on a fixed schedule that can
be slipped if trains are late beyond a certain point. It really works by
controling the driver. when the driver should leave the station,
The light will be turned green. This is the signal to the driver to close the
doors, and leave as soon as possible. We will record the error everytime
that a driver crosses an exit senosr, and eventually slip the schedule if a driver
is more than a certain amount late.

This object will also set off alarms, or caution for late arivals, or early arivals.

******/
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include "section.h"
#include "sim.h"
#include "alg.h"

#include "controlalg.h"



/*******************************************************************/
/**
ctrains[tn].time_at_green = calc_time_at_green(
last_time,
cycle_seconds,
ctrains[tn].slot_location,
sect->get_cycle_time_to_green(section),
sect->get_cycle_time(),
tn,
***/

double calc_time_at_green(double time,
                     double pace_location,
                     double slot_location,
                     double where_you_are,
                     double green_location,
                     double cycle_time,
                     int tn, double last,
                     double *error)
{
  /*
   This is an algorithm to determine when in the schedule a stop light should be turned green. It is
   assumed that this algorithm will be called just after entry to a section, in order to determine when
   the stop light should be turned green at the end of that section. It would not work if you were more
   than half a cycle from the section when this algorithm was called. It is only intended to be called
   if you are very close to the stop light.
   */

  // "time" is the time right now.
  // "pace_location" is where the imaginary pace train is (must be 0 to cycle time).
  // "slot_location" is how far in front of the imaginary pace train you SHOULD be.
  // "green_location" is how far (in seconds) from the start of a cycle, to a green light in this section.
  // "cycle_time" is the total time in seconds to make one cycle.

  // "should_be_here" is where you would be if you were EXACTY on schedule.
  double should_be_here; // Actually where you should be.
         should_be_here = pace_location + slot_location;

  if (should_be_here >= cycle_time)
  {
    should_be_here -= cycle_time; // Ok, past the end, subtract the cycle time.
  }
  if (green_location < should_be_here)
  {
    // Ok, you are  near the end of a cycle, and the light is near the start of a cycle.
    // No problem, this little trick will make the math work out right.
    green_location += cycle_time;
  }
  // Ok, the light should turn green in this many seconds.
  double time_to_green = green_location - should_be_here;

  // Now add to the current time, and this is the TIME when the light should turn green.
  double time_at_green = time + time_to_green;
  if (((time_at_green - last) > 3000))
  {
    char line[500];
  //mvprintw(28, 3, "t: %0.0lf, pl: %0.0lf, sl: %0.0lf, gl: %0.0lf, ct: %0.0lf, ttg: %0.0lf, tag: %0.0lf, l: %0.0lf",
   //        time, pace_location, slot_location, green_location, cycle_time, time_to_green, time_at_green, last);
    sprintf(line, "echo \"t: %0.0lf, pl: %0.0lf, sl: %0.0lf, gl: %0.0lf, ct: %0.0lf, ttg: %0.0lf, tag: %0.0lf, l: %0.0lf\" >>err.log",
           time, pace_location, slot_location, green_location, cycle_time, time_to_green, time_at_green, last);
    system(line);
    time_at_green -= cycle_time;
  }
  else if (tn == 0)
  {
  //mvprintw(29, 3, "t: %0.0lf, pl: %0.0lf, sl: %0.0lf, gl: %0.0lf, ct: %0.0lf, ttg: %0.0lf, tag: %0.0lf, l: %0.0lf",
   //        time, pace_location, slot_location, green_location, cycle_time, time_to_green, time_at_green, last);
  }
  *error = should_be_here - where_you_are;
  if (*error >= (1000))
  {
    *error -= cycle_time;
  }
  if (*error <= (-1000))
  {
    *error += cycle_time;
    //mvprintw(27, 3 + 8 * tn, "e: %05.0lf", *error);
  }

  while ((time - time_at_green) > (cycle_time / 2))
  {
    time_at_green += cycle_time;
  }
  while ((time_at_green - time) > (cycle_time / 2))
  {
    time_at_green -= cycle_time;
  }

  return time_at_green;
}

/*******************************************************************/

void control_algorithm_t::update_train(double t, int n)
{
  //mvprintw(32, 3 + 8 * n, "%0.0lf", t);
  mvprintw(35, 3 + 6 * n, "%0.0lf", ctrains[n].time_at_green);
  //mvprintw(34, 3 + 6 * n, "%c", ctrains[n].past_green?'T':'F');
  if (!ctrains[n].past_green && (t > ctrains[n].time_at_green))
  {
    int s = ctrains[n].section;
    int ns = get_next_section(s);
    ctrains[n].past_green = true;
    notify_obj->permit_section_entry(ns, tsections[ns].section_key);
    /****/
    ctrains[n].enter_time = t; // Ok, we assume they left on time;
    ctrains[n].section_est = get_next_section(s); // we SHOULD have left
    /****/
    //mvprintw(35, 3 + 6 * n, "%02d", s);
    //mvprintw(36, 3 + 6 * n, "%02d", ns);
  }
  //double time_in_section = t - (ctrains[n].enter_time + ctrains[n].amount_late);
  double time_in_section = t - (ctrains[n].enter_time);
  double s_time = sect->get_section_time(ctrains[n].section_est);
  ctrains[n].percent = (time_in_section / s_time) * 100.0;
  mvprintw(DATA_START+2, 3 + 6 * n, "%2d", ctrains[n].section_est);
  mvprintw(DATA_START+4, 3 + 6 * n, "%3.0lf", time_in_section);
  mvprintw(DATA_START+5, 3 + 6 * n, "%3.0lf", s_time);
  mvprintw(DATA_START+6, 3 + 6 * n, "%3.0lf", ctrains[n].percent);
  /**
  int section;
  double percent; // The percent of this section you have traveled
  double enter_time; // The time you SHOULD have enterd.
  double amount_late; // How late you were entering the station.
  double time_at_green; // The time the light should turn green.
  bool past_green;
  double slot_location; //seconds ahead of the imaginary pace train.
  **/
}

/*******************************************************************/

control_algorithm_t::control_algorithm_t(void)
{
  notify_obj = NULL;
  estop_key = -1;
  estop_active = false;
  cycle_seconds = 0.0;
  cycle_number = 0;
  stage = 0;
  total_slip = 0;
  for (int i=0; i < MAX_SECTIONS; i++)
  {
    tsections[i].has_system_entry = false;
    tsections[i].section_key = -1;
    tsections[i].system_entry_key = -1;
    tsections[i].has_train = false;
    tsections[i].entry_requested = false;
  }
}

/******************************************************************/



void control_algorithm_t::set_notify_object(
     alg_notify_object_t *the_object, int the_estop_key)
{
  notify_obj = the_object;
  estop_key = the_estop_key;
  stage++;
}

/******************************************************************/

void control_algorithm_t::initialize_sizes(int sec_cnt, int max_trains)
{
  if (stage != 1)
  {
    estop_active = true;
    notify_obj->estop(estop_key, "Called initialize_sizes out of order");
  }
  n_sections = sec_cnt;
  // Notice that I do NOT use max trains.
  stage++;
}

/******************************************************************/

void control_algorithm_t::set_section_keys(int keys[], int n_keys)
{
  if (stage != 2)
  {
    estop_active = true;
    notify_obj->estop(estop_key, "Called set_section_keys of order");
    return;
  }
  if (n_keys != n_sections)
  {
    estop_active = true;
    notify_obj->estop(estop_key,
          "Number of sections and section keys MUST be equal");
    return;
  }
  for (int i=0; i < n_sections; i++)
  {
    tsections[i].section_key = keys[i];
  }
  stage++;
}

/******************************************************************/

void control_algorithm_t::set_entry_sections(
           int entry_sections[], int entry_keys[], int num_entrys)
{
  if (stage != 3)
  {
    estop_active = true;
    notify_obj->estop(estop_key, "Called set_entry_sections out of order");
    return;
  }
  for (int i=0; i < num_entrys; i++)
  {
    int n = entry_sections[i];
    if ((n >= n_sections) || (n < 0))
    {
      char err[100];
      snprintf(err, sizeof(err), "Section out of range: %d\n", n);
      estop_active = true;
      notify_obj->estop(estop_key, err);
      return;
    }
    if (tsections[n].has_system_entry)
    {
      estop_active = true;
      notify_obj->estop(estop_key, "Duplicate entry section in list");
      return;
    }
    tsections[n].has_system_entry = true;
    tsections[n].system_entry_key = entry_keys[i];
  }
  stage++;
}

/******************************************************************/

void control_algorithm_t::initial_state(int train_locations[], int n)
{
  n_trains = n;
  if (stage != 4)
  {
    estop_active = true;
    notify_obj->estop(estop_key, "Called initial_state out of order");
  }
  for (int i=0; i < n_trains; i++)
  {
    int tn = train_locations[i];
    if ((tn >= n_sections) || (n < 0))
    {
      char err[100];
      snprintf(err, sizeof(err),
         "initial_state: Section out of range: %d\n", tn);
      estop_active = true;
      notify_obj->estop(estop_key, err);
      return;
    }
    tsections[tn].has_train = true;
    tsections[tn].train_number = i;
    ctrains[i].section = tn;
    ctrains[i].section_est = tn;
  }
  for (int i=0; i < n_sections; i++)
  {
   // For this algorithm, we ONLY permit entry when it is time to leave the station.
    notify_obj->prohibit_section_entry(i, tsections[i].section_key);
  }
  stage++;
}

/******************************************************************/

void control_algorithm_t::crossing_event(int section)
{
  //printf("%s %d\n", __FILE__, __LINE__);
  if ((section >= n_sections) || (section < 0))
  {
    char err[100];
    snprintf(err, sizeof(err),
         "crossing_event: Section out of range: %d\n", section);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return;
  }
  int last_section = get_last_section(section);
  // There must NOT be a train in the section you are crossing into.
  if ((tsections[section].has_system_entry) &&
        (tsections[section].entry_requested))
  {
    char err[100];
    snprintf(err, sizeof(err),
   "crossing_event: there was a train entering this section: %d\n",
      section);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return;
  }
  if (tsections[section].has_train)
  {
    char err[100];
    snprintf(err, sizeof(err),
   "crossing_event: there already was a train in this section: %d\n",
      section);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return;
  }
  // There MUST have been a train in the previous section.
  if (!tsections[last_section].has_train)
  {
    char err[100];
    snprintf(err, sizeof(err),
   "crossing_event: impossible, no train in previous section: %d\n",
      section);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return;
  }
  // A train has crossed into this section, so set has_train to true.
  int tn = tsections[last_section].train_number;
  if (tn == -1)
  {
    mvprintw(29, 3 + 8 * tn, "Error: train number is -1: sec %d", last_section);
  }
  tsections[section].has_train = true;
  tsections[section].train_number = tn;
  ctrains[tn].section = section;
  ctrains[tn].past_green = false;
  double error;
  ctrains[tn].time_at_green = calc_time_at_green(last_time, cycle_seconds, ctrains[tn].slot_location,
                          sect->get_cycle_location(section),
                          sect->get_cycle_time_to_green(section), sect->get_cycle_time(), tn,
      ctrains[tn].time_at_green, &error);
  ctrains[tn].amount_late = error;
  /****/
  ctrains[tn].enter_time = last_time - sect->get_time_to_detector();
  /****/
  //mvprintw(31, 3 + 8 * tn, "%0.0lf", ctrains[tn].time_at_green);
  // A train has crossed out of the previous section, so set has_train to false, and train number to -1.
  tsections[last_section].has_train = false;
  tsections[last_section].train_number = -1;

  notify_obj->prohibit_section_entry(section,
             tsections[section].section_key);


  //This is removed for this algorithm, since we will only turn the light green when needed.
/***
  notify_obj->permit_section_entry(last_section,
             tsections[last_section].section_key);
***/

}

/******************************************************************/

void control_algorithm_t::entry_event(int s)
{
  if (!tsections[s].entry_requested)
  {
    char err[100];
    snprintf(err, sizeof(err),
       "entry_event: Illegal event, no entry requested: %d\n", s);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return;
  }
  if (tsections[s].has_train)
  {
    char err[100];
    snprintf(err, sizeof(err),
   "entry_event: entry into section with train: %d\n", s);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return;
  }
  tsections[s].has_train = true;
  tsections[s].entry_requested = false;
  int tn = n_trains;
  tsections[s].train_number = tn;
  ctrains[tn].time_at_green =  sect->get_cycle_time_to_green(s);
  ctrains[tn].slot_location = sect->get_cycle_location(s) +
            sect->get_time_to_entrance(s) - cycle_seconds;
  if (ctrains[tn].slot_location < 0.0)
  {
    ctrains[tn].slot_location -= sect->get_cycle_time();
  }
  ctrains[tn].enter_time = last_time - sect->get_time_to_entrance(s);
  ctrains[tn].amount_late = 0.0; //Nobody late to start
  n_trains++;
  notify_obj->prohibit_section_entry(s, tsections[s].section_key);
}

/******************************************************************/

void control_algorithm_t::exit_event(int section)
{
  if (!tsections[section].has_system_entry)
  {
    char err[100];
    snprintf(err, sizeof(err),
       "exit_event: Illegal event, no exit in this section: %d\n",
      section);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return;
  }
  if (!tsections[section].has_train)
  {
    char err[100];
    snprintf(err, sizeof(err),
   "exit_event: impossible, there is NO train in this section: %d\n",
      section);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return;
  }
  tsections[section].has_train = false;

  notify_obj->permit_section_entry(section,
             tsections[section].section_key);
  tsections[section].entry_requested = false;
}

/******************************************************************/

bool control_algorithm_t::request_entry(int section)
{
  if (!tsections[section].has_system_entry)
  {
    char err[100];
    snprintf(err, sizeof(err),
       "request_entry: illegal request, no entry in this section: %d\n",
      section);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return false;
  }
  if (tsections[section].has_train)
  {
    char err[100];
    snprintf(err, sizeof(err),
   "request_entry: illegal request, there is a train in this section: %d\n",
      section);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return false;
  }
  int last_section = get_last_section(section);
  if (tsections[last_section].has_train)
  {
    char err[100];
    snprintf(err, sizeof(err),
"request_entry: illegal request, there is a train in the previous section: %d\n",
      last_section);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return false;
  }
  notify_obj->prohibit_section_entry(section,
             tsections[section].section_key);
  tsections[section].entry_requested = true;
  return true;
}

/******************************************************************/

void control_algorithm_t::cancel_entry(int section)
{
  if (!tsections[section].has_system_entry)
  {
    char err[100];
    snprintf(err, sizeof(err),
       "cancel_entry: illegal cancel, no entry in this section: %d\n",
      section);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return;
  }
  if (!tsections[section].entry_requested)
  {
    char err[100];
    snprintf(err, sizeof(err),
   "cancel_entry: illegal cancel, there was no request: %d\n",
      section);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return;
  }
  if (tsections[section].has_train)
  {
    char err[100];
    snprintf(err, sizeof(err),
   "cancel_entry: illegal cancel, there was a train in this section: %d\n",
      section);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return;
  }
  notify_obj->permit_section_entry(section,
                  tsections[section].section_key);
  tsections[section].entry_requested = false;
}

/******************************************************************/

int control_algorithm_t::get_next_section(int section)
{
  if (section >= (n_sections - 1))
  {
    return 0;
  }
  return (section + 1);
}

/******************************************************************/

int control_algorithm_t::get_last_section(int section)
{
  if (section == 0)
  {
    return (n_sections - 1);
  }
  return (section - 1);
}

/******************************************************************/

void control_algorithm_t::set_section_object(sections_t *s)
{
  sect = s;
}

double hslip = 0;
/******************************************************************/

void control_algorithm_t::update(double time, double increment)
{
  last_time = time;
  cycle_seconds += increment;
  if (cycle_seconds >= sect->get_cycle_time())
  {
    cycle_number++;
    cycle_seconds -= sect->get_cycle_time();
  }
  double max_late = 0.0;
  for (int i=0; i < n_trains; i++)
  {
    update_train(time, i);
    if (ctrains[i].amount_late > max_late)
    {
      max_late = ctrains[i].amount_late;
    }
  }
  if (max_late > 15)
  {
    double slip = max_late;
    while (slip > sect->get_cycle_time())
    {
      slip -= sect->get_cycle_time();
    }
    if (slip > 1000)
    {
      hslip = slip;
    }
    total_slip += slip;
    cycle_seconds -= slip;
    if (cycle_seconds < 0)
    {
      cycle_number--;
      cycle_seconds += sect->get_cycle_time();
    }
    for (int i=0; i < n_trains; i++)
    {
      ctrains[i].amount_late -= slip;
    }
  }

  mvprintw(DATA_START+3, 3, "update: %06.0lf, slip: %06.0lf, max late: %06.0lf, %04.0lf", time, total_slip, max_late, hslip);

}

/******************************************************************/

void control_algorithm_t::set_time_slots(double time[], int num_trains)
{
  for (int i=0; i < num_trains; i++)
  {
    ctrains[i].slot_location = time[i];
    double error;
    ctrains[i].time_at_green = calc_time_at_green(0.0, cycle_seconds, ctrains[i].slot_location,
                          sect->get_cycle_location(ctrains[i].section),
                          sect->get_cycle_time_to_green(ctrains[i].section), sect->get_cycle_time(), i,
                          ctrains[i].time_at_green, &error);
  }
}

/******************************************************************/

void control_algorithm_t::set_train_times(double time[], int num_trains)
{
  for (int i=0; i < num_trains; i++)
  {
    int s = ctrains[i].section;
    ctrains[i].slot_location = sect->get_cycle_location(s) +
            sect->get_section_time(s) -
            sect->get_dwell_time() -
            sect->get_time_to_detector();
    ctrains[i].time_at_green =  sect->get_cycle_time_to_green(s) - ctrains[i].slot_location;
    ctrains[i].enter_time = (-1.0) * sect->get_time_to_station(s);
    ctrains[i].amount_late = 0.0; //Nobody late to start
    ctrains[i].past_green = false;
    printf("ctrains[%d]: t_to_g = %0.1lf, sloc = %0.1lf, enter t = %0.1lf\n",
            i, ctrains[i].time_at_green, ctrains[i].slot_location, ctrains[i].enter_time);
  }
}


/******************************************************************/



