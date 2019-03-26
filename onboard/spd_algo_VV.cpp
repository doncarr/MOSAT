
#include <stdio.h>
#include <time.h>

#ifndef ARM
#include <ncurses.h>
#endif


#include "spd_algo.h"
#include "spd_algo_VV.h"


const char *states(int mode)
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

speed_algorithm_VV_t::speed_algorithm_VV_t(void)
{
    printf ("Starting speed algorithm VV\n");
    warning=0;
    previous_state = SPD_STATE_NORMAL;

}



/**************************************************************************************************/

speed_state_t speed_algorithm_VV_t::current_state(time_t now, double v_actual,double l_inf,double l_sup,double max){

	double inf=l_inf;
	double sup=l_sup;

	if(previous_state == SPD_STATE_WARN_LO)
		inf += SPD_STATE_DEADBAND;
	if(previous_state == SPD_STATE_WARN_HI)
		sup -= SPD_STATE_DEADBAND;

	if(v_actual >= max){
		previous_state = SPD_STATE_SHUTDOWN;
		return previous_state;
	}
	else if(v_actual >= sup){
		if(previous_state == SPD_STATE_WARN_HI){
			int t =  SPD_MAX_WARN_TIME - (now - warning);
			#ifndef ARM
			mvprintw(32,78,"Time left to slow down: %2d", t);
			#endif
			if((now - warning) > SPD_MAX_WARN_TIME){
				previous_state = SPD_STATE_SHUTDOWN;
				return previous_state;
			}
			else
				return SPD_STATE_WARN_HI;
		}
		else{
			warning = now;
			previous_state = SPD_STATE_WARN_HI;
			return previous_state;
		}
	}
	else if(v_actual >= inf){
		if(previous_state == SPD_STATE_WARN_HI){
			#ifndef ARM	
			mvprintw(32,78,"                           ");
			#endif
		}
		if(previous_state == SPD_STATE_WARN_LO){
			#ifndef ARM
			mvprintw(32,78,"                           ");
			#endif
		}
		previous_state = SPD_STATE_NORMAL;
		return previous_state;
	}
	else{
		#ifndef ARM
			mvprintw(32,78,"Going to slow");
		#endif
		previous_state = SPD_STATE_WARN_LO;
		return previous_state;
	}

return SPD_STATE_WARN_LO;
}

/**************************************************************************************************/

void speed_algorithm_VV_t::evaluate(time_t now, double actual_speed, double actual_distance,current_speed_limits_t limits, speed_results_t *results)
{
    speed_state_t state;

	state = current_state(now,actual_speed,limits.low,limits.high,limits.very_high);
	#ifndef ARM
	mvprintw(27,78,"  Actual: %5.1lf", actual_speed);
	mvprintw(28,78,"     Low: %5.1lf", limits.low);
	mvprintw(29,78,"    High: %5.1lf", limits.high);
	mvprintw(30,78,"Shutdown: %5.1lf", limits.very_high);

	mvprintw(31,78,"   State: %-10s, time: %8d",states(state), now);
	#endif

    results->state = state;
}

/*****************************************************************************/
