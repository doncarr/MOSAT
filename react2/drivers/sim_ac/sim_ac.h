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

sim_ac.h

Header file for ac sim driver.

**************************************************************************/


class ac_unit_sim_t
{
private:
  double last_time;
  bool ac_is_on;
  double time_on;
  double time_off;
  double next_transition;
  double outside_temp;
  double steady_state_cold;
  double steady_state_hot;
  double cold_temp;
  double hot_temp;
  double on_tau;
  double off_tau;
  exp_simulator_t sim_hot;
  exp_simulator_t sim_cold;
  void update_on(double t);
  void update_off(double t);
public:
  ac_unit_sim_t(double a_period, double a_fraction_on,
                double a_start_delay, double start_time);
  bool update(double t, double *the_cold_temp, double *the_hot_temp);
  void set_steady_state_hot(double temp){steady_state_hot = temp;};
  void set_steady_state_cold(double temp){steady_state_cold = temp;};
  void set_on_tau(double tau){on_tau = tau;};
  void set_off_tau(double tau){off_tau = tau;};
  void set_outside_temp(double temp){outside_temp = temp;};
};

class sim_ac_drv_t : public io_driver_t
{
public:
  FILE *fp;
  FILE *fpout;
  double last_time;
  bool di_data[64];
  bool do_data[64];
  double ao_data[32];
  double ai_data[32];
  ac_unit_sim_t *ac_sim[12];
  react_drv_base_t *react;

  sim_ac_drv_t(react_drv_base_t *r);
  void read(void);
  bool get_di(int channel);
  double get_ai(int channel);
  void send_do(int channel, bool val);
  void send_ao(int channel, double val);
  void close(void);
};

