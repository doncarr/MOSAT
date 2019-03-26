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


enum alarm_type_t {RT_TRAIN_LATE, RT_TRAIN_CLOSE, RT_UNEXPECTED_EVENT, RT_NET_FAIL}; 

enum alarm_severity_t {RT_NORMAL, RT_ALARM, RT_CAUTION};

const int RT_CLOSE_DEADBAND = 15;
const int RT_CLOSE_CAUTION = 240;
const int RT_CLOSE_ALARM = 180;
const int RT_LATE_ALARM = 40;
const int RT_MAX_ALARMS = 200;

const int RT_DWELL_TIME = 15;

class alarm_entry_t
{
  int train_id; // Train involved (if applicable)
  int section; // Section where error occurred (if applicable)
  int network; // Network where error occured (if applicable)
  time_t detect_time; // initial detect time for alarm.
  time_t clear_time; // time the alarm was cleared.
  alarm_type_t alarm_type; // Type of alarm
  alarm_severity_t alarm_severity; // Severity of alarm
};

class alarm_data_t
{
  alarm_entry_t actual_alarms[RT_MAX_ALARMS]; 
  alarm_entry_t alarm_history[RT_MAX_ALARMS]; 
};

class event_alg_t
{
public:
  virtual void update(time_t time) = 0;
  virtual void process_event(crossing_event_t ev) = 0;
  virtual ~event_alg_t(void){};
};

struct train_compare_t
{
  time_t enter_time;
  int actual;
  int ideal;
  bool on_return;
  int square;
};

struct alarm_compare_t
{
  time_t time_stamp;
};


