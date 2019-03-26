
#ifndef __TRAINPOS_INCLUDE__
#define __TRAINPOS_INCLUDE__

struct train_data_t
{
  int num; // sequential number assigned to trains entering service.
  char train_id[30]; // train id as given in the timetable.
  time_t service_entry_time; // exactly when the train entered service.
  time_t scheduled_service_entry_time; // when the train was scheduled to enter service.  Must implement.
  int line_location; // actual line location in seconds
  unsigned section; // The section which this train in.
               // Actually, "section" could be calculated, but, we avoid a search, and verify the section is correct.
               // Question: When should we say that a train has advanced to the start of the next section?
               //    Should it be:
               // 	1) The estimated time that the train should stop at the station? 
               // 	2) The estimated time when the train should start moving to leave the station?
               // 	3) When the last sensor is crossed in the previous sensor?
               // 	    As of now it is 3)!!
  unsigned next_sensor_to_cross; // The sensor which will be crossed next.
		// The sensorso are numbered 0 to n-1. (n = number of sensors in this section)
		// If the next sensor to cross = n, then the next sensor to cross is in the next section.
};

struct train_calcs_t
{
// This is data that can be calculated, and should not be in the shared structure. 
   int seconds_late; // seconds late for this train?
   int seconds_in_section; // seconds in the current section 
   double fraction_of_section_traveled; // Fraction of the section travelled
   int seconds_to_next_train; // number of seconds to next train in the system
   bool departed; // Has departed the station.
   time_t arival_time;  // time of last station arrival - I think.
   time_t section_entry_time; // or time of last station departure;
   bool unexpected; // This train is unexpected.
   bool switched_direction; // This train arrived at the final station and is coming back.
   bool crossed_last_sensor;
};

#endif

