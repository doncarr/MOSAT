/************************************************************************
This software is part of React, a control engine
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

#include <sys/time.h>

class timeaccum_t
{

private:
  struct timeval tv_start;
  struct timeval tv_total;
  int count;
public:
  static void enable(bool v);
  static bool is_enabled(void);
  timeaccum_t(void);
  void start(void);
  void stop(void);
  double get_total(void);
  long get_count(void) {return count;};
  double get_average(void);
  void print(int i);

};

class timejitter_t
{
private:
  static bool enabled;
  bool first_time;
  struct timeval last;
  double *times;
  int *n_occur;
  int n_times;
  int n_intervals;
  void add_occur(double d);
  void stdinit(double t[], int n);
public:
  static void enable(bool v);
  static bool is_enabled(void);
  void print_results(void);
  timejitter_t(double ti, double pct);
  timejitter_t(double t[], int n);
  void start_of_interval(void);
};


