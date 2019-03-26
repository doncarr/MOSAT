/************************************************************************
This is software to monitor and/or control trains.
Copyright (C) 2005,2006 Donald Wayne Carr 

This program is free software; you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation; either version 2 of the License, or 
(at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You should have received a copy of the GNU General Public License along 
with this program; if not, write to the Free Software Foundation, Inc., 
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>

# include <sys/types.h>
# include <sys/ipc.h>
#include <sys/msg.h>

#include "sim_msg.h"

static int sim_qid;

/**********************************************************/

int create_sim_queue(void)
{
  key_t mkey = ftok("./", 5);
  int myqid = msgget(mkey, IPC_CREAT | 0700);
  if (myqid == -1)
  {
    perror("Could not create message queue");
    exit(0);
  }
  printf("I got a message queue, id = %d\n", myqid);
  sim_qid = myqid;
  return myqid;
}

/**********************************************************/

int connect_sim_queue(void)
{
  key_t mkey = ftok("./", 5);
  int myqid = msgget(mkey, 0700);
  if (myqid == -1)
  {
    perror("Could not connect to message queue");
    exit(0);
  }
  printf("Found a message queue, id = %d\n", myqid);
  sim_qid = myqid;
  return myqid;
}

/**********************************************************/

int send_sim_msg(sim_msg_t msg)
{
  q_sim_msg_t msgbuf;
  msgbuf.mtype = 1;
  msgbuf.data = msg;
  int ret = msgsnd(sim_qid, &msgbuf, sizeof(msgbuf.data), 0);
  if (ret == -1)
  {
    perror("Could not send message");
    exit(0);
  }
  printf("send successful\n");
  return ret;
}

/**********************************************************/

int check_for_sim_msg(sim_msg_t *msg)
{
  q_sim_msg_t msgbuf;
  int ret = msgrcv(sim_qid, &msgbuf, sizeof(msgbuf.data), 1, IPC_NOWAIT);
  if (ret < 0)
  {
    return -1; // No message waiting or error 
  }
  printf("Message recieved ######\n");
  *msg = msgbuf.data;
  return 0;
}

/************************************************************/

