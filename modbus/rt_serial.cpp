
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <math.h>

#include <sys/types.h>

#include "rt_serial.h"


static bool serial_no_timeout = false;
static struct timeval serial_tv = {5,0};

struct termios saved_tty_parameters; /* saved serial port setting */
int rt_verbose;

int rt_read_serial(int fd, void *data, int sz)
{
  int total_read = 0;
  char *dp = (char *) data;

  while (total_read < sz)
  {
    int n = read(fd, dp + total_read, sz - total_read); 
    if (n == 0)
    {
      perror("read");
      return total_read;
    } 
    else if (n < 0)
    {
      perror("read");
      return n;
    }
    total_read += n;
  }
  return total_read;
}

/************************************************************************************/

void rt_close_serial(int device)
{
  if (tcsetattr (device,TCSANOW,&saved_tty_parameters) < 0)
    perror("Can't restore terminal parameters ");
  close(device);
}

/*****************************************************************/

int rt_set_timeout(int fd, float timeout) 
{
  struct termios rt_tio; 

  if (tcgetattr (fd,&rt_tio) < 0)
  {
    perror("Can't get terminal parameters ");
    return -1 ;
  }

  if (timeout == 0)
  {
    rt_tio.c_cc[VMIN]=1;
    rt_tio.c_cc[VTIME]=0;
    serial_no_timeout = true;
  }
  else
  {
    rt_tio.c_cc[VMIN]=1;
    rt_tio.c_cc[VTIME]=0;
    rt_tio.c_cc[VMIN]=0;
    rt_tio.c_cc[VTIME]= (int)(timeout * 10.0);
    serial_tv.tv_sec = int(timeout);
    serial_tv.tv_usec = int((timeout - trunc(timeout)) * 1000000.0);
  }

  if (tcsetattr(fd,TCSANOW,&rt_tio) < 0)
  {
    perror("Can't set terminal parameters ");
    return -1 ;
  }
  return 0;
}

/*****************************************************************/

int rt_flush_serial(int fd) 
{
  return tcflush(fd,TCIOFLUSH);
}

/*****************************************************************/

int rt_open_serial(const char *port, int baud_rate, float timeout) 
{
  /* The following 3 could be parameters. */
  int tmp_parity = 0; // no parity 
  int tmp_data_bits = 8; // 8 data bits
  int tmp_stop_bits = 1; // 1 stop bit
  /***********/

  int fd;
  struct termios rt_tio; 

  /* open the serial port */
  fd = open(port,O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY) ;
  //fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
  if(fd < 0)
  {
    char buf[60];
    snprintf(buf, sizeof(buf), "Opening %s", port);
    perror(buf) ;
    //exit(-1) ;
    return fd;
  }
  if (0 == isatty(fd))
  {
    printf("This is NOT a tty device, will not set baud rate\n");
    return fd; // It is not a tty, you won't be able to set the baud rate
  }
  else
  {
    printf("This is a tty device\n");
  }

  /* save the old serial port settings */
  if (tcgetattr (fd,&saved_tty_parameters) < 0)
  {
    perror("Can't get terminal parameters ");
    //exit(-1) ;
    return -1 ;
  }
  /* set the fields */
  bzero(&rt_tio,sizeof(&rt_tio));

  switch (baud_rate)
  {
    case 0:
      rt_tio.c_cflag = B0;
      break;
    case 50:
      rt_tio.c_cflag = B50;
      break;
    case 75:
      rt_tio.c_cflag = B75;
      break;
    case 110:
      rt_tio.c_cflag = B110;
      break;
    case 134:
      rt_tio.c_cflag = B134;
      break;
    case 150:
      rt_tio.c_cflag = B150;
      break;
    case 200:
      rt_tio.c_cflag = B200;
      break;
    case 300:
      rt_tio.c_cflag = B300;
      break;
    case 600:
      rt_tio.c_cflag = B600;
      break;
    case 1200:
      rt_tio.c_cflag = B1200;
      break;
    case 1800:
      rt_tio.c_cflag = B1800;
      break;
    case 2400:
      rt_tio.c_cflag = B2400;
      break;
    case 4800:
      rt_tio.c_cflag = B4800;
      break;
    case 9600:
      rt_tio.c_cflag = B9600;
      break;
    case 19200:
      rt_tio.c_cflag = B19200;
      break;
    case 38400:
      rt_tio.c_cflag = B38400;
      break;
    case 57600:
      rt_tio.c_cflag = B57600;
      break;
    case 115200:
      rt_tio.c_cflag = B115200;
      break;
    case 230400:
      rt_tio.c_cflag = B230400;
      break;
    default:
      rt_tio.c_cflag = B9600;
  }
  switch (tmp_data_bits)
  {
    case 7:
      rt_tio.c_cflag = rt_tio.c_cflag | CS7;
      break;
    case 8:
    default:
      rt_tio.c_cflag = rt_tio.c_cflag | CS8;
      break;
  }
  switch (tmp_parity)
  {
    case 1:
      rt_tio.c_cflag = rt_tio.c_cflag | PARENB;
      rt_tio.c_iflag = ICRNL;
      break;
   case -1:
      rt_tio.c_cflag = rt_tio.c_cflag | PARENB | PARODD;
      rt_tio.c_iflag = ICRNL;
      break;
   case 0:
   default:
      rt_tio.c_iflag = IGNPAR | ICRNL;
      break;
  }

  if (tmp_stop_bits == 2)
     rt_tio.c_cflag = rt_tio.c_cflag | CSTOPB;
     
  rt_tio.c_cflag = rt_tio.c_cflag | CLOCAL | CREAD;
  rt_tio.c_oflag = 0;
  rt_tio.c_lflag = 0; /*ICANON;*/

  if (timeout == 0)
  {
    rt_tio.c_cc[VMIN]=1;
    rt_tio.c_cc[VTIME]=0;
    serial_no_timeout = true;
  }
  else
  {
    rt_tio.c_cc[VMIN]=1;
    rt_tio.c_cc[VTIME]=0;
    rt_tio.c_cc[VMIN]=0;
    rt_tio.c_cc[VTIME]= (int)(timeout * 10.0);
    serial_tv.tv_sec = int(timeout);
    serial_tv.tv_usec = int((timeout - trunc(timeout)) * 1000000.0);
  }

  /* flush the serial port */
  tcflush(fd, TCIOFLUSH);

  if (-1 == fcntl(fd, F_SETFL, FASYNC))
  {
    perror("fcntl");
    //exit(-1) ;
    return -1 ;
  }

  /* write the settings for the serial port */
  if (tcsetattr(fd,TCSANOW,&rt_tio) <0)
  {
    perror("Can't set terminal parameters ");
    //exit(-1) ;
    return -1 ;
  }
  
  /* flush the serial port */
  tcflush(fd,TCIOFLUSH);
  
   if (rt_verbose)
   {
      printf("serial port ok:\n");
      printf("device name   %s\n", port);
      printf("speed         %d\n", baud_rate);
      printf("data bits     %d\n", tmp_data_bits);
      printf("stop bits     %d\n", tmp_stop_bits);
      printf("parity        %d\n", tmp_parity);
      printf("timeout:      %f\n", timeout);
   }
   return fd ;
}

/**************************************************************************/


