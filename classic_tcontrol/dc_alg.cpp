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
#include "dc_alg.h"

/******************************************************************/

dc_algorithm_t::dc_algorithm_t(void)
{
  notify_obj = NULL;
  estop_key = -1;
  estop_active = false;
  stage = 0;
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

void dc_algorithm_t::set_notify_object(
     alg_notify_object_t *the_object, int the_estop_key)
{
  notify_obj = the_object;
  estop_key = the_estop_key;
  stage++;
}

/******************************************************************/

void dc_algorithm_t::initialize_sizes(int sec_cnt, int max_trains)
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

void dc_algorithm_t::set_section_keys(int keys[], int n_keys)
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

void dc_algorithm_t::set_entry_sections(
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

void dc_algorithm_t::initial_state(int train_locations[], int n_locations)
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
    if (!tsections[i].has_train)
    {
      notify_obj->permit_section_entry(i, tsections[i].section_key);
    }
  }
  stage++;
}

/******************************************************************/

void dc_algorithm_t::crossing_event(int section)
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
  tsections[section].has_train = true;
  // A train has crossed out of the previous section, so set has_train to false.
  tsections[last_section].has_train = false;
  notify_obj->prohibit_section_entry(section,
             tsections[section].section_key);
  notify_obj->permit_section_entry(last_section,
             tsections[last_section].section_key);
}

/******************************************************************/

void dc_algorithm_t::entry_event(int section)
{
  if (!tsections[section].entry_requested)
  {
    char err[100];
    snprintf(err, sizeof(err),
       "entry_event: Illegal event, no entry requested: %d\n",
      section);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return;
  }
  if (tsections[section].has_train)
  {
    char err[100];
    snprintf(err, sizeof(err),
   "entry_event: entry into section with train: %d\n",
      section);
    estop_active = true;
    notify_obj->estop(estop_key, err);
    return;
  }
  // Ok, the train actuall entered, so set the request to false.
  tsections[section].entry_requested = false;
  tsections[section].has_train = true;
  notify_obj->prohibit_section_entry(section,
             tsections[section].section_key);
}

/******************************************************************/

void dc_algorithm_t::exit_event(int section)
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

bool dc_algorithm_t::request_entry(int section)
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

void dc_algorithm_t::cancel_entry(int section)
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

int dc_algorithm_t::get_next_section(int section)
{
  if (section >= (n_sections - 1))
  {
    return 0;
  }
  return (section + 1);
}

/******************************************************************/

int dc_algorithm_t::get_last_section(int section)
{
  if (section == 0)
  {
    return (n_sections - 1);
  }
  return (section - 1);
}

/******************************************************************

int main(int argc, char *argv[])
{

}

******************************************************************/


