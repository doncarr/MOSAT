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

#include <errno.h>
#include <math.h>

#include "rtcommon.h"
#include "spd_display.h"

static int last_col;
static char station1[20];
static char station2[20];

/*******************************************************************/

int xspd_wait_key(const char *msg)
{
  int mych;
  mvprintw(24, 2,"%-36s", msg); 
  refresh();
  while (-1 != getch());
  
  while (1)
  {
    mych = getch();
    //if ((mych ==  'q') || (mych == 'Q')) {endwin(); exit(0);}
    if (mych != -1)
    {
      break;
    } 
    usleep(10000);
    //if ((mych ==  'q') || (mych == 'Q')) {endwin(); exit(0);}
  }
  mvprintw(24,2,"got %c      ", mych); 
  refresh();
  mvprintw(15,2,"%-36s", ""); 
  refresh();
  return mych;
}

/*******************************************************************/

int spd_wait_key(const char *msg)
{
  int mych;
  mvprintw(15,2,"%-36s", msg); 
  refresh();
  while (-1 != getch());
  
  while (1)
  {
    mych = getch();
    //if ((mych ==  'q') || (mych == 'Q')) {endwin(); exit(0);}
    if (mych != -1)
    {
      break;
    } 
    usleep(400000);
    mvprintw(15,2,"%-36s", " "); 
    refresh();
    mych = getch();
    //if ((mych ==  'q') || (mych == 'Q')) {endwin(); exit(0);}
    if (mych != -1)
    {
      break;
    } 
    usleep(400000);
    mvprintw(15,2, msg); 
    refresh();
  }
  mvprintw(15,2,"%-36s", ""); 
  refresh();
  return mych;
}

/*********************************************************************/

void spd_redraw_segment(void)
{
  mvprintw(10,2,"%-12s", station1);
  mvprintw(10,COLS - 3 - 12,"%12s", station2);
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
  last_col = COLS;
  spd_redraw_all();
  refresh();
};

/*********************************************************************/

void spd_redraw_all()
{
  erase();
  box(stdscr, 0, 0);
                                                                                
  mvhline(2, 0, 0, COLS);
  mvhline(11, 0, 0, COLS);
  attron(A_ALTCHARSET);
  mvprintw(2,0,"%c", ACS_LTEE);
  mvprintw(2,COLS-1,"%c", ACS_RTEE);
  mvprintw(11,0,"%c", ACS_LTEE);
  mvprintw(11,COLS-1,"%c", ACS_RTEE);
  attroff(A_ALTCHARSET);
  mvhline(9, 2, 0, 4);
  mvhline(9, COLS-7, 0, 4);
  //mvprintw(8,2,"====");
  //mvprintw(8,COLS - 7,"====");
  int pct25 = (int) (2.0 + (double) (COLS-7) * 0.25); 
  int pct50 = (int) (2.0 + (double) (COLS-7) * 0.50); 
  int pct75 = (int) (2.0 + (double) (COLS-7) * 0.75); 
  mvprintw(10,pct50,"50");
  if (COLS > 55)
  {
    mvprintw(10,pct25,"25");
    mvprintw(10,pct75,"75");
  }
//#define ACS_ULCORNER    (acs_map['l'])  /* upper left corner */
//#define ACS_LLCORNER    (acs_map['m'])  /* lower left corner */
//#define ACS_URCORNER    (acs_map['k'])  /* upper right corner */
//#define ACS_LRCORNER    (acs_map['j'])  /* lower right corner */
//#define ACS_LTEE        (acs_map['t'])  /* tee pointing right */
//#define ACS_RTEE        (acs_map['u'])  /* tee pointing left */
//#define ACS_BTEE        (acs_map['v'])  /* tee pointing up */
//#define ACS_TTEE        (acs_map['w'])  /* tee pointing down */
//#define ACS_HLINE       (acs_map['q'])  /* horizontal line */
//#define ACS_VLINE       (acs_map['x'])  /* vertical line */
//#define ACS_PLUS        (acs_map['n'])  /* large plus or crossover */
//#define ACS_S1          (acs_map['o'])  /* scan line 1 */
//#define ACS_S9          (acs_map['s'])  /* scan line 9 */
//#define ACS_DIAMOND     (acs_map['`'])  /* diamond */
//#define ACS_CKBOARD     (acs_map['a'])  /* checker board (stipple) */
//#define ACS_DEGREE      (acs_map['f'])  /* degree symbol */
//#define ACS_PLMINUS     (acs_map['g'])  /* plus/minus */
//#define ACS_BULLET      (acs_map['~'])  /* bullet */
/* Teletype 5410v1 symbols begin here */

//#define ACS_LARROW      (acs_map[','])  /* arrow pointing left */
//#define ACS_RARROW      (acs_map['+'])  /* arrow pointing right */
//#define ACS_DARROW      (acs_map['.'])  /* arrow pointing down */
//#define ACS_UARROW      (acs_map['-'])  /* arrow pointing up */
//#define ACS_BOARD       (acs_map['h'])  /* board of squares */
//#define ACS_LANTERN     (acs_map['i'])  /* lantern symbol */
//#define ACS_BLOCK       (acs_map['0'])  /* solid square block */
 

}

/**********************************************************************/

//void spd_print_current(double desired, double actual, int type, bool warn,
//       double pct, double total_distance, double now, double total_time);

//spd_print_current(double, double, int, bool, double, double, double, double, spd_discrete_t*)
void spd_print_current(double desired, double actual, int type, bool warn, 
      double pct, double total_distance, double now, double total_time, spd_discrete_t *discretes)
{ 
  if (last_col != COLS)
  {
    erase();
    spd_redraw_all();
    spd_redraw_segment();
    last_col = COLS;
  }

  mvprintw(3,2,"Deseable: %5.1lf", desired);
  mvprintw(7,2,"  Actual: %5.1lf", actual);
  refresh();
  if (actual > 100) actual = 100;
  if (desired > 100) desired = 100;
  if (actual < 0) actual = 0;
  if (desired < 0) desired = 0;

  char a1[200];
  char a2[200];
  char a3[200];
  char a4[200];
  int maxcols = COLS - 5;
  int dcols = (int) ((double) maxcols * desired / 100.0); 
  int acols = (int) ((double) maxcols * actual / 100.0); 
  int pcols = (int) ((double) (maxcols-4) * pct / 100.0); 
  memset(a1, ' ', sizeof (a1));
  memset(a2, ' ', sizeof (a2));
  memset(a3, ' ', sizeof (a2));
  memset(a4, ' ', sizeof (a4));
  if (dcols > 0)
  {
    memset(a1, 'a', dcols);
    if (type == 0)
    {
      a1[dcols-1] = '<';
    }
    else if (type == 2)
    {
      a1[dcols-1] = '>';
    }
  }
  if (acols > 0)
  {
    memset(a2, 'a', acols);
  }
  a3[pcols] = 'a';
  a3[pcols+1] = 'a';
  a3[pcols+2] = 'a';
  a3[pcols+3] = 'a';
  a1[maxcols] = '\0';
  a2[maxcols] = '\0';
  a3[maxcols] = '\0';
  attron(A_ALTCHARSET);
  mvprintw(4,2,"%s", a1);
  mvprintw(6,2,"%s", a2);
  mvprintw(8,2,"%s", a3);
  attroff(A_ALTCHARSET);
  move(0,0);

  double left = total_distance * (1.0 - (pct/100.0));
  double my_left = left;
  if (left < 1.0) left = 0.0;
  mvprintw(12,2,"Tiempo: %3.0lf, Distancia: %6.1lf", 
           now, (pct/100.0) * total_distance);
  if (COLS < 70) mvprintw(13,2, "");
  else printw(", ");
  printw("Falta: %4.0lf, Porciento: %3.0lf%%", left, pct);
  int lcols;
  if (my_left < 100.0)
  {
    if (my_left < -30.0) my_left = -30.0; 
    double lpct = (100.0 - my_left) / 100.0;
    lcols = (int) ((double)( maxcols - 10) * lpct ); 
  }
  else
  {
    lcols = 0;
  }
  if (lcols > 0)
  {
    memset(a4, 'a', lcols);
  }
  a4[maxcols] = '\0';
  attron(A_ALTCHARSET);
  mvprintw(13,2,"%s", a4);
  attroff(A_ALTCHARSET);
  mvprintw(14,2 + maxcols - 10,"^");
    if (my_left < -5.0) mvprintw(15,maxcols - 10,"**CRRAAASH**");
    else mvprintw(15,maxcols - 10,"            ");
  refresh();

}

/*********************************************************************/
void spd_show_loading(int time)
{
  mvprintw(14,2,"Cargando y Descargando Pasajeros");
  for (int i=0; i < time; i++)
  {
    usleep(500000);
    mvprintw(14,2,"%-36s", " ");
    refresh();
    usleep(500000);
    mvprintw(14,2,"Cargando y Descargando Pasajeros");
    refresh();
  }
  mvprintw(14,2,"%-36s", " ");
}


/*********************************************************************/

void spd_create_image(const char *base_name, const char *gtitle, bool window)
{
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
    fprintf(fp, "set terminal png size 1000,720\n"); 
		    //xadd8e6 x0000ff xdda0dd x9500d3\n");
    //fprintf(fp, "set terminal png small color\n");
  }
  fprintf(fp, "%s\n", "set output\n");
  fprintf(fp, "plot \"%s.txt\" using 1:2 with lines lw 2 title \"actual\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 1:3 with lines lw 2 title \"deseable\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 1:4 with lines lw 2 title \"low\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 1:5 with lines lw 2 title \"high\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 1:6 with lines lw 2 title \"vhigh\"", base_name);
  fprintf(fp, "\n");

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

  mvprintw(24,2,"Cmd: %s", command);

  system(command);
}

/*********************************************************************/

void spd_print_auto(bool auto_mode)
{
  if (auto_mode) 
  {
    mvprintw(22,2,"Auto");
  }
  else
  {
    mvprintw(22,2,"     ");
  }
}


/*********************************************************************/

int spd_endwin(void)
{
  return endwin();
}

/*********************************************************************/

int spd_getch(void)
{
  return  getch();
}

/*********************************************************************/

int spd_beep(void)
{
  return beep();
}

/*********************************************************************/

int spd_prompt_user(const char *msg1, const char *msg2)
{
  char buf[60];
  snprintf(buf, sizeof(buf), "%s: %s", msg1, msg2); 
  return xspd_wait_key(buf);
}

/*********************************************************************/
void spd_show_performance(const onboard_performance_t *perf)
{
  erase();
  spd_redraw_all();
  mvprintw(3, 10, "----------- Esta Segmento -------------");
  mvprintw(4, 10, "Typ.: 98., Act.: 104, -6 segundos      ");
  mvprintw(5, 10, "--------------- Total -----------------");
  mvprintw(6, 10, "Typ.: 22:35, Act.: 23:09, -34 segundos ");
  refresh();
}


