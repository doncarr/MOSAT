
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "rt_serial.h" 

char *put_header(char *buf, char *mac_address, char n_retries, char msg_len)
{
  char byte;

  printf("dest mac = %02x %02x %02x\n", 
        mac_address[0], mac_address[1], mac_address[2]); 
  printf("n retries = %d\n", n_retries);
  printf("msg length = %d\n", msg_len);

  byte = 0x81;
  memcpy(buf, &byte, 1);
  buf += 1;

  byte = msg_len;
  memcpy(buf, &byte, 1);
  buf += 1;

  byte = 0x00; // Aerocomm Use, set to 0
  memcpy(buf, &byte, 1);
  buf += 1;

  byte = n_retries;
  memcpy(buf, &byte, 1);
  buf += 1;

  memcpy(buf, mac_address, 3);
  buf += 3;

  return buf; // return a point to where the rest of the message goes.
  
}

char mac_1[] = {0x00, 0x01, 0x02};
char mac_2[] = {0x45, 0x46, 0x47};

void send_msg(int fd, int n, char *msg)
{
  char buf[300];
  char *p;

  printf("Sending single message: %s\n", msg);

  char *the_mac = (n==0) ? mac_1 : mac_2;

  p = put_header(buf, the_mac, 5, strlen(msg));
  memcpy(p, msg, strlen(msg));
  write(fd, buf, 13 + strlen(msg));
}

int main(int argc, char *argv[])
{
  printf("------------------ line %d, msg = %s\n", __LINE__, argv[3]);
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

  if (argc > 3)
  {
    printf("msg = %s\n", argv[3]);
    
    send_msg(device, argv[2][0] - '0', argv[3]);
    exit(0);
  }
   /* print debugging informations */


   /*** Write ***/
   //char write_data;
   //write_data = 'x';

   while (1)
   {
     char buf[500];
     int slave_id;
     printf("Enter a slave id: ");
     fgets(buf, sizeof(buf), stdin);
     slave_id = atol(buf); 
     printf("Enter string to send to slave: ");
     fgets(buf, sizeof(buf), stdin);
     printf("Will send '%s' to slave %d\n", buf, slave_id);
     printf("Writing . . .\n ");

     write(device,buf,strlen(buf)-1); // Write everything but the character return.
     printf("Write done - %d chars\n", int(strlen(buf)-1));
     char read_data[100];
     int total_read = 0;
     while (1)
     {
       printf("Reading . . . \n");

       // Hector, change here to first read the header, then the rest.

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

     /****
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
     ***/

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

