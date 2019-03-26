
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <netinet/in.h>
#include <string.h>

#include "ppc_logger.h"
#include "train_detect.h"

/***********************************/

long xcalc_dif(long count, long last)
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

/***********************************/

event_detector_t::event_detector_t(void)
{
  evlog = NULL;
  state = EVDT_WAITING;
}

/***********************************/

void event_detector_t::set_evlog(event_log_base_t *evl)
{
  evlog = evl;
}

/***********************************/

void event_detector_t::set_initial_count(int a_departure_count, int a_arrival_count, int a_confirm_count)
{
  state = EVDT_WAITING;
  departure_first = a_departure_count;
  departure_last = a_departure_count;
  arrival_first = a_arrival_count;
  arrival_last = a_arrival_count;
  confirm_first = a_confirm_count;
  confirm_last = a_confirm_count;
  waiting_confirm = false;
  printf("Setting initial counts: %d, %d, %d\n", departure_first, arrival_first, confirm_first);
}

/***********************************/

void event_detector_t::set_ids(long a_departure_id, long a_arrival_id, long a_confirm_id)
{
  state = EVDT_WAITING;
  departure_id = a_departure_id;
  arrival_id = a_arrival_id;
  confirm_id = a_confirm_id;
  printf("Setting ids: %ld, %ld, %ld\n", departure_id, arrival_id, confirm_id);
}

/***********************************/

void event_detector_t::send_event(time_t the_start_time, long the_event_type)
{
  long the_id;
  if (evlog  != NULL)
  {
    if (the_event_type == EVDT_DEPARTURE)
    {
      the_id = 50 + departure_id; 
      printf("DEPART triggered: id: %ld\n", the_id);
    }
    else
    {
      the_id = 60 + arrival_id; 
      printf("ARRIVE triggered: id: %ld\n", the_id);
    }
    evlog->event_end(the_id, time(NULL), the_start_time, time(NULL), -1, -1, -1);  
  }
}

/***********************************/

void event_detector_t::check_confirm(void)
{
  //printf("--------- Confirm total: %d, first %d, last %d\n", confirm_total,
  //              confirm_first, confirm_last);
  if (confirm_total != (confirm_now - confirm_first))
  {
    printf("********* WRONG total: %d, first %d, last %d\n", confirm_total,
                confirm_first, confirm_last);
  }
  if (confirm_total > 4)
  {
    printf("Event Confirmed: counts: %d\n", confirm_total);
    send_event(saved_start_time, saved_event_type); 
    waiting_confirm = false;
  }
  else if ((now - start_confirm_time) > 70)
  {
    printf("------------------- Timeout, no confirm after 70 sec: %d\n", 
         confirm_total);
    waiting_confirm = false;
  }
}

/***********************************/

void event_detector_t::wait_all(void)
{
  if ((now - last_ad_change) > 15)
  {
    printf("Back to waiting: Final counts: %d %d\n", arrival_total, departure_total);
    state = EVDT_WAITING;
    departure_first = departure_last;
    arrival_first = arrival_last;
  }
}

/***********************************/

void event_detector_t::pending(void)
{
  if ((departure_dif > 0) || (arrival_dif > 0)) 
  {
    last_ad_change = now;
  }
  if  ((departure_total > 4) && (arrival_total > 4)) 
  {
    if ((confirm_id == -1) || (event_type == EVDT_ARRIVAL))
    {
      printf("Send event NOW no confirm %d %d\n", 
                         arrival_total, departure_total);
      send_event(start_time, event_type);
      state = EVDT_WAIT_ALL;
    }
    else
    {
      printf("event will be send on confirm: %d %d\n", 
                         arrival_total, departure_total);
      state = EVDT_WAIT_ALL;
      saved_start_time = start_time;
      saved_event_type = event_type;
      start_confirm_time = time(NULL);
      confirm_first = confirm_now; 
      confirm_last = confirm_now; 
      confirm_total = 0;
      confirm_dif = 0;
      waiting_confirm = true;
    }
  }
  else if ((now - last_ad_change) > 15)
  {
    printf("Timeout error: %d %d\n", arrival_total, departure_total);
    state = EVDT_WAITING;
  }
}

/***********************************/

void event_detector_t::waiting(void)
{
  if (departure_total > 0)
  {
    printf("Departure start: %d %d\n", 
                         arrival_total, departure_total);
    event_type = EVDT_DEPARTURE;
    state = EVDT_PENDING;
    start_time = time(NULL);
  }
  else if (arrival_total > 0)
  {
    printf("Arrival start: %d %d\n", 
                         arrival_total, departure_total);
    event_type = EVDT_ARRIVAL;
    state = EVDT_PENDING;
    start_time = time(NULL);
  }
}

/***********************************/

void event_detector_t::update(int departure_count, int arrival_count, int confirm_count)
{
  departure_total = xcalc_dif(departure_count, departure_first);
  arrival_total = xcalc_dif(arrival_count, arrival_first);
  confirm_total = xcalc_dif(confirm_count, confirm_first);
  if (confirm_total != (confirm_count - confirm_first))
  {
    printf("********* WRONG total: %d, first %d, last %d\n", confirm_total,
                confirm_first, confirm_last);
  }

  departure_dif = xcalc_dif(departure_count, departure_last);
  arrival_dif = xcalc_dif(arrival_count, arrival_last);
  confirm_dif = xcalc_dif(confirm_count, confirm_last);

  confirm_now = confirm_count;

  now = time(NULL);
  if ((departure_dif > 0) || (arrival_dif > 0)) 
  {
    last_ad_change = now;
  }

  if ((departure_dif > 100) || (arrival_dif > 100))
  {
    departure_first = departure_count;
    arrival_first = arrival_count;
  }
  //total_counts = xcalc_dif(count, start_count);
 
  if (state == EVDT_WAITING)
  {
    this->waiting();
  }
  else if (state == EVDT_PENDING)
  {
    this->pending();
  }
  else
  {
    this->wait_all();
  }

  if (waiting_confirm)
  {
    this->check_confirm();
  }
  departure_last = departure_count;
  arrival_last = arrival_count;
  confirm_last = confirm_count;
}

/***********************************/


