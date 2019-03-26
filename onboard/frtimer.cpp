
/*********************
> On Mon, 2005-06-27 at 15:50, danjperron wrote:
> > I create an extended 64 bit version of the free running timer.
> >
> >
> > But if I want to use it into some threads, I need to ensure that 2
> > threads reading tc4 will read the correct values;
> >
> >   No process interrupts the reading of the low 32 bits tc4 and the
> > high 32 bits tc4 sequence. I just don't want a thread reading tc4  at
> > the same time than an other one reading the high 32 bits.
> >
> >
> > check my routine , (inspire from Jesse Off ,messages #10), GetFRTime()
> >
> >
**************/
//> > ////// freeRunningTimer.c

#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<stdio.h>
#include<fcntl.h>

#include "frtimer.h"


int FRTimerHandle=-1;

unsigned char *FRTimerBase=NULL;
unsigned long *FRTimerLo=NULL;
unsigned long *FRTimerHi=NULL;

unsigned long FRTimerExtHi=0; // use to go behond 40 bit (extended)


long frt_us_diff(unsigned long long t2, unsigned long long t1)
{
  unsigned long long  elapsed_fr = FRTimerToUs(t2 - t1);
  return (long) elapsed_fr;
}


/* InitFRTimer
* map physical address
* return 0 if we can't mapped
* return 1 if everything ok
*/


int InitFRTimer(void)
{
      FRTimerHandle= open("/dev/mem",O_RDWR);
        if(FRTimerHandle<0) return(0);
      FRTimerBase = (unsigned char *)

        mmap(0,getpagesize(),PROT_WRITE|PROT_READ,MAP_SHARED,FRTimerHandle,0x80810000UL);

  if (MAP_FAILED == FRTimerBase)
  {
    perror("mmap");
    exit(0);
  }


      if(FRTimerBase  == NULL)
      {
            CloseFRTimer();
            return(0);
      }
      FRTimerLo=  (unsigned long *) (FRTimerBase + 0x60UL); // get Least significant long

      FRTimerHi=  (unsigned long *) (FRTimerBase + 0x64UL); //  get Most significant long

     *FRTimerHi=0x100;  // start Timer
     return(1);
}


/*  CloseFRTimer
*  release map of physical address
*/

void CloseFRTimer(void)
{

   if(FRTimerHandle>=0)
   {
            if(FRTimerBase)
         munmap(FRTimerBase,getpagesize());
         close(FRTimerHandle);
   }
    FRTimerHandle=-1;
      FRTimerBase=NULL;
      FRTimerLo=NULL;
      FRTimerHi=NULL;
}

/* ResetFRTimer
*  Reset and enable Free running timer
*/
void ResetFRTimer(void)
{
   *FRTimerHi= 0;
   *FRTimerHi= 0x100; // enable free running Timer
   FRTimerExtHi=0;     // reset extended bit
}


/* GetFRTimer
*  return current free running timer value
*/


unsigned long long GetFRTimer(void)
{
   // need to be run at least once every 300 hours
   // for the extended bit system to work!

   unsigned long long tval;

   unsigned long HiVal;

    // need to disable irq here
    //
    //  ??????

    tval = *FRTimerLo;


    HiVal= *FRTimerHi & 0xff;

    // need to enable irq here
    //
    //  ?????



    if(HiVal  < (FRTimerExtHi & 0xff))
    {
          // ok we got a wrapped Timer;

          FRTimerExtHi += 0x100;  // increment bit 40
    }
          FRTimerExtHi &= ~0xff;
          FRTimerExtHi |= HiVal;

      tval |= ((unsigned long long ) FRTimerExtHi) << 32;

    return(tval);
}




