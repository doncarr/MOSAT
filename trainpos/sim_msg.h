
#ifndef __SIM_MSG_INC__
#define __SIM_MSG_INC__
#include <time.h> //we only have to do this because maco is a bitch

#define SIM_TRIGGER_EXTRA_EVENT 1
#define SIM_DROP_NEXT_EVENT 2

struct sim_msg_t
{
  int type;
  unsigned section;
  unsigned sensor;
};

struct q_sim_msg_t
{
  long mtype;
  sim_msg_t data;
};

int create_sim_queue(void);
int connect_sim_queue(void);

int check_for_sim_msg(sim_msg_t *msg);
int send_sim_msg(sim_msg_t msg);


#endif
