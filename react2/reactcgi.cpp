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
#include <errno.h>

#include <sys/ipc.h>
#include <sys/msg.h>

#include <sys/time.h>
#include <sys/resource.h>

#include "reactmsg.h"

int myqid;

/*************************************************************************/


void strip_slash(char *s)
{
  int l = strlen(s);
  for (int i=0; i < l; i++)
  {
    if (s[i] == '\\')
    {
      for (int j=i; j < l; j++)
      {
        s[j] = s[j+1];
      }
    }
  }
}

/*************************************************************************/

void send_and_recieve(reactmsgbuf_t *msgbuf, reactmsgbuf_t *repbuf)
{
  /* Ok, first create a queue to recieve the result. */

  if (0 != setpriority(PRIO_PROCESS, 0, 2))
  {
    printf("Could not set the priority<br>\n");
  }
  int repqid = msgget(IPC_PRIVATE, IPC_CREAT | 0777);
  if (repqid == -1)
  {
    //perror("Could not create message queue");
    printf("Could not create message queue: %s<br>\n", strerror(errno) );
    return;
  }
  //printf("%d got a message queue, id = %d<br>\n", getpid(), repqid);

  msgbuf->mtype = 1;
  msgbuf->data.qid = repqid;
  printf("Enviando mensaje a maquina de control . . . <br>\n");
  int ret = msgsnd(myqid, msgbuf, sizeof(msgbuf->data), 0);
  if (ret == -1)
  {
    //perror("Could not send message");
    printf("Could not send message: %s<br>\n", strerror(errno) );
    printf("Maybe react is not running . . . <br>\n");
    return;
  }
  printf("Ok<br>\n");
  //printf("%d send successful, ret = %d<br>\n", getpid(), ret);

  printf("Esperando respuesta . . . <br>\n");

  ret = msgrcv(repqid, repbuf, sizeof(repbuf->data), 1, 0);
  //printf("%d\n", __LINE__);
  if (ret == -1)
  {
    //perror("Could not recieve msg");
    printf("Could not recieve message: %s<br>\n", strerror(errno) );
    return;
  }
  //printf("%d got the message: %s<br>\n", getpid(), repbuf->data.msg);
  printf("%s<br>\n", repbuf->data.msg);

  ret = msgctl(repqid, IPC_RMID, NULL);
  if (ret == -1)
  {
    //perror("Could not destroy the message queue");
    printf("Could not destroy the message queue: %s<br>\n", strerror(errno) );
    return;
  }
}

int main(int argc, char *argv[])
{

  printf("Content-type: text/html\n\n");

  printf("<html>");
  printf("<head>");
  printf("<meta http-equiv=\"content-type\"");
  printf("content=\"text/html; charset=ISO-8859-1\">");
  printf("<title>Comando Ejecutado</title>");
  printf("</head>");

  printf("%s\n", "<body style=\"color: rgb(0, 0, 0); background-color: rgb(204, 204, 204);\"");
  printf("%s\n", "link=\"#000099\" vlink=\"#990099\" alink=\"#000099\">");
  printf("<h2>Comando Ejecutado</h2>");
  const char *qidfile = "/home/carr/control/react/reactqid.txt";
  FILE *fp = fopen( qidfile, "r");
  if (fp == NULL)
  {
    //perror("reactqid.txt");
    printf("%s: %s<br>\n", qidfile, strerror(errno) );
    fflush(stdout);
    printf("%s\n", "<br><br><center>[<a HREF=\"#\" onClick=\"opener.focus(); window.close(); return false\">Cerrar ventana</a>] </center>");
    exit(0);
  }
  fscanf(fp, "%d", &myqid);
  fclose(fp);

  //printf("React qid is: %d<br>\n");

  reactmsgbuf_t msgbuf;
  msgbuf.mtype = 1;

  reactmsgbuf_t repbuf;

  for (int i=1; i < argc; i++)
  {
    strip_slash(argv[i]);
    printf("%s<br>\n", argv[i]);
    //char buf[50];
    //printf("Enter a command ('q' to quit): ");
    //fgets(buf, sizeof(buf), stdin);
    //if (buf[0] == 'q')
    //{
     // break;
    //}
    msgbuf.mtype = 1;
    strcpy(msgbuf.data.msg, argv[i]);
    send_and_recieve(&msgbuf, &repbuf);
  }
  printf("%s\n", "<br><br><center>[<a HREF=\"#\" onClick=\"opener.focus(); window.close(); return false\">Cerrar ventana</a>] </center>");

}


