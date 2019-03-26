
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <stdint.h>

#include <math.h>

#include "rtmodbus.h"

#include "spd_comm.h"

static double accell = 0.0; // m/s²
static double speed = 0.0; // km/h
static double distance = 0.0; // m

/*********************************************************************/

void reset_distance(int section)
{
  distance = 0.0;
}

/*********************************************************************/

void get_actual_speed_dist(int section, int t, double *a_dist, double *a_speed, spd_discrete_t *discretes)
{
  double last_speed = speed;
  speed += accell * 3.6;
  if (speed < 0) speed = 0;
  if (speed > 72.5) speed = 72.5;
  double avg_speed = (speed + last_speed) / 2;
  double delta_distance = avg_speed / 3.6;
  if (delta_distance < 0.0) delta_distance = 0;
  distance += delta_distance;
  *a_dist = distance;
  *a_speed = speed;
}

/*********************************************************************/

void init_io(void)
{
  accell = 0.0;
  speed = 0.0;
  distance = 0.0;
}

/*********************************************************************/

void spd_set_key(int ch)
{
  if (ch == '0')
  {
    accell = -0.1;
  }
  else if (ch == '1')
  {
    accell = 0.5;
  }
  else if (ch == '2')
  {
    accell = 1.0;
  }
  else if (ch == '9')
  {
    accell = -0.5;
  }
  else if (ch == '8')
  {
    accell = -1.0;
  }
}

/*********************************************************************/
