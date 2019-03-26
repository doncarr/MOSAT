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
#include <ncurses.h>

#include <errno.h>
#include <math.h>

#include "rtcommon.h"
#include "spd_display.h"

static int last_col;
static char station1[20];
static char station2[20];

/*******************************************************************/

int spd_wait_key(const char *msg)
{
  int mych;
  mvprintw(1,0,"%-24s", msg); 
  refresh();
  while (-1 != getch());
  
  mvprintw(0,0,"%-24s", " "); 
  mvprintw(0,0,"Ult +1.2 Tot -12.6"); 
  
  while (1)
  {
    mych = getch();
    if ((mych ==  'q') || (mych == 'Q')) {endwin(); exit(0);}
    if (mych != -1)
    {
      break;
    } 
    usleep(400000);
    mvprintw(1,0,"%-24s", " "); 
    refresh();
    mych = getch();
    if ((mych ==  'q') || (mych == 'Q')) {endwin(); exit(0);}
    if (mych != -1)
    {
      break;
    } 
    usleep(400000);
    mvprintw(1,0, msg); 
    refresh();
  }
  mvprintw(1,0,"%-24s", " "); 
  refresh();
  return mych;
}

/*********************************************************************/

void spd_redraw_segment(void)
{
  mvprintw(0,0,"%-24s", " ");
  mvprintw(0,0,"%s-%s", station1, station2);
  //mvprintw(10,COLS - 3 - 12,"%12s", station2);
}

/*********************************************************************/

void spd_init_segment(const char *st1, const char *st2)
{
  safe_strcpy(station1, st1, sizeof(station1));
  safe_strcpy(station2, st2, sizeof(station2));
  spd_redraw_segment();
}
                                                                                
/*********************************************************************/

void spd_init_screen()
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

  timeout(0);
  last_col = COLS;
  spd_redraw_all();
  refresh();
};

/*********************************************************************/

void spd_redraw_all()
{
  mvprintw(0,0,"%-24s", " ");
  mvprintw(1,0,"%-24s", " ");
}

/**********************************************************************/
static bool last_warn = false;
static int count_down = 0;

void spd_print_current(double desired, double actual, int type, bool warn, 
      double pct, double total_distance, double now, double total_time,  spd_discrete_t *discretes)
{ 
  if (last_col != COLS)
  {
    erase();
    spd_redraw_all();
    spd_redraw_segment();
    last_col = COLS;
  }

  const char *str = "-";
  char cnt_str[5];
  if (type == 0)
  {
    str = "<";
  }
  else if (type == 2)
  {
    str = ">";
  }
  if (!last_warn && warn)
  {
    count_down = 5;
  }
  if (warn)
  {
    snprintf(cnt_str, sizeof(cnt_str), "%d", count_down); 
    str = cnt_str;
    if (count_down > 0) 
    {
      count_down--;
    }
  }

  mvprintw(0,0,"%-24s", " ");
  mvprintw(0,0,"%s-%s", station1, station2);
  mvprintw(1,0,"%-24s", " ");
  mvprintw(1,0,"Des:%4.1lf %s Act:%4.1lf %2.0lf%%", desired, str, actual, pct);
  refresh();

  last_warn = warn;
}

/*********************************************************************/

void spd_show_loading(int time)
{
  mvprintw(0,0,"%-24s", " "); 
  mvprintw(0,0,"Ult +1.2 Tot -12.6"); 

  mvprintw(1,0,"Cargando y Descargando");
  for (int i=0; i < time; i++)
  {
    usleep(500000);
    mvprintw(1,0,"%-24s", " ");
    refresh();
    usleep(500000);
    mvprintw(1,0,"Cargando y Descargando");
    refresh();
  }
  mvprintw(1,0,"%-24s", " ");
}

/*********************************************************************/

void spd_print_auto(bool auto_mode)
{
  if (auto_mode)
  {
  }
  else
  {
  }
}

/*********************************************************************/

int spd_endwin(void)
{
  return endwin();
}

/*********************************************************************/

int spd_beep(void)
{
  return beep();
}

/*********************************************************************/

int spd_getch(void)
{
  return getch();
}

/*********************************************************************/

void spd_create_image(const char *base_name, const char *gtitle, bool window)
{
  return;
  const char *fname = "gnuplotoptions.txt";
  FILE *fp = fopen(fname, "w");
  if (fp == NULL)
  {
    perror(fname);
    return;
  }
  fprintf(fp, "set   autoscale\n");
  fprintf(fp, "unset log\n");
  fprintf(fp, "unset label\n");
  fprintf(fp, "set xtic auto\n");
  fprintf(fp, "set ytic auto\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set xlabel \"meters \"\n");
  fprintf(fp, "set ylabel \"km/h\"\n");

  fprintf(fp, "set title \"%s\"\n", gtitle);

  // The following line is basic black and white
  //fprintf(fp, "set terminal png size 800,600 xffffff x000000 x000000 x000000\n");
  // The following is default, probably black and white with red data
  //fprintf(fp, "set terminal png size 800,600\n");
  //The follow line is light blue background, blue font, magenta data line
  if (!window)
  {
    //fprintf(fp, "set terminal png size 1000,300 xadd8e6 x0000ff xdda0dd x9500d3\n");
    fprintf(fp, "set terminal png small color\n");
  }
  fprintf(fp, "%s\n", "set output\n");
  fprintf(fp, "plot \"%s.txt\" using 1:2 with lines lw 2 title \"actual\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 1:3 with lines lw 2 title \"deseable\"\n", base_name);

  fclose(fp);
  char command[500];
  if (window)
  {
    snprintf(command, sizeof(command), "gnuplot -persist %s", fname);
  }
  else
  {
    snprintf(command, sizeof(command), "gnuplot %s > %s.png", fname, base_name);
  }
  system(command);
}

/*********************************************************************/

void spd_show_performance(const onboard_performance_t *perf)
{
}


