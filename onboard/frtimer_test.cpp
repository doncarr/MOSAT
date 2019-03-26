
////A test program to check if the FRTimer works!

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "rtcommon.h"
#include "utimer.h"

#include "frtimer.h"

#define OUTPUT_ON 1
#define OUTPUT_OFF 0

#define TV_ELAPSED_US(x, y)     ((((x).tv_sec - (y).tv_sec) * 1000000) + \
        ((x).tv_usec - (y).tv_usec))

void Output(int Value)
{
      static int phy_out=0; // emulate real output
      unsigned long long tcount;

      tcount= GetFRTimer();
      if(phy_out==OUTPUT_ON)
      {
              if(!Value)
              {
                    phy_out=OUTPUT_OFF; // emulate off
              printf("Output OFF at  %llu (ms)\n",FRTimerToMs(tcount));fflush(stdout);
              }
      }
      else
      {
            if(Value)
              {

                   phy_out=OUTPUT_ON; // emulate  on
              printf("Output ON at %llu ms\n",FRTimerToMs(tcount));fflush(stdout);
              }
      }
}

int main(int argc, char *argv[])
{
  unsigned long long  lastcount;
  unsigned long long  currentcount;
  struct timeval now;
  struct timeval last_time;
  int first = 1;

  if(!InitFRTimer())
  {
    printf("Unable to Map Free running Timer\n");
    return(1);
  } 
  ResetFRTimer();


  utimer_t utimer;
  utimer.set_busy_wait(false);
  utimer.set_interval(1000000);
  utimer.set_start_time();

  gettimeofday(&last_time, NULL);

  lastcount = GetFRTimer();
  double total = 0.0;
  double total20 = 0.0;
  int n=0;
  int n20 = 0;
  while (1)
  {
    //sleep(1);
    utimer.wait_next();
    gettimeofday(&now, NULL);
    currentcount = GetFRTimer();
    unsigned long long  elapsed_fr =
       FRTimerToUs(currentcount - lastcount);
    int elapsed_time = TV_ELAPSED_US(now, last_time);
    printf("loop time:  %llu (us), %d\n", 
                  elapsed_fr, elapsed_time);
    lastcount = currentcount;
    last_time = now;
    //total = total + (double) elapsed_fr;
    long x = elapsed_fr;
    //printf("Total = %lf, %ld\n", total, x);
    total += (double) x;
    n++;
    if (n == 20)
    {
      double avg = total / 20.0;
      printf("------ Average = %0.6lf\n", avg);
      n = 0; 
      total = 0.0;
      if (first == 0)
      {
        n20++;
        total20 += avg;
        printf("------ Average20s = %0.6lf\n", total20 / ((double) n20));
      }
      first = 0;
    } 
  }
}


int main2(void)
{

    int cycle;

    unsigned long long  basecount;
    unsigned long long  currentcount;


    if(!InitFRTimer())
    {
          printf("Unable to Map Free running Timer\n");
          return(1);
    }

      ResetFRTimer();
      basecount =0;


      for(cycle=0;cycle<5;cycle++)
      {
        printf("cycle %d\n",cycle);fflush(stdout);
        while(1)
        {
                currentcount = GetFRTimer()- basecount;


            if(FRTimerToMs(currentcount) < 1330ULL) // 1.33 seconds
               Output(OUTPUT_ON);
            else
               Output(OUTPUT_OFF);

            if(FRTimerToMs(currentcount) > 5000ULL) // 5 seconds
               {
                     basecount +=  MsToFRTimer(5000ULL);  // basecount to next cycle
                 break;
               }
        }
      }



      printf("Done\n");

      CloseFRTimer();

  return(0);
}


