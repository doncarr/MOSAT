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

#include "sim.h"

main(int argc, char *argv[])
{

  burnersim_t bsim(50.0, 10.0, 50.0);

  FILE *fp = fopen("simout.txt", "w");
  double burner_setting = 80.0;
  for (int i=0; i < 1000; i++)
  {
    double time = double(i) / 100.0;
    double val = bsim.nextValue(time, 50.0, 10.0, burner_setting);
    if (i == 500)
    {
      burner_setting = 20.0;
    }
    fprintf(fp, "%lf %lf\n", time, val);
  }
  fclose(fp);
}
