

#include <stdio.h>

#include <ctype.h>

#include "rt_serial.h"

#include "simple_ascii.h"

int main(int argc, char *argv[])
{
   int device;
   int n_reads = 0;
   int n_writes = 0;
   int n_serial = 0;
   char dos[20];

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
    ret_val = rt_read_serial(device, data, 1); 
    data[1] = '\0';
    if (ret_val < 0)
    {
      printf("Error reading serial port\n");
    }
    else
    {
      printf("Got this: %s\n", data);
    }

    if (data[0] == 'R')
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
    else if (data[0] == 'W')
    {
      n_writes++;
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
        write(device, "OK\n\r", 4);
      }
      printf("%d writes so far\n", n_reads);
    }
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
    else
    {
      printf("You FOOL, that is NOT correct: %c\n", data[0]);
      write(device, "02\n\r", 4);
    }
  }
}
