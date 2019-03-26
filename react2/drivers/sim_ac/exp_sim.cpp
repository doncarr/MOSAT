

#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "exp_sim.h"

exp_simulator_t::exp_simulator_t(void)
{
  current_value = 100;
  steady_state_value = 150;
  tau = 45;
}

double exp_simulator_t::get_current_value(double a_delta_time)
{
  double delta_value =  steady_state_value - current_value;
  double fraction = 1.0 - exp(-a_delta_time / tau);
  
  current_value = current_value + (delta_value * fraction);
  return current_value;
}


/***
int main(int argc, char *argv[])
{
  printf("e = %lf\n", exp(1.0));
  printf("fraction = %lf\n", 1.0 - exp(-1.0));
  exp_simulator_t esim;
  esim.set_tau(50.0);
  esim.set_steady_state(0);
  esim.set_current_value(100);
  for(int i=1; true; i++)
  {
    printf("%d: temp = %lf\n", i, esim.get_current_value(1.0));
    sleep(1);
  }
}
***/

