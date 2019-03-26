
#include <stdio.h>
#include <time.h>

#define rt_CBTC_MIN_SEPERATION_DISTANCE (400)
#define rt_CBTC_MAX_TRAINS (100)

struct rt_CBTC_train_t
{
  int train_id;
  time_t last_communication_time;
  double last_speed;
  double last_location;
  bool in_service;
};

enum rt_CBTC_message_type_t // Messages to and from the communications manager
{
  rt_CBTC_ENTER_REQUEST_MSG,
  rt_CBTC_EXIT_NOTIFY_MSG,
  rt_CBTC_STATUS_MSG,
  rt_CBTC_MOVEMENT_REQUEST,
  rt_CBTC_COMM_FAILED,

  rt_CBTC_ENTER_OK,
  rt_CBTC_ENTER_PROHIBIT,
  rt_CBTC_EXIT_ACKNOWLEDGE,
  rt_CBTC_MOVEMENT_DENIED,
  rt_CBTC_MOVEMENT_ALLOWED,
};

struct rt_CBTC_message_t 
{
  rt_CBTC_message_type_t type;
  int train_id_t;
  double speed;
  double location;
};

static rt_CBTC_train_t trains[rt_CBTC_MAX_TRAINS];
static int n_trains = 0;
static int last_train_id = -1;
static int first_train_id = -1;
static double end_of_line = 0.0;

/***************************************************************/

bool check_invariants(void)
{
}

bool movement_authorized(int train_id, double d1, double d2)
{
  if (train_id == last_train_id)
  {
    return (d2 < end_of_line);
  }
  else
  {
  }
}

/***************************************************************/

void process_CBTC_message(rt_CBTC_message_t msg)
{
  switch (msg.type)
  {
    case rt_CBTC_STATUS_MSG:
      break;
    case rt_CBTC_ENTER_REQUEST_MSG:
      break;
    case rt_CBTC_EXIT_NOTIFY_MSG:
      break;
    case rt_CBTC_COMM_FAILED:
      break;
    default:
      break;
  }
}

/***************************************************************/

void wait_messages(void)
{
}

/***************************************************************/

int main(int argc, char *argv[])
{
  for (int i=0; i < rt_CBTC_MAX_TRAINS; i++)
  {
    trains[i].train_id = -1;
    trains[i].last_communication_time = 0;
    trains[i].last_speed = 0.0;
    trains[i].last_location = 0.0;
    trains[i].in_service = false;
  }
  wait_messages();
}

/***************************************************************/
