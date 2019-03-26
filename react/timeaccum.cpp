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

#include <stdio.h>

#include "timeaccum.h"

static bool timejitter_enabled = false;
static bool timeaccum_enabled = false;

/***************************************************************/

void timejitter_t::enable(bool v)
{
  timejitter_enabled = v;
}

/***************************************************************/

bool timejitter_t::is_enabled(void)
{
  return timejitter_enabled;
}

/***************************************************************/

long us_tv_dif(struct timeval tv2, struct timeval tv1)
{
  long secdif = tv2.tv_sec - tv1.tv_sec;
  long usecdif = tv2.tv_usec - tv1.tv_usec;
  return usecdif + (secdif * 1000000);
}

/***************************************************************/

timejitter_t::timejitter_t(double interval, double pct)
{
  double *tms = new double[10];
  double tenp = (2.0 * interval * pct)  / 10.0;
  double start = interval - (interval * pct) + (tenp / 2.0);
  for (int i=0; i < 10; i++)
  {
    tms[i] = start;
    start += tenp;
  }
  stdinit(tms, 10);
}

/***************************************************************/

timejitter_t::timejitter_t(double t[], int n)
{
  stdinit(t, n);
}

/***************************************************************/

void timejitter_t::stdinit(double t[], int n)
{
  first_time = true;
  times = t;
  n_times = n;
  n_occur = new int[n+1];
  for (int i=0; i < n+1; i++)
  {
    n_occur[i] = 0;
  }
  n_intervals = 0;
}

/***************************************************************/

void timejitter_t::print_results(void)
{
  if (!timejitter_enabled) return;
  printf("Jitter results for %d intervals\n", n_intervals);
  printf("%6d (%0.1lf) (t < %lf)\n", n_occur[0],
         100.0 * ((double) n_occur[0] / (double) n_intervals), times[0]);
  for (int i=1; i < n_times; i++)
  {
    const char *s = (i == (n_times / 2)) ? "***" : "";
    printf("%6d (%0.1lf) (%lf < t < %lf) %s\n", n_occur[i],
         100.0 * ((double) n_occur[i] / (double) n_intervals), times[i-1], times[i], s);
  }
  printf("%6d (%0.1lf) (t > %lf)\n", n_occur[n_times],
      100.0 * ((double) n_occur[n_times] / (double) n_intervals), times[n_times-1]);
}

/***************************************************************/

void timejitter_t::add_occur(double d)
{
  n_intervals++;
  for (int i=0; i < n_times; i++)
  {
    if (d < times[i])
    {
      //if (i != (n_times / 2)) printf("jitter out %d, %lf\n", n_intervals, d);
      n_occur[i]++;
      return;
    }
  }
  n_occur[n_times]++;
}

/***************************************************************/

void timejitter_t::start_of_interval(void)
{
  if (!timejitter_enabled) return;
  if (first_time)
  {
    gettimeofday(&last, NULL);
    first_time = false;
    return;
  }
  struct timeval now;
  gettimeofday(&now, NULL);
  long udif = us_tv_dif(now, last);
  double ddif = (double) udif / 1000000.0;
  add_occur(ddif);
  last = now;
}

/***************************************************************/

void timeaccum_t::enable(bool v)
{
  timeaccum_enabled = v;
}

/***************************************************************/

bool timeaccum_t::is_enabled(void)
{
  return timeaccum_enabled;
}

/***************************************************************/

void timeaccum_t::start(void)
{
  if (!timeaccum_enabled) return;
  gettimeofday(&tv_start, NULL);
}

/***************************************************************/

void timeaccum_t::stop(void)
{
  if (!timeaccum_enabled) return;
  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  tv_total.tv_sec += tv_now.tv_sec - tv_start.tv_sec;
  tv_total.tv_usec += tv_now.tv_usec - tv_start.tv_usec;
  if (tv_total.tv_usec >= 1000000)
  {
    tv_total.tv_sec++;
    tv_total.tv_usec -= 1000000;
  }
  count++;
}

/***************************************************************/

double timeaccum_t::get_total(void)
{
  return (double) tv_total.tv_sec + ((double) tv_total.tv_usec / 1000000.0);
}

/***************************************************************/

double timeaccum_t::get_average(void)
{
  return ((double) tv_total.tv_sec / (double) count) +
          (((double) tv_total.tv_usec / (double) count) / 1000000);

}

/***************************************************************/

timeaccum_t::timeaccum_t(void)
{
  count = 0;
  tv_total.tv_sec = 0;
  tv_total.tv_usec = 0;
}

/***************************************************************/

void timeaccum_t::print(int i)
{
  if (!timeaccum_enabled) return;
  printf("%d: n = %ld, avg = %lf, total = %lf\n",
   i, this->get_count(), this->get_average(), this->get_total());
}
