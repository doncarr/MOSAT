/************************************************************************
This software is part of React, a control engine
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
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include "reactpoint.h"
#include "displaydata.h"

struct display_list_t
{
  int n;
  int line;
  bool analog;
};

#define NORMAL_COLOR (1)
#define ALARM_COLOR (2)
#define CAUTION_COLOR (3)
#define FAILED_COLOR (4)

/*********************************************************************/

void react_set_color(int y, int x, int n, short color)
{
  short attr = A_NORMAL;
  switch (color)
  {
    case NORMAL_COLOR:
     attr = A_NORMAL;
     break;
    case CAUTION_COLOR:
     attr = A_BOLD;
     break;
    case ALARM_COLOR:
     attr = A_BOLD;
     break;
    case FAILED_COLOR:
     attr = A_NORMAL;
     break;
  }
  mvchgat(y, x, n, attr, color, NULL);
}


/*********************************************************************/

void test_colors(short bg)
{
  short colors[9] = {COLOR_BLACK, COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW,
                      COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE};
  short attrs[3] = {A_NORMAL, A_BOLD, A_DIM};
  int n = 0;
  int x = 5;
  int y = 5;
  colors[0] = bg;
  for (int i=0; i < 9; i++)
  {
    for (int j=i+1; j < 9; j++)
    {
      int k=0;
      {
      init_pair(n, colors[i], colors[j]);
      mvprintw(y, x, "Testing %2d, %2d, %2d", i, j, k);
      mvchgat(y, x, 19, attrs[k], n, NULL);
      n++;
      y++;
      init_pair(n, colors[j], colors[i]);
      mvprintw(y, x, "Testing %2d, %2d, %2d", j, i, k);
      mvchgat(y, x, 19, attrs[k], n, NULL);
      n++;
      y++;
      if (y > 30)
      {
         y = 5;
         x += 21;
      }
      }
    }
  }
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
//LINES and COLS

//            COLOR_RED
//            COLOR_GREEN
  short fg, bg;
  pair_content(COLOR_PAIR(0), &fg, &bg);

/***
<span style="color: rgb(255, 0, 0);">Red</span><br>
<span style="color: rgb(51, 255, 51);">Green</span><br>
<span style="color: rgb(51, 102, 255);">Blue</span><br>
<span style="color: rgb(204, 204, 204);">LightGray</span><br>
Black<br>
<span style="color: rgb(255, 255, 255);">White</span><br>
<span style="color: rgb(255, 255, 102);">Yellow<br>
<span style="color: rgb(255, 204, 0);">Orange<br>
<span style="color: rgb(102, 102, 102);">DarkGray</span><br>
****/

  // This does not work.
  //init_color(COLOR_YELLOW, 255,255,102);
  //init_color(COLOR_RED, 255,200,200);


  init_pair(NORMAL_COLOR, fg, bg);
  init_pair(ALARM_COLOR, COLOR_RED, bg);
  init_pair(CAUTION_COLOR, COLOR_YELLOW, COLOR_BLACK);
  init_pair(FAILED_COLOR, COLOR_BLACK, COLOR_MAGENTA);

//  init_pair(2, COLOR_GREEN, bg);
//  init_pair(3, COLOR_BLACK, COLOR_YELLOW);
//  init_pair(4, COLOR_BLUE, bg);
//  init_pair(5, COLOR_MAGENTA, bg);
//  init_pair(6, COLOR_CYAN, bg);
//  init_pair(7, COLOR_BLACK, COLOR_WHITE);
//             COLOR_YELLOW
//             COLOR_BLUE
//             COLOR_MAGENTA
//             COLOR_CYAN
//             COLOR_WHITE
//#define COLOR_BLACK     0
//#define COLOR_RED       1
//#define COLOR_GREEN     2
//#define COLOR_YELLOW    3
//#define COLOR_BLUE      4
//#define COLOR_MAGENTA   5
//#define COLOR_CYAN      6
//#define COLOR_WHITE     7



  timeout(0);
  box(stdscr, 0, 0);
  //mvhline(2, 2, 0, COLS - 5);
  //mvhline(8, 2, 0, COLS - 5);

  //test_colors(bg);
  //mvprintw(30, 5, "Can change color? %s\n", can_change_color() ? "Yes" : "No");

}

/*************************************************************************/

bool search_for_tag(const char *tag, display_info_t *dinfo,
                      display_list_t *list)
{
  for (int i=0; i < dinfo->n_analog; i++)
  {
    if (0 == strcasecmp(tag, dinfo->adata[i].tag))
    {
      list->n = i;
      list->analog = true;
      return true;
    }
  }
  for (int i=0; i < dinfo->n_discrete; i++)
  {
    if (0 == strcasecmp(tag, dinfo->ddata[i].tag))
    {
      list->n = i;
      list->analog = false;
      return true;
    }
  }
  printf("Tag not found: %s\n", tag);
  return false;
}

/*************************************************************************/

void display_all(display_info_t *dinfo, display_list_t *list, int nl)
{
  int color[] = {NORMAL_COLOR, ALARM_COLOR, CAUTION_COLOR, FAILED_COLOR};
  for (int i=0; i < nl; i++)
  {
    if (list[i].analog)
    {
      mvprintw(list[i].line, 2,"%s", dinfo->adata[list[i].n].tag);
      mvprintw(list[i].line, 24,"%s", dinfo->adata[list[i].n].description);
      mvprintw(list[i].line, 46,"%-10.2lf", dinfo->adata[list[i].n].pv);
      react_set_color(list[i].line, 46, 10, color[i%4]);
    }
    else
    {
      mvprintw(list[i].line, 2,"%s", dinfo->ddata[list[i].n].tag);
      mvprintw(list[i].line, 24,"%s", dinfo->ddata[list[i].n].description);
      mvprintw(list[i].line, 46,"%-6s", dinfo->ddata[list[i].n].pv);
      react_set_color(list[i].line, 46, 10, color[i%4]);
    }
  }
}

/*************************************************************************/

int main(int argc, char *argv[])
{
  key_t mykey;
  // First open the file and read the shared memory id.
  FILE *fp = fopen("shmid.txt", "r");
  if (fp == NULL)
  {
    perror("shmid.txt");
  }
  fscanf(fp, "%d", &mykey);
  fclose(fp);

  // Now attach to shared memory.
  void *myshmp = shmat(mykey, NULL, 0);
  if (myshmp == (void *) -1)
  {
    perror("Could not attach to shared memory");
    printf("React is probably not running!!\n");
    exit(0);
  }
  printf("I attached to shared memory, addr = %p\n", myshmp);

  char *cp = (char *) myshmp;
  display_info_t dinfo;
  get_display_pointers(myshmp, &dinfo);
  int n_display;
  const int maxlist = 20;
  display_list_t dlist[maxlist];
  int nl = 0;
  int line = 2;
  for (int i=1; (i < argc) && (i < maxlist); i++)
  {
    if (search_for_tag(argv[i], &dinfo, &dlist[nl]))
    {
      printf("Found %s, adding to list\n", argv[i]);
      dlist[nl].line = line;
      nl++;
      line++;
    }
  }

  init_screen();
  /****
  initscr();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  curs_set(0);

  timeout(0);
  box(stdscr, 0, 0);
  ***/

  bool quit = false;
  while (!quit)
  {
    int ch = getch();
    switch (ch)
    {
      case 'q':
      case 'Q':
        quit = true;
        break;
    }
    display_all(&dinfo, dlist, nl);
    refresh();
    usleep(100000);
  }
  endwin();
}

/*************************************************************************/

