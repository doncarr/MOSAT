

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "rt_serial.h"

#include "simple_ascii.h"

int main(int argc, char *argv[])
{
   int device;
   int n_reads = 0;
   int n_do_writes = 0; 
   int n_ao_writes = 0;
   //int n_serial = 0;
   char dos[50];
   int ao_vals[50];

   snprintf(dos, sizeof(dos), "%s", "000000000000\n\r");

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
   printf("Opening device %s . . . \n", dev_name);
   device = rt_open_serial(dev_name,9600,0.0);
   printf("Device opened: %d \n", device);


   char data[200];
   int ret_val;

  while(true)
  {
    ret_val = rt_read_serial(device, data, 2); 
    data[2] = '\0';
    if (ret_val < 0)
    {
      printf("Error reading serial port\n");
    }
    else
    {
      printf("Got this: %s\n", data);
    }

    //if ((data[0] == '$') && (data[1] == 'R'))
    if (0 == strncmp(data, "$R", 2))
    {
      n_reads++;
      write(device, "04\n\r", 4);
      printf("04\n\r");
      for (int i=0; i < 4; i++)
      {
        write(device, "+12.345678\n\r", 12);
        printf("+12.345678\n\r");
      }
      write(device, "101010101001\n\r", 14);
      printf("101010101001\n\r");
      write(device, dos, 14);
      printf("%s", dos);
      printf("%d reads so far\n", n_reads);
    }
    //snprintf(buf, sizeof(buf), "$WDO%02d%c\n\r", channel, val ? '1' : '0' )
    else if (0 == strncmp(data, "$W", 2))
    {
      ret_val = rt_read_serial(device, data+2, 2); 
      data[4] = '\0';
     
      if (0 == strncmp(data, "$WDO", 4))
      {
        n_do_writes++;
        printf("Reading Command bytes\n");
        ret_val = rt_read_serial(device, data, 3); 
        data[3] = '\0';
        printf("Write command: %s\n", data);
        if (ret_val < 0)
        {
          printf("Error reading serial port\n");
          write(device, "01\n\r", 4);
        }
        if (!isdigit(data[0]) || !isdigit(data[1]) || 
          ((data[2] != '0') && (data[2] != '1')) )
        {
          printf("bad message\n");
          write(device, "05\n\r", 4);
        }

        else
        {
          int ch = data[1] - '0'; 
          ch += (data[0] - '0') * 10;
          bool val = (data[2] == '1');
          printf("%s channel %d\n", val ? "Turning On" : "Turning OFF", ch);
          if ((ch >= 0) && (ch < 12))
          {
            dos[ch] = val ? '1' : '0';
          }
          //write(device, "OK\n\r", 4);
        }
        printf("%d DO writes so far\n", n_do_writes);
      }
      if (0 == strncmp(data, "$WAO", 4))
      {
        //  snprintf(buf, sizeof(buf), "$WAO%02d%05d\n\r", channel, val);
   
        n_ao_writes++;
        printf("Reading Command bytes\n");
        ret_val = rt_read_serial(device, data, 7); 
        data[7] = '\0';
        printf("Write AO command: %s\n", data);
        if (ret_val < 0)
        {
          printf("Error reading serial port\n");
          write(device, "01\n\r", 4);
        }
        else
        {
          int ch = data[1] - '0'; 
          ch += (data[0] - '0') * 10;
          ao_vals[ch] = atoi(data+2);
          printf("sending %d, channel %d\n", ao_vals[ch], ch);
          //write(device, "OK\n\r", 4);
        }
        printf("%d AO writes so far\n", n_ao_writes);
      }
    }
    /*** Not implemented here
    else if (data[0] == 'S')
    {
      n_serial++;
      write(device, "04\n\r", 4);
      write(device, "10.4AEC29CDBAAB\n\r", 17);
      write(device, "10.4AEC29CDBAAB\n\r", 17);
      write(device, "10.4AEC29CDBAAB\n\r", 17);
      write(device, "10.4AEC29CDBAAB\n\r", 17);
      printf("%d serial number requests so far\n", n_serial);
    }
    ****/
    else
    {
      printf("You FOOL, that is NOT correct: %c\n", data[0]);
      write(device, "02\n\r", 4);
    }
  }
}
