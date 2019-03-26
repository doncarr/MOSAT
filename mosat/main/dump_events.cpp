

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "ppc_logger.h"

/***

struct crossing_event_data_t
{
  long seq_num;
  long id;
  long n_pulses;
  time_t start_time;
  time_t end_time;
};


struct ppcevlog_t
{
  long seq_num; // Next sequence number to use.
  long index; // Where to write the next value.
  long total_valid; // How many are valid so far.
  crossing_event_data_t events[500];
};
*******/

int main(int argc, char *argv[])
{
  ppcevlog_t evlog;
  FILE *fp = fopen("/nvram/siteur/elogfile", "r");
  fread(&evlog, sizeof(evlog), 1, fp);
  printf("Header: %ld, %ld, %ld\n", 
    evlog.header.seq_num, evlog.header.index, evlog.header.total_valid);
  int emax = sizeof(evlog.events) / sizeof(evlog.events[0]);
  long loc = evlog.header.index - evlog.header.total_valid;
  if (loc < 0)
  {
    loc += emax;
  }
  for (int i=0; i < evlog.header.total_valid; i++)
  {
    crossing_event_data_t ev = evlog.events[loc];
    char buf[100];
    struct tm mytm;
    time_t stime = ev.start_time;
    time_t etime = ev.end_time;
#ifdef PPC
  stime -= (5 * 60 * 60);
  etime -= (5 * 60 * 60);
#endif
    localtime_r(&stime, &mytm);
    strftime(buf, sizeof(buf), "%F\t%T", &mytm);
    printf("%d\t%ld\t%s\t%s\t", i+1, ev.id, get_ev_string(ev.id), buf);
    localtime_r(&etime, &mytm);
    strftime(buf, sizeof(buf), "%T", &mytm);
    printf("%s\t%ld\t%ld\n", buf, ev.n_pulses, ev.seq_num);
    loc++;
    if (loc >= emax) loc = 0;
  }

}



