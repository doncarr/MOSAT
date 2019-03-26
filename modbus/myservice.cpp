#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <stdlib.h>

#include <pthread.h>

#include "mtype.h"
#include "modio.h"
#include "modsim.h"
#include "trace.h"

//#define MYPORT 3490    /* the port users will be connecting to */
#define MYPORT 502    /* Modbus port */

#define BACKLOG 10     /* how many pending connections queue will hold */

/********************************/

void *service_client(void *data)
{
  int sock = *((int *) data);
  printf("Wow, a thread to service requests!!!\n");
  mod_tcpip_t mts(sock);
  ModSerialSim modsim(&mts);
  while (true)
  {
    if (-1 == modsim.process_request())
    {
      break;
    }
  }
  printf("Client disocnnected, exiting . . . \n");
  return NULL;
}

/********************************/

int main(int argc, char *argv[])
{
  int sockfd, new_fd;  /* listen on sock_fd, new connection on new_fd */
  int ret;
  struct sockaddr_in my_addr;    /* my address information */
  struct sockaddr_in their_addr; /* connector's address information */
  socklen_t sin_size;

  sockfd = socket(AF_INET, SOCK_STREAM, 0); /* do some error checking! */
  if (sockfd < 0)
  {
    perror("error creating socket");
    exit(0);
  }

  my_addr.sin_family = AF_INET;         /* host byte order */
  my_addr.sin_port = htons(MYPORT);     /* short, network byte order */
  my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
  memset(&(my_addr.sin_zero), 0, 8);        /* zero the rest of the struct */

  /* don't forget your error checking for these calls: */
  ret = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));
  if (ret < 0)
  {
    perror("error on bind");
    exit(0);
  }

  ret = listen(sockfd, BACKLOG);
  if (ret < 0)
  {
    perror("error on listen");
    exit(0);
  }

  sin_size = sizeof(struct sockaddr_in);
  react_trace.set_level(0);
  //int child_n;
  for (int i=1; true; i++)
  {
    printf("I am the dad!\n");
    printf("modsim: waiting for clients to connect on %d\n", MYPORT);
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd < 0)
    {
      perror("error on accept");
      exit(0);
    }
    //child_n = i;
    int retval;
    pthread_t thr;
    retval = pthread_create(&thr, NULL, service_client, &new_fd);
    if (retval != 0)
    {
      perror("can't create thread");
      exit(0);
    }
    retval = pthread_detach(thr);
    if (retval != 0)
    {
      perror("can't detach thread");
      exit(0);
    }
 
    //pid_t pid = fork();
    //if (pid == 0) break;
    
  }

  printf("Creating a modsim to service requests\n");
  mod_tcpip_t mts(new_fd);
  ModSerialSim modsim(&mts);
  while (true)
  {
    if (-1 == modsim.process_request())
    {
      break;
    }
  }
  printf("Client disocnnected, exiting . . . \n");
}
