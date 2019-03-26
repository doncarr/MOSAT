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

#define _XOPEN_SOURCE /* glibc2 needs this */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

# include <sys/types.h>
# include <sys/ipc.h>
#include <sys/msg.h>

#include "msg_queue.h"
#include "tcontrol.h"
#include "train_sim.h"
#include "rtcommon.h"
#include "sim_msg.h"

#include "ap_config.h"

#include "section_reader.h"

ap_config_t ap_config(',');

/********************************************************/

class my_notify_t : public sim_ev_notify_t
{
public:
  void trigger_arrival(unsigned section, time_t now);
  void trigger_departure(unsigned section, time_t now);
  void trigger_crossing(unsigned the_section, unsigned the_sensor, time_t now);
};


static int qid;

/********************************************************/

void my_notify_t::trigger_crossing(unsigned the_section, unsigned the_sensor, time_t now)
{
  char buf[30];
  struct tm mytm;
  localtime_r(&now, &mytm);
  strftime(buf, sizeof(buf), "%T", &mytm);

  crossing_queue_event_t msgbuf;
  msgbuf.mtype = 1;
  msgbuf.data.time_stamp = now;
  msgbuf.data.section = the_section;
  msgbuf.data.departure = false;
  msgbuf.data.sensor = the_sensor;
  msgbuf.data.update = false;
  int ret = msgsnd(qid, &msgbuf, sizeof(msgbuf.data), 0);
  if (ret == -1)
  {
    perror("Could not send message");
    exit(0);
  }
  printf("Crossing(%s): section %d, sensor %d\n", buf, the_section, the_sensor);
}


/********************************************************/

void my_notify_t::trigger_arrival(unsigned section, time_t now)
{
  char buf[30];
  struct tm mytm;
  localtime_r(&now, &mytm);
  strftime(buf, sizeof(buf), "%T", &mytm);

  crossing_queue_event_t msgbuf;
  msgbuf.mtype = 1;
  msgbuf.data.time_stamp = now;
  msgbuf.data.section = section;
  msgbuf.data.departure = false;
  msgbuf.data.sensor = 1;
  msgbuf.data.update = false;
  int ret = msgsnd(qid, &msgbuf, sizeof(msgbuf.data), 0);
  if (ret == -1)
  {
    perror("Could not send message");
    exit(0);
  }
  printf("Arrival(%s): section %d, sensor %d\n", buf, section, msgbuf.data.sensor);
}

/********************************************************/

void my_notify_t::trigger_departure(unsigned section, time_t now)
{
  char buf[30];
  struct tm mytm;
  localtime_r(&now, &mytm);
  strftime(buf, sizeof(buf), "%T", &mytm);

  crossing_queue_event_t msgbuf;
  msgbuf.mtype = 1;
  msgbuf.data.time_stamp = now;
  msgbuf.data.section = section;
  msgbuf.data.departure = true;
  msgbuf.data.sensor = 0;
  msgbuf.data.update = false;
  msgbuf.data.skip = false;
  int ret = msgsnd(qid, &msgbuf, sizeof(msgbuf.data), 0);
  if (ret == -1)
  {
    perror("Could not send message");
    exit(0);
  }
  printf("Departure(%s): section %d, sensor %d\n", buf, section, msgbuf.data.sensor);
}

/********************************************************/

void send_update(time_t now)
{
  crossing_queue_event_t msgbuf;
  msgbuf.mtype = 1;
  msgbuf.data.time_stamp = now;
  msgbuf.data.section = 0;
  msgbuf.data.departure = false;
  msgbuf.data.sensor = 0;
  msgbuf.data.update = true;
  msgbuf.data.skip = false;
  int ret = msgsnd(qid, &msgbuf, sizeof(msgbuf.data), 0);
  if (ret == -1)
  {
    perror("Could not send message");
    exit(0);
  }
}

/********************************************************/

void print_help(void)
{
  printf("Usage: line_sim [-c <config-file>] [-d <home dir>] [-rt]\n");
}



/********************************************************/

int main(int argc, char *argv[])
{
  bool use_actual_time = false;
  long wait_time = 100000;
  const char *config_file = "tpconfig.txt";
  const char *home_dir = "./";
  int current_arg;
  for (current_arg=1; current_arg < argc; current_arg++)
  {

    if (0 == strcmp(argv[current_arg], "-c"))
    {
      if (argc > (current_arg + 1))
      {
        current_arg++;
        printf("using config file: %s\n", argv[current_arg]);
        config_file = argv[current_arg];
      }
      else
      {
        print_help();
        printf("For -c option, you MUST specify the config file, %d, %d\n",
                   argc, current_arg);
        exit(1);
      }
    }
    else if (0 == strcmp(argv[current_arg], "-d"))
    {
      if (argc > (current_arg + 1))
      {
        home_dir = argv[current_arg];
        chdir(home_dir);
        home_dir = "./";
      }
      else
      {
        print_help();
        printf("For -d option, you MUST specify the directory, %d, %d\n", argc, current_arg);
        exit(1);
      }
    }
    else if (0 == strcmp(argv[current_arg], "-rt"))
    {
      current_arg++;
      wait_time = 1000000;
      use_actual_time = true;
    }
    else if (0 == strcmp(argv[current_arg], "--help"))
    {
      print_help();
      exit(0);
    }
    else
    {
      printf("Invalid argument: %s\n", argv[current_arg]);
      print_help();
      exit(1);
    }
  }

  ap_config.read_file(config_file);


  qid = connect_message_queue();
  create_sim_queue();

  time_t now;
  //time_t start;
  class my_notify_t my_notify;
  class train_sim_t sim(&my_notify);
  now = sim.read_day();
  //start = now;

  const char *sections_file = ap_config.get_config("SECTIONS_FILE");
  if (sections_file == NULL)
  {
    sections_file = "sections.txt";
  }

  sim.read_sections(sections_file);

  sections.read_section_file();

  if (use_actual_time)
  {
    while(true)
    {
      sim.update(now);
      send_update(now);
      now++;
      if (now > time(NULL)) break;
    }
  }


  sim_msg_t sim_msg;
  int i = 0;
  while(true)
  {
    //printf("-------------------- %d\n", i);
    sim.update(now);
    send_update(now);
 
    if (0 == check_for_sim_msg(&sim_msg))
    {
      printf("sim command recieved: type %d, section %d, sensor %d\n",
         sim_msg.type, sim_msg.section, sim_msg.sensor);
      if (sim_msg.type == 1)
      {
         if (sim_msg.sensor == 0)
         {
           printf("----------triggering departure at section %d\n", sim_msg.section);
           my_notify.trigger_departure(sim_msg.section, now);
         }
         else if (sim_msg.sensor == 1)
         {
           printf("----------triggering arrival at section %d\n", sim_msg.section + 1);
           my_notify.trigger_arrival(sim_msg.section + 1, now);
         }
      }
      else if (sim_msg.type == 5)
      {
        if ((sim_msg.section >= 1) && (sim_msg.section <= 25))
        {
          printf("!!!!!!!!!! set speedup to %d\n", sim_msg.section);
          wait_time = 1000000 / sim_msg.section;
        }
        else
        {
          printf("@@@@@@@@@@@@ Ignoring speedup message, must be from 1 to 25\n");
        }
      }
    } 
    else
    {
      //printf("no sim message . . . \n");
    } 

    if (use_actual_time)
    {
      now = time(NULL);
      usleep(wait_time);
    }
    else
    {
      now++;
      if (i > 5400) usleep(wait_time);
    }


    i++;
  }
}

/********************************************************/



