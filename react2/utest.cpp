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
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>


#include "utimer.h"

/*******************************************************************/

int main(int argc, char *argv[])
{
  // a main for testing
  struct timeval mytv;
  struct timespec myts;
  struct timeval times[10000];
  utimer_status_t s[10000];
  long max = 100;
  long updates_per_second = 2000;
  long interval = 1000000 / updates_per_second;

  utimer_t utimer;
  utimer.set_interval(interval);
  utimer.set_start_time();
  s[0] = utimer.get_status();
  gettimeofday(&mytv, NULL);
  times[0] = mytv;
  //printf("time: %ld, %6ld\n", mytv.tv_sec, mytv.tv_usec);
  for (long i=0; i < max; i++)
  {
    //printf("%d\n", i);
    utimer.wait_next();
    //s[i+1] = utimer.get_status();
    //mytv = utimer.elapsed_time();
    gettimeofday(&mytv, NULL);
    times[i+1] = mytv;
    //printf("elapsed time: %ld, %ld\n", mytv.tv_sec, mytv.tv_usec);
    //long dif = usec_timer_dif(tv2);
    myts.tv_sec = 0;
    myts.tv_nsec = 1;
    //nanosleep(&myts, NULL);
  }
  double total = 0;
  for (int i=0; i < max + 1; i++)
  {
    printf("time: %ld, %6ld", times[i].tv_sec, times[i].tv_usec);
    if (i>0)
    {
      long d = times[i].tv_usec - times[i-1].tv_usec;
      printf(", dif = %ld", d);
      total += d;
    }
    printf("\n");
  }
  printf("Average = %lf\n", total / (double) (max-1));
  /****
  for (int i=0; i < max + 1; i++)
  {
    utimer_print(s[i]);
    printf("\n");
  }
  ****/
}

/*******************************************************************/


