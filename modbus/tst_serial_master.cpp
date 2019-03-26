
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "rt_serial.h" 

int main(int argc, char *argv[])
{
   const char *dev_name;
   int device;
   //int j;

   /**
   Mbm_trame trame;
   int result;
   int data_in[256];
   int data_out[256];
   ***/

   if (argc > 1)
   {
     dev_name = argv[1];
   }
   else
   {
     dev_name = "/dev/ttyUSB0";
   }

   rt_verbose = 1;
   /* open device */
   printf("Opening device (%s). . . \n", dev_name);
   //device = rt_open_serial("/dev/ttyUSB0",9600, 2.0);
   //device = rt_open_serial("/dev/ttyUSB0",38400, 2.0);
   //device = rt_open_serial("/dev/ttyUSB0",57600, 2.0);
   device = rt_open_serial(dev_name,57600, 2.0);

   printf("Device opened: %d \n", device);

   /* print debugging informations */


   /*** Write ***/
   //char write_data;
   //write_data = 'x';

   while (1)
   {
     char buf[500];
     printf("Enter string: ");
     fgets(buf, sizeof(buf), stdin);
     printf("Writing . . .\n ");
     buf[strlen(buf)-1] = 0x0D;
     int nw = write(device,buf,strlen(buf)); // Write everything but the character return.
     printf("Write done - %d chars\n", nw);
     for (int i=0; i < nw; i++) printf("%02X ", int(buf[i]));
     printf("\n");
     char read_data[100];
     int total_read = 0;
     while (1)
     {
       printf("Reading . . . \n");
       int n =  rt_read_serial(device, read_data + total_read,sizeof(read_data) - 1 - total_read);
       if (n > 0)
       {
         total_read += n;
         read_data[total_read] = '\0';
         printf("Data read: %s, %d chars\n", read_data, n);
       }
       else
       {
         printf("TIMEOUT: %d chars\n", total_read);
         total_read = 0;
         break;
       }
     }
     /***
     unsigned short analog_values[8];
     char *p = read_data;
     for (j=0; j < 8; j++)
     {
       analog_values[j] = atol(p);
       while ((*p != '\n') && (*p != '\0') && (*p != '\r')) p++; 
       if (*p != '\0') p++;
     }
     for (j=0; j < 8; j++)
     {
       printf("val[%d] = %hu\n", j, analog_values[j]);
     }
     ****/

   }   
   /*** Write ***/

   
   /*** Read ***/
   char read_data;

   printf("Reading . . . \n");
   read(device,&read_data,1);
   printf("Data read: %c, %X\n", read_data, read_data);
  /** done read **/

   rt_close_serial(device);

   return 0;
}

