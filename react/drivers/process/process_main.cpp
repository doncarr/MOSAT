


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

#include "logfile.h"

logfile_t *logfile;


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

static io_driver_t *io_driver;

mqd_t mq_fd_1;
mqd_t mq_fd_2;

class dummy_db_t : public react_drv_base_t
{
public:
  double get_time(void);
};

dummy_db_t dummy_db;

/*****************************************************************/

double dummy_db_t::get_time(void)
{
  struct timeval result;
  gettimeofday(&result, NULL);
  return double(result.tv_sec) + (double(result.tv_usec) / 1000000.0);
}

/*****************************************************************/

void read_io(void)
{
  io_driver->read();
  for (int i=0; i < 32; i++)
  {
    read_buf.ai_vals[i] = io_driver->get_ai(i);
  }

  for (int i=0; i < 32; i++)
  {
    read_buf.ai_vals[i] = io_driver->get_di(i);
  }

  for (int i=0; i < 32; i++)
  {
    read_buf.count_vals[i] = io_driver->get_count(i);
  }
  io_driver->end_read();
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

  printf("Got a message of %d bytes\n", rval);

  switch(write_buf.type)
  {
    case SEND_AO:
      io_driver->send_ao(write_buf.channel, write_buf.ao_val);
      break;
    case SEND_DO: 
      io_driver->send_do(write_buf.channel, write_buf.do_val);
      break;
    case READ_INPUTS:
      read_io();
      break;
  }
}

/*****************************************************************/

int main(int argc, char *argv[])
{
  /*io_driver = load_iodriver(&dummy_db, 
     "../modbusdrv/libreactmodbusdrv.so", 
     "new_reactmodbus", "librtmodbus.so", NULL); */
  logfile = new logfile_t(false, true);
  io_driver = load_iodriver(&dummy_db, 
     "../simple_ascii/libsimple_ascii_drv.so", 
     "new_simple_ascii", NULL, "/dev/ttyUSB0");
//./drivers/simple_ascii/libsimple_ascii_drv.so|new_simple_ascii||/dev/ttyUSB0|

  int err;
  err = mq_unlink("/com.adaptivertc.react.p1");
  if (err < 0)
  {
     perror("p1");
  }
  err = mq_unlink("/com.adaptivertc.react.p2");
  if (err < 0)
  {
     perror("p2");
  }
  mq_fd_1 = mq_open("/com.adaptivertc.react.p1", O_RDWR | O_CREAT, 0755, NULL);
  if (mq_fd_1 == ((mqd_t) -1))
  {
    perror("mq_open");
    return 0;
  }
  mq_fd_2 = mq_open("/com.adaptivertc.react.p2", O_RDWR | O_CREAT, 0755, NULL);
  if (mq_fd_2 == ((mqd_t) -1))
  {
    perror("mq_open");
    return 0;
  }
  while (true)
  {
    check_message();
  }

  return 0;
}

/*****************************************************************/

