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



/*******************************************************************/

struct control_train_t
{
  int section;       // This will be incremented when you cross the sensor.
  int section_est;   // This will be incremented when you SHOULD have 
                     // left the station.
  double enter_time; // The time you enterd, first estimated, 
                     // then corrected when you cross the sensor.
  double amount_late; // How late you were entering the station.
  double time_at_green; // The time the light should turn green.
  bool past_green;
  double slot_location; //seconds ahead of the imaginary pace train.
  double percent;
};

/*******************************************************************/

struct control_tsection_t
{
  bool has_system_entry;
  int section_key;
  int system_entry_key;

  bool has_train;
  int train_number;
  bool entry_requested;
};

/*******************************************************************/

class control_algorithm_t : public base_algorithm_t
{
private:
  alg_notify_object_t *notify_obj;
  sections_t *sect;
  control_tsection_t tsections[MAX_SECTIONS];
  control_train_t ctrains[MAX_TRAINS];
  int n_sections;
  int estop_key;
  bool estop_active;
  int stage;
  int n_trains;
  int cycle_number;
  double cycle_seconds;
  double total_slip;
  double last_time;

  int get_next_section(int section);
  int get_last_section(int section);

  void slip_schedule(double time);
  void update_train(double t, int n);
public:
  control_algorithm_t(void);
  void set_notify_object(alg_notify_object_t *notify_object, int estop_key);
  void initialize_sizes(int n_sections, int max_trains);
  void set_section_keys(int keys[], int n_keys);
  void set_entry_sections(int entry_sections[], int keys[], int num_entrys);
  void initial_state(int train_locations[], int n_locations);

  void crossing_event(int section);
  void entry_event(int section);
  void exit_event(int section);
  bool request_entry(int section);
  void cancel_entry(int section);

  void set_section_object(sections_t *s);
  void update(double time, double increment);
  void set_train_times(double time[], int n_trains);
  void set_time_slots(double time[], int num_trains);
};


