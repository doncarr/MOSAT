
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

/********************************************************************

Routine to initialize and test the TS-DIO24.

Written by Donald W. Carr

********************************************************************/

// Start for PC/104 8 bit read/write to I/O
static volatile unsigned char *pc104_start; 
static volatile unsigned char *dio24_r0;
static volatile unsigned char *dio24_r1;
static volatile unsigned char *dio24_r2;
static volatile unsigned char *dio24_r3;
static volatile unsigned char *dio24_r4;
static volatile unsigned char *dio24_r5;
static volatile unsigned char *dio24_r6;
static volatile unsigned char *dio24_r7;

// Pick the correct base depending on jumper settings
#define DIO24_BASE (0x0100) 
//#define DIO24_BASE (0x0108)
//#define DIO24_BASE (0x0110)
//#define DIO24_BASE (0x0118)

/********************************************************************/

int main(int argc, char *argv[]) 
{
  int size = getpagesize();

  printf("Page Size = %d\n", size);

  int fd = open("/dev/mem", O_RDWR|O_SYNC);
  if (fd == -1)
  {
    perror("open /dev/mem");
    exit(1);
  }

  pc104_start = (volatile unsigned char *) mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x11E00000);

  if (MAP_FAILED == pc104_start)
  {
    perror("mmap");
    exit(1);
  }

  // Assign the addresses for all of the registers
  dio24_r0 = (unsigned char *) (pc104_start + DIO24_BASE + 0);
  dio24_r1 = (unsigned char *) (pc104_start + DIO24_BASE + 1);
  dio24_r2 = (unsigned char *) (pc104_start + DIO24_BASE + 2);
  dio24_r3 = (unsigned char *) (pc104_start + DIO24_BASE + 3);
  dio24_r4 = (unsigned char *) (pc104_start + DIO24_BASE + 4);
  dio24_r5 = (unsigned char *) (pc104_start + DIO24_BASE + 5);
  dio24_r6 = (unsigned char *) (pc104_start + DIO24_BASE + 6);
  dio24_r7 = (unsigned char *) (pc104_start + DIO24_BASE + 7);

  *dio24_r3 = 0x00; // disaple all interupts 
  *dio24_r4 = 0x03; // select borts B and C for input 
  printf("Wrote, r3 = 0x00 (no interrupts) , r1 = 0x03 (B,C input)\n");

  if (*dio24_r0 == 0x54)
  {
    printf("Card detected!!\n"); 
  }
  else
  {
    printf("Card not detected, check base address, or card not installed\n"); 
  }

  *dio24_r5 = 0xA5; // Set some bits for port A.
 
  while (1) // Now read all of the ports once a second
  {
    sleep(1);
    unsigned char byte3 = *dio24_r3;
    printf("0-7: %02x %02x %02x %02x %02x %02x %02x %02x\n", 
           (int) *dio24_r0, (int) *dio24_r1, (int) *dio24_r2, (int) *dio24_r3, 
           (int) *dio24_r4, (int) *dio24_r5, (int) *dio24_r6, (int) *dio24_r7);
  }
}

/********************************************************************/


