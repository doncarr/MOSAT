
#ifndef __MSG_QUEUE_INC__
#define __MSG_QUEUE_INC__
#include <time.h> //we only have to do this because maco is a bitch

struct crossing_event_t
{
  time_t time_stamp;
  unsigned section;
  bool departure;
  unsigned sensor;
  bool update;
  bool skip;
};

struct crossing_queue_event_t
{
  long mtype;
  crossing_event_t data;
};

int create_message_queue(void);
int connect_message_queue(void);
void msgq_send_departure(time_t now, int section);
void msgq_send_arrival(time_t now, int section);
void msgq_send_update(time_t now);


#endif
