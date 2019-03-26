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

#ifndef __TCONTROL_INC__
#define __TCONTROL_INC__

const int MAX_SEGMENTS = 40;
const int MAX_TIMES = 10;
const int MAX_ALGS = 5;
const int MAX_ALARM_TYPES = 5;
const int MAX_ALARM_SEVERITY = 5;

/*****
struct crossing_event_t
{
  time_t time_stamp;
  int section;
  bool departure;
  bool update;
  bool skip;
};

struct crossing_queue_event_t
{
  long mtype;
  crossing_event_t data;
};
********/

struct alarm_info_t
{
  int section;
  int type;
  int severity;
};

class alarm_notify_base_t
{
public:
  virtual void estop(const char *reason) = 0;
  virtual void set_alarm_state(alarm_info_t alarm_info) = 0;
  virtual ~alarm_notify_base_t(void){};
};

struct config_info_t
{
  alarm_notify_base_t *notify;
  int n_segments;
  double times[MAX_SEGMENTS][MAX_TIMES];
  int n_times;
  int end_segment;
};

struct alarm_votes_t
{
  int severity_votes[MAX_ALGS];
  int severity;
};

class alarm_admin_t
{
private:
  int n_sections;
  int n_algs;
  int n_types;
  alarm_votes_t severities[MAX_SEGMENTS][MAX_ALARM_TYPES];
  void set_new_severity(int section, int type, int new_severity, int old_severity);
public:
  alarm_admin_t(int a_n_sects, int a_n_algs, int a_n_types);
  void reset(void);
  void estop(int alg, const char *reason);
  void set_alarm(int alg, alarm_info_t info);
  void evaluate_alarms(void);
  void ack_alarm(int section, int type, int new_severity);
};

class alarm_notify_obj_t : public alarm_notify_base_t
{
private:
  int algorithm_number;
  alarm_admin_t *admin;
public:
  alarm_notify_obj_t(int alg_num, alarm_admin_t *anAdmin);
  void estop(const char *reason);
  void set_alarm_state(alarm_info_t alarm_info);
};

class base_alg_t
{
public:
  virtual void reset(void) = 0;
  virtual void train_inserted(int section) = 0;
  virtual void train_exited(int section) = 0;
  virtual void initialize(config_info_t cinfo) = 0;
  virtual void process_event(crossing_event_t event) = 0;
  virtual ~base_alg_t(void){};
};

class event_dispatch_t
{
private:
  base_alg_t *algs[MAX_ALGS];
  int n_algs;
public:
  event_dispatch_t(base_alg_t *some_algs[], int n);
  void new_event(crossing_event_t event);
};

//int create_message_queue(void);
//int connect_message_queue(void);

//#define SECTIONS_FILE "line1_sections.txt"
//#define TIMETABLE_FILE "laboral.txt"

//#define SECTIONS_FILE "data/india_sections.txt"
//#define TIMETABLE_FILE "data/india_weekdays.txt"


#endif
