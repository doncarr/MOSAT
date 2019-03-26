
#include <stdio.h>
#include <stdlib.h>

#include "../include/rtcommon.h"
#include "../include/arg.h"

#include "onboard.h"
#include "spd_algo.h"
#include "profile_reader.h"

/*******************************************************/

profile_reader_t::profile_reader_t(void)
{
  last_section = -1;
  my_index = 0;
  last_type = 1;
  type = 1;
  all_profiles = true;
  the_line = 1;
}

/*******************************************************/


static double calc_speed(double t, double t1, double s1, double t2, double s2)
{
  // Here, we do a simple interpolation between two points.
  double conversion_factor, raw_span;
  raw_span = t2 - t1;
  if (raw_span == 0)
  {
    conversion_factor = 0.0;
  }
  else
  {
    conversion_factor = (s2 - s1) / (t2 - t1);
  }
  return ((t - t1) * conversion_factor) + s1;
}


/*********************************************************************/

int profile_reader_t::read_profiles(void)  //sdef_t the_profile[], int max)
{
  int argc, line_num;
    argc = 7;
    if (argc < 3)
    {
      printf("xWrong number of args: %d\n", argc);
      exit(0);
    }
  delim_file_t df(600, 50, '|', '#');
    if (argc < 3)
    {
      printf("xxWrong number of args: %d\n", argc);
      exit(0);
    }
  char **argv;
  int n_segments = 0;
  if (all_profiles)
  {
    if (the_line == 1)
    {
      argv = df.first("line1_profile_all.txt", &argc, &line_num);
    }
    else
    {
      argv = df.first("line2_profile_all.txt", &argc, &line_num);
    }
  }
  else
  {
    if (the_line == 1)
    {
      argv = df.first("line1_profile.txt", &argc, &line_num);
    }
    else
    {
      argv = df.first("line2_profile.txt", &argc, &line_num);
    }
  }
  if (argc != 2)
  {
    printf("xxxWrong number of args: %d\n", argc);
    exit(0);
  }
  for (int i=0; (argv != NULL) && (i < RT_MAX_SECTIONS); i++)
  {
    if (argc != 2)
    {
      printf("Wrong number of args (first line): %d\n", argc);
      exit(0);
    }
    printf("%s: %s\n", argv[0], argv[1]);
    safe_strcpy(section_profile[i].st1, argv[0], sizeof(section_profile[i].st1));
    section_profile[i].distance = atof(argv[1]);
    section_profile[i].total_distance = atof(argv[1]);
    argv = df.next(&argc, &line_num);
    //printf("argc = %d, %s, %d\n", argc, __FILE__, __LINE__);
    if (argc < 3)
    {
      printf("Wrong number of args: %d\n", argc);
      exit(0);
    }

    if (argv == NULL)
    {
      printf("No more args after %s\n", argv[0]);
      exit(0);
    }


    if (argc < 3)
    {
      printf("Wrong number of args: %d\n", argc);
      exit(0);
    }
    //printf("argc = %d, %s, %d\n", argc, __FILE__, __LINE__);

    if (argc < 3)
    {
      printf("Wrong number of args: %d\n", argc);
      exit(0);
    }

    section_profile[i].n = argc;
    section_profile[i].dist = new double[argc];
    section_profile[i].speed = new double[argc];
    section_profile[i].low = new double[argc];
    section_profile[i].high = new double[argc];
    section_profile[i].very_high = new double[argc];
    for (int j=0; j < argc; j++)
    {
      section_profile[i].dist[j] = atof(argv[j]);
    } 

    if (all_profiles)
    {
      argv = df.next(&argc, &line_num);
      if ((argv == NULL) || (argc != section_profile[i].n))
      {
        printf("Wrong number of args: %d, expected %d\n", argc, section_profile[i].n);
        exit(0);
      }
      for (int j=0; j < argc; j++)
      {
        section_profile[i].low[j] = atof(argv[j]);
      } 
    }

    argv = df.next(&argc, &line_num);
    if ((argv == NULL) || (argc != section_profile[i].n))
    {
      printf("Wrong number of args: %d, expected %d\n", argc, section_profile[i].n);
      exit(0);
    }
    for (int j=0; j < argc; j++)
    {
      section_profile[i].speed[j] = atof(argv[j]);
    } 

    if (all_profiles)
    {
      argv = df.next(&argc, &line_num);
      if ((argv == NULL) || (argc != section_profile[i].n))
      {
        printf("Wrong number of args: %d, expected %d\n", argc, section_profile[i].n);
        exit(0);
      }
      for (int j=0; j < argc; j++)
      {
        section_profile[i].high[j] = atof(argv[j]);
      } 
      
      argv = df.next(&argc, &line_num);
      if ((argv == NULL) || (argc != section_profile[i].n))
      {
        printf("Wrong number of args: %d, expected %d\n", argc, section_profile[i].n);
        exit(0);
      }
      for (int j=0; j < argc; j++)
      {
        section_profile[i].very_high[j] = atof(argv[j]);
      } 
    }
    argv = df.next(&argc, &line_num);
    n_segments++;
  }

  double mydiff[4] = {-6, 0, 5, 12};
  FILE *fp = fopen("junk.txt", "w");
  for (int i=0; i < n_segments; i++)
  {
    fprintf(fp, "%s|%0.1lf|\n", section_profile[i].st1, section_profile[i].distance);
    for (int j=0; j < section_profile[i].n; j++)
    {
      fprintf(fp, "%0.1lf|", section_profile[i].dist[j]);
    }
    fprintf(fp, "\n");
    for (int cnt=0; cnt < 4; cnt++)
    {
      for (int j=0; j < section_profile[i].n; j++)
      {
        fprintf(fp, "%0.1lf|", section_profile[i].speed[j] + mydiff[cnt]);
      }
      fprintf(fp, "\n");
    }
  }
  n_sections = n_segments;
  return n_segments;
}


/*********************************************************************/

void profile_reader_t::calc_desired(int a_section, 
       double a_speed, double a_distance, 
      current_speed_limits_t *limits, int *mode, bool *warn)
{
  if (a_section != last_section)
  {
    my_index = 0;
    last_section = a_section;
    //spd_beep();
  }

  //mvprintw(16,2,"ind: %2d, n: %2d, %6.0lf %6.0lf", my_index + 2, profile[a_section].n,
   //                a_distance, profile[a_section].dist[my_index+1]);

  if (a_distance > section_profile[a_section].dist[my_index+1])
  {
    if ((my_index + 2) < section_profile[a_section].n)
    {
      my_index++;
      //spd_beep();
    }
  } 
  *warn = false;
  double distance_to_next = section_profile[a_section].dist[my_index+1] - a_distance; 
  double distance_in_5_sec = a_speed * (1.0/3.6) * 5.5;
  if (distance_to_next < distance_in_5_sec) 
       /* The idea is to warn the driver 5 seconds BEFORE he needs to decelerate. 
          We believe there is currently a lot of wasted time with drivers braking before it is needed.
          If we can tell them exactly when to start braking, we might save some seconds in each segment.
          This obviously needs some work before it is a final design! */
  {
    *warn = true;
  }

  if (section_profile[a_section].speed[my_index+1] < section_profile[a_section].speed[my_index])
  {
    type = 0;
  }
  else if (section_profile[a_section].speed[my_index+1] > section_profile[a_section].speed[my_index])
  {
    type = 2;
  }
  else
  {
    type = 1;
  }

  if (type != 1) 
  {
    *warn = false;
  }

 /*mvprintw(15,2,"Section: %2d, index: %2d, spd1: %5.0lf, spd2: %5.1lf, d1: %8.1lf, d2: %8.1lf, n: %d", a_section, my_index, 
          profile[a_section].speed[my_index], profile[a_section].speed[my_index + 1],
          profile[a_section].dist[my_index], profile[a_section].dist[my_index + 1],
                                      profile[a_section].n); */
  double spd = calc_speed(a_distance, section_profile[a_section].dist[my_index],
              section_profile[a_section].speed[my_index], 
              section_profile[a_section].dist[my_index+1], 
              section_profile[a_section].speed[my_index+1]);
  if (spd < 0.0) spd = 0.0;
  double low = calc_speed(a_distance, section_profile[a_section].dist[my_index],
              section_profile[a_section].low[my_index], 
              section_profile[a_section].dist[my_index+1], 
              section_profile[a_section].low[my_index+1]);
  if (low < -1.0) low = -1.0;
  double high = calc_speed(a_distance, section_profile[a_section].dist[my_index],
              section_profile[a_section].high[my_index], 
              section_profile[a_section].dist[my_index+1], 
              section_profile[a_section].high[my_index+1]);
  if (high < 5.0) high = 5.0;
  double very_high = calc_speed(a_distance, section_profile[a_section].dist[my_index],
              section_profile[a_section].very_high[my_index], 
              section_profile[a_section].dist[my_index+1], 
              section_profile[a_section].very_high[my_index+1]);
  if (very_high < 8.0) very_high = 8.0;
  //mvprintw(16,2,"%4.1lf < %4.1lf > %4.1lf >> %4.1lf", low, spd, high, very_high); 
  limits->desired = spd;
  limits->low = low;
  limits->high = high;
  limits->very_high = very_high;
  //*speed = spd;
  *mode = type;
  last_type = type;
}

/********************************************************************

void print_profile(int n)
{
  FILE *fp;
  fp = fopen("line1_profile.txt", "w");
  for (int i=0; i < n; i++)
  {
    fprintf(fp, "%s|%0.0lf|\n", vel_profile[i].st1, vel_profile[i].distance);
    for (int j=0; j < vel_profile[i].n; j++)
    {
      fprintf(fp, "%0.1lf|", vel_profile[i].dist[j]);
    }
    fprintf(fp, "\n");
    for (int j=0; j < vel_profile[i].n; j++)
    {
      fprintf(fp, "%0.1lf|", vel_profile[i].speed[j]);
    }
    fprintf(fp, "\n");
  }
}
**************************************/

/**********************************************************************/
