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

#define COLD_VALVE_DO_CH 8
#define DRAIN_VALVE_DO_CH 9

#define LO_LEVEL_CH 8
#define HI_LEVEL_CH 9

#define TEMP1_CH 0
#define TEMP2_CH 1
#define LEVEL_CH 2


#define FILL_TIME (35.0)
#define DRAIN_TIME (20.0)

#define PUMP_ON_TIME (30.0)
#define PUMP_OFF_TIME (35.0)


#define LO_SWITCH_CM (33.0)
#define HI_SWITCH_CM (38.0)

#define SWITCH_DEADBAND (1.0)


class simtinadrv_t : public io_driver_t
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
  double drain_rate;
  double water_temp;

  double actual_fill_rate;

  bool cooling;
  bool draining;

  double last_time;
  double the_level;

  simtinadrv_t(react_drv_base_t *r);
  void read(void);
  bool get_di(int channel);
  double get_ai(int channel);
  void send_do(int channel, bool val);
  void send_ao(int channel, double val);
  void close(void);
};

