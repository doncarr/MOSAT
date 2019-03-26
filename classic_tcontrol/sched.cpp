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

#include "section.h"
#include "arg.h"
#include "rtcommon.h"

struct action_t
{
  bool is_add;
  int train;
  double time;
};

class scheduler_t
{
private:
  int n_slots;
  int n_actions;
  int next_action;
  int n_lines;
  double last_time;
  double time_slots[25];
  bool in_system[25];
  double cycle_time;
  double entrance_time_from_start;
  double times[25][30];
  double time_table[60][25*30];
  action_t actions[10000];
public:
  scheduler_t(void);
  void set_even_slots(int n, double ctime, double t_from_start);
  void calc_times(int n_hours);
  void read_schedule(void);
  void print(void);
  bool get_next_action(bool *is_add, int *train, double *time);
  bool is_active(int train, double time);
};

sections_t *sect;

/*****************************************************************/

int split_time(char *str)
{
  /* This takes a time string in the format MM:SS and converts to
   * seconds
   */
  char temp[30];
  safe_strcpy(temp, (const char*) str, sizeof(temp));
  int argc;
  char *argv[3];
  argc = get_delim_args(temp, argv,  ':', 3);
  //printf("str '%s', n %d", str, argc);
  //for (int i=0; i < argc; i++) printf(", %s", argv[i]);
  //printf("\n");
  if (argc == 1)
  {
    return atol(argv[0]) * 60;
  }
  else if (argc == 2)
  {
    return (3600 * atol(argv[0])) + (60 * atol(argv[1]));
  }
  else if (argc == 3)
  {
    return (3600 * atol(argv[0])) + (60 * atol(argv[1]) + atol(argv[2]));
  }
  else
  {
    return 0;
  }
}


/*********************************************************************/

void scheduler_t::set_even_slots(int n, double ctime, double t_from_start)
{
  cycle_time = ctime;
  n_slots = n;
  entrance_time_from_start = t_from_start;
  double time_between_slots = cycle_time / n_slots;
  double total = 0.0;
  for (int i=0; i < n_slots; i++)
  {
    time_slots[i] = total;
    in_system[i] = false;
    total += time_between_slots;
  }
}

/*********************************************************************/

void scheduler_t::read_schedule(void)
{
  FILE *fp = fopen("sched.txt", "r");
  if (fp == NULL)
  {
    printf("Can't open sched.txt\n");
  }
  char line[300];

  //int n = 0;
  //double total_time = 0;
  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    char tmp[300];
    int argc;
    char *argv[25];
    safe_strcpy(tmp, (const char*)line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '|', 25);
    if (argc == 0)
    {
      continue;
    }
    else if (argv[0][0] == '#')
    {
      continue;
    }
    else if (argc < 3)
    {
      printf("sched.txt: Wrong number of args, line %d\n", i+1);
      continue;
    }
    printf("%s", line);
    bool is_add;
    int secs;
    if (argv[0][0] == '+')
    {
      is_add = true;
    }
    else if (argv[0][0] == '-')
    {
      is_add = false;
    }
    else
    {
      printf("sched.txt: invalid first arg: %s, line %d\n", argv[0], i+1);
      continue;
    }

    secs = split_time(argv[1]);
    //safe_strcpy(sections[n].name, argv[0], sizeof(sections[n].name));
    printf("%s: %02d:%02d:%02d", is_add?"add":"del", secs / 3600, (secs % 3600) / 60, secs % 60 );
    double action_time = secs;
    for (int j=2; j < argc; j++)
    {
      int n = atol(argv[j]);
      printf(", %d", n);
      for (int k = 0; k < n_lines; k++)
      {
        if (times[n][k] >= action_time)
        {
          action_time =times[n][k];
          break;
        }
      }
      actions[n_actions].train = n;
      actions[n_actions].time = action_time;
      actions[n_actions].is_add = is_add;
      n_actions++;
    }
    printf("\n");
  }
}

/*********************************************************************/

void scheduler_t::calc_times(int n_hours)
{
  n_lines = int(double(n_hours) * 3600.0 / cycle_time);
  for (int i=0; i < n_slots; i++)
  {
    printf("%0.0lf ", time_slots[i]);
  }
  printf("\n");
  for (int i=0; i < n_slots; i++)
  {
    printf("   %2d     ", i);
  }
  printf("\n");
  printf(
"------------------------------------------------------------------------------------------------------------------------");
  printf("\n");
  for (int i=0; i < n_lines; i++)
  {
    for (int j=0; j < n_slots; j++)
    {
      double t = ((double(i) * cycle_time) + time_slots[j]) + entrance_time_from_start;
      int secs = (int) t + (6 * 3600);
      times[j][i] = secs;
      int h = secs / 3600;
      int m = (secs % 3600) / 60;
      int s = (secs % 3600) % 60;
      printf("%02d:%02d:%02d  ", h, m, s);
    }
    printf("\n");
  }
}

/*********************************************************************/

bool scheduler_t::get_next_action(bool *is_add, int *train, double *time)
{
  if (next_action > (n_actions - 1))
  {
    return false;
  }
  *is_add = actions[next_action].is_add;
  *train = actions[next_action].train;
  *time = actions[next_action].time;
  if (*is_add && in_system[*train])
  {
    printf("train %d is already in the system\n", *train);
  }
  if (!*is_add && !in_system[*train])
  {
    printf("train %d is NOT in the system\n", *train);
  }
  if ((*time - last_time) < 5.0)
  {
    int secs = (int) *time;
    int h = secs / 3600;
    int m = (secs % 3600) / 60;
    int s = (secs % 3600) % 60;
    printf("Actions less than 5 minutes apart: %02d:%02d:%02d\n", h, m, s);
  }
  in_system[*train] = *is_add;
  last_time = *time;
  next_action++;
  return true;
}

/*********************************************************************/

void scheduler_t::print(void)
{
  for (int i=0; i < sect->get_n_sections(); i++)
  {
    int next_action = 0;
    for (int j=0; j < n_slots; j++) in_system[j] = false;
    double station_time = sect->get_cycle_time_to_station(i);
    for (int j=0; j < n_lines; j++)
    {
      for (int k=0; k < n_slots; k++)
      {
        double now =  times[k][j] + station_time;
        while ((actions[next_action].time < now) && (next_action < n_actions))
        {
          in_system[actions[next_action].train] = actions[next_action].is_add;
          next_action++;
        }
        if (in_system[k])
        {
          time_table[i][j * n_slots + k] = now;
        }
        else
        {
          time_table[i][j * n_slots + k] = -1.0;
        }
        //double times[25][30];
        //double time_table[60][25*30];
      }
    }
  }
  FILE *fp = fopen("timetable.txt", "w");
  if (fp == NULL)
  {
    printf("Can't open timetable.txt");
    exit(0);
  }
  for (int i=0; i < (n_slots * n_lines); i++)
  {
    int count = 0;
    char line[500];
    line[0] = '\0';
    for (int j=0; j < sect->get_n_sections(); j++)
    {
      int secs = (int) time_table[j][i];
      if (secs >= 0)
      {
        count++;
        int h = secs / 3600;
        int m = (secs % 3600) / 60;
        int s = (secs % 3600) % 60;
        char temp[20];
        snprintf(temp, sizeof(temp), "%02d:%02d:%02d\t", h, m, s);
        safe_strcat(line, temp, sizeof(line));
        //fprintf(fp, "%02d:%02d:%02d\t", h, m, s);
      }
      else
      {
        safe_strcat(line, "\t", sizeof(line));
        //fprintf(fp, "\t");
      }
    }
    if (count > 0)
    {
      fprintf(fp, "%s\n", line);
    }
  }
  fclose(fp);
}

/*********************************************************************/

scheduler_t::scheduler_t(void)
{
  last_time = 0.0;
  n_slots = 0;
  n_actions = 0;
  next_action = 0;
  n_lines = 0;
}

/*********************************************************************/

int main(int argc, char *argv[])
{
  sect = new sections_t;
  sect->read_file();
  scheduler_t *s = new scheduler_t;
  s->set_even_slots(12, 62.0 * 60.0, 40.0);
  s->calc_times(16);
  s->read_schedule();
  bool is_add; int train; double time;
  while (s->get_next_action(&is_add, &train, &time))
  {
    int secs = (int) time;
    int h = secs / 3600;
    int m = (secs % 3600) / 60;
    int s = (secs % 3600) % 60;
    printf("%s train %d at %02d:%02d:%02d\n", is_add?"insert":"remove", train, h, m, s);
  }
  s->print();
}

/*********************************************************************/
