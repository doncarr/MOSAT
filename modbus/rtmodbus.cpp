
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "rtcommon.h"
#include "rt_serial.h"
#include "mtype.h"
#include "modio.h"
#include "hprtu.h"
#include "hp2mod.h"
#include "trace.h"

rtmodbus_t *rt_create_modbus_serial(const char *device_name, int baudrate, float timeout, int unit_id)
{
  rt_mod_serial_client_t *msc = new rt_mod_serial_client_t(device_name, baudrate, timeout);
  MODSerial *modc = new MODSerial(msc);
  modc->set_address(unit_id);
  return modc;
}


rtmodbus_t *rt_create_modbus_tcpip(const char *device, int port, int timeout, int unit_id)
{
  // For right now, it only works with TCP/IP, but we need
  // to make it work with serial devices as well.
  printf("Creating a modbus connection: %s\n", device);
  char dstr[100];
  safe_strcpy(dstr, device, sizeof(dstr));

  char *dest_ip = dstr;
  printf("ip = %s, port =%d\n", dest_ip, port);

  int sockfd;
  struct sockaddr_in dest_addr;   /* will hold the destination addr */

  sockfd = socket(AF_INET, SOCK_STREAM, 0); /* do some error checking! */
  if (sockfd < 0)
  {
    react_trace.dperror(6, "Can't create socket");
    exit(0);
  }

  struct timeval tv;
  tv.tv_sec = timeout;
  tv.tv_usec = 0;
  int err;

  err = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  if (err != 0)
  {
    react_trace.dperror(6, "setsockopt");
  }

  err = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
  if (err != 0)
  {
    react_trace.dperror(6, "setsockopt");
  }

  dest_addr.sin_family = AF_INET;        /* host byte order */
  short temp = (short) port; 
  dest_addr.sin_port = htons(temp); /* short, network byte order */
  dest_addr.sin_addr.s_addr = inet_addr(dest_ip);
  printf("Using %s\n", dest_ip);
  memset(&(dest_addr.sin_zero), 0, 8);       /* zero the rest of the struct */

  printf("Connecting on %d\n", port);
  int n = connect(sockfd, (struct sockaddr *)&dest_addr,
                  sizeof(struct sockaddr));
  if (n < 0)
  {
    react_trace.dperror(6, "Can't connect to port");
    exit(0); 
  }

  mod_tcpip_client_t *mtp = new mod_tcpip_client_t(sockfd, &dest_addr);
  MODSerial *modc = new MODSerial(mtp);
  modc->set_address(unit_id);
  return modc;
}


rtmodbus_t *rt_create_modbus(const char *device)
{
  int baudrate = 9600;
  int port = 502;
  float timeout = 1;
  int unit_id = 0;
  char sdev[80];
  int mod_type;
  int debug_level = 0;

  if (0 == strncmp("/dev/", device, 5))
  {
    mod_type = RT_MODBUS_SERIAL;
  }
  else
  {
    mod_type = RT_MODBUS_TCPIP;
  }

  const char *p = device;
  int i;
  for (i=0; (*p != ':') && (*p != '\0'); i++, p++)
  {
    sdev[i] = device[i]; 
  }
  sdev[i] = '\0';
  printf("sdev = %s\n", sdev);
  if (*p != '\0')
  {
    p++;
    if (mod_type == RT_MODBUS_SERIAL)
    {
      baudrate = atoi(p);
      printf("baudrate = %d\n", baudrate);
    }
    else
    {
      port = atoi(p);
      printf("port = %d\n", port);
    }
  }

  for ( ; (*p != ':') && (*p != '\0'); p++)
  {
  }
  if (*p != '\0')
  {
    p++;
    timeout = atof(p);
    printf("timeout = %f\n", timeout);
  }

  for ( ; (*p != ':') && (*p != '\0'); p++)
  {
  }
  if (*p != '\0')
  {
    p++;
    unit_id = atoi(p);
    printf("unit id = %d\n", unit_id);
  }

  for ( ; (*p != ':') && (*p != '\0'); p++)
  {
  }
  if (*p != '\0')
  {
    p++;
    debug_level = atoi(p);
    printf("debug level = %d\n", debug_level);
  }

  rtmodbus_t *modb;
  if (0 == strncmp("/dev/", device, 5))
  {
    modb = rt_create_modbus_serial(sdev, baudrate, timeout, unit_id);
  }
  else
  {
    modb = rt_create_modbus_tcpip(sdev, port, timeout, unit_id);
  }
  modb->set_debug_level(debug_level);
  return modb;
}
