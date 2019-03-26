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
#include "spd_algo.h"
#include "spd_algo_DC.h"
#include "spd_algo_RD.h"
#include "spd_algo_VV.h"

#include "spd_display.h"
#include "spd_comm.h"

#include "profile_reader.h"
#include "sim_reader.h"
                                                                                
#define DIF_LEN (100)
#define DRIVER_DELAY (10)
#define RT_FACTOR (100)

#include "arg.h"
#include "ap_config.h"

//static int n_sections;
static int the_line = 1;
static bool all_profiles = false;

ap_config_t ap_config;

const char *L2V1_names[] = {
"Tetlan-Aurora", 
"Aurora-SanJacinto", 
"SanJacinto-SanAndres", 
"SanAndres-CristobalDeO", 
"CristobalDeO-Obaltos", 
"Obaltos-BelisarioDom", 
"BelisarioDom-SanJuanDeDio", 
"SanJuanDeDio-PlazaUnivers", 
"PlazaUnivers-Juarez"
};

const char *L2V2_names[] = {
"Juarez-PlazaUnivers", 
"PlazaUnivers-SanJuanDeDio", 
"SanJuanDeDio-BelisarioDom", 
"BelisarioDom-Oblatos", 
"Oblatos-CristobalDe0", 
"CristobalDe0-SanAndres", 
"SanAndres-SanJacinto", 
"SanJacinto-Aurora", 
"Aurora-Tetlan"
};


const char *L1V1_names[] = {
"Per.Sur-Tesoro", 
"Tesoro-Espana", 
"Espana-PatriaSur", 
"PatriaSur-IslaRaza", 
"IslaRaza-18deMarzo", 
"18deMarzo-Urdaneta", 
"Urdaneta-UdDeportivo", 
"UdDeportivo-StaFilomena", 
"StaFilomena-Washington", 
"Washington-Mexicaltzing", 
"Mexicaltzing-Juarez", 
"Juarez-Refugio", 
"Refugio-Mezquitan", 
"Mezquitan-AvCamacho", 
"AvCamacho-DivNorte", 
"DivNorte-Atemajac", 
"Atemajac-Dermatologi", 
"Dermatologi-Per.Norte", 
};

const char *L1V2_names[] = {
"Per.Norte-Dermatologi", 
"Dermatologi-Atemajac", 
"Atemajac-DivNorte", 
"DivNorte-AvCamacho", 
"AvCamacho-Mezquitan", 
"Mezquitan-Refugio", 
"Refugio-Juarez", 
"Juarez-Mexicaltzing", 
"Mexicaltzing-Washington", 
"Washington-StaFilomena", 
"StaFilomena-UdDeportivo", 
"UdDeportivo-Urdaneta", 
"Urdaneta-18deMarzo", 
"18deMarzo-IslaRaza", 
"IslaRaza-PatriaSur", 
"PatriaSur-Espana", 
"Espana-Tesoro", 
"Tesoro-Per.Sur", 
};


/*******************************************************************/

void dispatch_algorithms(time_t now, int command, double speed, double distance)
{

}

/*******************************************************************/

sim_reader_t sreader_1;
sim_reader_t sreader_2;
sim_reader_t *sreader = &sreader_1;
profile_reader_t preader_1;
profile_reader_t preader_2;
profile_reader_t *preader = &preader_1;
int n_seg;

void pick_station(int *line_via, int *station)
{
  //if (pick_mode)
  {
    //printf("Picking line . . .\n");
    const char *line_list[4] = {
          "Linea 1, Via 1 (Per Sur)", 
          "Linea 1, Via 2 (Per Nte)", 
          "Linea 2, Via 1 (Tetlan)",
          "Linea 2, Via 2 (Juarez)"
         };
    int opt = select_from_list(4, line_list, "Selecionar la Linea");
    *line_via = opt; 
    //the_line = opt + 1;
    //preader->set_line(opt+1);
    //sreader->set_line(opt+1);
    if (opt == 0)
    {
      preader = &preader_1;
      sreader = &sreader_1;
      int ns = 18;
      int opt = select_from_list(ns, L1V1_names, "Selecionar la estacion");
      *station = opt;
    }
    if (opt == 1)
    {
      *station = opt;
      preader = &preader_1;
      sreader = &sreader_1;
      int ns = 18;
      int opt = select_from_list(ns, L1V2_names, "Selecionar la estacion");
      *station = opt + 18;
    }
    else if (opt == 2)
    {
      preader = &preader_2;
      sreader = &sreader_2;
      int ns = 9;
      int opt = select_from_list(ns, L2V1_names, "Selecionar la estacion");
      *station = opt;
    }
    else if (opt == 3)
    {
      preader = &preader_2;
      sreader = &sreader_2;
      int ns = 9;
      int opt = select_from_list(ns, L2V2_names, "Selecionar la estacion");
      *station = opt + 11;
    }
    n_seg = preader->get_n_sections();
  }

/*****
  if (line_via == 0) // Line1 Via1
  {
    preader = &preader_1;
    sreader = &sreader_1;
    start = start_station;
  }
  else if (line_via == 1) // Line1 Via2
  {
    preader = &preader_1;
    sreader = &sreader_1;
    start = start_station + 18;
  }
  else if (line_via == 2) // Line1 Via2
  {
    preader = &preader_2;
    sreader = &sreader_2;
    start = start_station;
  }
  else if (line_via == 3) // Line1 Via2
  {
    preader = &preader_2;
    sreader = &sreader_2;
    start = start_station + 11;
  }
******/
}

/**********************************************************************/

int main(int argc, char *argv[])
{
  FILE *pfp = NULL;
  bool sim_mode;
  char base_name[200];
  int start = 0;

  //print_profile();
  //react_trace.set_level(5);
  speed_algorithm_DC_t spd_DC;
  speed_algorithm_RD_t spd_RD;
  speed_algorithm_VV_t spd_VV;

  const char *config_file = "onboard_config.txt";

  sim_mode = false;
  //bool pick_mode = false;
  bool free_running_mode = false;
  int current_arg;
  bool create_profiles = false;
  bool detect_with_doors = false;
  sreader_1.set_line(1);
  sreader_2.set_line(2);
  preader_1.set_all(false);
  preader_1.set_line(1);
  preader_2.set_all(false);
  preader_2.set_line(2);

  for (current_arg=1; current_arg < argc; current_arg++)
  {
    if (0 == strcasecmp(argv[current_arg], "-s"))
    {
      printf("Setting simulation mode . . \n");
      sim_mode = true;
    } 
    else if (0 == strcasecmp(argv[current_arg], "-p"))
    {
      printf("Setting pick mode . . \n");
      //pick_mode = true;
    } 
    else if (0 == strcasecmp(argv[current_arg], "-l"))
    {
      printf("Setting free running mode (libre) . . \n");
      free_running_mode = true;
    } 
    else if (0 == strcasecmp(argv[current_arg], "-L1"))
    {
      printf("Setting to line 1 . . \n");
      preader = &preader_1;
      sreader = &sreader_1;
      the_line = 1;
    } 
    else if (0 == strcasecmp(argv[current_arg], "-L2"))
    {
      printf("Setting to line 2 . . \n");
      preader = &preader_2;
      sreader = &sreader_2;
      the_line = 2;
    } 
    else if (0 == strcasecmp(argv[current_arg], "-f"))
    {
      if (argc > (current_arg + 1))
      {
        current_arg++;
        start = atol(argv[current_arg]);
        printf("Setting to first section to %d \n", start);
      }
      else
      {
        printf("Can't set first section, not enough args\n");
      }
    }
    else if (0 == strcasecmp(argv[current_arg], "-cp"))
    {
      create_profiles = true;
      printf("Create profiles ON\n");
    }
    else if (0 == strcasecmp(argv[current_arg], "-a"))
    {
      all_profiles = true;
      preader_1.set_all(true);
      preader_2.set_all(true);
      printf("All profiles ON\n");
    }
    else if (0 == strcasecmp(argv[current_arg], "-door"))
    {
      detect_with_doors = true;
      printf("using doors to detect arrival/departure\n");
    }
    else if (0 == strcasecmp(argv[current_arg], "-c"))
    {
      if (argc > (current_arg + 1))
      {
        current_arg++;
        config_file = argv[current_arg];
        printf("Setting the config file name to %s \n", config_file);
      }
      else
      {
        printf("Can't read the config file name, not enough args\n");
      }
    }
  }

  ap_config.read_file(config_file);

  //spd_init_screen();


//  int n_seg = read_profile(vel_profile, 
 //          sizeof(vel_profile) / sizeof(vel_profile[0]));

  preader_1.read_profiles();
  preader_2.read_profiles();

  if (!sim_mode)
  {
    init_io();
  }
  else
  {
    sreader_1.read_sim_data();
    sreader_2.read_sim_data();
  }

  spd_init_screen();

  int line_via = 1;
  int start_station = 0;

  pick_station(&line_via, &start_station);
  start = start_station;


  bool done = false;
  bool auto_mode = false;
  bool auto_end = true;
  bool fast = false;
  bool continue_mode = false;

  utimer_t utimer;
  utimer.set_busy_wait(false);
  utimer.set_interval(1000000);

  //FILE *fp = fopen("out.txt", "w");
  char dirname[200];
  dirname[0] = '\0';
  //printf("starting loop ....\n");
  int j;
  n_seg = preader->get_n_sections();
  for (j=start; true; j = (j+1) % n_seg)
  {
    int next = (j + 1) % n_seg;
    int dif_index = 0; 
    int type = 1;
    bool warn = false;
    //int nseg = profile[j].n;

    double total_distance;
    double total_time;

    total_distance = preader->get_distance(j); //get_total_dist(j);
    total_time = preader->get_time(j);

    double distance = 0.0;
    double actual = 0;
    double desired = 0;
    spd_discrete_t discretes;
    discretes.doors_open = false;
    //mvprintw(20,2,"Cargando y Descargando Pasajeros"); 
    //refresh();

    spd_init_segment(preader->get_station_name(j), preader->get_station_name(next)); 
    spd_show_performance(NULL);
    if (!continue_mode)
    {
      if (auto_mode)
      {
       // printf("show loading ....\n");
        //spd_show_loading(fast?2:4);
        sleep(fast?2:4);
      }
      else
      {
        if (detect_with_doors)
        {
          while(true)
          {
            if (sim_mode)
            {
              sreader->get_sim_speed_dist(j, -1, &distance, &actual, &discretes);
            } 
            else
            {
              get_actual_speed_dist(-1, -1, &distance, &actual, &discretes);
            }
            if (!discretes.doors_open) break;
            //spd_show_loading(1);
            usleep(100000);
          }
        }
        else
        {
          //printf("press key to exit ....\n");
          int key = spd_wait_key("Pres. tecla para salir"); 
          if ((key == 'q') || (key == 'Q'))
          {
            pick_station(&line_via, &start_station);
            j = start_station;
            j--; // This is because starting the next loop will increment.
            if (j < 0) j = n_seg - 1;
            continue;
          }
        }
      }
      //mvprintw(20,2,"%-36s", " "); 
    }
    spd_redraw_all();
    spd_init_segment(preader->get_station_name(j), preader->get_station_name(next)); 
    continue_mode = false;
    //spd_init_segment(vel_profile[j].st1, vel_profile[next].st1); 
    //beep();
    //mvprintw(18,2,"%d: total dist = %lf", j, total_distance); 
    //refresh();
    int time_in_section = 0;
    if (!sim_mode) reset_distance(j);

    /***/
    if (create_profiles)
    {

      char fname[200];
    
      if ((j == 0) || (dirname[0] == '\0'))
      {
      	if (!dir_exists("profiles"))
      		system("mkdir profiles");
        for (int k=1; true; k++)
        {
          snprintf(dirname, sizeof(dirname), "profiles/profile%d", k);
          if (!dir_exists(dirname)) 
          {
            char cmd[200];
            snprintf(cmd, sizeof(cmd), "mkdir %s", dirname);
            system(cmd);
            snprintf(cmd, sizeof(cmd), "date >%s/date.txt", dirname);
            system(cmd);
            #ifndef ARM
            mvprintw(25,2,"Using: %s", dirname);
            #endif
            break;
          }
        }
      }

      snprintf(fname, sizeof(fname), "%s/p%02d.txt", dirname, j);
      snprintf(base_name, sizeof(base_name), "%s/p%02d", dirname, j);
      #ifndef ARM
      mvprintw(26,2,"File: %s %s", fname, base_name);
      #endif
      pfp = fopen(fname, "w");
      if (pfp == NULL)
      {
        spd_endwin(); 
        printf("Can't open file: %s\n", fname);
      }
    }
    else
    {
      pfp = NULL;
    }


    utimer.set_start_time();
    for (int i=0; true; i++)
    {
      if (!free_running_mode)
      {
        utimer.wait_next();
        //if (utimer.late_time() > 1.0)
        //{
        //  utimer.set_start_time();
        //}
      }
      double now = (double) i / 1.0;
     

      if (sim_mode)
      {
        //mvprintw(22,2,"Sim Mode"); 
        sreader->get_sim_speed_dist(j, time_in_section, &distance, &actual, &discretes);
      } 
      else
      {
        //mvprintw(22,2,"Live Mode"); 
        get_actual_speed_dist(j, time_in_section, &distance, &actual, &discretes);
      } 
      
      //dispatch_algorithms(time_t now, int command, double speed, double distance)

      struct current_speed_limits_t limits;
      //calc_desired(j, actual, distance, &limits, &type, &warn);
      preader->calc_desired(j,  actual, distance, &limits, &type, &warn);
 
      desired = limits.desired;

      speed_results_t results[3];
      spd_DC.evaluate((time_t) now, actual, distance, limits, &results[0]);  
      spd_RD.evaluate((time_t) now, actual, distance, limits, &results[1]);  
      spd_VV.evaluate((time_t) now, actual, distance, limits, &results[2]);  
      if ( (results[0].state == SPD_STATE_SHUTDOWN) || 
                 (results[0].state == SPD_STATE_WARN_HI))
      {
        warn = true;
      }
      else
      {
        warn = false;
      }

      alg_compare(results, 3);

      time_in_section++;


      if (pfp != NULL)
      {
        fprintf(pfp, "%lf %lf %lf %lf %lf %lf\n", 
          distance, actual, desired, limits.low, limits.high, limits.very_high);
      }

      //mvprintw(21,2,"Distance: %5.1lf, Speed: %4.0lf", distance, actual); 

      spd_print_current(desired, actual, type, warn, 
        100.0 * distance / total_distance, total_distance,
           now, total_time, &discretes);

      /*****
      double left = total_distance - distance;
      if (left < 1.0) left = 0.0;
      mvprintw(12,2,"Tiempo: %5.1lf, Distancia: %4.0lf", now, distance); 
      if (COLS < 70) mvprintw(13,2, "");
      else printw(", ");
      printw("Falta: %4.0lf, Porciento: %3.0lf%%", left, (distance / total_distance) * 100.0);
      refresh();
      ****/
      if (detect_with_doors)
      {
        /***
        printf("checking doors: %lf, %s\n", (distance / total_distance) * 100,
                 discretes.doors_open ? "Open" : "Closed");
        **/
        if (discretes.doors_open &&  
          ((distance / total_distance) > 0.95)) break;
      }
      else if (auto_end) 
      {
        if (((distance / total_distance) > 0.98) && (actual == 0)) break;
      }
      int ch = spd_getch();
      if (ch == 'f') {fast = true; utimer.set_interval(200000);}
      else if (ch == 's') {fast = false; utimer.set_interval(1000000);}
      else if (ch == 'c') {continue_mode = false; break;} 
      else if (ch == 'q') 
      {
        pick_station(&line_via, &start_station);
        j = start_station;
        j--; // This is because starting the next loop will increment.
        if (j < 0) j = n_seg - 1;
        //done = true; 
        break;
      } 
      else if (ch == 'a') {auto_mode = true; spd_print_auto(true);} 
      else if (ch == 'm') {auto_mode = false; spd_print_auto(false);} 
      else if (ch == 'e') {auto_end = !auto_end;} 
      else spd_set_key(ch);
      //if (!fast) usleep(1000000);
      //else usleep(200000);
      dif_index++;
    }

    if (pfp != NULL)
    {
      fclose(pfp);
      pfp = NULL;
      char gtitle[50];
      snprintf(gtitle, sizeof(gtitle), "%s-%s", 
           preader->get_station_name(j), preader->get_station_name(next));
            //vel_profile[j].st1, vel_profile[next].st1);   
      #ifndef ARM
      mvprintw(23,2,"graph: %s %s", base_name, gtitle);
      #endif
      spd_create_image(base_name, gtitle, false);
      spd_create_image(base_name, gtitle, true);
    }
    else
    {
      #ifndef ARM
      mvprintw(23,2,"graph: pfp is NULL");
      #endif
    }

    if (done) break;
  }
  sleep(1);
  spd_wait_key("Hit any key to exit"); 
  spd_endwin();
}

/***********************************************************************************************************/
