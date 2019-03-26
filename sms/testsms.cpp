#include<sys/time.h>
#include <time.h>
#include "sms.h"
#include "sms_implement.h"


int main(int argc, char *arvg[])
{
	time_t seconds1, seconds2;
	
	sms mensajero("localhost", "root", "root", "gammu","./");

	mensajero.sms_prueba();
	
	smsMessage mensaje;
	
	struct timeval tv1,tv2;
//struct timezone tz;
//struct tm *tm;
//gettimeofday(&tv, NULL);
//tm=localtime(&tv.tv_sec);
//printf(" %d:%02d:%02d %d \n", tm->tm_hour, tm->tm_min,tm->tm_sec, tv.tv_usec);
	
	//seconds1 = time (NULL);
	gettimeofday(&tv1, NULL);

	
	mensajero.sms_send_member("Probando","Rene","Normal");
	
	//seconds2 = time (NULL);
	gettimeofday(&tv2, NULL);
	
	
	//printf("Time before send: %ld\nTime after send:%ld\n",seconds1,seconds2);
	printf("Diferencia:%ld\n",(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec));
	
	//seconds1 = time (NULL);
	gettimeofday(&tv1, NULL);
	mensaje = mensajero.next_sms();
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
