/************************************************************************
This is software to monitor and/or control trains.
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
#include <stdlib.h>

#include "msg_queue.h"
#include "tcontrol.h"

/********************************************/

alarm_admin_t::alarm_admin_t(int a_n_sects, int a_n_algs, int a_n_types)
{
  n_sections = a_n_sects;
  n_algs = a_n_algs;
  n_types = a_n_types;
  this->reset();
}

/********************************************/

void alarm_admin_t::set_alarm(int alg, alarm_info_t info)
{
  // First check to make sure all of the data is valid.
  if ((alg < 0) || (alg >= n_algs))
  {
    printf("invalid algorithm number\n");
    exit(0);
  }
  if ((info.section < 0) || (info.section >= n_sections))
  {
    printf("invalid section number\n");
    exit(0);
  }
  if ((info.type < 0) || (info.type >= n_types))
  {
    printf("invalid type number\n");
    exit(0);
  }
  if ((info.severity < 0) || (info.severity >= MAX_ALARM_SEVERITY))
  {
    printf("invalid severity number\n");
    exit(0);
  }
  severities[info.section][info.type].severity_votes[alg] = info.severity;
}

/********************************************/

void alarm_admin_t::reset(void)
{
  for (int i=0; i < n_sections; i++)
  {
    for (int j=0; j < n_types; j++)
    {
      severities[i][j].severity = 0;
      for (int k=0; k < n_algs; k++)
      {
        severities[i][j].severity_votes[k] = 0;
      }
    }
  }
}

/********************************************/

void alarm_admin_t::ack_alarm(int section, int type, int new_severity)
{
  // Remove from un-ack list.
  // Add to operator action history.
}

/********************************************/

void alarm_admin_t::set_new_severity(int section, int type, int new_severity, int old_severity)
{
  if (new_severity == 0)
  {
    // Add to history
    // Remove from active
  }
  else
  {
    if (old_severity == 0)
    {
      // Add to history
      // Add to active
      // Add to un-acknoleged
    }
    else if (new_severity > old_severity)
    {
      // Change un-acknowledged, the severity is greater, they need to acknowledge that
      // Change active
      // Add to history
    }
    else
    {
      // Change active
      // No Change to acknowledged, they alread acknowledged a higher severity.
      // Add to history
    }
  }
}

/********************************************/

void alarm_admin_t::evaluate_alarms(void)
{
  int max;
  for (int i=0; i < n_sections; i++)
  {
    for (int j=0; j < n_types; j++)
    {
      int test_sev = severities[i][j].severity_votes[0];
      for (int k=1; k < n_algs; k++)
      {
        int sk = severities[i][j].severity_votes[k];
        if (test_sev != sk)
        {
          // This should never happen if all work correctly!
          printf("Severities don't agree: %d %d\n", i, j);
        }
        if (test_sev < sk)
        {
          test_sev = sk;
        }
      }
      if (test_sev != severities[i][j].severity)
      {
        set_new_severity(i, j, test_sev, severities[i][j].severity);
        severities[i][j].severity = test_sev;
      }
      max = 0;
      for (int k=0; k < n_algs; k++)
      {
        int sev = severities[i][j].severity_votes[k];
        if (sev > max)
        {
          max = sev;
        }
      }
      int old = severities[i][j].severity;
      if (max != old)
      {
        printf("New alarm state: %d %d %d\n", i, j, max);
        severities[i][j].severity = max;
      }
    }
  }
}

/********************************************/

void alarm_admin_t::estop(int alg, const char *reason)
{
}

/********************************************/

