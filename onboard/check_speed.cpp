
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>

static const int STATE_SHUTDOWN = 3;
static const int STATE_WARN_HI = 2;
static const int STATE_NORMAL = 1;
static const int STATE_WARN_LO = 0;

static const double STATE_DEADBAND = 0.5; 
static const int MAX_WARN_TIME = 20; 

time_t warn_start_time = 0;
static int last_state = STATE_NORMAL;

/**************************************************************************************************/

char *state_to_string(int mode)
{
  switch (mode)
  {
    case STATE_SHUTDOWN: return "SHUTDOWN"; break;
    case STATE_WARN_HI: return  "WARN_HI"; break;
    case STATE_WARN_LO: return  "WARN_LO"; break;
    case STATE_NORMAL: return   "NORMAL"; break;
    default: return "**UNDEFINED**"; break;
  }
  return "**UNDEFINED**";
}

/**************************************************************************************************/

void check_speed_init(void)
{
  last_state = STATE_NORMAL;
}

/**************************************************************************************************/

int check_speed(time_t now, double actual, double the_lo_limit, double the_hi_limit, double shutdown_limit)
{
  double hi_limit = the_hi_limit - 0.001; 
  double lo_limit = the_lo_limit + 0.001; 
  if (last_state == STATE_WARN_HI)
  {
    hi_limit -= STATE_DEADBAND - 0.001;
  }
  if (last_state == STATE_WARN_LO)
  {
    lo_limit += STATE_DEADBAND + 0.001;
  }

  if (actual >= shutdown_limit)
  {
    last_state = STATE_SHUTDOWN;
    return STATE_SHUTDOWN;
  }
  else if (actual >= hi_limit)
  {
    if (last_state == STATE_WARN_HI)
    {
      int time_left = MAX_WARN_TIME - (now - warn_start_time);
      mvprintw(9,2,"Time left to slow down: %2d", time_left);
      if ((now - warn_start_time) > MAX_WARN_TIME)
      {
        last_state = STATE_SHUTDOWN;
        return STATE_SHUTDOWN;
      }
      else
      {
        return STATE_WARN_HI;
      }
    }
    else 
    {
      warn_start_time = now;
      last_state = STATE_WARN_HI;
      return STATE_WARN_HI;
    }
  }
  else if (actual > lo_limit)
  {
    if (last_state == STATE_WARN_HI) mvprintw(9,2,"                                      ");
    last_state = STATE_NORMAL;
    return STATE_NORMAL;
  }
  else
  {
    last_state = STATE_WARN_LO;
    return STATE_WARN_LO;
  }
  return STATE_WARN_LO;
}

/**************************************************************************************************/

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

  double actual_spd = 70; 

  check_speed_init();

  while (1)
  {
    mvprintw(8,2,"   -    ");
    int mych = getch();
    if ((mych ==  'q') || (mych == 'Q')) {endwin(); exit(0);}

    if (mych ==  's')
    {
      mvprintw(8,2,"Hit 's'");
      actual_spd -= 0.1;
    }
    if (mych ==  'S')
    {
      mvprintw(8,2,"Hit 'S'");
      actual_spd -= 1.0;
    }

    if (mych ==  'f') 
    {
      mvprintw(8,2,"Hit 'f'");
      actual_spd += 0.1;
    }
    if (mych ==  'F') 
    {
      mvprintw(8,2,"Hit 'F'");
      actual_spd += 1.0;
    }

//int check_speed(time_t now, double actual, double the_lo_limit, double the_hi_limit, double shutdown_limit)
    time_t now = time(NULL);
    int state = check_speed(now, actual_spd, 68.0, 72.0, 75.0);
    mvprintw(2,2,"  Actual: %5.1lf", actual_spd);
    mvprintw(3,2,"     Low: %5.1lf", 68.0);
    mvprintw(4,2,"    High: %5.1lf", 72.0);
    mvprintw(5,2,"Shutdown: %5.1lf", 75.0);
    mvprintw(6,2,"   State: %-10s", state_to_string(state));
    struct tm mytm;
    char buf[10];
    localtime_r(&now, &mytm);
    strftime(buf, sizeof(buf), "%T", &mytm);
    mvprintw(7,2,"    Time: %s", buf);

    refresh();
    if (state == STATE_SHUTDOWN)
    {
      mvprintw(10,2,"STOPPING THE TRAIN *********");
      refresh();
      sleep(5);
      endwin(); 
      exit(0);
    }
    usleep(100000);
  }
  endwin();

}
