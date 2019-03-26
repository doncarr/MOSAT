
////// FreeRunningTimer.h

#ifndef FREE_RUNNING_TIMER_H
#define FREE_RUNNING_TIMER_H


int InitFRTimer(void);
void CloseFRTimer(void);
void ResetFRTimer(void);
unsigned long long GetFRTimer(void);
long frt_us_diff(unsigned long long t2, unsigned long long t1);

//B.T.W.  for more precision use  since 64bit = 500 000 years
#define FRTimerToMs(A)  (((unsigned long long) A) * 100ULL / 98304ULL)
#define MsToFRTimer(A)  (((unsigned long long) A) * 98304ULL / 100ULL)

#define FRTimerToUs(A)  (((unsigned long long) A) * 1000000ULL / 983293ULL)
//#define FRTimerToUs(A)  (((unsigned long long) A) *   1000000ULL / 983040ULL)

//#define FRTimerToMs(A)  (((unsigned long long) A) / 984ULL)
//#define MsToFRTimer(A)  (((unsigned long long) A) * 984ULL)

//#define FRTimerToMs(A)  ((unsigned long long) A >> 10)
//#define MsToFRTimer(A)  ((unsigned long long) A << 10)

int IsFRTimerPassed(unsigned long long BaseTime, unsigned long long CurrentTime);
#endif


