
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

  // TS-7260
  page_start = (unsigned char *) mmap(0, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x12C00000);
  
  //page_start = (unsigned char *) mmap(0, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fd,   0x72000000);

  if (MAP_FAILED == page_start)
  {
    perror("mmap");
    exit(0);
  }  

  xdio_0_start = page_start + 0x00; // 0 offset on 7260 

  printf("Using base page address: %p\n", page_start);
 
  xdio_0_r0 = (unsigned char *) (xdio_0_start + 0);
  xdio_0_r1 = (unsigned char *) (xdio_0_start + 1);
  xdio_0_r2 = (unsigned char *) (xdio_0_start + 2);
  xdio_0_r3 = (unsigned char *) (xdio_0_start + 3);

  /****
  xdio_1_r0 = (unsigned char *) (xdio_1_start + 0);
  xdio_1_r1 = (unsigned char *) (xdio_1_start + 1);
  xdio_1_r2 = (unsigned char *) (xdio_1_start + 2);
  xdio_1_r3 = (unsigned char *) (xdio_1_start + 3);
  ******/

  // Set xdio core as a PWM output 
  *xdio_0_r0 = 0x00; 
  *xdio_0_r1 = 0xff; 
  *xdio_0_r0 = 0xC0; 
  *xdio_0_r1 = 0x00; 
  *xdio_0_r2 = 0x0F; // 0x0f = 15 + 2 = 17 ticks 
  *xdio_0_r3 = 0x0F; // 0x0f = 15 + 2 = 17 ticks 
  *xdio_0_r0 = 0xC3; // 32.768 khz clock. 
   // Frequency = 32768 / 34 = 963.8 Hz = 1927.5 edges per second. 


/***
* Set mode 0 (0x0 to reg 0x0)
* Set all pins to output. (0xff to reg 0x1)
* Set mode 3 (0xc0 to reg 0x0)
* High time and low time value 0xf (0xf to reg 0x2 and 0x3)
* Start PWM with 32.768Khz clock (0xc3 to reg 0x0)

***/
  while(true)
  {
    sleep(100);
  }

}

/******************************************************************/

