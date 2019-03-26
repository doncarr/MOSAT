

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <errno.h>
#include <signal.h>

#include "rt_serial.h" 

static void my_sighandler(int sig)
{
}


int main(int argc, char *argv[])
{
   char dev_name[100];
   char msg[100];
   int device[50];
   int baudrate;

   signal(SIGIO , my_sighandler);

   rt_verbose = 1;

   if (argc < 3)
   {
     printf("Error, must be at least 2 arguments\n");
     printf("find_serial_ports <baude rate> <port 1> <port 2> . . . <port N>\n");
   }
   
   printf("Using a baudrate of %s for all ports\n", argv[1]);
   baudrate = atoi(argv[1]);

   for (int i=2; i < argc; i++)
   {
     snprintf(dev_name, sizeof(dev_name), "/dev/%s", argv[i]);
     printf("Opening device (%s). . . \n", dev_name);
     device[i] = rt_open_serial(dev_name, baudrate, 0.5);
     if (device[i] < 0)
     {
       printf("device could not be opened: %s\n", dev_name);
     }
     else
     {
       printf("Device opened: %s, fd = %d\n", dev_name, device[i]);
     }
   }
   
   while (true)
   {
     for (int i=2; i < argc; i++)
     {
       if (device[i] < 0) continue;
     
       snprintf(dev_name, sizeof(dev_name), "/dev/%s", argv[i]);
       snprintf(msg, sizeof(msg), "this is %s\n\r", dev_name);
       //snprintf(msg, sizeof(msg), "$R3");
       printf("Writing to %s: %s", dev_name, msg);
       //printf("%s:%d\n", __FILE__, __LINE__);
       int bytes = write(device[i],msg,strlen(msg)); 
       //printf("%s:%d\n", __FILE__, __LINE__);
       if (bytes != int(strlen(msg)))
       {
         printf("Can not write to device, return value = %d\n", bytes);
       }
       //printf("%s:%d\n", __FILE__, __LINE__);
     }
     printf("\n");

     for (int i=2; i < argc; i++)
     {
       //printf("%s:%d\n", __FILE__, __LINE__);
       char buf[500]; 
       if (device[i] < 0) continue;
       //printf("%s:%d\n", __FILE__, __LINE__);
       printf("Waiting for msg from %s: ", argv[i]); 
       //printf("%s:%d\n", __FILE__, __LINE__);
       int bytes = read(device[i],buf,sizeof(buf)); 
       //printf("%s:%d\n", __FILE__, __LINE__);
       if (bytes > 0)
       {
         buf[bytes] = '\0';
         printf("%s\n", buf);
       }
       else
       {
         printf("Timeout\n");
       }
       //printf("%s:%d\n", __FILE__, __LINE__);
       
     }
    struct timespec remain;
    struct timespec request;
    request.tv_nsec = 0; 
    request.tv_sec = 2; 
    //printf("waiting (%ld, %ld)\n", request.tv_sec, request.tv_nsec);

    int retval = nanosleep(&request, &remain);
    while (retval == -1)
    {
      if (errno == EINTR)
      {
        request = remain; // Ok, a signal screwed things up, sleep again.
        printf("waiting for rest(%ld, %ld)\n", request.tv_sec, request.tv_nsec);
        retval = nanosleep(&request, &remain);
      }
      else if (errno == EINVAL)
      {
        printf("Error in utimer code %s:%d\n", __FILE__, __LINE__);
        break;
      }
      else if (errno == EFAULT)
      {
        printf("System error in utimer (EFAULT) %s:%d\n", __FILE__, __LINE__);
        break;
      }
      else
      {
        break; // Should never happen, but break to avoid infinite loop; 
      }

    }
    
   }

   for (int i=2; i < argc; i++)
   {
     if (device[i] < 0) continue;
     rt_close_serial(device[i]);
   }

   return 0;
}



