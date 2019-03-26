
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


#include <math.h>

#include "rtmodbus.h"

#include "spd_comm.h"
#include "frtimer.h"

static unsigned short tsarm_read_count(long *usecs);

static unsigned long long  fr_lastcount;
static unsigned long long  fr_currentcount;


/*********************************************************************/

static double total_distance = 0.0;
//static const double circ = 0.694 * M_PI;
//static const double circ = 0.695 * M_PI;
static const double circ = 0.691 * M_PI; //  la motriz M033 se reperfilo el 08/Nov/2005,
// se reperfila cada 50 mil a 60 mil kms (de 5 a 6 meses).
//const double meters_per_pulse = (circ / 110.0);
const double meters_per_pulse = (circ / 220.0); // this box counts edges!
//static const double meters_per_pulse = 10.0;
static const int count_channel = 2;
static const  int speed_channel = 0;

static long last_count;

static int count_history[2];

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
  long dummy;
  last_count =  tsarm_read_count(&dummy);
  for (int i=0; i < hsize; i++) count_history[i] = last_count;
  hindex = 0;
}

/*********************************************************************/

void get_actual_speed_dist(int section, int t, double *dist, double *speed, spd_discrete_t *discretes)
{
  int current_count;
  int speed_count;
  long elapsed_usecs;

  current_count = tsarm_read_count(&elapsed_usecs);

  //double frequency = ((double) n_pulses) / (((double) elapsed_us) / 500000.0);


  speed_count = current_count; 

  //double ai_speed = (((double) speed_count) / 6553.5) * 0.5; /* This gives you the value in volts */
  //ai_speed *= 10.0; /* Now 1 volt = 10 km/h */

  total_distance += calc_dif(current_count, last_count) * meters_per_pulse;
  double fdistance = calc_dif(current_count, count_history[hindex]) * meters_per_pulse;
  double fspeed = (((fdistance / (double) hsize) * 3600.0) / 1000.0) * (((double) elapsed_usecs) * (1.0 / 1000000.0));

  //fprintf(fp, "\t%lf\t%lf", total_distance, speed);
  //fprintf(fp, "\n");
  //fflush(fp);

  last_count = current_count;
  count_history[hindex] = current_count;
  hindex = (hindex + 1) % hsize;
  *dist = total_distance;
  *speed = fspeed; //ai_speed;
}

/*********************************************************************/

static unsigned char *page_start;

static unsigned char *xdio_0_start;
static unsigned char *xdio_0_r0;
static unsigned char *xdio_0_r1;
static unsigned char *xdio_0_r2;
static unsigned char *xdio_0_r3;

static unsigned char *xdio_1_start;
static unsigned char *xdio_1_r0;
static unsigned char *xdio_1_r1;
static unsigned char *xdio_1_r2;
static unsigned char *xdio_1_r3;



void init_io(void)
{
  printf("Initializing tsarm to read high speed counter\n");
  int fd = open("/dev/mem", O_RDWR|O_SYNC);
  //xdio at 0x12c00000-0x12c00003
  page_start = (unsigned char *) mmap(0, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fd,   0x72000000);
  //page_start = (unsigned char *) mmap(0, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x12C00000);

  xdio_0_start = page_start + 0x40;
  xdio_1_start = page_start + 0x44;

  xdio_0_r0 = (unsigned char *) (xdio_0_start + 0);
  xdio_0_r1 = (unsigned char *) (xdio_0_start + 1);
  xdio_0_r2 = (unsigned char *) (xdio_0_start + 2);
  xdio_0_r3 = (unsigned char *) (xdio_0_start + 3);

  xdio_1_r0 = (unsigned char *) (xdio_1_start + 0);
  xdio_1_r1 = (unsigned char *) (xdio_1_start + 1);
  xdio_1_r2 = (unsigned char *) (xdio_1_start + 2);
  xdio_1_r3 = (unsigned char *) (xdio_1_start + 3);

  // Set xdio core #0 as a counter input
  *xdio_0_r0 = 0x41;
  *xdio_0_r1 = 0x00;

  // Set xdio core #1 as a PWM output
  *xdio_1_r0 = 0x00;
  *xdio_1_r1 = 0xff;
  *xdio_1_r0 = 0xC0;
  *xdio_1_r1 = 0x00;
  *xdio_1_r2 = 0x0F; // 0x0f = 15 + 2 = 17 ticks
  *xdio_1_r3 = 0x0F; // 0x0f = 15 + 2 = 17 ticks
  *xdio_1_r0 = 0xC3; // 32.768 khz clock.

  InitFRTimer();
  ResetFRTimer();
  fr_lastcount = GetFRTimer();

  reset_distance(0);
}

/*********************************************************************/

unsigned short tsarm_read_count(long *usecs)
{
  unsigned char byte2 = *xdio_0_r2;
  unsigned char byte3 = *xdio_0_r3;
  fr_currentcount = GetFRTimer();

  long elapsed_us =  frt_us_diff(fr_currentcount, fr_lastcount);
  //long n_pulses = calc_dif((long) count16, (long) last_count);
  //double frequency = ((double) n_pulses) / (((double) elapsed_us) / 500000.0);

  unsigned short count16;
  count16 = ((unsigned short) byte3) << 8;
  count16 = count16 + ((unsigned short) byte2);

  fr_lastcount = fr_currentcount;
  *usecs = elapsed_us;
  return (count16);
}

/*********************************************************************/

void spd_set_key(int ch)
{
}

/*********************************************************************/
