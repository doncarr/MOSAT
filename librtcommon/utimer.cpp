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
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>


#include "utimer.h"
/*******************************************************************

  This is a timer for a process that needs to execute actions at
  regular intervals. All intervals are given in micro-seconds, but of course
  the timing would greatly depend on the underlying OS and the
  loading on the system. That said, the average time between
  executions will be very very close since I read the current time and
  re-calculate the wait time every time. So, if you were late on the
  previous interval because of other processes getting the CPU, or
  inacuracies in the way the OS does things, I will wait less
  the next time.

*******************************************************************/

void utimer_t::normalize_timespec(struct timespec *ats)
{
  while (ats->tv_nsec >= 1000000000)
  {
    ats->tv_sec += 1;
    ats->tv_nsec -= 1000000000;
  }
  while (ats->tv_nsec < 0)
  {
    ats->tv_sec -= 1;
    ats->tv_nsec += 1000000000;
  }
}

/*******************************************************************/

void utimer_t::inc_time(void)
{
  /*
   * This increments the time for the next event by the
   * given number of microsecents.
   */

  //printf("inc_time (before): %ld %ld\n", next_time.tv_sec, next_time.tv_nsec);
  next_time.tv_sec += interval.tv_sec;
  next_time.tv_nsec += interval.tv_nsec;
  //printf("inc_time (after1): %ld %ld\n", next_time.tv_sec, next_time.tv_nsec);
  normalize_timespec(&next_time);
  //printf("inc_time (after2): %ld %ld\n", next_time.tv_sec, next_time.tv_nsec);
}

/*******************************************************************/

void utimer_t::my_busy_wait()
{
  // Probably should NOT use busy wait anymroe . . . . . . 
  struct timespec ts;
  bool done = false;
  while (!done)
  {
    clock_gettime(CLOCK_MONOTONIC, &ts);
    if ((ts.tv_sec == next_time.tv_sec) && (ts.tv_nsec > next_time.tv_nsec)) done = true;
    else if (ts.tv_sec > next_time.tv_sec) done = true;
  }
}

/*******************************************************************/

void utimer_t::wait_next(void)
{
  /*
   * This procedure waits until the next time interval. Notice that
   * we check the current time and only wait for the REMAINING time.
   * This takes into account processing time or the fact that this
   * process might not have gottong the CPU right away, or just
   * inacuracies in the way the OS handles sleep. For instance in
   * DOS, the wait would be to the nearest 55 miliseconds.
   */
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  //printf("time is: %ld %ld\n", ts.tv_sec, ts.tv_nsec);
  if (busy_wait)
  {
    //printf("Calling busy wait: %ld %ld\n", next_time.tv_sec, next_time.tv_nsec);
    my_busy_wait();
  }
  else
  {
    //printf("waiting for: %ld %ld\n", next_time.tv_sec, next_time.tv_nsec);
    int retval = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,  &next_time, NULL);
    while (retval == -1)
    { 
      // I have been told that eventually, this timer will NEVER return because of an interrupt
      // But, I am still checking to be sure . . . . . 
      if (errno == EINTR)
      {
        retval = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,  &next_time, NULL);
      }
      else if (errno == EINVAL)
      {
        //printf("Error in utimer code %s:%d\n", __FILE__, __LINE__);
        break;
      }
      else if (errno == EFAULT)
      {
        //printf("System error in utimer (EFAULT) %s:%d\n", __FILE__, __LINE__);
        break;
      }
      else
      {
        break; // Should never happen, but break to avoid infinite loop; 
      }
    }
  }
  //printf("time expired: %ld %ld\n", next_time.tv_sec, next_time.tv_nsec);
  inc_time();
}

/*******************************************************************/
void utimer_t::set_start_time(void)
{
  clock_gettime(CLOCK_MONOTONIC, &next_time);
  printf("current time: %ld %ld\n", next_time.tv_sec, next_time.tv_nsec);
  inc_time();
  printf("next time: %ld %ld\n", next_time.tv_sec, next_time.tv_nsec);
}

/*******************************************************************/

utimer_t::utimer_t()
{
  /*
   * Constructor for the timer, default to a start time of now
   * and a 1 second interval.
   */

  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  printf("time now is: %ld %ld\n", ts.tv_sec, ts.tv_nsec);

  ts.tv_sec = 1;
  ts.tv_nsec = 0;

  busy_wait = false; // Default, do not use busy wait, use usleep(). 
  set_interval(ts); // Default interval is 1 second.
  printf("Interval is: %ld %ld\n", interval.tv_sec, interval.tv_nsec);
  set_start_time(); // Default start time is now
  printf("Interval is: %ld %ld\n", interval.tv_sec, interval.tv_nsec);
  printf("next_time is: %ld %ld\n", next_time.tv_sec, next_time.tv_nsec);
}

/*******************************************************************
  struct timespec res, mc, rc;

  clock_getres(CLOCK_REALTIME, &res);
  printf("Res = %ld secs, %ld nanoseconds\n", res.tv_sec, res.tv_nsec);
  clock_getres(CLOCK_MONOTONIC, &res);
  printf("Res = %ld secs, %ld nanoseconds\n", res.tv_sec, res.tv_nsec);

  clock_gettime(CLOCK_MONOTONIC, &mc);
  clock_gettime(CLOCK_REALTIME, &rc);
  time_t now = time(NULL);
  printf("MonotonicClock = %ld secs, %ld nanoseconds\n", mc.tv_sec, mc.tv_nsec);
  printf("RealtimeClock = %ld secs, %ld nanoseconds\n", rc.tv_sec, rc.tv_nsec);
  printf("Time = %ld\n", now);


int main(int argc, char *argv[])
{
  // a main for testing
  utimer_t utimer;
  utimer.set_interval(1000000);
  utimer.set_start_time();
  struct timeval mytv;
  for (int i=0; true; i++)
  {
    printf("%d\n", i);
    utimer.wait_next();
    mytv = utimer.elapsed_time();
    printf("elapsed time: %ld, %ld\n", mytv.tv_sec, mytv.tv_usec);
    usleep(400000L);
  }
}

*******************************************************************/


