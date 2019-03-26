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
#include <ncurses.h>
#include <string.h>

#include "rtcommon.h"
#include "section.h"
#include "alg.h"
#include "dc_alg.h"
#include "jg_alg.h"
#include "startup_alg.h"
#include "controlalg.h"
#include "sim.h"
#include "event.h"


static const int n_algs = 4;
bool control_startup = false;

startup_algorithm_t *startup_alg;
double current_time = 0.0;

//LINES and COLS
int ncol = 118;
double max_distance;
double min_distance;

bool last_stop_light[60];
bool actual_stop_light[60];
bool alg_light_settings[60][n_algs];

static int stop_light_loc_x[MAX_SECTIONS];
static int stop_light_loc_y[MAX_SECTIONS][MAX_ALGORITHMS];

static int estop_active;

//bool entry_station[MAX_SECTIONS];
//bool entry_lights[MAX_SECTIONS];

/***
void trip_detector_crossed(int s);
void trip_exit_system(int s);
void trip_enter_system(int s);
void trip_entered_station(int s);
void trip_left_station(int s);
***/
void trip_event(event_t *event);

sections_t *sect;
bool is_cw = false;
bool start_up = false;
bool end_up = true;


alg_notify_object_t *notify_objs[MAX_ALGORITHMS];
base_algorithm_t *algs[MAX_ALGORITHMS];
int second = 0;

int skeys[MAX_SECTIONS];
int estop_key = 1234;
int entry_keys[] = {4321};
int esects[] = {0};
int ntrains;
sim_train_t *trains;
int tlocs24[] = {0,1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34};
int tlocsxx[] = {0,2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34};
int tlocsyy[] = {2, 4, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34};
int tlocs12[] = {0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 32};
int tlocs6[] = {0, 6, 12, 18, 24, 30};
double initial_t_times[MAX_TRAINS];
double slot_times[MAX_TRAINS];
double dtypes[] = {10.0, 8.2, 5.5, 3.9, 0.8, 0.0, -1.2, -3.2, -6.2, -8.2, -8.9, -9.7};

int tlocs[30];

/*********************************************************************/

int generate_locations(double train_spacing)
{
  double cycle_time = sect->get_cycle_time();
  int n_tr = int(cycle_time / train_spacing);
  int n_sec = sect->get_n_sections();
  for (int i=0; i < n_tr; i++)
  {
    double fraction = double(i) / double(n_tr);
    tlocs[i] = int(fraction * n_sec);
    printf("Train[%d] starts a station %d\n", i, tlocs[i]);
  }
  return n_tr;
}


/*********************************************************************/

sim_train_t::sim_train_t(void)
{
  init(0, 0, 0, 0.0, false);
}

/*********************************************************************/

double sim_train_t::get_location(double t)
{
  int next = sect->get_next_section(section);
  if (at_station)
  {
    return sect->get_section_start(next);
  }
  else
  {
    double fract = (t - time_entered) / sim_time_to_station;
    if (fract < 0.0) fract = 0.0;
    if (fract > 1.0) fract = 1.0;
    return sect->get_location(section, fract);
  }

}

/*********************************************************************/

bool sim_train_t::is_forward()
{
  bool fwd = sect->is_forward(section);
  return fwd;
}

/*********************************************************************/

void sim_train_t::update(double t)
{
  if (!active)
  {
    return;
  }
  int next = sect->get_next_section(section);
  double time_in_section = t - time_entered;
  event_t event;
  // Ok, if you have been selected to exit, AND this is an exit section, AND, you are at the exit, get out.
  if (sect->has_entrance(section) && exiting && (time_in_section >= sect->get_time_to_entrance(section)))
  {
    event.type = SYSTEM_EXIT_EVENT;
    event.section = section;
    trip_event(&event);
    //trip_exit_system(section);
    active = false;
  }
  if (!past_detector && (time_in_section > sect->get_time_to_detector()))
  {
    // Ok, you are past the dector. Send the detecor crossed event.
    past_detector = true;
    //printf("Detector crossed at section %d\n");
    event.type = CROSSING_EVENT;
    event.section = section;
    trip_event(&event);
    //trip_detector_crossed(section);
  }
  if (!arrived_at_station && (time_in_section > sim_time_to_station))
  {
    // Ok, you have arrived at the station.
    // Send the station entered event.
    at_station = true;
    arrived_at_station = true;
    event.type = STATION_ENTER_EVENT;
    event.section = section;
    trip_event(&event);
    //trip_entered_station(section);
  }
  else if (at_station &&
     (time_in_section > (sim_time_to_station + sect->get_dwell_time())) &&
      actual_stop_light[next])
  {
    // Ok, passengers have boarded and you have closed the doors.
    // If the light is green GO!!
    // Send the left station event.
    event.type = STATION_EXIT_EVENT;
    event.section = section;
    trip_event(&event);
    //trip_left_station(section);
    at_station = false;
    arrived_at_station = false;
    past_detector = false;
    time_entered = t - sect->get_time_to_entrance(section);
    just_entered = false;
    section = next;
    sim_time_to_station = sect->get_sim_time(section, driver_type) - sect->get_dwell_time();
  }
}

/*********************************************************************/

void sim_train_t::select_for_exit(void)
{
  exiting = true;
}

/*********************************************************************/

void sim_train_t::init_at_entry(int n, int s, double time, double dtype)
{
  at_station = false;
  exiting = false;
  past_detector = true;
  active = true;
  just_entered = true;
  section = s;
  driver_type = dtype;
  sim_time_to_station = sect->get_section_time(s) - sect->get_dwell_time();

  time_entered = time - sim_time_to_station + sect->get_time_to_entrance(s);
  number = n;
}

/*********************************************************************/

void sim_train_t::init(int n, int s, double time, double dtype, bool a)
{
  active = a;
  number = n;
  just_entered = false;
  exiting = false;
  if (!active)
  {
    //printf("Inactive train\n");
    section = -1;
    at_station = false;
    past_detector = false;
    time_entered = 0.0;
    driver_type = 0.0;
  }
  section = s;
  driver_type = dtype;
  at_station = true;
  past_detector = true;
  sim_time_to_station = sect->get_sim_time(s, driver_type) - sect->get_dwell_time();
  time_entered = time - sim_time_to_station;
  //printf("New train %d, section %d, drv type: %0.1lf\n", n, s, dtype);
  //printf("Activated train at section %d\n", s);
}

/*********************************************************************/

void check_crossing_event(int s)
{
  if (!actual_stop_light[s])
  {
    // This is very serious, a crossing event when the light was red.
    /***/
    endwin();
    printf("\nInvalid event, the light was red at section %d\n\n", s);
    estop_active = true;
    /***/
  }
}


/*********************************************************************/
static int skip_one = false;
void skip_event(void)
{
  skip_one = true;
}

void trip_event(event_t *event)
{
  switch (event->type)
  {
    case CROSSING_EVENT:
  if (skip_one)
  {
    mvprintw(37, 2, "Event skipped at section %d, type %d\n", event->section, event->type);
    skip_one = false;
    return;
  }
      check_crossing_event(event->section);
      for (int i=0; i < n_algs; i++)
      {
        algs[i]->crossing_event(event->section);
      }
      break;
    case SYSTEM_ENTRY_EVENT:
      for (int i=0; i < n_algs; i++)
      {
        algs[i]->entry_event(event->section);
      }
      break;
    case SYSTEM_EXIT_EVENT:
      for (int i=0; i < n_algs; i++)
      {
        algs[i]->exit_event(event->section);
      }
      break;
    default:
      break;
  }
}

/*********************************************************************

void trip_detector_crossed(int s)
{
  //printf("Crossing event at section %d\n", s);
  for (int i=0; i < n_algs; i++)
  {
    algs[i]->crossing_event(s);
  }
}


void trip_enter_system(int s)
{
  for (int i=0; i < n_algs; i++)
  {
    algs[i]->entry_event(s);
  }
}


void trip_exit_system(int s)
{
  for (int i=0; i < n_algs; i++)
  {
    algs[i]->exit_event(s);
  }
}


void trip_entered_station(int s)
{
}


void trip_left_station(int s)
{
}

*********************************************************************/

int get_next_section(int s)
{
  return 0;
}

/*********************************************************************/

void set_initial_conditions(int train_section[], int n)
{
}

/*********************************************************************/

class my_alg_notify_object_t : public alg_notify_object_t
{
private:
  int n;
public:
  my_alg_notify_object_t(int num) {n = num;};
  void estop(int key, const char *reason);
  void permit_section_entry(int section, int key);
  void prohibit_section_entry(int section, int key);
  void permit_system_entry(int section, int key);
  void prohibit_system_entry(int section, int key);
};

/*********************************************************************/

void my_alg_notify_object_t::estop(int key, const char *reason)
{
  char cmd[200];
  snprintf(cmd,sizeof(cmd), "echo \"Estop called at %0.1lf (key%d, alg %d): %s\" >> err.log", 
              current_time, key, n, reason);
  system(cmd);
  endwin();
  printf("Estop called at %0.1lf (key %d, alg %d, sec %d): %s\n", 
              current_time, key, n, second, reason);
  estop_active = true;
  //exit(1);
}

/*********************************************************************/

void green_light(int x, int y)
{
  mvprintw(x, y, "o");
  //mvchgat(x, y, 1, A_NORMAL, 2, NULL);
  mvchgat(x, y, 1, A_BOLD, 2, NULL);
}

/*********************************************************************/

void red_light(int y, int x)
{
  mvprintw(y, x, "x");
  //mvchgat(y, x, 1, A_NORMAL, 1, NULL);
  mvchgat(y, x, 1, A_BOLD, 1, NULL);
}

/*********************************************************************/

void my_alg_notify_object_t::permit_section_entry(int s, int key)
{
//int mvwchgat(WINDOW *win, int y, int x, int n,
//             attr_t attr, short color, const void *opts)
//             COLOR_BLACK
//             COLOR_RED
//             COLOR_GREEN
//             COLOR_YELLOW
//             COLOR_BLUE
//             COLOR_MAGENTA
//             COLOR_CYAN
//             COLOR_WHITE

//       int chgat(int n, attr_t attr, short color,
//             const void *opts)
//       int wchgat(WINDOW *win, int n, attr_t attr,
//             short color, const void *opts)
//       int mvchgat(int y, int x, int n, attr_t attr,
//             short color, const void *opts)



  /***
  bool last_setting = true;
  for (int i=0; i < n_algs; i++)
  {
    if (!alg_light_settings[s][i]) last_setting = false;
  }
  ***/
 
  alg_light_settings[s][n] = true;
  green_light(stop_light_loc_y[s][n], stop_light_loc_x[s]);
  /****
  bool is_on = true;
  alg_light_settings[s][n] = true;
  //mvprintw(stop_light_loc_y[s][n], stop_light_loc_x[s], "O");
  //mvchgat(stop_light_loc_y[s][n], stop_light_loc_x[s], 1, A_NORMAL, 2, NULL);
  green_light(stop_light_loc_y[s][n], stop_light_loc_x[s]);
  for (int i=0; i < n_algs; i++)
  {
    if (!alg_light_settings[s][i]) is_on = false;
  }
  actual_stop_light[s] = is_on;
  
  if (!last_setting && is_on)
  {
    if (control_startup) startup_alg->light_to_green_event(s, current_time);
  }
  *****/
  //printf("permit section entry (%d): %d\n", key, section);
}

/*********************************************************************/
void eval_stop_lights(void)
{
  for (int n = 0; n < 3; n++) {
  for (int i=0; i < sect->get_n_sections(); i++) 
  {
    bool is_on = true;
    for (int j=0; j < n_algs; j++)
    {
      if (!alg_light_settings[i][j]) is_on = false;
    }
    actual_stop_light[i] = is_on;
    if (last_stop_light[i] != is_on)
    {
      if (is_on)
      {
        if (control_startup) startup_alg->light_to_green_event(i, current_time);
      }
      else
      {
      }
    }
    last_stop_light[i] = is_on;  
  }
  }
}

/*********************************************************************/

void my_alg_notify_object_t::prohibit_section_entry(int s, int key)
{
  alg_light_settings[s][n] = false;
  red_light(stop_light_loc_y[s][n], stop_light_loc_x[s]);
  //actual_stop_light[s] = false;
  //mvprintw(stop_light_loc_y[s][n], stop_light_loc_x[s], "X");
  //mvchgat(stop_light_loc_y[s][n], stop_light_loc_x[s], 1, A_NORMAL, 1, NULL);
  //printf("prohibit section entry (%d): %d\n", key, section);
}

/*********************************************************************/

void my_alg_notify_object_t::permit_system_entry(int section, int key)
{
  //printf("permit system entry (%d): %d\n", key, section);
}

/*********************************************************************/

void my_alg_notify_object_t::prohibit_system_entry(int section, int key)
{
  //printf("permit system entry (%d): %d\n", key, section);
}

/*********************************************************************/

void init_screen()
{
  initscr();
  start_color();
  use_default_colors();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  curs_set(0);
  ncol = COLS - 7;
//LINES and COLS

//            COLOR_RED
//            COLOR_GREEN
  short fg, bg;
  pair_content(COLOR_PAIR(0), &fg, &bg);


  init_pair(1, COLOR_RED, bg);
  init_pair(2, COLOR_GREEN, bg);
  init_pair(3, COLOR_BLACK, COLOR_YELLOW);
  init_pair(4, COLOR_BLUE, bg);
  init_pair(5, COLOR_MAGENTA, bg);
  init_pair(6, COLOR_CYAN, bg);
  init_pair(7, COLOR_BLACK, COLOR_WHITE);
//             COLOR_YELLOW
//             COLOR_BLUE
//             COLOR_MAGENTA
//             COLOR_CYAN
//             COLOR_WHITE


  timeout(0);
  box(stdscr, 0, 0);

  int line2 = 6 + 2 * n_algs;
  mvhline(2, 0, 0, COLS);
  mvhline(line2, 0, 0, COLS);
  attron(A_ALTCHARSET);
  mvprintw(2,0,"%c", ACS_LTEE);
  mvprintw(2,COLS-1,"%c", ACS_RTEE);
  mvprintw(line2,0,"%c", ACS_LTEE);
  mvprintw(line2,COLS-1,"%c", ACS_RTEE);
  attroff(A_ALTCHARSET);
 


}

/*********************************************************************/

int get_x(double d)
{
  int x = (int)(((d - min_distance) / (max_distance - min_distance)) * (double) ncol);
  if (x < 0)
  {
    x = 0;
  }
  if (x > (ncol))
  {
    x = ncol;
  }
  return x+1;
}

/*********************************************************************/

void init_substations(void)
{
  double sub[] = {0, 853, 3282, 5230, 7507, 9032, 11018, 12855, 14135, 15421};

  for (unsigned i=0; i < (sizeof(sub) / sizeof(sub[0])); i++)
  {
    int x = get_x(sub[i]) + 2;
    int y = n_algs + 4;
    mvprintw(y, x, "+");
  }

}
    
/****
Periferico Sur 0 
Españita 853
Polanco 3282
López Mateos 5230
Washington 7507
Juárez 1 9032
Mezquitan 11018
División del Norte II 12855
Atemajac 14135
Periférico Norte 15421
****/

/*********************************************************************/

void init_stations(void)
{

  for (int i=0; i < sect->get_n_sections(); i++)
  {
    double loc = sect->get_section_start(i);
    bool fwd = sect->is_forward(i);
    int y_loc[n_algs];

    if (loc > max_distance)
    {
      loc = max_distance;
    }
    if (loc < min_distance)
    {
      loc = min_distance;
    }

    int x = get_x(loc);
    x += 2; // we need to add this because in the other case, we are filling a string that is place 2 to the right.


    bool is_up = is_cw?fwd:!fwd;
    if (x <= 2)
    {
      is_up = start_up;
    }
    else if (x >= (ncol-2))
    {
      is_up = end_up;
    }


    for (int j=0; j < n_algs; j++)
    {
      if (is_up)
      {
        y_loc[j] = 3 + j;
      }
      else
      {
        y_loc[j] = (2 * n_algs) + 5 - j;
      }
      mvprintw(y_loc[j], x, "|");
      stop_light_loc_y[i][j] = y_loc[j];
    }

    {
      char name[25];
      int yname = 7 + (2 * n_algs);
      safe_strcpy(name, sect->get_name(i), sizeof(name));
      for (unsigned j=0; j < strlen(name); j++)
      {
        mvprintw(yname, x, "%c", name[j]);
        mvchgat(yname, x, 1, A_NORMAL, 4, NULL);
        yname++;
      }
    }
    x = (fwd) ? x+1 : x-1;
    stop_light_loc_x[i] = x;

    for (int j=0; j < n_algs; j++)
    {
      if (alg_light_settings[i][j])
      {
        //mvprintw(y_loc[j], x, "O");
        green_light(y_loc[j], x);
      }
      else
      {
        //mvprintw(y_loc[j], x, "X");
        red_light(y_loc[j], x);
      }
    }
  }
  init_substations();
}

/*********************************************************************/

void print_time(double t)
{
  int secs = (int) t + (6 * 3600);
  int h = secs / 3600;
  int m = (secs % 3600) / 60;
  int s = (secs % 3600) % 60;
  mvprintw(DATA_START+8, 2, "%02d:%02d:%02d", h, m, s);

}

/*********************************************************************/

void print_percents(double t)
{
  char a1[200];
  char a2[200];
  memset(a1, '-', sizeof (a1));
  memset(a2, '-', sizeof (a2));

  for (int i=0; i < ntrains; i++)
  {
    if (!trains[i].get_active())
    {
      continue;
    }
    double loc = (int) trains[i].get_location(t);
    bool fwd = trains[i].is_forward();
    bool at_station = trains[i].is_at_station();
    if (loc > max_distance)
    {
      //mvprintw(20,2,"error %0.1lf", loc);
      loc = max_distance;
    }
    if (loc < min_distance)
    {
      //mvprintw(21,2,"error %0.1lf", loc);
      loc = min_distance;
    }
    int x = get_x(loc);

    bool is_up = is_cw?fwd:!fwd;
    if (x <= 2)
    {
      is_up = start_up;
    }
    else if (x >= (ncol-2))
    {
      is_up = end_up;
    }

    int n = trains[i].get_number();
    char tch = '0' + n;
    if (n > 9)
    {
      tch = 'A' + n - 10;
    }
    else
    {
      tch = '0' + n;
    }
    bool just_enter = trains[i].get_just_entered();
    bool exiting = trains[i].get_exiting();
    char *a = is_up?a1:a2;
    if (fwd)
    {
      a[x] = tch;
      if (!at_station)
      {
        if (just_enter)
        {
          a[x-1] = '*';
          a[x+1] = '*';
          a[x+2] = '>';
        }
        else if (exiting)
        {
          a[x-1] = '@';
          a[x+1] = '@';
          a[x+2] = '>';
        }
        else
        {
          a[x+1] = '>';
        }
      }
    }
    else
    {
      a[x] = tch;
      if (!at_station)
      {
        if (just_enter)
        {
          a[x-1] = '*';
          a[x+1] = '*';
          a[x-2] = '<';
        }
        else if (exiting)
        {
          a[x-1] = '@';
          a[x+1] = '@';
          a[x-2] = '<';
        }
        else
        {
          a[x-1] = '<';
        }
      }
    }
    //mvprintw(19, 2 + i * 5, "%4.0lf", pct);
  }
  a1[ncol+2] = '\0';
  a2[ncol+2] = '\0';
  //printf("%s\n", a1);
  //printf("%s\n", a2);
  mvprintw(3 + n_algs,2,"%s", a1);
  mvprintw(5 + n_algs,2,"%s", a2);
  move(0,0);
}

/*********************************************************************/

void remove_train(double t)
{
  int s = sect->get_last_section(0);
  for (int i=0; i < ntrains; i++)
  {
    if (trains[i].get_active() && (s == trains[i].get_section()))
    {
      trains[i].select_for_exit();
      continue;
    }
  }
}

/*********************************************************************/

void add_train(double t)
{
  bool allow = true;
  int n = -1;
  for (int i=0; i < n_algs; i++)
  {
    if (!algs[i]->request_entry(0))
    {
      allow = false;
    }
  }
  if (allow)
  {
    for (int i=0; i < ntrains; i++)
    {
      if (!trains[i].get_active())
      {
        n = i;
        break;
      }
    }
    //trains = new sim_train_t[60];
    //printf("%s %d\n", __FILE__, __LINE__);
    //for (int i=0; i < ntrains; i++)
    //void sim_train_t::init(int n, int s, double time, double dtype, bool a)
    beep();
    flash();
    trains[n].init_at_entry(n, 0, t, 0.0);
    //ntrains++;
    event_t event;
    event.type = SYSTEM_ENTRY_EVENT;
    event.section = 0;
    trip_event(&event);
    //trip_enter_system(0);
  }
  else
  {
    for (int i=0; i < n_algs; i++)
    {
      algs[i]->cancel_entry(0);
    }
  }
}

/*********************************************************************/

int main(int argc, char *argv[])
{
  bool use_jl = false;
  bool control_dist = true;


  for (int i=1; i < argc; i++)
  {
    if (0 == strcmp(argv[i], "-jg"))
    {
      use_jl = true;
    }
    if (0 == strcmp(argv[i], "-nc"))
    {
      control_dist = false;
    }
    if (0 == strcmp(argv[i], "-sa"))
    {
      control_startup = true;
    }
  }

  for (unsigned i=0; i < (sizeof(skeys) / sizeof(skeys[0])); i++)
  {
    skeys[i] = i * 5 + i;
  }

  sect = new sections_t();
  sect->read_file();

  for (int i=0; i < sect->get_n_sections(); i++)
  {
    actual_stop_light[i] = false;
  }

  for (int i=0; i < n_algs; i++)
  {
    notify_objs[i] = new my_alg_notify_object_t(i);
  }

  int zmap[MAX_SECTIONS];
  for (int i=0; i < sect->get_n_sections(); i++)
  {
     zmap[i]= sect->get_substation(i);
  }

  dc_algorithm_t *dc_alg1;
  dc_algorithm_t *dc_alg2;
  dc_algorithm_t *dc_alg3;
  dc_algorithm_t *dc_alg4;
  jl_algorithm_t *jl_alg;
  control_algorithm_t *controlalg;
  dc_alg1 = new dc_algorithm_t;
  dc_alg2 = new dc_algorithm_t;
  dc_alg3 = new dc_algorithm_t;
  dc_alg4 = new dc_algorithm_t;
  jl_alg = new jl_algorithm_t;
  startup_alg = new startup_algorithm_t;
  controlalg = new control_algorithm_t;

  if (control_startup)
  {
    algs[0] = startup_alg;
  }
  else
  {
    algs[0] = dc_alg1;
  }

  if (use_jl)
  {
    algs[1] = jl_alg;
  }
  else
  {
    algs[1] = dc_alg2;
  }
  algs[2] = dc_alg3;

  if (control_dist)
  {
    algs[3] = controlalg;
  }
  else
  {
    algs[3] = dc_alg4;
  }

  double time_between;
  if (argc > 1)
  {
    time_between = atof(argv[1]);
  }
  else
  {
    time_between = 5.0 * 60.0;
  }

  ntrains = 30;
  unsigned n_to_init = generate_locations(time_between);
  //unsigned n_to_init = sizeof(tlocs) / sizeof(tlocs[0]);
  for (unsigned i=0; i < n_to_init; i++)
  {
    initial_t_times[i] = sect->get_time_to_station(tlocs[i]);
    slot_times[i] = double (i) * (sect->get_cycle_time() / n_to_init);
  }
  if (control_dist) controlalg->set_section_object(sect);
  for (int i=0; i < n_algs; i++)
  {
    algs[i]->set_notify_object(notify_objs[i], estop_key);
    algs[i]->initialize_sizes(sect->get_n_sections(), 30);
    algs[i]->set_section_keys(skeys, sect->get_n_sections()); algs[i]->set_entry_sections(esects, entry_keys, 1);
    algs[i]->initial_state(tlocs, n_to_init);
  }
  if (control_startup) startup_alg->set_station_zones(zmap, sect->get_n_sections());
  if (control_startup) startup_alg->initial_stoplight_settings(actual_stop_light, sect->get_n_sections());
  if (control_dist) controlalg->set_train_times(initial_t_times, n_to_init);
  if (control_dist) controlalg->set_time_slots(slot_times, n_to_init);
  trains = new sim_train_t[30];
  for (unsigned i=0; i < n_to_init; i++)
  {
    double next_dtype;
    if (i < sizeof(dtypes))
    {
      next_dtype = dtypes[i];
    }
    else
    {
      next_dtype = 0.0; 
    }
    trains[i].init(i, tlocs[i], 0.0, next_dtype, true);
  }

  max_distance = sect->get_section_start(0);
  min_distance = sect->get_section_start(0);
  double max_time = 0.0;
  int max_section = 0;
  double total_time = 0.0;
  for (int i=0; i < sect->get_n_sections(); i++)
  {
    printf("%c ", actual_stop_light[i] ? 'T':'F');
    double d = sect->get_section_start(i);
    if (d > max_distance) max_distance = d;
    if (d < min_distance) min_distance = d;
    double t = sect->get_section_time(i);
    total_time += t;
    if (t > max_time)
    {
      max_time = t;
      max_section = i;
    }
  }
  printf("\n");
  printf("max: %0.1lf, min: %0.1lf\n", max_distance, min_distance);
  for (unsigned i=0; i < n_to_init; i++)
  {
    double loc = (int) trains[i].get_location(10);
    printf("train %u at %0.1lf\n", i, loc);
  }

  int snext = sect->get_next_section(max_section);
  max_time += 15; // safety factor.
  printf("Bottle neck is at %s - %s, max 1 train every %0.0lf seconds\n",
        sect->get_name(max_section), sect->get_name(snext),  max_time);
  printf("Total round trip time is %02d:%02d, max trains = %0.1lf\n",
    (int) total_time / 60, (int) total_time % 60, total_time / max_time);

  char st[20];
  fgets(st, sizeof(st), stdin);


  init_screen();

  init_stations();

  FILE *fpn = fopen("run.txt", "w");
  estop_active = false;
  //bool fast = false;
  //bool vfast = false;
  bool libre = false;
  int speedup = 1;
  for (int i=0; i < 1000000; i++)
  {
    current_time = (double) i;
    print_time((double) i);
    second = i;
    int ch = getch();

    if (ch == 'q')
    {
      break;
    }
    else if (ch == 's')
    {
      //fast = false;
      //vfast = false;
      libre = false;
      speedup = 1;
    }
    else if (ch == 'f')
    {
      //fast = true;
      //vfast = false;
      libre = false;
      speedup = 10;
    }
    else if (ch == 'v')
    {
      libre = false;
      //fast = false;
      //vfast = true;
      speedup = 50;
    }
    else if ((ch >= '1') && (ch <= '9'))
    {
      speedup = ch - '0';
      libre = false;
    }
    else if (ch == 'l')
    {
      libre = true;
      //fast = false;
      //vfast = false;
    }
    else if (ch == 'i')
    {
      add_train(double(i));
    }
    else if (ch == 'x')
    {
      remove_train(double(i));
    }
    else if (ch == 'z')
    {
      while ('z' != getch())
      {
        usleep(10000);
      }
    }

    // Ok, here is the control and simulation
    int n_running = 0;
    for (int j=0; j < ntrains; j++)
    {
      trains[j].update((double) i);
      if (!trains[j].is_at_station() && trains[j].is_active()) n_running++;
    }
    fprintf(fpn, "%d\t%d\n", i, n_running);

    if (i == 100)
    {
      // Generate a spurious event
      /***
      event_t event;
      event.type = CROSSING_EVENT;
      event.section = 7;
      trip_event(&event);
      // Skip an event
      skip_event();
      ****/
    }

    print_percents(double (i));
    if (control_dist) controlalg->update(double(i), 1.0);
    if (control_startup) startup_alg->update((double)i);

    eval_stop_lights();

    if (!libre)
    {
      usleep(1000000 / speedup);
    }
     /*****
    if (fast)
    {
      usleep(50000);
    }
    else if (vfast)
    {
      usleep(10000);
    }
    else if (libre)
    {
    }
    else
    {
      usleep(1000000);
    }
    ****/
    if (estop_active)
    {
      break;
    }
  }
  endwin();
  printf("Screen size: %d lines, %d columns\n", LINES, COLS);

/**
  virtual void set_notify_object(
            alg_notify_object_t *notify_object, int estop_key) = NULL;
  virtual void initialize_sizes(int n_sections, int max_trains) = NULL;
  virtual void set_section_keys(int keys[], int n_keys) = NULL;
  virtual void set_entry_sections(
         int entry_sections[], int entry_keys, int num_entrys) = NULL;
  virtual void initial_state(int train_locations[], int n_locations) = NULL;
**/
}
