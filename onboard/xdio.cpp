
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
#include <string.h>

//#include "rtcommon.h"

#include "utimer.h"

#include "frtimer.h"

/******************************************************************/

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

#define TV_ELAPSED_US(x, y)     ((((x).tv_sec - (y).tv_sec) * 1000000) + \
        ((x).tv_usec - (y).tv_usec))

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

int main(int argc, char *argv[]) 
{
  int fd = open("/dev/mem", O_RDWR|O_SYNC);

  //xdio at 0x12c00000-0x12c00003
  //xdio_start = (unsigned char *) mmap(0, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x12C00000);
  
  page_start = (unsigned char *) mmap(0, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fd,   0x72000000);
  if (MAP_FAILED == page_start)
  {
    perror("mmap");
    exit(0);
  }  

  xdio_0_start = page_start + 0x40;
  xdio_1_start = page_start + 0x44;

  printf("Using base page address: %p\n", page_start);
 
  xdio_0_r0 = (unsigned char *) (xdio_0_start + 0);
  xdio_0_r1 = (unsigned char *) (xdio_0_start + 1);
  xdio_0_r2 = (unsigned char *) (xdio_0_start + 2);
  xdio_0_r3 = (unsigned char *) (xdio_0_start + 3);

  xdio_1_r0 = (unsigned char *) (xdio_1_start + 0);
  xdio_1_r1 = (unsigned char *) (xdio_1_start + 1);
  xdio_1_r2 = (unsigned char *) (xdio_1_start + 2);
  xdio_1_r3 = (unsigned char *) (xdio_1_start + 3);

  // Set xdio core #0 as a counter input
  if (argc > 1)
  {
    printf("Using FAST clock for xdio 0");
    *xdio_0_r0 = 0x40; 
  }
  else
  {
    printf("Using SLOW clock for xdio 0");
    *xdio_0_r0 = 0x41; 
  }
  *xdio_0_r1 = 0x00; 

  // Set xdio core #1 as a PWM output 
  *xdio_1_r0 = 0x00; 
  *xdio_1_r1 = 0xff; 
  *xdio_1_r0 = 0xC0; 
  *xdio_1_r1 = 0x00; 
  *xdio_1_r2 = 0x0F; // 0x0f = 15 + 2 = 17 ticks 
  *xdio_1_r3 = 0x0F; // 0x0f = 15 + 2 = 17 ticks 
  *xdio_1_r0 = 0xC3; // 32.768 khz clock. 
   // Frequency = 32768 / 34 = 963.8 Hz = 1927.5 edges per second. 


/***
* Set mode 0 (0x0 to reg 0x0)
* Set all pins to output. (0xff to reg 0x1)
* Set mode 3 (0xc0 to reg 0x0)
* High time and low time value 0xf (0xf to reg 0x2 and 0x3)
* Start PWM with 32.768Khz clock (0xc3 to reg 0x0)

***/

/***
  *xdio_r0 = 0x82;
  *xdio_r1 = 0x00;
   for discrete outputs
  *xdio_r0 = 0x00; 
  *xdio_r1 = 0xFF; 
****/

/***/
  utimer_t utimer;
  utimer.set_busy_wait(false);
  utimer.set_interval(1000000);
  utimer.set_start_time();
/****/

  if(!InitFRTimer())
  {
    printf("Unable to Map Free running Timer\n");
    return(1);
  }

  ResetFRTimer();


  printf("Wrote, r0 = 0x43, r1 = 0x00\n");
 
  unsigned long long  fr_lastcount;
  unsigned long long  fr_currentcount;

  //struct timeval now;
  //struct timeval last_time;
  //gettimeofday(&last_time, NULL);

  unsigned short last_count; 
  last_count = ((unsigned short) *xdio_0_r3) << 8;
  last_count = last_count + ((unsigned short) *xdio_0_r2); 
  fr_lastcount = GetFRTimer();

  while (1)
  {
    utimer.wait_next();
    //usleep(1000000);
    //gettimeofday(&now, NULL);

    unsigned char byte0 = *xdio_0_r0; 
    unsigned char byte1 = *xdio_0_r1; 
    unsigned char byte2 = *xdio_0_r2; 
    unsigned char byte3 = *xdio_0_r3; 

    unsigned short count16;
    count16 = ((unsigned short) byte3) << 8;
    //printf("%04hx\n", count16);
    count16 = count16 + ((unsigned short) byte2); 
    //printf("%04hx\n", count16);

    fr_currentcount = GetFRTimer();
    //int elapsed_time = TV_ELAPSED_US(now, last_time);
    long elapsed_us =  frt_us_diff(fr_currentcount, fr_lastcount); 
    long n_pulses = calc_dif((long) count16, (long) last_count);
    double frequency = ((double) n_pulses) / (((double) elapsed_us) / 500000.0);

 

    printf("--- %02hhx %02hhx %02hhx %02hhx -  %hx %hu  %ld %ld %0.1lf\n", 
         byte0, byte1, byte2, byte3, count16, count16, n_pulses, elapsed_us, frequency);
    
    fr_lastcount = fr_currentcount;
    last_count = count16;
    //last_time = now;
  }
}

/******************************************************************/

