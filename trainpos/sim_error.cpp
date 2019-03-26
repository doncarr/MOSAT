
#include <stdio.h>
#include <stdlib.h>
#include "sim_msg.h"


int main(int argc, char *argv[])
{
  if (argc < 4)
  {
    printf("You must enter type, section, sensor\n");
    exit(0);
  }
  connect_sim_queue();
  sim_msg_t msg;
  msg.type = atol(argv[1]); 
  msg.section = atol(argv[2]); 
  msg.sensor = atol(argv[3]); 
  send_sim_msg(msg);
}
