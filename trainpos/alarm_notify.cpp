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

// This class is to prevent algorithms from accessing
// any other functions of the alarm administrator, or calling
// it with the incorrect algorithm number. Algorithms will only
// be given a pointer to the alarm_notify_base_t, which is pure
// virtual and only has two functions:
//       estop(), and set_alarm_state()
// When these functions are called, the algorithm will be
// automatically identified.

alarm_notify_obj_t::alarm_notify_obj_t(int alg_num, alarm_admin_t *anAdmin)
{
  algorithm_number = alg_num;
  admin = anAdmin;
}

void alarm_notify_obj_t::estop(const char *reason)
{
}

void alarm_notify_obj_t::set_alarm_state(alarm_info_t alarm_info)
{
  admin->set_alarm(algorithm_number, alarm_info);
}


