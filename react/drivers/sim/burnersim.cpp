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


#include <stdio.h>
#include <math.h>

#include "sim.h"

/*******************************************************/

void burnersim_t::set_input_temp(double val)
{
  // Assume degrees centigrade.
  // How about just a fixed delay for this to get to the burner.
  fInputTemp = val;
}

/*******************************************************/

burnersim_t::burnersim_t(double aInputTemp,
		double aFlowRate, double aBurnerSetting)
{
  fLastBurnerSetting = aBurnerSetting;
  fLastInputTemp = aInputTemp;
  fLastFlowRate = aFlowRate;
  fLastOutputTemp = calcSteadyStateTemp(fLastInputTemp,
                                           fLastFlowRate,
                                           fLastBurnerSetting);
  //printf("starting output temp: %lf\n", fLastOutputTemp);
  fInputTemp = aInputTemp;
  fFlowRate = aFlowRate;
  fBurnerSetting = aBurnerSetting;
  fFirst = true;
  fDelay = 0.0;
  fTau = 0.3;
}

/*******************************************************/

void burnersim_t::set_burner(double val)
{
  // Assume kilocalories / second
  // We need to work on the parameters for the delay here.
  // Possibly a fixed delay where nothing happens, then a
  // ramp to the new value.
  fBurnerSetting = val;
}

/*******************************************************/

void burnersim_t::set_flow_rate(double val)
{
  // Assume liters / second
  // Maybe no delay needed?
  fFlowRate = val;
}

/*******************************************************/

double burnersim_t::getValue()
{
  return fCurrentOutputTemp;
}

/*******************************************************/

double burnersim_t::calcSteadyStateTemp(double inputTemp,
                                     double inputFlow,
                                     double burnerSetting)
  {
    // Flow Rate: Liters / sec
    // Input Temp: degrees C
    // Burner Setting in kilocalories / sec

    return  inputTemp + ((10.0 * burnerSetting) / inputFlow);
  }

/*******************************************************/

double burnersim_t::nextValue(double aTime, double newInputTemp,
	       double newFlowRate, double newBurnerSetting)
{
  if (fFirst)
  {
    fLastTime = aTime;
    fFirst = false;
    return fLastOutputTemp;
  }
  if ((newInputTemp != fLastInputTemp) ||
      (newFlowRate != fLastFlowRate) ||
      (newBurnerSetting != fLastBurnerSetting))
  {
    //printf("New setting: %lf, %lf, %lf\n",
//		    newInputTemp, newFlowRate, newBurnerSetting);
  }

  double deltaTime = aTime - fLastTime;
  double steadyStateTemp =
               calcSteadyStateTemp(fLastInputTemp,
                                  fLastFlowRate,
                                  fLastBurnerSetting);
  double deltaTemp = steadyStateTemp - fLastOutputTemp;
  double fraction = 1.0 - exp(-deltaTime/fTau);
  fCurrentOutputTemp = fLastOutputTemp +
      (deltaTemp * fraction);

  fLastBurnerSetting = newBurnerSetting;
  fLastInputTemp = newInputTemp;
  fLastFlowRate = newFlowRate;
  fLastOutputTemp = fCurrentOutputTemp;
  fLastTime = aTime;
  //double x = 0.125;
  //printf("%lf, %lf, %lf, %lf, %lf, %lf\n", deltaTime, fraction,
//		    deltaTemp, fLastBurnerSetting,
//		    steadyStateTemp, fCurrentOutputTemp);
  return fCurrentOutputTemp;
}

/*******************************************************/
