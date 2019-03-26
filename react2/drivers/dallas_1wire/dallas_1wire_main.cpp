

#include <stdio.h>

#include "rt_serial.h"

#include "dallas_1wire.h"


int main(int argc, char *argv[])
{
   int device;

   char dev_name[60];
   if (argc > 1)
   {
     snprintf(dev_name, sizeof(dev_name), "/dev/%s", argv[1]);
   }
   else
   {
     snprintf(dev_name, sizeof(dev_name), "/dev/ttyUSB0");
   }

   rt_verbose = 1;
   printf("Opening device . . . \n");
   device = rt_open_serial(dev_name,9600,0.0);
   printf("Device opened: %d \n", device);

   write(device, "x", 1);

   char data[200];
   int ret_val = rt_read_serial(device, data, sizeof(data)); 
   if (ret_val < 0)
   {
     printf("Error reading serial port\n");
   }
   else
   {
     printf("Got this: %s\n", data);
   }
   
   /* Toño y René, Aqui son ejemplos de abrir puerta serial */ 

}
