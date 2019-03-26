
#include <stdio.h>
#include <time.h>

#ifndef ARM
#include <ncurses.h>
#endif


#include "spd_algo.h"
#include "spd_algo_DC.h"

const char *state_to_string(int mode)
{
  switch (mode)
  {
    case SPD_STATE_SHUTDOWN: return "SHUTDOWN"; break;
    case SPD_STATE_WARN_HI: return  "WARN_HI"; break;
    case SPD_STATE_WARN_LO: return  "WARN_LO"; break;
    case SPD_STATE_NORMAL: return   "NORMAL"; break;
    default: return "**UNDEFINED**"; break;
  }
  return "**UNDEFINED**";
}


/*****************************************************************************/

speed_algorithm_DC_t::speed_algorithm_DC_t(void)
{
  printf("Initializing speed_algorithm_DC_t\n"); 
  last_state = SPD_STATE_NORMAL;
  warn_start_time = 0;
}


/**************************************************************************************************/

speed_state_t speed_algorithm_DC_t::check_speed(time_t now, double actual, 
		 double the_lo_limit, double the_hi_limit, double shutdown_limit)
{
  double hi_limit = the_hi_limit - 0.001; 
  double lo_limit = the_lo_limit + 0.001; 
  if (last_state == SPD_STATE_WARN_HI)
  {
    hi_limit -= SPD_STATE_DEADBAND - 0.001;
  }
  if (last_state == SPD_STATE_WARN_LO)
  {
    lo_limit += SPD_STATE_DEADBAND + 0.001;
  }

  if (actual >= shutdown_limit)
  {
    last_state = SPD_STATE_SHUTDOWN;
    return SPD_STATE_SHUTDOWN;
  }
  else if (actual >= hi_limit)
  {
    if (last_state == SPD_STATE_WARN_HI)
    {
      int time_left = SPD_MAX_WARN_TIME - (now - warn_start_time);
      #ifndef ARM
      mvprintw(32,2,"Time left to slow down: %2d", time_left);
      #endif
      if ((now - warn_start_time) > SPD_MAX_WARN_TIME)
      {
        last_state = SPD_STATE_SHUTDOWN;
        return SPD_STATE_SHUTDOWN;
      }
      else
      {
        return SPD_STATE_WARN_HI;
      }
    }
    else 
    {
      warn_start_time = now;
      last_state = SPD_STATE_WARN_HI;
      return SPD_STATE_WARN_HI;
    }
  }
  else if (actual > lo_limit)
  {
    #ifndef ARM
    if (last_state == SPD_STATE_WARN_HI) mvprintw(32,2,"                                      ");
    #endif
    last_state = SPD_STATE_NORMAL;
    return SPD_STATE_NORMAL;
  }
  else
  {
    last_state = SPD_STATE_WARN_LO;
    return SPD_STATE_WARN_LO;
  }
  return SPD_STATE_WARN_LO;
}

/*****************************************************************************/

void speed_algorithm_DC_t::evaluate(time_t now, double actual_speed, double actual_distance,
		                current_speed_limits_t limits, speed_results_t *results)
{
  speed_state_t state;
  state = check_speed(now, actual_speed, limits.low, limits.high, limits.very_high);

  #ifndef ARM
  mvprintw(27,2,"  Actual: %5.1lf", actual_speed);
  mvprintw(28,2,"     Low: %5.1lf", limits.low);
  mvprintw(29,2,"    High: %5.1lf", limits.high);
  mvprintw(30,2,"Shutdown: %5.1lf", limits.very_high);

  mvprintw(31,2,"   State: %-10s, time: %8d", state_to_string(state), now);
  #endif

  results->state = state;
}

/*****************************************************************************/
