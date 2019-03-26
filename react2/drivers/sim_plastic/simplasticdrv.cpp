/************************************************************************
This software is part of React, a control engine
Copyright (C) 2005,2006 Donald Wayne Carr 

This program is free software; you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation; either version 2 of the License, or 
(at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You should have received a copy of the GNU General Public License along 
with this program; if not, write to the Free Software Foundation, Inc., 
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
***********************************************************************/


/************************************************************************

Iodriver.c

Contains code for input/output drivers.

*************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> // for open(...)
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>

#include "reactpoint.h"
#include "db_point.h"
#include "db.h"
#include "iodriver.h"

#include "simplasticdrv.h"

/***********************************************************************

class simplasticdrv_factory_t : public io_driver_factory_t
{
private:
  react_drv_base_t *drvb;
  
public:
  simplasticdrv_factory_t(react_drv_base_t *r) {drvb = r;};
  io_driver_t *new_driver(const char *config_data, char *err, int esz)
     {return new simplasticdrv_t(drvb);};
  const char *long_description(void) {return "PumpSimulator";};
  const char *short_discription(void) {return "plasticSim";}; 
  const char *abbreviation(void) {return "PSIM";};
  ~simplasticdrv_factory_t(void) {};
};


***********************************************************************/
/*

extern "C" io_driver_factory_t *new_simplasticdrv_factory_1_0(react_drv_base_t *r, const char *option)
{
  return new simplasticdrv_factory_t(r);
}

***********************************************************************/

/***
static double add_rand_noise(double val, double full_scale, double fraction_noise)
{
  double rand_mult = (((double) random() / (double) RAND_MAX) 
        * 2.0 * fraction_noise) - fraction_noise;
  return val + (full_scale * rand_mult);
}
***/

/**********************************************************************/

extern "C" io_driver_t *new_simplasticdrv(react_drv_base_t *r, const char *option)
{
  return new simplasticdrv_t(r);
}

/***********************************************************************/


simplasticdrv_t::simplasticdrv_t(react_drv_base_t *r)
{
  //printf("Read: ");
  printf("Creating sim plastic driver . . . .\n");
  driver_name = "simplasticdrv";
  driver = 1;
  card = 1;
  base_addr = 0;
  fp = NULL;
  react = r;

  memset(di_data, '\0', sizeof(di_data));
  memset(do_data, '\0', sizeof(do_data));
  memset(ai_data, '\0', sizeof(ai_data));
  memset(ao_data, '\0', sizeof(ao_data));

  start = react->get_time();

  /* plastic */
    f1_stateinfo.enter_time = start; 
    f1_stateinfo.wait_time = 60;
    f1_stateinfo.last_direction = false;  // UP
    f1_stateinfo.state = F1_WAITING_TO_CHANGE;
    f2_stateinfo.enter_time = start; 
    f2_stateinfo.wait_time = 120;
    f2_stateinfo.last_direction = false;  // UP
    f2_stateinfo.state = F1_WAITING_TO_CHANGE;
    motor_on = false;

  di_data[TANK_HI_LEVEL_DI] = false;
  di_data[TANK_HI_LEVEL_DI] = true;

  tank_temperature = START_TEMPERATURE;
  tank_level = START_LEVEL;

  ai_data[TANK_TEMPERATURE_CH1] = tank_temperature;
  ai_data[TANK_TEMPERATURE_CH2] = tank_temperature;
  /* plastic */

  // Start the level at the middle.
}

/***********************************************************************/

void simplasticdrv_t::send_ao(int aChannel, double aVal)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return;
  }
  //printf("New AO[%d] = %f\n", aChannel, aVal);
  ao_data[aChannel] = aVal;
  ai_data[aChannel] = aVal;
}

/***********************************************************************/

void simplasticdrv_t::send_do(int aChannel, bool aVal)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return;
  }
  //di_data[aChannel] = aVal;
  do_data[aChannel] = aVal;
}


/***********************************************************************/

double simplasticdrv_t::get_ai(int aChannel)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return 0.0;
  }
  return ai_data[aChannel];
}

/***********************************************************************/

void simplasticdrv_t::close(void)
{
}

/***********************************************************************/

/***********************************************************************/
void simplasticdrv_t::state_machine_filter_1(double now)
{
  double time_in_state = now - f1_stateinfo.enter_time; 
  switch (f1_stateinfo.state) {
	case F1_WAITING_TO_CHANGE : if ( time_in_state > f1_stateinfo.wait_time && !motor_on ) 
                                       { 
								 motor_on = true;
                                         f1_stateinfo.wait_time = 2;  
                                         f1_stateinfo.state = F1_MOTOR_ON;
								 f1_stateinfo.enter_time = now;
                                       }  
                                     break;
 	case F1_MOTOR_ON: if ( time_in_state > f1_stateinfo.wait_time) 
                             { 
                                f1_stateinfo.wait_time = 5;  
                                f1_stateinfo.state = F1_OPEN_VALVE;
			        f1_stateinfo.enter_time = now;
                                if ( f1_stateinfo.last_direction )
				    di_data[F1_UP_CH] = true;
                                else
                                    di_data[F1_DN_CH] = true;
                                /* Open the Up or the down valve */
                             }  
                             break;
 	case F1_OPEN_VALVE: if ( time_in_state > f1_stateinfo.wait_time) 
                             { 
                                f1_stateinfo.wait_time = 2;  
                                f1_stateinfo.state = F1_CLOSE_VALVE;
				f1_stateinfo.enter_time = now;
                                if ( f1_stateinfo.last_direction )
				    di_data[F1_UP_CH] = false;
                                else
                                    di_data[F1_DN_CH] = false;
                                f1_stateinfo.last_direction = !f1_stateinfo.last_direction;
                                 /* cLOSE  the Up or Down valve */
                             }  
                             break;
 	case F1_CLOSE_VALVE: if ( time_in_state > f1_stateinfo.wait_time) 
                             { 
						  motor_on = false;
                                f1_stateinfo.wait_time = 60;   // TODO: Random time
			                                       // 60 +/- 10 
                                f1_stateinfo.state = F1_WAITING_TO_CHANGE;
						  f1_stateinfo.enter_time = now;
                             }  
                             break;
                                  
 
	}
}

/***********************************************************************/

/***********************************************************************/
void simplasticdrv_t::state_machine_filter_2(double now)
{
  double time_in_state = now - f2_stateinfo.enter_time; 
  switch (f2_stateinfo.state) {
	case F1_WAITING_TO_CHANGE : if ( time_in_state > f2_stateinfo.wait_time && !motor_on ) 
                                       { 
								 motor_on = true;
                                         f2_stateinfo.wait_time = 2;  
                                         f2_stateinfo.state = F1_MOTOR_ON;
								 f2_stateinfo.enter_time = now;
                                       }  
                                     break;
 	case F1_MOTOR_ON: if ( time_in_state > f2_stateinfo.wait_time) 
                             { 
                                f2_stateinfo.wait_time = 5;  
                                f2_stateinfo.state = F1_OPEN_VALVE;
			        f2_stateinfo.enter_time = now;
                                if ( f2_stateinfo.last_direction )
				    di_data[F2_UP_CH] = true;
                                else
                                    di_data[F2_DN_CH] = true;
                                /* Open the Up or the down valve */
                             }  
                             break;
 	case F1_OPEN_VALVE: if ( time_in_state > f2_stateinfo.wait_time) 
                             { 
                                f2_stateinfo.wait_time = 2;  
                                f2_stateinfo.state = F1_CLOSE_VALVE;
				f2_stateinfo.enter_time = now;
                                if ( f2_stateinfo.last_direction )
				    di_data[F2_UP_CH] = false;
                                else
                                    di_data[F2_DN_CH] = false;
                                f2_stateinfo.last_direction = !f2_stateinfo.last_direction;
                                 /* cLOSE  the Up or Down valve */
                             }  
                             break;
 	case F1_CLOSE_VALVE: if ( time_in_state > f2_stateinfo.wait_time) 
                             { 
						  motor_on = false;
                                f2_stateinfo.wait_time = 120;   // TODO: Random time
			                                       // 60 +/- 10 
                                f2_stateinfo.state = F1_WAITING_TO_CHANGE;
						  f2_stateinfo.enter_time = now;
                             }  
                             break;
                                  
 
	}
}
bool simplasticdrv_t::get_di(int aChannel)
{
  if ((aChannel < 0) || (aChannel > 31))
  {
    return false;
  }
  return di_data[aChannel];
}

/***********************************************************************/

void simplasticdrv_t::read(void)
{
  double now = react->get_time();
  double elapsed_time = now - last_time;
  last_time = now;
  if (elapsed_time < 0.05) return;

  /* plastic */
  double fill_rate = 0 ;
  double temperature_rate = 0 ;
  
  if (do_data[COLD_WATER_VALVE_DO])
  {
    fill_rate += COLD_WATER_FILL_RATE;
    temperature_rate = VALVE_ON_TEMPERATURE_RATE;
  }
  else 
    temperature_rate = VALVE_OFF_TEMPERATURE_RATE;

  if (do_data[SUCCTION_VALVE_DO])
    fill_rate += PUMP_DRAIN_RATE;
 
  tank_level += fill_rate * elapsed_time;
  tank_temperature += temperature_rate * elapsed_time;

  di_data[TANK_HI_LEVEL_DI] = tank_level > TANK_HI_LEVEL_CM;
  di_data[TANK_LO_LEVEL_DI] = tank_level > TANK_LO_LEVEL_CM;
  

  ai_data[TANK_TEMPERATURE_CH1] = tank_temperature;
  ai_data[TANK_TEMPERATURE_CH2] = tank_temperature;
  ai_data[TANK_LEVEL_CH] = tank_level;

  state_machine_filter_1(now);
  state_machine_filter_2(now);
  di_data[MOTOR_CH] = motor_on;
    
  /* plastic */

}

/***********************************************************************/
