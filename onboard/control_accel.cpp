
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <math.h>


/**************************************************************************************************/
static const int SIM_ACCEL = 0;
static const int SIM_BRAKE = 1;

static int mode = SIM_ACCEL;
static int level = 0;
static double new_accel = 0;
static double cur_accel = 0;
static double cur_speed = 0;;
static double cur_kmh = 0;;
static unsigned long count16 = 0;

static const double circ = 0.691 * M_PI; //  la motriz M033 se reperfilo el 08/Nov/2005,
static const double meters_per_pulse = (circ / 220.0); // this box counts edges!

/*************************************************************************************************/

double calc_speed(void)
{

  if ((cur_kmh > 70.0) && (mode == SIM_ACCEL)) 
  {
    double dif = cur_kmh - 70.0;
    new_accel = (1.0 / 60.0) - (dif * (1.0/120.0));
  }
  else
  {
    new_accel = double(level) / 60.0;
    if (mode == SIM_BRAKE) new_accel *= -1;
  }

  cur_accel = cur_accel + (0.3 * (new_accel - cur_accel));
  cur_speed += cur_accel;
  if (cur_speed < 0.0) cur_speed = 0.0;
  cur_kmh = cur_speed * 3.6;

  count16 += (int) ((0.1) * cur_speed / meters_per_pulse); 
  if (count16 > 65535) count16 -= 65535;

}

/*************************************************************************************************/

int main(int argc, char *argv[])
{
  initscr();
  start_color();
  use_default_colors();
  cbreak();
  noecho();
  timeout(0);
  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  curs_set(0);

  short fg, bg;
  pair_content(COLOR_PAIR(0), &fg, &bg);


  for (int i=0; true; i++)
  {
    mvprintw(8,2,"   -    ");
    int mych = getch();
    if ((mych ==  'q') || (mych == 'Q')) {endwin(); exit(0);}

    if (mych ==  'b')
    {
      mvprintw(8,2,"Hit 'b'");
      mode = SIM_BRAKE;
    }
    if (mych ==  'a')
    {
      mvprintw(8,2,"Hit 'a'");
      mode = SIM_ACCEL;
    }

    if ((mych ==  '0') ||  
       (mych == '1') ||
       (mych == '2') ||
       (mych == '3') ||
       (mych == '4') ||
       (mych == '5') ||
       (mych == '6') ||
       (mych == '7') ||
       (mych == '8') ||
       (mych == '9') )
    {
      mvprintw(8,2,"Hit '%c'", mych);
      level = mych - '0';
    }

    time_t now = time(NULL);
    calc_speed();

    mvprintw(1,2,"spd (km/h): %5.1lf", cur_kmh);
    mvprintw(2,2,"spd  (m/s): %5.1lf", cur_speed);
    mvprintw(3,2,"     Level: %3d", level);
    mvprintw(4,2,"      Mode: %-10s", (mode == SIM_ACCEL) ? "Accel": "Brake");
    mvprintw(5,2,"  NewAccel: %5.2lf", new_accel * 10.0);
    mvprintw(5,2,"  CurAccel: %5.2lf", cur_accel * 10.0);
    mvprintw(6,2,"     State: %-10s", "");
    struct tm mytm;
    char buf[10];
    localtime_r(&now, &mytm);
    strftime(buf, sizeof(buf), "%T", &mytm);
    mvprintw(7,2,"      Time: %s", buf);
    mvprintw(8,2,"     count: %8lu", count16);

    refresh();

    usleep(100000);
  }
  endwin();

}
