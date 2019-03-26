
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <sys/mman.h> 

#ifdef PPC
// Sixnet headers
extern "C" 
{
  #include <oemdefs.h>
  #include <iodb.h>
}
#endif
/***/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <signal.h>


#include <pthread.h>

#include "mtype.h"
//#include "modio.h"
//#include "hprtu.h"
//#include "hp2mod.h"
#include "utimer.h"
#include "evlog.h"

#include "ppc_logger.h"
#include "train_detect.h"

#include "rtcommon.h"
#include "arg.h"

//#define DEST_PORT 3490 
//#define DEST_PORT 502 
//#define DEST_PORT 80 

#define MYPORT 3490    /* the port users will be connecting to */
#define BACKLOG 10

static int sockfd;
static int n_clients = 0;
static int client_sock[20];
static int semid;

class ppc_logger_t *logger;


/*************************************/

void create_sems(void)
{
  key_t mkey = ftok("/home/carr", 5);
  semid = semget(mkey, 2, IPC_CREAT | 0755);
  if (semid == -1)
  {
    perror("Could not create a semaphore");
    exit(0);
  }
  printf("I got a semaphore, id = %d\n", semid);

  union semun
  {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
  } semarg;
  semarg.val = 1;
  int ret = semctl(semid, 0, SETVAL, semarg);
  if (ret == -1)
  {
    perror("Could not set value");
  }
  semarg.val = 1;
  ret = semctl(semid, 1, SETVAL, semarg);
  if (ret == -1)
  {
    perror("Could not set value");
  }
}

/*************************************/

static void sem_wait(int n)
{
  struct sembuf sops[1];
  sops[0].sem_num = n;
  sops[0].sem_op = -1;
  sops[0].sem_flg = 0;
  int ret = semop(semid, sops, 1);
  if (ret == -1)
  {
    perror("Could not deccrement semephore\n");
    exit(0);
  }
}

/*************************************/

static void sem_signal(int n)
{
  struct sembuf sops[1];
  sops[0].sem_num = n;
  sops[0].sem_op = +1;
  sops[0].sem_flg = 0;
  int ret = semop(semid, sops, 1);
  if (ret == -1)
  {
    perror("Could not increment semephore\n");
    exit(0);
  }
}

/*************************************/

void my_sighandler(int signum)
{
  sem_wait(0);
  for (int i=0; i < n_clients; i++)
  {
    printf("Shuting down client socket %d\n", i);
    int sock =  client_sock[i];
    int err = shutdown(sock, SHUT_RDWR);
    if (err != 0)
    {
      perror("shutdown");
    }
    if (0 != close(sock))
    {
      perror("close");
    }
  } 
  printf("Shuting down main socket\n");
  int err = shutdown(sockfd, SHUT_RDWR);
  if (err != 0)
  {
    perror("shutdown");
  }
  if (0 != close(sockfd))
  {
    perror("close");
  }
  exit(0);
}


/****************************************************/

int send_event_to_one(int fd, crossing_event_data_t ev)
{
  char send_buffer[5 * sizeof(long)];
  train_detector_t::cev_to_net(&ev, send_buffer, sizeof(send_buffer));
  int n = send(fd, send_buffer, sizeof(send_buffer), MSG_DONTWAIT | MSG_NOSIGNAL);
  if (n != sizeof(send_buffer))
  {
    // Ok, the client disconnected, the network is down, or somthing bad happend.
    // Close the socket, they will have to re-connect.
    perror("send");
    if (0 != shutdown(fd, SHUT_RDWR))
    {
      perror("shutdown");
    }
    if (0 != close(fd))
    {
      perror("close");
    }
    return -1;
  }
  return 0;
}

/****************************************************/

int send_old_events(int fd)
{
  char buf[8];  
  printf("Reading time, seq num from new client . . . \n");
  int n = recv(fd, buf, sizeof(buf), MSG_WAITALL);
  if (n != sizeof(buf))
  { 
    printf("Could not read time, seq num\n");
    return -1;
  }
  time_t the_time;
  long the_seq_num;
  memcpy(&the_time, buf, 4);
  memcpy(&the_seq_num, buf + 4, 4);
  the_time = ntohl(the_time);
  the_seq_num = ntohl(the_seq_num);
  printf("Got values: %ld, %ld\n", the_time, the_seq_num);
  /***
  if ((the_time == 0) && (the_seq_num == -1))
  {
    return 0;
  }
  ***/
  printf("Sending old events . . . \n");
  crossing_event_data_t ev;
  crossing_event_data_t lastev;
  int ecount = 0;
  sem_wait(1); 
  {
    int ret = logger->first_event(the_time, the_seq_num, &ev);
    printf("First event sent: %ld\n", ev.seq_num);
    lastev = ev;
    while (ret == 0)
    {
      lastev = ev;
      ecount++;
      //printf("Sending seq_num %ld\n", ev.seq_num);
      ret = send_event_to_one(fd, ev);
      if (ret != 0)
      {
        return -1;
      }
      ret = logger->next_event(&ev);
    }
  }
  sem_signal(1); 
  printf("Last event: %ld, of %d\n", lastev.seq_num, ecount);
  return 0;
}

/****************************************************/

void *wait_connections(void *data)
{
  int new_fd;  /* listen on sock_fd, new connection on new_fd */
  int ret;
  struct sockaddr_in my_addr;    /* my address information */
  struct sockaddr_in their_addr; /* connector's address information */
  socklen_t sin_size;

  printf("Using port %d\n", MYPORT);
  sockfd = socket(AF_INET, SOCK_STREAM, 0); /* do some error checking! */
  if (sockfd < 0)
  {
    perror("error creating socket");
    exit(0);
  }

  my_addr.sin_family = AF_INET;         /* host byte order */
  my_addr.sin_port = htons(MYPORT);     /* short, network byte order */
  my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
  memset(&(my_addr.sin_zero), 8, 0);        /* zero the rest of the struct */

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

  for (int i=0; true; i++)
  {
    printf("Connect thread: waiting for client %d on %d, at pos %d\n", 
         i, MYPORT, n_clients);    
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd < 0)
    {
      perror("error on accept");
      exit(0);
    }

    int retval = send_old_events(new_fd);
    
    if (retval != 0)
    {
      perror("send old events");
      close(new_fd);
      break;
    } 

    sem_wait(0); 
    { // Critical section starts here.
      client_sock[n_clients] = new_fd;
      printf("Added client at %d, fd = %d\n", n_clients, new_fd);
      n_clients++;
      if (n_clients >= 20) n_clients = 0;
    }
    sem_signal(0);
  }
  return NULL;
}

/****************************************************/

void send_event(crossing_event_data_t ev)
{
  if (ev.id != EV_WATCHDOG)
  {
    sem_wait(1);
    {
      logger->log_event(&ev);
    }
    sem_signal(1);
  }
  sem_wait(0);
  { // Critical section starts here.
    for (int i=0; i < n_clients; i++)
    {
      if (client_sock[i] == -1) continue;
      printf("Sending to client %d, fd = %d\n", i, client_sock[i]);
      /***
      int n = send(client_sock[i], send_buffer, sizeof(send_buffer), MSG_DONTWAIT | MSG_NOSIGNAL);
      if (n != sizeof(send_buffer))
      {
        // Ok, the client disconnected, the network is down, or somthing bad happend. 
        // Close the socket, they will have to re-connect.
        perror("send");
        if (0 != shutdown(client_sock[i], SHUT_RDWR))
        {
          perror("shutdown");
        }
        if (0 != close(client_sock[i]))
        {
          perror("close");
        }
        ****/
        // Also, remove this puppy from the list.
      int ret = send_event_to_one(client_sock[i], ev);
      if (ret != 0)
      {
        printf("Client %d, fd %d,  not responding\n", i, client_sock[i]); 
        for (int j=i; j < (n_clients - 1); j++)
        {
          printf("Client %d replaced with %d\n", j, j+1); 
          client_sock[j] = client_sock[j+1]; 
        }
        n_clients--;
        printf("There are now %d clients\n", n_clients); 
        for (int k=0; k < n_clients; k++) printf("Client %d, fd %d\n", k, client_sock[k]);
      }
    }
  }
  sem_signal(0);
}

/****************************************************/

class my_event_log_t : public event_log_base_t
{
  int sock;
public:
  void event_start(long id, time_t the_time, long counts);
  void event_continue(long id, time_t the_time, long counts);
  void event_end(long id, time_t the_time, time_t start_time, time_t end_time,
          long total_counts, long start_count, long end_count);
  void event_disconnect(long id, time_t the_time);
  void event_reconnect(long id, time_t the_time);
};

/****************************************************/

void my_event_log_t::event_start(long id, time_t the_time, long counts)
{
  char buf[100];
  struct tm mytm;
  localtime_r(&the_time, &mytm);
  strftime(buf, sizeof(buf), "%F\t%T", &mytm);
  printf("%ld\tSTART\t%s\t%ld\n", id, buf, counts);
}

/****************************************************/

void my_event_log_t::event_continue(long id, time_t the_time, long counts)
{
  // For now we won't print anthing.
}

/****************************************************/

void my_event_log_t::event_end(long id, time_t the_time, time_t start_time, time_t end_time,
          long total_counts, long start_count, long end_count)
{
  char buf[100];
  struct tm mytm;
  localtime_r(&start_time, &mytm);
  strftime(buf, sizeof(buf), "%F\t%T", &mytm);
  printf("%ld\t%s\t%s\t", id, get_ev_string(id), buf);
  localtime_r(&end_time, &mytm);
  strftime(buf, sizeof(buf), "%F\t%T", &mytm);
  printf("%s\t%ld\t\n", buf, total_counts);
  
  crossing_event_data_t ev;
  ev.seq_num = 0;
  ev.id = id;
  ev.n_pulses = total_counts;
  ev.start_time = start_time;
  ev.end_time = end_time;

  send_event(ev);

  /***
  int fd;
  char *p = (char *) mmap(0, 100, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  msync(p, 100, MS_ASYNC);
  munmap(p, 100);
  ***/
  //static void cev_from_net(crossing_event_data_t *ev, void *buf, int buf_size);
  //static void cev_to_net(crossing_event_data_t *ev, void *buf, int buf_size);
}

/****************************************************/

void my_event_log_t::event_disconnect(long id, time_t the_time)
{
  char buf[100];
  struct tm mytm;
  localtime_r(&the_time, &mytm);
  strftime(buf, sizeof(buf), "%F\t%T", &mytm);
  printf("%ld\tDISCON\t%s\n", id, buf);

  crossing_event_data_t ev;
  ev.seq_num = -1;
  ev.id = id + 200;
  ev.n_pulses = 0;
  ev.start_time = time(NULL);
  ev.end_time = ev.start_time;
  send_event(ev);
}

/****************************************************/

void my_event_log_t::event_reconnect(long id, time_t the_time)
{	
  char buf[100];
  struct tm mytm;
  localtime_r(&the_time, &mytm);
  strftime(buf, sizeof(buf), "%F\t%T", &mytm);
  printf("%ld\tRECON\t%s\n", id, buf);
  crossing_event_data_t ev;
  ev.seq_num = 0;
  ev.id = id + 300;
  ev.n_pulses = 0;
  ev.start_time = time(NULL);
  ev.end_time = ev.start_time;
  send_event(ev);
}

/****************************************************/

uint16 myuvals[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int read_ai(uint16 start_address, int n_to_read, uint16 *vals)
{
  int ecode;
#ifdef PPC
  ecode = IODBRead(ANALOG_IN, start_address, n_to_read, (void *)vals, NULL);
#else
  ecode = 0;
  memcpy(vals, myuvals, n_to_read * sizeof(uint16));
#endif
  
  return ecode;
}

/****************************************************/

int read_di(uint16 start_address, int n_to_read, bool *vals)
{
  int ecode;
#ifdef PPC
  ecode = IODBRead(DISCRETE_IN, start_address, n_to_read, (void *)vals, NULL);
#endif
  return ecode;
}

/****************************************************/

void create_thread(void)
{
  int retval;
  pthread_t thr;
  retval = pthread_create(&thr, NULL, wait_connections, NULL);
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
}

/****************************************************/

int main(int argc, char *argv[])
{
  utimer_t utimer;
  utimer.set_busy_wait(false);
  utimer.set_interval(200000);
  utimer.set_start_time();
  uint16 uvals[20];
  bool dvals[16];

  create_sems();

  logger = new ppc_logger_t("/nvram/siteur/elogfile");

  signal(SIGHUP, my_sighandler);
  signal(SIGQUIT, my_sighandler);
  signal(SIGTERM, my_sighandler);
  signal(SIGFPE, my_sighandler);
  signal(SIGILL, my_sighandler);
  signal(SIGSEGV, my_sighandler);
  signal(SIGINT, my_sighandler);

  create_thread();
  const char *lfile = "/nvram/siteur/recordlog.txt";
  FILE *lfp = fopen(lfile, "w");
  if (lfp == NULL)
  {
    perror(lfile);
    exit(0);
  }

  const char *fname = "/home/carr/rtu_channel_config.txt";
  FILE *fp = fopen(fname, "r");
  if (fp == NULL)
  {
    perror(fname);
    exit(0);
  }

  char line[300];
  int n_lines = 0;
  pthread_t thread;
  const int MAXTD = 8;
  const int MAXED = 8;
  int ntd = 0;
  int ned = 0;
  train_detector_t td[MAXTD];
  event_detector_t event_detect[MAXED];
  int cch[MAXTD];
  int dch[MAXTD];
  int each[MAXED];
  int edch[MAXED];
  int ecch[MAXED];
  my_event_log_t mylog;
  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    if (ntd >= MAXTD) break;
    if (ned >= MAXED) break;
    char tmp[300];
    int argc;
    char *argv[25];

    fprintf(lfp, "Line %d: %s\n", i, line);
    safe_strcpy(tmp, line, sizeof(tmp));
    argc = get_delim_args(tmp, argv, '|', 10);
    char first_char = argv[0][0];
    printf("Line %d: %s, first: %c\n", i, line, first_char);
    if (argc == 0)
    {
      continue;
    }
    if ((first_char == 'X') || (first_char == 'x'))
    {
      if (argc != 4)
      {
        fprintf(lfp, "Line %d, wrong number of args:  %s\n", i, line);
      }
      else
      {
        int sensor_id = atol(argv[1]);
        int counter_channel = atol(argv[2]);
        int discrete_channel = atol(argv[3]);
        printf("Creating crossing detector for: %d, %d, %d\n", 
                 sensor_id, counter_channel, discrete_channel);
        fprintf(lfp, "Creating detector for: %d, %d, %d\n", 
                 sensor_id, counter_channel, discrete_channel);
        td[ntd].set_id(sensor_id);
        td[ntd].set_evlog(&mylog);
        cch[ntd] = counter_channel;
        dch[ntd] = discrete_channel;
        ntd++;
      }
    }
    if ((first_char == 'E') || (first_char == 'e'))
    {
      if (argc != 7)
      {
        fprintf(lfp, "Line %d, wrong number of args:  %s\n", i, line);
      }
      else
      {
        long ids[3];
        int channels[3];
        for (int j=0; j < 3; j++)
        {
          ids[j] = atol(argv[j+1]);
          channels[j] = atol(argv[j+4]);
        }
        event_detect[ned].set_ids(ids[0], ids[1], ids[2]);
         
        printf("Creating event detector for: %d, %d, %d\n", 
                  channels[0], channels[1], channels[2]);
        event_detect[ned].set_evlog(&mylog);
        each[ned] = channels[0];
        edch[ned] = channels[1];
        ecch[ned] = channels[2];
        if (ecch[ned] < 0) ecch[ned] = 0;
        ned++;
      }
    }
  }

  int count[8];


  /****
  train_detector_t td[2];
  td[0].set_id('1');
  td[0].set_evlog(&mylog);
  td[1].set_id('2');
  td[1].set_evlog(&mylog);
  int ch[2];
  ch[0] = 7;
  ch[1] = 9;
  int dch[2];
  dch[0] = 1;
  dch[1] = 3;
  int nt = 2;
  ***/

  read_ai(0, 16, uvals); 
  for (int i=0; i < ntd; i++)
  {
    td[i].set_initial_count(uvals[cch[i]]);
  }
  for (int i=0; i < ned; i++)
  {
    event_detect[i].set_initial_count(uvals[each[i]], uvals[edch[i]], uvals[ecch[i]]);
  }
  fflush(lfp);

  int evntimes = 100;
  int n_records = 14;
  int ntimes = 0;
  int confirm_count = 0;
  bool confirm_on = false;
  bool trigger_depart = true;
  while (true)
  {
    utimer.wait_next();
    //if (utimer.late_time() > 1.0)
    //{
    //  utimer.set_start_time();
    //}

    /***
    modc.send_do(chan, dval);
    if (dval == 0) chan = (chan + 1) % 8;
    dval = (dval == 0) ? 1 : 0;
    ****/
    read_di(0, 16, dvals); 
    read_ai(0, 16, uvals); 
    /***
    for (int i=0; i < 16; i++)
    {
      printf("%d ", (int) uvals[i]);
    }
    ***/
    //printf(" di: ");
    //for (int i=0; i < 4; i++) dvals[i] ? printf("1 ") : printf("0 ");
    //printf("\n");
    for (int i=0; i < ntd; i++)
    {
      //count[i] = uvals[cch[i]]; 
      //printf("ai[%d]:%d\n" ch[i], count[i]);
      td[i].update(uvals[cch[i]], dvals[dch[i]]);
    }
    for (int i=0; i < ned; i++)
    {
      event_detect[i].update(uvals[each[i]], uvals[edch[i]], uvals[ecch[i]]); 
    }

    if (n_records < 12)
    {
      for (int i=0; i < 12; i++)
      {
        myuvals[i] += 2;
      }
      n_records += 2;
    } 
    
    evntimes++;
    if (evntimes > 200)
    {
      if (trigger_depart)
      {
        confirm_on = true;
        confirm_count = 0;
        for (int i=0; i < 8; i++)
        {
          myuvals[i] += 2;
        }
      }
      else
      {
        for (int i=8; i < 12; i++)
        {
          myuvals[i] += 2;
        }
      }
      trigger_depart = !trigger_depart;
      n_records = 2;
      evntimes = 0;
    }
    if (confirm_on)
    {
      if (confirm_count > 90)
      {
        myuvals[12] += 6;
        confirm_on = false;
      }
      confirm_count++;
    }

    ntimes++;
    if (ntimes > 50)
    {
      for (int i=0; i < ntd; i++)
      {
        count[i] = uvals[cch[i]]; 
        printf("ai[%d]:%d\n", cch[i], uvals[cch[i]]);
      }
      for (int i=0; i < 16; i++)
      {
        printf("%d ", (int) uvals[i]);
      }
      printf("\n");
      printf(" di: ");
      for (int i=0; i < 16; i++) dvals[i] ? printf("1 ") : printf("0 ");
      printf("\n");
      ntimes = 0;
      crossing_event_data_t ev;
      ev.seq_num = -1;
      ev.id = EV_WATCHDOG;
      ev.n_pulses = -1;
      ev.start_time = time(NULL);
      ev.end_time = ev.start_time; 
      send_event(ev);
    }
  }
}


