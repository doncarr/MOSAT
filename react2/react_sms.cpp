/************************************************************************
 * This software is part of React, a control engine
 * Copyright (C) 2008 Donald Wayne Carr, Carlos René Diaz 
 *
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "db_point.h"
#include "db.h"
#include "sms.h"
#include "ap_config.h"

static sms_base *sms_object = NULL;

ac_point_t *ac_unit[12];

/***************************************************************/

static char acmsg[120];
const char * process_message(const char * message)
{
  acmsg[0] = '\0';
  for(int i=0; i < 6; i++)
  {
    char status[50];
    if (ac_unit[i] == NULL) continue; 
    ac_unit[i]->get_status(status, sizeof(status));
    safe_strcat(acmsg, status, sizeof(acmsg)); 
  }
  return acmsg;
}

/***************************************************************/

void db_point_t::send_sms_group(const char *msg, const char *group)
{
  printf("Message for %s: %s\n", group, msg);
  if(sms_object == NULL)
  {
        printf("***********Error, SMS not enabled\n");
  	return;
  }
  else
  	sms_object->sms_send_group(msg,group);
  
}

/***************************************************************/

void react_t::init_sms(void)
{
	bool use_sms = ap_config.get_bool("SMS_ENABLED", false);

        if (!use_sms) 
	{
		printf("SMS is not Enabled - skipping initialization\n");
		return;
	}
	printf("Initializating SMS . . . \n");
        void *handle;

	const char * sms_so = 
             ap_config.get_config("SMS_SO", "../sms/librtsms.so");

	handle = dlopen("libmysqlclient.so",RTLD_LAZY | RTLD_GLOBAL);
	if(handle== NULL)
	{
		fprintf(stderr,"%s\n",dlerror());
		exit(1);
	}

	handle = dlopen(sms_so,RTLD_LAZY);
	if(handle== NULL)
	{
		fprintf(stderr,"%s\n",dlerror());
		exit(1);
	}
	new_sms_t fn = (new_sms_t) dlsym(handle,"create_new_sms_object");
	
	char *error;
	
	if((error = dlerror()) != NULL)
	{
		fprintf(stderr,"%s\n",error);
		exit(1);
	}
	
	sms_object = (*fn)("./");
	printf("SMS initialization done.\n");

	for (int i=0; i < 6; i++)
        {
            char actag[30];
            snprintf(actag, sizeof(actag), "AC%d", i+1); 
      	    db_point_t *db_point = db->get_db_point(actag);
      	    if ((db_point == NULL) || (db_point->point_type() != AC_POINT))
            {
      	      ac_unit[i] = NULL;
      	      printf("Bad AC point: %s\n", actag);
            }
      	    else
            {
      	      ac_unit[i] = (ac_point_t *) db_point;
            }
        }

}

/***************************************************************/

void react_t::check_sms(void)
{
	if(sms_object == NULL)
		return;
	printf("Checking for SMS messages . . . . \n");
	smsMessage message = sms_object->next_sms();
	if(!message.success())
		return;
	printf("Got a message, processing . . . . \n");
	const char *reply = process_message(message.getMessage());
	printf("Sending reply . . . \n");
	sms_object->sms_send(reply,message.getNumber());
	printf("Done Sending\n");
	
}

/***************************************************************/

