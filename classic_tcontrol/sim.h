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


#ifndef __SIM_INCLUDE__
#define __SIM_INCLUDE__

#define DATA_START 30

enum sim_type_t {SIM_SLAVE, SIM_RANDOM, SIM_FIXED};

class event_notify_object_t
{
public:
  virtual void trip_detector_crossed(int s) = 0;
  virtual void trip_enter_system(int s) = 0;
  virtual void trip_exit_system(int s) = 0;
  virtual void trip_entered_station(int s) = 0;
  virtual void trip_left_station(int s) = 0;
  virtual ~event_notify_object_t(void){};
};

class stop_lights_t
{
public:
  virtual bool is_on(int n) = 0;
  virtual ~stop_lights_t(void){};
};

class sim_train_t
{
private:
  event_notify_object_t *notify_object;
  //sections_t *sect;
  //stop_lights_t *stop_lights;
  sim_type_t sim_type;
  int number;
  bool active;
  bool just_entered;
  bool exiting;
  int section;
  double time_entered;
  bool arrived_at_station;
  bool at_station;
  bool past_detector;
  double sim_time_to_station;
  double driver_type;
public:
  sim_train_t(void);
  void set_notify_object(event_notify_object_t *eno);
  void set_sections_object(sections_t *sct);
  void set_stop_lights(stop_lights_t *sl);
  void update(double t);
  bool is_at_station() {return at_station;};
  bool is_active() {return active;};
  int get_number(void) {return number;};
  bool get_just_entered(void) {return just_entered;};
  bool get_exiting(void) {return exiting;};
  bool get_active(void) {return active;};
  int get_section(void) {return section;};
  double get_location(double t);
  void select_for_exit(void);
  void init(int n, int section, double time, double driver_type, bool active);
  void init_at_entry(int n, int s, double time, double dtype);
  bool is_forward();
};


struct sim_object_t
{
private:
public:
  void update(double t);
  void set_light(bool state);
  void ready_entry(int section);
  void start_entry(int section);
  void ready_exit(int section);
};


#endif

