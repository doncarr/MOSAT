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

#define MAX_ZONES (20)
                                                                                                                             
struct zone_t
{
  bool blocked;
  int blocked_for;
  double time_free;
  int n_stations;
  int stations[10];
};


struct startup_tsection_t
{
  bool has_system_entry;
  int section_key;
  int system_entry_key;

  bool has_train;
  bool entry_requested;
};

class startup_algorithm_t : public base_algorithm_t
{
private:
  FILE *fplog;
  int n_zones;
  int key;
  int zone_mapping[MAX_SECTIONS];
  zone_t zones[MAX_ZONES];
  double current_time;

  alg_notify_object_t *notify_obj;
  startup_tsection_t tsections[60];
  int n_sections;
  int estop_key;
  bool estop_active;
  int stage;
  int get_next_section(int section);
  int get_last_section(int section);
public:
  startup_algorithm_t(void);
  void set_notify_object(alg_notify_object_t *notify_object, int estop_key);
  void initialize_sizes(int n_sections, int max_trains);
  void set_section_keys(int keys[], int n_keys);
  void set_entry_sections(int entry_sections[], int keys[], int num_entrys);
  void initial_state(int train_locations[], int n_locations);

  void set_station_zones(int *zmap, int n_secs);
  void initial_stoplight_settings(bool *lights, int n);

  void crossing_event(int section);
  void entry_event(int section);
  void exit_event(int section);
  bool request_entry(int section);
  void cancel_entry(int section);

  void light_to_green_event(int section, double t);
  void update(double t);
};


/**********************************************************************/

