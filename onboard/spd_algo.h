
#ifndef __spd_algo_INC__
#define __spd_algo_INC__

const double SPD_STATE_DEADBAND = 0.5;
const int SPD_MAX_WARN_TIME = 20;

enum speed_state_t 
{
  SPD_STATE_SHUTDOWN,
  SPD_STATE_WARN_HI,
  SPD_STATE_NORMAL,
  SPD_STATE_WARN_LO
};

struct speed_results_t
{
  bool stop_train;
  speed_state_t state;
};

struct current_speed_limits_t
{
  double desired;
  double low;
  double high;
  double very_high;
};

class speed_algorithm_base_t
{
public:
  virtual void evaluate(time_t now, double actual_speed, double actual_distance, 
	current_speed_limits_t limits, speed_results_t *results) = 0;
  virtual ~speed_algorithm_base_t(){};
};

class speed_algorithms_t
{
private:
  int num_algorithms;
  speed_algorithm_base_t *algorithms[3];
  speed_results_t alg_results[3];
  speed_results_t final_result;
  void resolve_votes(speed_results_t alg_results[], speed_results_t *final_result);
public:
  speed_algorithms_t(void);
  void evaluate(time_t now, double actual_speed, double actual_distance, 
		  current_speed_limits_t limits, speed_results_t *results);
};

void alg_compare(speed_results_t *results, int n);

#endif

