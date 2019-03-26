#include <stdio.h>
#include <unistd.h>

#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "iodriver.h"

#include "random_tools.h"
#include "exp_sim.h"
#include "sim_ac.h"

/***********************************************************************/

ac_unit_sim_t::ac_unit_sim_t(double a_period, double a_fraction_on, double a_start_delay, double start_time)
{
  time_on = a_period * a_fraction_on;
  time_off = a_period * (1.0 - a_fraction_on);
  on_tau = 10.0;
  off_tau = 18.0;
  ac_is_on = false;
  last_time = start_time;
  next_transition = start_time + a_start_delay;
  outside_temp = 33.4;
  steady_state_cold = 4.5;
  steady_state_hot = 55.8;
  sim_cold.set_steady_state(outside_temp);
  sim_cold.set_current_value(outside_temp + 2.2);
  sim_cold.set_tau(off_tau);
  sim_hot.set_steady_state(outside_temp);
  sim_hot.set_current_value(outside_temp - 1.8);
  sim_hot.set_tau(off_tau);
}

/***********************************************************************/

bool ac_unit_sim_t::update(double time_now, 
            double *the_cold_temp, double *the_hot_temp)
{
  double elapsed_time = time_now - last_time;
  last_time = time_now;  
  if (ac_is_on)
  {
    update_on(time_now);
  }
  else
  {
    update_off(time_now);
  }
  cold_temp = sim_cold.get_current_value(elapsed_time);
  cold_temp = add_rand_noise(cold_temp, 100.0, 0.01);
  hot_temp = sim_hot.get_current_value(elapsed_time);
  hot_temp = add_rand_noise(hot_temp, 100.0, 0.01);
  *the_hot_temp = hot_temp;
  *the_cold_temp = cold_temp;
  return ac_is_on;
}

/***********************************************************************/

void ac_unit_sim_t::update_on(double time_now)
{
  //printf("ON: next: %0.1lf\n", next_transition);
  if (time_now > next_transition)
  {
    ac_is_on = false;
    double next_time_off = add_rand_noise(time_off, time_off, 0.15); 
    next_transition = time_now + next_time_off;
    // Ok, if the unit is off, the steady state value is the outside temp
    sim_hot.set_steady_state(outside_temp);
    sim_hot.set_tau(off_tau);
    sim_cold.set_steady_state(outside_temp);
    sim_cold.set_tau(off_tau);
  }
}

/***********************************************************************/

void ac_unit_sim_t::update_off(double time_now)
{
  //printf("OFF: next: %0.1lf\n", next_transition);
  if (time_now > next_transition)
  {
    ac_is_on = true;
    double next_time_on = add_rand_noise(time_on, time_on, 0.12); 
    next_transition = time_now + next_time_on;

    sim_hot.set_steady_state(steady_state_hot);
    sim_hot.set_tau(on_tau);
    sim_cold.set_steady_state(steady_state_cold);
    sim_cold.set_tau(on_tau);
  }
}

/***********************************************************************/
