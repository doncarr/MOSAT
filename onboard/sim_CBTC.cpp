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

#ifndef ARM
#include <ncurses.h>
#endif

#include "rtcommon.h"

#include "utimer.h"

#include "onboard.h"

#include "profile_reader.h"
#include "sim_reader.h"
                                                                                
#define DIF_LEN (100)
#define DRIVER_DELAY (10)
#define RT_FACTOR (100)

#include "arg.h"

#include "ap_config.h"

//static int n_sections;

ap_config_t ap_config;

/*******************************************************************/

sim_reader_t sreader_1;

sim_reader_t sreader_2;

sim_reader_t *sreader = &sreader_1;

profile_reader_t preader_1;

profile_reader_t preader_2;

profile_reader_t *preader = &preader_1;

int n_seg;


/**********************************************************************/

int main(int argc, char *argv[])
{
  const char *config_file = "onboard_config.txt";

  sreader_1.set_line(1);

  sreader_2.set_line(2);

  preader_1.set_all(true);

  preader_1.set_line(1);

  preader_2.set_all(true);

  preader_2.set_line(2);

  ap_config.read_file(config_file);

  preader_1.read_profiles();

  preader_2.read_profiles();

  sreader_1.read_sim_data();

  sreader_2.read_sim_data();

  utimer_t utimer;

  utimer.set_busy_wait(false);

  utimer.set_interval(1000000);

  int j;
  n_seg = preader->get_n_sections();

  for (j=0; j < n_seg; j++)
  {
    double total_distance = preader->get_distance(j); //get_total_dist(j);

    double total_time = sreader->get_total_time(j);

    double distance = 0.0;

    double actual = 0;

    spd_discrete_t discretes;

    sreader->get_sim_speed_dist(j, -1, &distance, &actual, &discretes);

    int time_in_section = 0;

    printf("\nLeaving station %s\n", preader->get_station_name(j));
    printf("Section %d entered, total distance: %lf, total time: %lf\n\n", j, total_distance, total_time);

    utimer.set_start_time();
    for (int i=0; true; i++)
    {
      utimer.wait_next();
      double now = (double) i / 1.0;
     
      sreader->get_sim_speed_dist(j, time_in_section, &distance, &actual, &discretes);

      printf("    Time: %d, distance: %0.1lf, speed %0.1lf\n", i, distance, actual);

      time_in_section++;
      if (now > total_time) break;
    }
    printf("\nArrived at station %s\n\n", preader->get_station_name((j+1) % n_seg));
    sleep(5);
  }
  sleep(1);
  printf("Simulation Done\n");
}

/***********************************************************************************************************/
