
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

/********************************************************************

Routine to initialize and test the TS-NVRAM2.

Written by Donald W. Carr

********************************************************************/

static unsigned char *pc104_start; // Start for PC/104 8 bit read/write to I/O
static unsigned char *nvram2_r0;
static unsigned char *nvram2_r1;
static unsigned char *nvram2_r2;
static unsigned char *nvram2_r3;
static unsigned char *nvram2_r4;
static unsigned char *nvram2_r5;
static unsigned char *nvram2_r6;
static unsigned char *nvram2_r7;

// Pick the correct base depending on jumper settings
#define NVRAM2_BASE (0x0140) 


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

  pc104_start = (unsigned char *) mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x11E00000);

  if (MAP_FAILED == pc104_start)
  {
    perror("mmap");
    exit(1);
  }

  // Assign the addresses for all of the registers
  nvram2_r0 = (unsigned char *) (pc104_start + NVRAM2_BASE + 0);
  nvram2_r1 = (unsigned char *) (pc104_start + NVRAM2_BASE + 1);
  nvram2_r2 = (unsigned char *) (pc104_start + NVRAM2_BASE + 2);
  nvram2_r3 = (unsigned char *) (pc104_start + NVRAM2_BASE + 3);
  nvram2_r4 = (unsigned char *) (pc104_start + NVRAM2_BASE + 4);
  nvram2_r5 = (unsigned char *) (pc104_start + NVRAM2_BASE + 5);
  nvram2_r6 = (unsigned char *) (pc104_start + NVRAM2_BASE + 6);
  nvram2_r7 = (unsigned char *) (pc104_start + NVRAM2_BASE + 7); 

  *nvram2_r5 = 0x80; // Configure to use linear memory that
                 // appears at 0x2A80_0000 to 0x2A9F_FFFF (2 MB) 

  printf("Wrote, r5 = 0x80 (Use linear memory that starts at 0x2A80_0000)\n");

  if ((*nvram2_r0 == 0x77) && (*nvram2_r1 == 0xd9))
  {
    printf("TS-NVRAM2 detected at 0x%x!!\n", NVRAM2_BASE); 
  }
  else
  {
    printf("TS-NVRAM2 NOT  detected, at %x\n", NVRAM2_BASE);
    printf("Check base address, or card not installed\n"); 
  }

  printf("PLD vsersion is: 0x%x\n", *nvram2_r2); 

  if ((*nvram2_r3 & 0x01) != 0) 
  {
    printf("RED LED is on (BAD), battery is near failure\n");
  }
  else
  {
    printf("RED LED is off (GOOD), battery is NOT near failure\n");
  }

  if ((*nvram2_r3 & 0x02) != 0) 
  {
    printf("GREEN LED is on (GOOD), PC/104 power is greater than 4.5V\n");
  }
  else
  {
    printf("GREEN LED is off (BAD), PC/104 power is less than 4.5V\n");
  }
 
  printf("Here are all the registers: (hex):\n");

  printf("0-7: %02x %02x %02x %02x %02x %02x %02x %02x\n", 
      (int) *nvram2_r0, (int) *nvram2_r1, (int) *nvram2_r2, (int) *nvram2_r3,
      (int) *nvram2_r4, (int) *nvram2_r5, (int) *nvram2_r6, (int) *nvram2_r7);

  char *nvram_mem_start = (char *) mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x2A800000);

  if (MAP_FAILED == nvram_mem_start)
  {
    perror("mmap");
    exit(1);
  }
  char *mystring = "Here is some data, please keep it for me!!\n";
  printf("Printing a string to the start of nvram: %s\n", mystring);
  sprintf(nvram_mem_start, "%s\n", mystring);

}

/********************************************************************/


