
#include <stdio.h>
#include <unistd.h>

#define HUM_LO(silo_start)       (*(silo_start + 0))
#define HUM_HI(silo_start)       (*(silo_start + 1))
#define TEMP_LO(silo_start)      (*(silo_start + 2))
#define TEMP_HI(silo_start)      (*(silo_start + 3))
#define ON_OFF_MOD(silo_start)   (*(silo_start + 4))
#define ON_OFF_CH(silo_start)    (*(silo_start + 5))
#define AUTO_MAN_MOD(silo_start) (*(silo_start + 6))
#define AUTO_MAN_CH(silo_start)  (*(silo_start + 7))
#define N_FANS(silo_start)       (*(silo_start + 8))

#define ON_MOD(silo_start, fan)  (*(silo_start +  9 + (4*fan)))
#define ON_CH(silo_start, fan)   (*(silo_start + 10 + (4*fan)))
#define OFF_MOD(silo_start, fan) (*(silo_start + 11 + (4*fan)))
#define OFF_CH(silo_start, fan)  (*(silo_start + 12 + (4*fan)))

unsigned short config[200] = 
{
   5, 
   68, 75, 20, 32, 0, 0, 0, 1, 2, 
       /*1*/ 0, 0, 0, 2,  
       /*2*/ 0, 1, 0, 2, 
   70, 76, 20, 32, 0, 2, 0, 3, 4, 
       /*1*/ 0, 3, 0, 7,  
       /*2*/ 0, 4, 0, 7, 
       /*3*/ 0, 5, 0, 7, 
       /*4*/ 0, 6, 0, 7,
   68, 75, 20, 32, 0, 4, 0, 5, 2, 
       /*1*/ 0, 8, 0, 10,  
       /*2*/ 0, 9, 0, 10,
   /* next modbus */
   68, 75, 20, 32, 5, 0, 5, 1, 2, 
       /*1*/ 5, 0, 5, 2,  
       /*2*/ 5, 1, 5, 2,
   69, 76, 20, 32, 5, 2, 5, 3, 3, 
       /*1*/ 5, 3, 5, 6,  
       /*2*/ 5, 4, 5, 6, 
       /*3*/ 5, 5, 5, 6,
};
// number of silos
// 
// For each silo
//
// humidity low
// humidtiy high
// temperature low
// temperature high
// number of fans
//
// For each fan
//
// modbus ID for ON discrete output (0 for local)
// channel for ON discrete output
// modbus ID for OFF discrete output (0 for local)
// channel for on discrete output

/********************
Rules:

The ON DO for every fan must now have the same modbus id and channel.
   (fans must be turned on one at a time)
The OFF DO for fans from different silos can not have the same modbus id and channel.
   (Each silo must be controlled individually)
All of the controls for a silo must be on the same modbus ID
   (Each silo must be controlled from the same PCB)
There can not be two ON/OFF or AUTO/MANUAL discrete inputs that have the same modbus id and channel.
   (Each switch must be wired to a different input)
All channels must be in the acceptable range for the device they are on.
*********************/

struct silo_state_t
{
  unsigned short next_on_off;
  unsigned short next_time;
  bool in_range;
  bool all_on;
  bool all_off;

  bool is_on_feedback;
  bool on_switch;
  bool auto_switch;
};

struct silo_config_t
{
  unsigned short *fan_start;
  unsigned short upper_hum;
  unsigned short lower_hum;
  bool disable;
  unsigned short *on_do_channel;
  unsigned short *on_do_mod_id;
  unsigned short *off_do_channel;
  unsigned short *off_do_mod_id;
};

#define MAX_SILOS (10)

struct silo_state_t silo_state[MAX_SILOS];

unsigned short *silo_start[MAX_SILOS];
short n_silos;
unsigned short *config_start = config;

/**************************************************************/

void print_info(void)
{
  printf("There are %d silos\n", n_silos);
  for (int i=0; i < n_silos; i++)
  {
    printf("Silo %d has %hu fans ----------------------------------\n", i, N_FANS(silo_start[i]));
    printf("  HUM lo: %hu, HUM hi: %hu, TEMP lo: %hu, TEMP hi: %hu\n", 
      HUM_LO(silo_start[i]), HUM_HI(silo_start[i]),
      TEMP_LO(silo_start[i]), TEMP_HI(silo_start[i]));
    printf("  ON/OFF mod: %hu, ON/OFF ch: %hu, AUTO/MAN mod: %hu, AUTO/MAN ch: %hu\n", 
      ON_OFF_MOD(silo_start[i]), ON_OFF_CH(silo_start[i]),
      AUTO_MAN_MOD(silo_start[i]), AUTO_MAN_CH(silo_start[i]));
    for (int j=0; j < N_FANS(silo_start[i]); j++)
    {
      printf("    Fan %d: ON mod: %hu, ON ch: %hu, OFF mod: %hu,  OFF ch: %hu\n", j,
           ON_MOD(silo_start[i], j), ON_CH(silo_start[i],j),
           OFF_MOD(silo_start[i], j), OFF_CH(silo_start[i], j));
    }
  }
}

/**************************************************************/

void set_addresses(void)
{
  short n = 0;
  n_silos = config_start[0];  
  n++;

  // Here, we set the starting address of each silo.
  for (int i=0; i < n_silos; i++)
  {
    silo_start[i] = config_start + n; 
    int silo_size = 9 + (4 * config_start[n+8]);
    n += silo_size;
  }
}

/**************************************************************/

void process_one_silo(unsigned short *start, silo_state_t *state, long now)
{
  int n_fans = N_FANS(start); 
}

/**************************************************************/

void main_loop(short now)
{
  for (int i=0; i < n_silos; i++)
  {
    printf("processing silo %d\n", i);
    process_one_silo(silo_start[i], &silo_state[i], now);
  }
}

/**************************************************************/

int main(int argc, char *argv[])
{
  set_addresses();
  print_info();
  short now = 0;
  while(true)
  {
    main_loop(now);
    sleep(1);
    now++;
  }
}

/**************************************************************/

