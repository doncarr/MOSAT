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
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <math.h>

#include "rtcommon.h"
#include "utimer.h"

#include "spd_display.h"
#include "spd_comm.h"

                                                                                
#define DIF_LEN (100)
#define DRIVER_DELAY (10)
#define RT_FACTOR (100)
#define MAXS (100)
//#define DISTANCE

#include "arg.h"

int main(int argc, char *argv[])
{

  init_io();
  sleep(1);
  spd_init_screen();
  sleep(1);

  utimer_t utimer;
  utimer.set_busy_wait(false);
  utimer.set_interval(1000000);
  utimer.set_start_time();

  spd_init_segment("-------------", "-------------"); 

  double now = 0.0;
  while(true)
  {
    utimer.wait_next();
    if (utimer.late_time() > 1.0)
    {
      utimer.set_start_time();
    }
   
    double actual = 0;
    double distance = 0;
    spd_discrete_t discretes;

    get_actual_speed_dist(0, 0, &distance, &actual, &discretes);
      
    spd_print_current(0, actual, 0, false, 0, distance, now, 0);

    int ch = spd_getch();
    if (ch == 'q') {exit(0);} 
    now++;
  }
  sleep(1);
  spd_wait_key("Hit any key to exit"); 
  spd_endwin();
}

/***********************************************************************************************************/
