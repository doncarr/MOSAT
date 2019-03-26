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


#include <time.h>
#include <sys/time.h>
#include <unistd.h>

struct utimer_status_t
{
  struct timeval start_time;
  struct timeval current_time;
  struct timeval next_time;
  struct timeval elapsed_time;
  long time_left;
  long interval;
};

void utimer_print(utimer_status_t s);

class utimer_t
{
private:
  struct timespec next_time;
  struct timespec interval;
  bool busy_wait;
  void inc_time(void);
  void normalize_timespec(struct timespec *ats);
  void my_busy_wait();
public:
  utimer_t();
  utimer_t(struct timespec ts){set_interval(ts); busy_wait=false; set_start_time();};
  //struct timeval elapsed_time(void);
  void set_busy_wait(bool v) {busy_wait = v;};
  void set_interval(struct timespec ts) {interval.tv_sec = ts.tv_sec; interval.tv_nsec = ts.tv_nsec;};
  void set_interval(int usecs) {interval.tv_sec = usecs/1000000; 
                                interval.tv_nsec = (usecs%1000000)*1000;};
  void set_start_time(void);
  // void print_status(void);
  //utimer_status_t get_status(void);
  void wait_next(void);
  //double late_time(void);
  //bool late(void);
};


/*******************************************************************/

