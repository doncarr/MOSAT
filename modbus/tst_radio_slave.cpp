
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "rt_serial.h"

int main(int argc, char *argv[])
{
   int device;
   int my_id;
   //int i, j;

   char dev_name[60];
   if (argc > 1)
   {
     snprintf(dev_name, sizeof(dev_name), "/dev/%s", argv[1]);
   }
   else
   {
     snprintf(dev_name, sizeof(dev_name), "/dev/ttyUSB0");
     //snprintf(dev_name, sizeof(dev_name), "/dev/ttyS0");
   }

   if (argc > 2)
   {
     my_id = atol(argv[2]);
   }
   else
   {
     my_id = getpid();
   }
   

   rt_verbose = 1;
   /* open device */
   printf("Opening device . . . \n");
   device = rt_open_serial(dev_name,57600,0.0);
   printf("Device opened: %d \n", device);
   if (device == -1)
   {
     exit(1);
   }

   /* print debugging informations */


   
   /*** Read ***/
   // write(device, "x", 1);
   char read_data[100];
   char write_data[100];
   //int total_read = 0;
   int count = 0;
   printf("Using device: %s, Id: %d\n", dev_name, my_id);
   while (1)
   {
     printf("Reading . . . \n");
     int n =  rt_read_serial(device,read_data,sizeof(read_data) - 1);
     printf("Read returned!!\n");
     count++;
     if (n > 0)
     { 
       read_data[n] = '\0';
       printf("Data read: %s, %d chars\n", read_data, n);
       snprintf(write_data, sizeof(write_data), "Slave %d, got: %s\n", 
                  my_id, read_data);
       write(device, "Slave %d, got: %s\n", 12);
     }
     else
     {
       perror("Error Reading Serial");
     } 
   }
  /** done read **/
   rt_close_serial(device);
   return 0;
}

