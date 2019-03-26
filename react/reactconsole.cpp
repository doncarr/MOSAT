/************************************************************************
This software is part of React, a control engine
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
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/msg.h>

#include <sys/time.h>
#include <sys/resource.h>

#include "reactmsg.h"

int myqid;

void send_and_recieve(reactmsgbuf_t *msgbuf, reactmsgbuf_t *repbuf)
{
  /* Ok, first create a queue to recieve the result. */

  if (0 != setpriority(PRIO_PROCESS, 0, 2))
  {
    printf("Could not set the priority\n");
  }
  int repqid = msgget(IPC_PRIVATE, IPC_CREAT | 0755);
  if (repqid == -1)
  {
    perror("Could not create message queue");
    exit(0);
  }
  printf("%d got a message queue, id = %d\n", getpid(), repqid);

  msgbuf->mtype = 1;
  msgbuf->data.qid = repqid;
  printf("%d sending msg on %d\n", getpid(), myqid);
  int ret = msgsnd(myqid, msgbuf, sizeof(msgbuf->data), 0);
  if (ret == -1)
  {
    perror("Could not send message");
    printf("Maybe react is not running?\n");
    exit(0);
  }
  printf("%d send successful, ret = %d\n", getpid(), ret);

  printf("%d recieving message on qid: %d\n", getpid(), repqid);
  ret = msgrcv(repqid, repbuf, sizeof(repbuf->data), 1, 0);
  //printf("%d\n", __LINE__);
  if (ret == -1)
  {
    perror("Could not recieve msg");
    exit(0);
  }
  printf("%d got the message: %s\n", getpid(), repbuf->data.msg);
  printf("%s\n", repbuf->data.msg);

  ret = msgctl(repqid, IPC_RMID, NULL);
  if (ret == -1)
  {
    perror("Could not destroy the message queue");
    exit(0);
  }
}

int main(int argc, char *argv[])
{
  FILE *fp = fopen("reactqid.txt", "r");
  if (fp == NULL)
  {
    perror("reactqid.txt");
  }
  fscanf(fp, "%d", &myqid);
  fclose(fp);

  reactmsgbuf_t msgbuf;
  msgbuf.mtype = 1;

  reactmsgbuf_t repbuf;

  while(true)
  {
    char buf[50];
    printf("Enter a command ('q' to quit): ");
    fgets(buf, sizeof(buf), stdin);
    if (buf[0] == 'q')
    {
      break;
    }
    printf("command = %s\n", buf);
    msgbuf.mtype = 1;
    strcpy(msgbuf.data.msg, buf);
    send_and_recieve(&msgbuf, &repbuf);
  }
}


