
#ifndef __SPC_COMM_INC__
#define __SPC_COMM_INC__

#include "onboard.h"

void reset_distance(int section);
void connect_modbus(void);
void get_actual_speed_dist(int section, int t, double *dist, double *speed, spd_discrete_t *discretes);
void init_io(void);
void spd_set_key(int ch);

#endif
