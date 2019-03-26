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

#include "msg_queue.h"

static int qid;

/**********************************************************/

int create_message_queue(void)
{
  key_t mkey = ftok("./", 1);
  int myqid = msgget(mkey, IPC_CREAT | 0700);
  if (myqid == -1)
  {
    perror("Could not create message queue");
    exit(0);
  }
  printf("I got a message queue, id = %d\n", myqid);
  qid = myqid;
  return myqid;
}

/**********************************************************/

int connect_message_queue(void)
{
  key_t mkey = ftok("./", 1);
  int myqid = msgget(mkey, 0700);
  if (myqid == -1)
  {
    perror("Could not connect to message queue");
    exit(0);
  }
  printf("Found a message queue, id = %d\n", myqid);
  qid = myqid;
  return myqid;
}

/**********************************************************/

void msgq_send_departure(time_t now, int section)
{
  crossing_queue_event_t msgbuf;
  msgbuf.mtype = 1;
  msgbuf.data.time_stamp = now;
  msgbuf.data.section = section;
  msgbuf.data.departure = true;
  msgbuf.data.update = false;
  int ret = msgsnd(qid, &msgbuf, sizeof(msgbuf.data), 0);
  if (ret == -1)
  {
    perror("Could not send message");
    exit(0);
  }
}

/**********************************************************/

void msgq_send_arrival(time_t now, int section)
{
  crossing_queue_event_t msgbuf;
  msgbuf.mtype = 1;
  msgbuf.data.time_stamp = now;
  msgbuf.data.section = section;
  msgbuf.data.departure = false;
  msgbuf.data.update = false;
  int ret = msgsnd(qid, &msgbuf, sizeof(msgbuf.data), 0);
  if (ret == -1)
  {
    perror("Could not send message");
    exit(0);
  }
}

/************************************************************/

void msgq_send_update(time_t now)
{
  crossing_queue_event_t msgbuf;
  msgbuf.mtype = 1;
  msgbuf.data.time_stamp = now;
  msgbuf.data.section = 0;
  msgbuf.data.departure = false;
  msgbuf.data.update = true;
  msgbuf.data.skip = false;
  int ret = msgsnd(qid, &msgbuf, sizeof(msgbuf.data), 0);
  if (ret == -1)
  {
    perror("Could not send message");
    exit(0);
  }
}

/************************************************************/

