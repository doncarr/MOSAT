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



const double DWELL_TIME = 20.0;
const double TIME_TO_DETECTOR = 10.0;

struct section_t
{
  char name[25];
  double distance;
  double time_to_station;
  double time_to_green;
  double time_to_departure;
  double total_time; // Total time.
  double cycle_location; // At what time does this section start
  double cycle_time_to_green; // Time in cycle to green light
  bool has_entrance;
  double time_to_entrance; // When an entry event is recieved, this it the
                          // time from the start of section.
  bool is_forward; // do the trains travel away from the start, or towards the start?
  int substation; // Which electrical substation zone is this section in
  double accel_end_after_sensor; // how long after the sensor is crossed does acceleration end?
};

class sections_t
{
private:
  int n_sections;
  section_t sections[60];
  double dwell_time;
  double time_to_detector;
  double cycle_time;
public:
  sections_t(void);
  int get_n_sections(void) {return n_sections;};
  double get_sim_time(int s, double driver_type = 0.0);
  double get_section_time(int s) {return sections[s].total_time;};
  double get_time_to_green(int s) {return sections[s].time_to_green;};
  double get_section_start(int s) {return sections[s].distance;};
  double get_section_length(int s);
  double get_time_to_station(int s) {return sections[s].time_to_station;};
  bool has_entrance(int s) {return sections[s].has_entrance;};
  double get_time_to_entrance(int s) {return sections[s].time_to_entrance;};
  double get_cycle_time_to_green(int s) {return sections[s].cycle_time_to_green;};
  double get_cycle_time_to_station(int s) {return sections[s].cycle_time_to_green - dwell_time;};
  double get_cycle_location(int s) {return sections[s].cycle_location;};
  double get_dwell_time(void) {return dwell_time;};
  double get_time_to_detector(void) {return time_to_detector;};
  double get_cycle_time(void) {return cycle_time;};
  bool is_forward(int s) {return sections[s].is_forward;};
  const char *get_name(int s) {return sections[s].name;};
  int get_substation(int s) {return sections[s].substation;};
  void read_file(void);
  double get_factor(double driver_type);
  int get_next_section(int s);
  int get_last_section(int s);
  double get_location(int s, double fraction_traveled);
};

