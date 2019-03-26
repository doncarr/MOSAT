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


/*************************************************************************

simriver.h

Header file for sim driver.

**************************************************************************/

/*  plastic */

#define TANK_HI_LEVEL_DI 9
#define TANK_LO_LEVEL_DI 8

#define COLD_WATER_VALVE_DO 8
#define SUCCTION_VALVE_DO 9

#define F1_UP_CH 0
#define F1_DN_CH 1
#define F2_UP_CH 2
#define F2_DN_CH 3

#define MOTOR_CH 6

#define TANK_HI_LEVEL_CM (27.0)
#define TANK_LO_LEVEL_CM (24.0)

#define START_TEMPERATURE (55.0)
#define START_LEVEL (25.0)

#define FILTER1_CHANGE_TIME (60.0)
#define FILTER2_CHANGE_TIME (120.0)

#define COLD_WATER_FILL_RATE (20.0/60)
#define PUMP_DRAIN_RATE (-35.0/60)

#define VALVE_ON_TEMPERATURE_RATE (-10.0/60)
#define VALVE_OFF_TEMPERATURE_RATE (20.0/60)

#define TANK_TEMPERATURE_CH1 0
#define TANK_TEMPERATURE_CH2 1
#define TANK_LEVEL_CH 2

#define F1_WAITING_TO_CHANGE 0
#define F1_MOTOR_ON 1 
#define F1_OPEN_VALVE 2 
#define F1_CLOSE_VALVE 3
#define F1_MOTOR_OFF 4

struct f1_stateinfo_t {
    double enter_time; 
    double wait_time;
    bool last_direction; 
    int state;
};

struct f2_stateinfo_t {
    double enter_time; 
    double wait_time;
    bool last_direction; 
    int state;
};


class simplasticdrv_t : public io_driver_t
{
public:
  FILE *fp;
  FILE *fpout;
  bool di_data[64];
  bool do_data[64];
  double ao_data[32];
  double ai_data[32];
  react_drv_base_t *react;

  bool filling;
  double start;
  double fill_rate;
  double plastic_rate;
  double last_time;
  double the_level;

  double tank_temperature; 
  double tank_level;
  f1_stateinfo_t f1_stateinfo;
  f2_stateinfo_t f2_stateinfo;
  bool motor_on;

  simplasticdrv_t(react_drv_base_t *r);
  void read(void);
  bool get_di(int channel);
  double get_ai(int channel);
  void send_do(int channel, bool val);
  void send_ao(int channel, double val);
  void close(void);
  void state_machine_filter_1(double now);
  void state_machine_filter_2(double now); 

};

