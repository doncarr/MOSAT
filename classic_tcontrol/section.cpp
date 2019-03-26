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
#include <math.h>
#include <malloc.h>
#include <string.h>

#include "rtcommon.h"
#include "arg.h"

#include "section.h"

/**********************************************************************/

sections_t::sections_t()
{
  dwell_time = DWELL_TIME;
  time_to_detector = TIME_TO_DETECTOR;
}

/**********************************************************************/

double sections_t::get_factor(double driver_type)
{
  double driver_mult = driver_type * -0.01;
  double rand_mult = (((double) random() / (double) RAND_MAX) * 0.02) - 0.01;
  return (1.0 + driver_mult + rand_mult);
}

/**********************************************************************/
int sections_t::get_last_section(int s)
{
  int last = s-1;
  if (last < 0)
  {
    last = n_sections - 1;
  }
  return last;
}

/**********************************************************************/
int sections_t::get_next_section(int s)
{
  int next = s+1;
  if (next > (n_sections - 1))
  {
    next = 0;
  }
  return next;
}

/**********************************************************************/

double sections_t::get_sim_time(int s, double driver_type)
{
  return sections[s].total_time * get_factor(driver_type);
}

/**********************************************************************/

double sections_t::get_location(int s, double fraction_traveled)
{
  double len = get_section_length(s);
  if (sections[s].is_forward)
  {
    return sections[s].distance + (len * fraction_traveled);
  }
  else
  {
    return sections[s].distance - (len * fraction_traveled);
  }
}

/**********************************************************************/
double sections_t::get_section_length(int s)
{
  int next = get_next_section(s);
  double len = get_section_start(s) - get_section_start(next);
  return fabs(len);
}

/**********************************************************************/

void sections_t::read_file(void)
{
  FILE *fp = fopen("sections.txt", "r");
  if (fp == NULL)
  {
    printf("Can't open sections.txt\n");
  }
  char line[300];

  int n = 0;
  double total_time = 0;
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
    else if (argc != 8)
    {
      printf("pid.dat: Wrong number of args, line %d\n", i+1);
      continue;
    }
    printf("%s", line);
    safe_strcpy(sections[n].name, argv[0], sizeof(sections[n].name));
    sections[n].total_time = atof(argv[1]);
    sections[n].distance = atof(argv[2]);
    sections[n].time_to_station =
       sections[n].total_time - dwell_time - time_to_detector;
    sections[n].time_to_green =
       sections[n].total_time - time_to_detector;
    sections[n].has_entrance =
      (argv[3][0] == '1') || (argv[3][0] == 'T') || (argv[3][0] == 't');
    sections[n].time_to_entrance = atof(argv[4]);
    sections[n].is_forward =
      (argv[5][0] == '1') || (argv[5][0] == 'T') || (argv[5][0] == 't');
    sections[n].substation = atol(argv[6]);
    sections[n].accel_end_after_sensor = atof(argv[7]);

    sections[n].cycle_location = total_time;
    sections[n].cycle_time_to_green = total_time + sections[n].time_to_green;

    total_time +=  sections[n].total_time;

    printf("sect[%d]: ttl = %0.1lf, tts = %0.1lf, ttg = %0.1lf, cl = %0.1lf, cttg: %0.1lf, sbst = %d, ae: %0.1lf\n",
      n, sections[n].total_time , sections[n].time_to_station , sections[n].time_to_green,
      sections[n].cycle_location , sections[n].cycle_time_to_green, sections[n].substation,
      sections[n].accel_end_after_sensor);
    n++;
  }
  cycle_time = total_time;
  printf("Total round trip = %d:%d\n", 
             int(cycle_time) / 60, int(cycle_time) % 60);
  n_sections = n;
}

/**********************************************************************

int main(int argc, char *argv[])
{

  sections_t sections;
  sections.read_file();
  for (int i=0; i < sections.get_n_sections(); i++)
  {
    printf("%0.2lf, %0.2lf, %c, %0.2lf, %c, %0.2lf\n",
      sections.get_section_time(i),
      sections.get_section_start(i),
      sections.has_entrance(i)?'T':'F',
      sections.get_time_from_entrance(i),
      sections.is_forward(i)?'T':'F',
      sections.get_section_length(i));
  }
}

****/


