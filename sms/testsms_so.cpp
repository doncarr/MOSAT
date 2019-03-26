
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
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/time.h>
#include <time.h>
#include <dlfcn.h>
#include "sms.h"

int main(int argc, char *arvg[])
{
	time_t seconds1, seconds2;

        printf("Loading ./librtsms.so\n");
        void *handle = dlopen("./librtsms.so",RTLD_LAZY);
        if(handle== NULL)
        {
                fprintf(stderr,"%s\n",dlerror());
                exit(1);
        }
        printf("Looking up factory function\n");
        new_sms_t fn = (new_sms_t) dlsym(handle,"create_new_sms_object");

        char *error;

        if((error = dlerror()) != NULL)
        {
                fprintf(stderr,"%s\n",error);
                exit(1);
        }

        sms_base *mensajero; 
        printf("Calling the factory function\n");
        mensajero = (*fn)("/home/thaumiel/svn/trunk/sms/");

	//sms mensajero("localhost", "root", "root", "gammu","./");

	mensajero->sms_prueba();
	
	smsMessage mensaje;
	
	struct timeval tv1,tv2;
//struct timezone tz;
//struct tm *tm;
//gettimeofday(&tv, NULL);
//tm=localtime(&tv.tv_sec);
//printf(" %d:%02d:%02d %d \n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
	
	//seconds1 = time (NULL);
	gettimeofday(&tv1, NULL);

	
	mensajero->sms_send_member("Probando","Rene","Normal");
	
	//seconds2 = time (NULL);
	gettimeofday(&tv2, NULL);
	
	
	//printf("Time before send: %ld\nTime after send:%ld\n",seconds1,seconds2);
	printf("Diferencia:%ld\n",(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec));
	
	//seconds1 = time (NULL);
	gettimeofday(&tv1, NULL);
	mensaje = mensajero->next_sms();
	if(mensaje.success())
	
		printf("Se recibio el siguiente mensaje: %s\n",mensaje.getMessage());
	else
		printf("No se recibio mensaje");
	//seconds2 = time (NULL);
	gettimeofday(&tv2, NULL);
	printf("Diferencia:%ld\n",(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec));
	//printf("Time before receive: %ld\nTime after receive:%ld\n",seconds1,seconds2);
	
	//printf("Mensaje: %s\nNumero: %s\nNombre: %s\nGrupo: %s\n",mensaje.getMessage(),mensaje.getNumber(),mensaje.getName(),mensaje.getGroup());
	
	return 0;
}

/*
formato de mensajes status.1
AC1.get_cold();
*/
