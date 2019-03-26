
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <mqueue.h>

#include "utimer.h"
#include "db.h"
#include "iodriver.h"
#include "process_drv.h"

/**
struct process_mq_read_t
{
  bool di_vals[32];
  bool do_vals[32];
  double ai_vals[32];
  long count_vals[32];
};

struct process_mq_send_t
{
  int card;
  int channel;
  double ao_val;
  bool do_val;
};
**/

process_mq_send_t write_buf;

process_mq_read_t read_buf;

mqd_t mq_fd_1;
mqd_t mq_fd_2;

/*****************************************************************/

void read_io(void)
{
  int rval = mq_send(mq_fd_2, (char *) &read_buf, sizeof(read_buf), 0);
  if (rval == -1)
  {
    perror("mq_send");
    return;
  }
  printf("Send Success\n");
}

/*****************************************************************/

void check_message(void)
{
  unsigned prio;
  char msgbuf[8192];
  printf("Waiting for a message . . . . \n");
  int rval = mq_receive(mq_fd_1, msgbuf, sizeof(msgbuf), &prio);
  if (rval == -1)
  {
    perror("mq_recieve");
    return; 
  }
  memcpy(&write_buf, msgbuf, sizeof(write_buf));
}

/*****************************************************************/

int main(int argc, char *argv[])
{
  mq_fd_1 = mq_open("/com.adaptivertc.react.p1", 
           O_RDWR | O_NONBLOCK, 0755, NULL);
  if (mq_fd_1 == ((mqd_t) -1))
  {
    perror("mq_open");
    return 0;
  }
  mq_fd_2 = mq_open("/com.adaptivertc.react.p2", 
                O_RDWR | O_NONBLOCK, 0755, NULL);
  if (mq_fd_2 == ((mqd_t) -1))
  {
    perror("mq_open");
    return 0;
  }

  struct mq_attr attr;
  int err;
  err = mq_getattr(mq_fd_1, &attr);
  if (err == -1)
  {
    perror("mq1");
  }
  printf("There are %ld messages in mq 1\n",  attr.mq_curmsgs);

  err = mq_getattr(mq_fd_2, &attr);
  if (err == -1)
  {
    perror("mq2");
  }
  printf("There are %ld messages in mq 2\n",  attr.mq_curmsgs);

  unsigned prio;
  char msgbuf[8192];
  while(true)
  {
    printf("Waiting for a message on q 1 . . . . \n");
    int rval = mq_receive(mq_fd_1, msgbuf, sizeof(msgbuf), &prio);
    if (rval == -1)
    {
      perror("mq_recieve");
      break;
    }
  }

  while(true)
  {
    printf("Waiting for a message on q 2. . . . \n");
    int rval = mq_receive(mq_fd_2, msgbuf, sizeof(msgbuf), &prio);
    if (rval == -1)
    {
      perror("mq_recieve");
      break;
    }
  }

  return 0;

  return 0;
}

/*****************************************************************/

