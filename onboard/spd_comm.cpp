
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef ARM
#include <ncurses.h>
#endif

#include <string.h>
#include <stdint.h>

#include <math.h>

#include "rtmodbus.h"

#include "spd_comm.h"

#include "ap_config.h"

/*********************************************************************/

//static mod_tcpip_client_t *mtp;
static rtmodbus_t *modc;
//static MODSerial *modc;
static double total_distance = 0.0;
//static const double circ = 0.694 * M_PI;
//static const double circ = 0.695 * M_PI;
//  Verificamos 12 de Mayo 2008 que es 0.665 en la motriz de pruebas
static double wheel_diameter = 0.665;
static double circ = wheel_diameter * M_PI; //  la motriz M038? 
// se reperfila cada 50 mil a 60 mil kms (de 5 a 6 meses).
static double meters_per_pulse = (circ / 110.0);
//static const double meters_per_pulse = 10.0;

static int count_channel = 8;
static const  int speed_channel = 0;

static int door_channel = 2;
static bool door_invert = false;
static bool use_left_right_doors = false;
static int left_door_channel = 5;
static int right_door_channel = 6;


static long total_count = 0;
static long last_count = 0;
static int count_history[2] = {0,0};
//static struct timeval time_history[3]; 
static int hindex;
static const int hsize = sizeof(count_history) / sizeof(count_history[0]);

double time_dif(struct timeval tv1, struct timeval atv)
{
  /*
   * This returns the difference in seconds between two
   * timeval structures.
   */

  long secdif = tv1.tv_sec - atv.tv_sec;
  long usecdif = tv1.tv_usec - atv.tv_usec;
  return (double) secdif + ((double) usecdif / 1000000);
}


/*********************************************************************/

long calc_dif(long count, long last)
{
  if (last > count)
  {
    // Ok, we rolled over
    return count + 65536 - last;
  }
  else
  {
    return count - last;
  }
}

/*********************************************************************/

void reset_distance(int section)
{
  total_distance = 0.0;
  total_count = 0;
  uint16_t uvals[20];
  modc->read_ai(0, 16, uvals);
  last_count = uvals[count_channel];
  for (int i=0; i < hsize; i++) count_history[i] = last_count;
  hindex = 0;
}

/*********************************************************************/

void set_calculation_information(double the_diameter, int the_count_channel)
{
  wheel_diameter = the_diameter;
  circ = wheel_diameter * M_PI;
  meters_per_pulse = (circ / 110.0);
  count_channel = the_count_channel;
}

/*********************************************************************/

void connect_modbus(void)
{
  set_calculation_information(ap_config.get_double("diameter", 0.665),
                              ap_config.get_int("count_channel", 8));
  door_channel = ap_config.get_int("door_channel", 2);
  door_invert =  ap_config.get_bool("door_invert", false); 
  use_left_right_doors = ap_config.get_bool("use_left_right_doors", false);
  left_door_channel = ap_config.get_int("left_door_channel", 9);
  right_door_channel = ap_config.get_int("right_door_channel", 10);


  modc = rt_create_modbus(ap_config.get_config("modbus_ip", "172.16.115.99"));
  modc->set_debug_level(10);
  modc->set_address(ap_config.get_int("modbus_address", 0));
  reset_distance(0);
}

/***********************************************************************/

void get_actual_speed_dist(int section, int t, double *dist, double *speed, spd_discrete_t *discretes)
{
  int current_count;
  int speed_count;
  uint16_t uvals[20];
  bool dvals[20];
  char buf[100];


  modc->read_di(0, 16, dvals);
  modc->read_ai(0, 16, uvals);

  current_count = uvals[count_channel];
  speed_count = uvals[speed_channel]; 

  for (int i=0; i < 16; i++)
  {
    buf[i] = dvals[i] ? '1' : '0';
    buf[i+1] = '\0';
    discretes->raw_bits[i] = dvals[i];
  }
  #ifndef ARM
  mvprintw(17,2,"%s %s", buf, dvals[door_channel] ? "OPEN" : "CLOSED");
  #endif
  if (use_left_right_doors)
  {
    bool ltopen = dvals[left_door_channel];
    if (door_invert) ltopen = !ltopen; 
    bool rtopen = dvals[right_door_channel];
    if (door_invert) rtopen = !rtopen; 
    discretes->doors_open = ltopen | rtopen;
  }
  else
  {
    discretes->doors_open = dvals[door_channel];
    if (door_invert)  discretes->doors_open = !discretes->doors_open;
  }
  // When we have the inputs, we need to fix the following:
  discretes->left_open = false;
  discretes->right_open = false;
  discretes->master = false;

  /***
  for (int i=0; i < 16; i++)
  {
    printf("%d ", (int) uvals[i]);
  }
  fprintf(fp, "%s", buf);
  ***/
  /**
  for (int i=0; i < nt; i++)
  {
    count[i] = uvals[ch[i]];
    //fprintf(fp, "\t%d", (int) count[i]);
    //td[i].update(uvals[ch[i]], dvals[dch[i]]);
  }
  **/
  double ai_speed = (((double) speed_count) / 6553.5) * 0.5; /* This gives you the value in volts */
  ai_speed *= 10.0; /* Now 1 volt = 10 km/h */
  long count_diff = calc_dif(current_count, last_count); 
  total_count += count_diff;
  total_distance += double(count_diff) * meters_per_pulse;
  double fdistance = calc_dif(current_count, count_history[hindex]) * meters_per_pulse;
  double fspeed = ((fdistance / (double) hsize) * 3600.0) / 1000.0; 
  //fprintf(fp, "\t%lf\t%lf", total_distance, speed);
  //fprintf(fp, "\n");
  //fflush(fp);
  discretes->total_count = total_count;
  discretes->current_count = current_count;
  *dist = double(total_count) * meters_per_pulse;
  last_count = current_count;
  count_history[hindex] = current_count;
  hindex = (hindex + 1) % hsize;
  //*dist = total_distance;
  *speed = fspeed; //ai_speed;
}

/*********************************************************************/

void init_io(void)
{
  connect_modbus();
}

/*********************************************************************/

void spd_set_key(int ch)
{
}

/*********************************************************************/
