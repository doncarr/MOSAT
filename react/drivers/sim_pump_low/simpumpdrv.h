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

#define LO_LEVEL_CH 0
#define HI_LEVEL_CH 1
#define HI_HI_LEVEL_CH 2

#define P1V_DO_CH 4
#define P2V_DO_CH 5

#define P1_DO_CH 1
#define P2_DO_CH 2
#define P3_DO_CH 3 

#define P1V_DI_CH 6
#define P2V_DI_CH 7

#define P1_DI_CH 8
#define P2_DI_CH 9
#define P3_DI_CH 10 

#define MIXER_DI_CH 5 

#define P1_AI_CH 5
#define P2_AI_CH 4
#define P3_AI_CH 3

#define LEVEL_CH 2

#define PUMP_ON_TIME (30.0)
#define PUMP_OFF_TIME (35.0)
#define LO_SWITCH_CM (100.0)
#define HI_SWITCH_CM (275.0)
#define HI_HI_SWITCH_CM (450.0)

#define SWITCH_DEADBAND (12.0)

class simpumpdrv_t : public io_driver_t
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
  double pump_rate;
  double last_time;
  double the_level;

  simpumpdrv_t(react_drv_base_t *r);
  void read(void);
  bool get_di(int channel);
  double get_ai(int channel);
  void send_do(int channel, bool val);
  void send_ao(int channel, double val);
  void close(void);
};

