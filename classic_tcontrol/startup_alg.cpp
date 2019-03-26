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

#include "alg.h"
#include "startup_alg.h"

/******************************************************************/

startup_algorithm_t::startup_algorithm_t(void)
{
  notify_obj = NULL;
  estop_key = -1;
  estop_active = false;
  stage = 0;
  current_time = 0.0;
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

void startup_algorithm_t::set_notify_object(
     alg_notify_object_t *the_object, int the_estop_key)
{
  notify_obj = the_object;
  estop_key = the_estop_key;
  stage++;
}

/******************************************************************/

void startup_algorithm_t::initialize_sizes(int sec_cnt, int max_trains)
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

void startup_algorithm_t::set_section_keys(int keys[], int n_keys)
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

void startup_algorithm_t::set_entry_sections(
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

void startup_algorithm_t::initial_state(int train_locations[], int n_locations)
{
  if (stage != 4)
  {
    estop_active = true;
    notify_obj->estop(estop_key, "Called initial_state out of order");
  }
  for (int i=0; i < n_locations; i++)
  {
    int n = train_locations[i];
    if ((n >= n_sections) || (n < 0))
    {
      char err[100];
      snprintf(err, sizeof(err),
         "initial_state: Section out of range: %d\n", n);
      estop_active = true;
      notify_obj->estop(estop_key, err);
      return;
    }
    tsections[n].has_train = true;
  }
  for (int i=0; i < n_sections; i++)
  {
    notify_obj->permit_section_entry(i, tsections[i].section_key);
  }
  stage++;
}

/******************************************************************/

void startup_algorithm_t::crossing_event(int section)
{
  int zn = zone_mapping[section];  
  if  (zones[zn].blocked && (zones[zn].blocked_for == section))
  {
    // After crossing the sensor, we give them 10 more seconds to 
    // stop accelerating.
    zones[zn].time_free = current_time + 10;
    fprintf(fplog, "zone %d, blocking 10 more seconds at: %0.1lf\n", zn, current_time);
  }
}

/******************************************************************/

void startup_algorithm_t::entry_event(int section)
{
}

/******************************************************************/

void startup_algorithm_t::exit_event(int section)
{
}

/******************************************************************/

bool startup_algorithm_t::request_entry(int section)
{
  return false;
}

/******************************************************************/

void startup_algorithm_t::cancel_entry(int section)
{
}

/******************************************************************/

int startup_algorithm_t::get_next_section(int section)
{
  if (section >= (n_sections - 1))
  {
    return 0;
  }
  return (section + 1);
}

/******************************************************************/

int startup_algorithm_t::get_last_section(int section)
{
  if (section == 0)
  {
    return (n_sections - 1);
  }
  return (section - 1);
}

/******************************************************************/



void startup_algorithm_t::light_to_green_event(int section, double t)
{
  int zn = zone_mapping[section];  
  zone_t *this_zone = &zones[zn];
  fprintf(fplog, "block zone %d, startup in section %d, at %lf\n", zn, section, t);
  if (this_zone->blocked)
  {
    // Error
  }
  this_zone->blocked = true;
  this_zone->blocked_for = section;
  this_zone->time_free = t + 30; // Block other trains from starting for 30 seconds, or
                                 // ten seconds after crossing the sensor,
                                 // whichever comes first.
  fprintf(fplog, "\tSections: ");
  for (int i=0; i < this_zone->n_stations; i++)
  {
    int s = this_zone->stations[i];
    if (s != section)
    {
      fprintf(fplog, "(%d) ", s);
      notify_obj->prohibit_section_entry(s, key);
    }
    else 
    {
      // Ok, we can not block the train that just got green.
      fprintf(fplog, "*(%d)* ", s);
    }
  }
  fprintf(fplog, "\n");
  fflush(fplog);
}

/**********************************************************************/

void startup_algorithm_t::update(double t)
{
  current_time = t;
  for (int i=0; i < n_zones; i++)
  {
    if (zones[i].blocked && (t > zones[i].time_free))
    {
      int n = zones[i].n_stations;
      fprintf(fplog, "Time up, cancelling block in zone %d, at %0.0lf\n", i, t);
      //zone_t *this_zone = &zones[i];
      fprintf(fplog, "\tSections (%d): ", n);
      for (int j=0; j < n; j++)
      {
        int s = zones[i].stations[j];
        if (s != zones[i].blocked_for)
        {
          fprintf(fplog, "(%d) ", s);
          notify_obj->permit_section_entry(s, key);
        }
        else 
        {
          // Ok, here, we do not want to command a green if we never commanded red.
          fprintf(fplog, "*(%d)* ", s);
        }
      }
      fprintf(fplog, "\n");
      zones[i].blocked = false;
      zones[i].time_free = -1;
      zones[i].blocked_for = -1;
    }
  } 
  fflush(fplog);
}

/**********************************************************************/

void startup_algorithm_t::set_station_zones(int *zmap, int n_secs)
{
  for (int i=0; i < MAX_ZONES; i++)
  {
    zones[i].blocked = false;
    zones[i].n_stations = 0;
    zones[i].time_free = 0;
  }
  int zmax = 0;
  for (int i=0; i < n_secs; i++)
  {
    zone_mapping[i] = zmap[i];
    int z = zmap[i];
    zones[z].stations[zones[z].n_stations] = i; 
    zones[z].n_stations++;
    if (z > zmax) zmax = z;
  }
  n_zones = zmax+1;
  fplog = fopen("stlog.txt", "w");
}
                                                                                                                             
/**********************************************************************/

void startup_algorithm_t::initial_stoplight_settings(bool *lights, int n_secs)
{
  for (int i=0; i < n_secs; i ++)
  {
     if (lights[i])
     {
        light_to_green_event(i, 0.0);
     } 
  }
}
/**********************************************************************/

