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
#include "jl_alg.h"
#include "controlalg.h"
#include "sim.h"
#include "event.h"

class tdisplay_t
{
private:
  int stop_light_loc_x[MAX_SECTIONS];
  int stop_light_loc_y[MAX_SECTIONS][MAX_ALGORITHMS];
  int ncol;
  int n_algs;
  double max_distance;
  double min_distance;
  sections_t *sect;
  bool is_cw;
  bool start_up;
  bool end_up;
  void green_light(int x, int y);
  void red_light(int y, int x);
  void init_screen();
  int get_x(double d);
  void print_percents(double t, sim_train_t *trains, int ntrains);
  void init_stations(void);
public:
  tdisplay_t(int n_algorithms, int start_row, sections_t *sect);
  void set_light(int section, int algorithm);
  void redraw(sim_train_t *trains, int ntrains);
  void print_time(double t);
};


/*********************************************************************/

void tdisplay_t::redraw(sim_train_t *trains, int ntrains)
{
}

/*********************************************************************/

tdisplay_t::tdisplay_t(int n_algorithms, int start_row, sections_t *s)
{
  sect = s;
  n_algs = n_algorithms;
  is_cw = false;
  start_up = false;
  end_up = true;
  max_distance = sect->get_section_start(0);
  min_distance = sect->get_section_start(0);
  double max_time = 0.0;
  int max_section = 0;
  double total_time = 0.0;
  for (int i=0; i < sect->get_n_sections(); i++)
  {
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
  printf("max: %0.1lf, min: %0.1lf\n", max_distance, min_distance);

}

/*********************************************************************/

void tdisplay_t::green_light(int x, int y)
{
  mvprintw(x, y, "o");
  //mvchgat(x, y, 1, A_NORMAL, 2, NULL);
  mvchgat(x, y, 1, A_BOLD, 2, NULL);
}

/*********************************************************************/

void tdisplay_t::red_light(int y, int x)
{
  mvprintw(y, x, "x");
  //mvchgat(y, x, 1, A_NORMAL, 1, NULL);
  mvchgat(y, x, 1, A_BOLD, 1, NULL);
}

/*********************************************************************/

void tdisplay_t::init_screen()
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

  mvhline(2, 2, 0, COLS - 5);
  mvhline(6 + 2 * n_algs, 2, 0, COLS - 5);

}

/*********************************************************************/

int tdisplay_t::get_x(double d)
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

void tdisplay_t::init_stations(void)
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
      for (int j=0; j < strlen(name); j++)
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
      red_light(y_loc[j], x);
      /***
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
      ****/
    }
  }
}

/*********************************************************************/

void tdisplay_t::print_time(double t)
{
  int secs = (int) t + (6 * 3600);
  int h = secs / 3600;
  int m = (secs % 3600) / 60;
  int s = (secs % 3600) % 60;
  mvprintw(39, 2, "%02d:%02d:%02d", h, m, s);

}

/*********************************************************************/

void tdisplay_t::print_percents(double t, sim_train_t *trains, int ntrains)
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


int main(int argc, char *argv[])
{
}
