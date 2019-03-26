#ifndef __RT_SERIAL_INC__ 
#define __RT_SERIAL_INC__

extern int rt_verbose;	/* print debug informations */

int rt_open_serial(const char *port, int baud , float timeout);	/* open device and configure it */

void rt_close_serial(int device); /* close device and restore old parmeters */

int rt_read_serial(int fd, void *data, int sz); // special routine to wait for all data 

int rt_set_timeout(int fd, float timeout); // set the timeout

int rt_flush_serial(int fd); // flush the serial port

#endif
