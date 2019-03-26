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



#ifndef __SIM_INC__
#define __SIM_INC__

#include <stdio.h>
#include <math.h>

class burnersim_t
{
private:
  double fInputTemp;
  double fFlowRate;
  double fBurnerSetting;
  double fCurrentOutputTemp;
  double fLastOutputTemp;
  double fLastBurnerSetting;
  double fLastTempBeforeBurner;
  double fLastInputTemp;
  double fLastFlowRate;
  bool fFirst;
  double fLastTime;
  double fDelay;
  double fTau;
  double fEfectiveBurnerSetting;
  double *fBurnerHistory;
  int current_history;
public:
  burnersim_t(double aInputTemp, double aFlowRate, double aBurnerSetting);
  void set_input_temp(double val);
  void set_burner(double val);
  void set_flow_rate(double val);
  double calcSteadyStateTemp(double inputTemp,
                                     double inputFlow,
                                     double burnerSetting);
  double getValue();
  double nextValue(double aTime, double newInputTemp,
		  double newInputFlow, double newBurnerSetting);
};

#endif
