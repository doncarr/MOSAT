
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <time.h>

#include "rtcommon.h"
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
  crossing_event_data_t events[EVMAXEVENTS];
};
*******/

/*********************************************************************/

bool day_changed(time_t t1, time_t t2)
{
  struct tm mytm1;
  struct tm mytm2;
#ifdef PPC
  t1 -= (5 * 60 * 60);
  t2 -= (5 * 60 * 60);
#endif
  localtime_r(&t1, &mytm1);
  localtime_r(&t2, &mytm2);
  return (mytm1.tm_yday != mytm2.tm_yday);
//                      int     tm_yday;        /* day in the year */
}

/*********************************************************************/

ppc_logger_t::ppc_logger_t(const char *fname)
{
  if (!file_exists(fname))
  {
    printf("File does not exist, creating: %s\n", fname);
    fp = fopen(fname, "w+");
    if (fp == NULL)
    {
      perror(fname);
      return;
    }
    data.header.index = 0;
    data.header.seq_num = 0;
    data.header.total_valid = 0;
    fwrite(&data, sizeof(data), 1, fp);
    fclose(fp);
  }
  else
  {
    printf("File exists, will use existing data: %s\n", fname);
  }

  printf("Opening %s\n", fname);
  fp = fopen(fname, "r+");
  if (fp == NULL)
  {
    perror(fname);
    return;
  }
  printf("reading data\n");
  rewind(fp);
  fread(&data, sizeof(data), 1, fp);
  long emax =  EVMAXEVENTS;
  if (data.header.total_valid > emax)
  {
    data.header.total_valid = emax;
  }
  printf("Header: %ld, %ld, %ld\n",
         data.header.index, data.header.seq_num, data.header.total_valid);

  /***
  fd = open(fname, O_RDWR | O_CREAT);
  if (fd == -1)
  {
    perror(fname);
    data = NULL;
    return;
  }
  void *p = mmap(NULL, sizeof(ppcevlog_t), PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if ((p == MAP_FAILED) || (p == NULL))
  {
    printf("map failed\n");
    perror(fname);
    data = NULL;
    return;
  }
  printf("File is mapped: %p\n", p);
  data = (ppcevlog_t *) p; 
  ****/
}

/*********************************************************************/

ppc_logger_t::~ppc_logger_t(void)
{
  if (fp == NULL) return;
  fclose(fp);
}

/*********************************************************************/

long ppc_logger_t::log_event(crossing_event_data_t *ev)
{
  if (day_changed(data.header.last_time, ev->start_time))
  {
    printf("Day Changed, setting sequence number to zero\n");
    data.header.seq_num = 0; // Start each day with a seq number of 0;
  }
  data.header.last_time = ev->start_time;
  printf("Logging event at: %ld, seq: %ld, total: %ld\n", data.header.index, data.header.seq_num, 
             data.header.total_valid);
  ev->seq_num = data.header.seq_num;
  data.events[data.header.index] = *ev;
  //msync(&data->events[data->index], sizeof(crossing_event_data_t), MS_ASYNC);
  long offset = ((long) &data.events[data.header.index]) - (long) &data;
  data.header.index++;
  if (data.header.index >= EVMAXEVENTS)
  {
    data.header.index = 0;
  }
  data.header.seq_num++;
  if (data.header.total_valid < EVMAXEVENTS)
  {
    // Here we have reached the size of the logfile and 
    // are overwriting old entries.
    data.header.total_valid++;
  }
  rewind(fp);	
  fwrite(&data.header, sizeof(data.header), 1, fp);
  fflush(fp);
  fseek(fp, offset, SEEK_SET);
  fwrite(((char *) &data) + offset, sizeof(crossing_event_data_t), 1, fp);
  fflush(fp);
  return ev->seq_num;
  //msync(&data->seq_num, 3 * sizeof(long), MS_ASYNC);
}

/*********************************************************************/

int ppc_logger_t::get_event(long seq_num, crossing_event_data_t *ev)
{
  if ((seq_num < 0) || (seq_num >= data.header.total_valid))
  {
    return -1;
  }
  *ev = data.events[seq_num];
  return 0;
}

/*********************************************************************/

void ppc_logger_t::erase_log(void)
{
  data.header.index = 0;
  data.header.seq_num = 0;
  data.header.total_valid = 0;
  rewind(fp);
  fwrite(&data.header, sizeof(data.header), 1, fp);
  fflush(fp);
  //msync(&data->seq_num, 3 * sizeof(long), MS_ASYNC);
}

/*********************************************************************/

int ppc_logger_t::find_oldest_event(time_t time)
{
  printf("Searching for oldest, time %ld\n", time);
  bool first = true;
  long index = -1;
  long min = -1; 
  for (int i=0; i < EVMAXEVENTS; i++)
  {
    if (!day_changed(time, data.events[i].start_time))
    {
      printf("oldest, seq_num = %ld\n", data.events[i].seq_num);
      if (first)
      {
        first = false;
        index = i;
        min = data.events[i].seq_num; 
      }
      else if (data.events[i].seq_num < min) 
      {
        index = i;
        min = data.events[i].seq_num; 
      }
    }
  }
  printf("Oldest = %ld, at %ld\n", min, index);
  return index;
}

/*********************************************************************/

int ppc_logger_t::find_event(time_t time, long seq_num)
{
  if (seq_num < 0) return -1;
  printf("find_event, looking for %ld\n", seq_num);
  for (int i=0; i < EVMAXEVENTS; i++)
  {
    if (!day_changed(time, data.events[i].start_time) && 
       (data.events[i].seq_num == seq_num)) 
    {
      printf("find_event, found at %d\n", i);
      return i;
    }
  }
  return -1;
}

/*********************************************************************/
int ppc_logger_t::first_event(time_t time, long seq_num, crossing_event_data_t *ev)
{
  search_time = time;
  search_seq_num = seq_num;
  int i = find_event(search_time, search_seq_num);
  if (i == -1)
  {
    i = find_oldest_event(search_time);
    if (i == -1)
    {
      search_index = -1; 
      return -1;
    }
  }
  {
    search_index = (i + 1) % EVMAXEVENTS;
    /***
    if (search_index == data.header.index) 
    {
      search_index = -1; 
      return -1;
    }
    ***/
    if ((search_index != data.header.index) && 
        (!day_changed(search_time, data.events[search_index].start_time)))
    {
      *ev = data.events[search_index];
      search_index = (search_index + 1) % EVMAXEVENTS;
      return 0;
    }
  }
  search_index = -1;
  return -1;
}

/*********************************************************************/

int ppc_logger_t::next_event(crossing_event_data_t *ev)
{
  if (search_index == -1) return -1;
  if ((search_index != data.header.index) && 
        (!day_changed(search_time, data.events[search_index].start_time)))
  {
    *ev = data.events[search_index];
    search_index = (search_index + 1) % EVMAXEVENTS;
    return 0;
  }
  return -1;
}

/*********************************************************************/
