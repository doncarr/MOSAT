
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <netinet/in.h>
#include <string.h>

#include "ppc_logger.h"
#include "train_detect.h"

/***********************************/

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

/***
struct crossing_event_data_t
{
  long seq_num;
  long id;
  long n_pulses;
  time_t start_time;
  time_t end_time;
};
*****/
/***********************************/

void train_detector_t::cev_to_net(crossing_event_data_t *ev, void *buf, int buf_size)
{
  unsigned long tmp;
  char *p = (char *) buf;

  tmp = htonl(ev->seq_num);
  memcpy(p, &tmp, sizeof(tmp));
  p+= sizeof(tmp);
  tmp = htonl(ev->id);
  memcpy(p, &tmp, sizeof(tmp));
  p+= sizeof(tmp);
  tmp = htonl(ev->n_pulses);
  memcpy(p, &tmp, sizeof(tmp));
  p+= sizeof(tmp);
  tmp = htonl(ev->start_time);
  memcpy(p, &tmp, sizeof(tmp));
  p+= sizeof(tmp);
  tmp = htonl(ev->end_time);
  memcpy(p, &tmp, sizeof(tmp));
}

/***********************************/

void train_detector_t::cev_from_net(crossing_event_data_t *ev, void *buf, int buf_size)
{
  unsigned long tmp;
  char *p = (char *) buf;

  memcpy(&tmp, p, sizeof(tmp));
  ev->seq_num = ntohl(tmp);
  p+= sizeof(tmp);
  memcpy(&tmp, p, sizeof(tmp));
  ev->id = ntohl(tmp);
  p+= sizeof(tmp);
  memcpy(&tmp, p, sizeof(tmp));
  ev->n_pulses = ntohl(tmp);
  p+= sizeof(tmp);
  memcpy(&tmp, p, sizeof(tmp));
  ev->start_time = ntohl(tmp);
  p+= sizeof(tmp);
  memcpy(&tmp, p, sizeof(tmp));
  ev->end_time = ntohl(tmp);
}

/***********************************/

train_detector_t::train_detector_t(void)
{
  detect = false;
  detect_time = 0;
  last_time = 0;
  start_count = 0;
  last_count = 0;
  total_counts = 0;
  count_dif = 0;
  disconnected = false;
  disconnect_detect = false;
  evlog = NULL;
}

/***********************************/

void train_detector_t::set_evlog(event_log_base_t *evl)
{
  evlog = evl;
}

/***********************************/

void train_detector_t::set_initial_count(int count)
{
  detect = false;
  detect_time = 0;
  last_time = 0;
  total_counts = 0;
  count_dif = 0;
  last_count = start_count = count;
}

/***********************************/

void train_detector_t::set_id(long an_id)
{
  my_id = an_id;
}

/***********************************/

void train_detector_t::update(int count, bool connect)
{
  count_dif = calc_dif(count, last_count);
  if (count_dif > 100)
  {
    set_initial_count(count);
  }
  total_counts = calc_dif(count, start_count);

  if (!detect)
  {
    if (total_counts > 0)
    {
      last_time = detect_time = time(NULL);
      detect = true;
      total_counts = calc_dif(count, start_count);
      if (evlog != NULL)
      {
        evlog->event_start(my_id, detect_time, total_counts);
      }
      /****
      print_event(my_id, "Start", detect_time);
      evlog_printf("Counts: %d ", total_counts);
      *****/
    }
  }
  else
  {
    if (count_dif > 0)
    {
      last_time = time(NULL);
      if (evlog != NULL)
      {
        evlog->event_continue(my_id, last_time, total_counts);
      }
      // evlog_printf("%d ", total_counts);
    }
    else if (time(NULL) > (last_time + 15))
    {
      if (evlog != NULL)
      {
        evlog->event_end(my_id, time(NULL), detect_time, last_time, total_counts, start_count, count);  
      }
      /*****
      evlog_printf("\n");
      print_event(my_id, "End", time(NULL));
      print_log(my_id, "EVENT", detect_time, last_time,
          total_counts, start_count, count);
      *****/
      start_count = count;
      detect = false;
    }
  }

  last_count = count;

  if (disconnected)
  {
    if (connect)
    {
      if (evlog != NULL)
      {
        evlog->event_reconnect(my_id, time(NULL));
      }
      //print_event(my_id, "*** Connection detected\n", time(NULL));
      disconnected = false;
      disconnect_detect = false;
    }
  }
  else
  {
    if (disconnect_detect)
    {
      if (time(NULL) > (disconnect_time + 60))
      {
        if (evlog != NULL)
        {
          evlog->event_disconnect(my_id, time(NULL));
        }
        //print_event(my_id, "******* Disconnection detected\n", time(NULL));
        disconnected = true;
      }
      else if (connect)
      {
        disconnect_detect = false;
        disconnected = false;
      }
    }
    else
    {
      if (!connect)
      {
        disconnect_detect = true;
        disconnected = false;
        disconnect_time = time(NULL);
      }
    }
  }
}

/***********************************/


