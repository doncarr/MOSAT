/***************************************************************************
Copyright (c) 2002,2003,2004 Donald Wayne Carr 

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS IN THE SOFTWARE.
*************************************************************************/



/***********************************************************************
Ok this is the program for each RTU. Each RTU will communicate with the
central system AND with each near neighbor. Most RTUs will actually run two
copies of this program, since most stations will have trains arriving in
both directions. Each RTU will be connected to a stoplight (red/green),
automatic brake. exit senosr, and proximity sensor. The output for the
stop light and the brake would typically be the same.
************************************************************************/

#include <stdio.h>
#include <time.h>

struct message_t
{
  short source;
  short type;
};

// This is the object to abstract all of the commands and communications.
class control_object_t
{
public:
  int read_counter(void){return 0;};
  int reset_counter(void){return 0;};
  bool read_prox_sensor(void){return false;};
  void set_stoplight_and_brake(bool is_on){};
  void send_to_next_neighbor(short mtype){};
  void send_to_last_neighbor(short mtype){};
  void send_to_central(short mtype){};
  void alarm_detected(short type, const char *msg){};
  message_t *get_message(void){return NULL;};
};

#define TRAIN_CROSSED_SENSOR (1)
#define TURN_LIGHT_RED (2)
#define TURN_LIGHT_GREEN (3)
#define HAVE_TRAIN_AT_STATION (4)
#define NO_TRAIN_AT_STATION (5)
#define STOP (6)
#define START (7)
#define READY_TO_RUN (8)
#define ALIVE (9)
#define LIVING (10)

#define WHEEL_COUNT (8)

class station_controller_t
{
private:
  bool stopped;
  bool starting;
  bool recieved_from_next;
  bool recieved_from_last;
  bool recieved_from_central;
  bool allow_green;
  bool central_wants_green;
  bool is_green;
  bool have_train_in_next_segment;
  bool have_train_in_last_segment;
  bool have_train_at_station;
  bool count_detected;
  time_t count_detect_time;
  time_t green_time;
  time_t start_time;
  control_object_t *control;
  void reset(void);
  void evaluate_stoplight(void);
  void check_counter(void);
  void check_green_time(void);
  void check_ready_to_start(void);
  void check_startup_progress(void);
  void check_internal_state(void);
public:
  station_controller_t(control_object_t *a_ctrl);
  void run(void);
  void stop(void);
  void process_central_message(short type);
  void process_next_neightbor_message(short type);
  void process_last_neighbor_message(short type);
};

/************************************************************************/

void station_controller_t::check_internal_state(void)
{
  bool all_ok = true; // Ok, first assume all ok, and start checking.
  if (all_ok)
  {
    return;
  }
  if (!stopped && starting)
  {
    all_ok = false;
  }
  else if (!stopped && (recieved_from_next || recieved_from_last || recieved_from_central))
  {
    all_ok = false;
  }
  control->alarm_detected(0, "Invalid internal state");
  starting = false;
  stopped = true;
  recieved_from_next = false;
  recieved_from_last = false;
  recieved_from_central = false;
  central_wants_green = false;
  allow_green = false;
  control->set_stoplight_and_brake(true);
}

/************************************************************************/

void station_controller_t::check_startup_progress(void)
{
  // This procedure is just to check the startup progress. Once we
  // recieve a startup message from one of our neighbors or central
  // control, we should recieve all of the messages very soon. It is
  // important to understand that the messages could be recieved in
  // ANY order, and all code must assume this.
  time_t now = time(NULL);
  if ((now - start_time) > 15)
  {
    control->alarm_detected(0, "Startup faild, not all messages recieved");
    starting = false;
    stopped = true;
    recieved_from_next = false;
    recieved_from_last = false;
    recieved_from_central = false;
    central_wants_green = false;
    allow_green = false;
  }
}

/************************************************************************/

void station_controller_t::check_ready_to_start(void)
{
  // Really, this procedure is just to check if we got startup messages
  // from the central control, and each near neighbor.
  if (!starting)
  {
    starting = true;
    start_time = time(NULL);
  }
  else if ( recieved_from_next && recieved_from_last && recieved_from_central)
  {
    // Ok, we got messages from all three, we know where trains are in the
    // last and next section. We can now run.
    recieved_from_next = false;
    recieved_from_last = false;
    recieved_from_central = false;
    starting = false;
    stopped = false;
    control->reset_counter();
    control->send_to_central(READY_TO_RUN);
  }
}

/************************************************************************/

void station_controller_t::check_green_time(void)
{
  /*
     Ok, here, we do not allow the light to be green more than
     30 seconds, or we assume something bad happened. This is also
     added saftey, since our goal is to keep the stop lights
     in the safe state (red), except when there is a train crossing.
   */
  time_t now = time(NULL);
  if (is_green && ((now - green_time) > 30))
  {
    control->alarm_detected(0, "Train did not leave station");
    control->set_stoplight_and_brake(true);
    is_green = false;
  }
}

/************************************************************************/

void station_controller_t::check_counter(void)
{
  /*
     This procedure checks the counter to see if a train crosssed.
     Once we start getting pulses, we should get all of the pulses
     very soon.
   */
  time_t now = time(NULL);
  int counts = control->read_counter();
  if (counts > 0)
  {
    if (counts >= WHEEL_COUNT) // Ok, a train left the station.
    {
      if (!have_train_in_last_segment)
      {
        // This should NEVER happen. If there was NOT a train
        // in the last section, there would be no trin to
        // cross. This indicates a serious internal problem.
        control->alarm_detected(0, "Crossing not expected");
      }
      if (have_train_in_next_segment)
      {
        // This should NEVER happen. If there is a train in
        // the next segment, the light should have been red,
        // and the brake on, so it indicates something is
        // terribly wrong.
        control->alarm_detected(0, "Crossing not expected");
      }
      have_train_in_next_segment = true;
      have_train_in_last_segment = false;
      allow_green = false;
      // for safety, we will also turn the light red. RIGHT AWAY.
      control->set_stoplight_and_brake(false);
      is_green = false;
      count_detected = false;
      // Also reset the counter to zero.
      control->reset_counter();
      // We need to tell central control, and our neighbors.
      control->send_to_next_neighbor(TRAIN_CROSSED_SENSOR);
      control->send_to_last_neighbor(TRAIN_CROSSED_SENSOR);
      control->send_to_central(TRAIN_CROSSED_SENSOR);
    }
    else if (count_detected && ((now - count_detect_time) > 30))
    {
      // Something bad happened. Once a train starts crossing,
      // we should get all the pulses. If not, there is something
      // wrong with the hardware, or the train made an emergency
      // stop.
      control->alarm_detected(0, "Detected partial count");
      count_detected = false;
      control->reset_counter();
    }
    else if (!count_detected)
    {
      // Ok, we detected the first counts, should be a train.
      count_detect_time = now;
      count_detected = true;
    }
  }
}

/************************************************************************/

void station_controller_t::run(void)
{
  // Ok, run the controller.
  // 1) If starting up, we will check the progress.
  // 2) If we are stoped, nothing more to do.
  // 3) We will check the counter to see if a train crossed.
  // 4) Make sure the light has not been green too long.
  if (starting)
  {
    check_startup_progress();
    return;
  }
  if (stopped)
  {
    return;
  }
  check_counter();
  check_green_time();
}

/***********************************************************************/

void station_controller_t::process_central_message(short type)
{
  switch (type)
  {
    case TURN_LIGHT_RED:
      central_wants_green = false;
      evaluate_stoplight();
      break;
    case TURN_LIGHT_GREEN:
      central_wants_green = true;
      evaluate_stoplight();
      break;
    case STOP:
      reset();
      break;
    case ALIVE:
      control->send_to_central(LIVING);
      break;
    case START:
      // Ok, we got the message to start up. We need to talk
      // to our neighbors and central to let them know if
      // there is a train in the station or not.
      if (!stopped)
      {
        control->alarm_detected(0, "Message not valid on running system");
      }
      short msg;
      if (control->read_prox_sensor())
      {
        msg = HAVE_TRAIN_AT_STATION;
      }
      else
      {
        msg = NO_TRAIN_AT_STATION;
      }
      recieved_from_central = true;
      have_train_in_last_segment = control->read_prox_sensor();
      control->send_to_central(msg);
      control->send_to_next_neighbor(msg);
      control->send_to_last_neighbor(msg);
      check_ready_to_start();
      break;
    case TRAIN_CROSSED_SENSOR:
    case HAVE_TRAIN_AT_STATION:
    case NO_TRAIN_AT_STATION:
    case READY_TO_RUN:
    default:
      control->alarm_detected(0, "Invalid message recieved");
      break;
  }
}
/***********************************************************************/

void station_controller_t::process_next_neightbor_message(short type)
{
  switch (type)
  {
    case TRAIN_CROSSED_SENSOR:
      // You should NOT recieve this message if there
      // was not a train in the next station.
      if (!have_train_in_next_segment)
      {
        control->alarm_detected(0, "Unexpected message");
      }
      have_train_in_next_segment = false;
      if (have_train_in_last_segment)
      {
        allow_green = true;
      }
      break;
    case HAVE_TRAIN_AT_STATION:
    case NO_TRAIN_AT_STATION:
      // This is only a startup message, you should not
      // recieve it while running.
      if (!stopped)
      {
        control->alarm_detected(0, "Message not valid on running system");
      }
      else
      {
        recieved_from_next = true;
        // Ok, now we know which adjacent segments have trains.
        have_train_in_next_segment = (type == HAVE_TRAIN_AT_STATION);
        // Ok, we assume that all trains are at a station for startup. So,
        // if there is a train in the last section, it is an our station.
        // Remember, there is a train at the last station, in will not be
        // in the previous segment untill it crosses the sensor after the
        // station.
        have_train_in_last_segment = control->read_prox_sensor();
        // Only allow a green if we are expecting a train, and there is no train
        // in the next segment.
        allow_green = !have_train_in_next_segment && have_train_in_last_segment;
        check_ready_to_start();
      }
      break;
    case TURN_LIGHT_RED:
    case TURN_LIGHT_GREEN:
    case STOP:
    case START:
    case READY_TO_RUN:
    default:
      control->alarm_detected(0, "Invalid message recieved");
      break;
  }
}

/***********************************************************************/

void station_controller_t::process_last_neighbor_message(short type)
{
  switch (type)
  {
    case TRAIN_CROSSED_SENSOR:
      if (have_train_in_last_segment)
      {
        // This is potentially a very serious message.
        // We think we had a train in the previous section, and
        // now we recieved a message that another entered.
        // This should never happen.
        control->alarm_detected(0, "Unexpected message");
      }
      have_train_in_last_segment = true;
      if (!have_train_in_next_segment)
      {
        allow_green = true;
      }
      break;
    case HAVE_TRAIN_AT_STATION:
    case NO_TRAIN_AT_STATION:
      // This is only a startup message, you should not
      // recieve it while running.
      if (!stopped)
      {
        control->alarm_detected(0, "Message not valid on running system");
      }
      else
      {
        // Ok, for the message from the last segment, the important thing is
        // that we know the rtu in the last segment is alive. We really do not
        // need to know that there is a train at the last station.
        recieved_from_last = true;
        check_ready_to_start();
      }
      break;
    case TURN_LIGHT_RED:
    case TURN_LIGHT_GREEN:
    case STOP:
    case START:
    case READY_TO_RUN:
    default:
      control->alarm_detected(0, "Invalid message recieved");
      break;
  }
}

/***********************************************************************/

station_controller_t::station_controller_t(control_object_t *a_ctrl)
{
  control = a_ctrl;
  reset();
}

/************************************************************************/

void station_controller_t::evaluate_stoplight(void)
{
  if (allow_green && central_wants_green)
  {
    control->set_stoplight_and_brake(false);
    green_time = time(NULL);
    is_green = true;
  }
  else
  {
    control->set_stoplight_and_brake(true);
  }
}

/************************************************************************/

void station_controller_t::reset(void)
{
  stopped = true;
  starting = false;
  central_wants_green = false;
  allow_green = false;
  have_train_in_next_segment = false;
  have_train_in_last_segment = false;
  have_train_at_station = false;
  count_detected = false;
  recieved_from_next = false;
  recieved_from_last = false;
  recieved_from_central = false;
  control->reset_counter();
  control->set_stoplight_and_brake(true);
}

/************************************************************************/

int main(int argc, char *argv[])
{
  while (true)
  {
  }
}

/************************************************************************/

