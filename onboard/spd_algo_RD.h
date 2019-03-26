
class speed_algorithm_RD_t : public speed_algorithm_base_t 
{
private:
	time_t warning_beginning;
	speed_state_t previous_state;
	speed_state_t current_state(time_t now, double actual, 
    double the_lo_limit, double the_hi_limit, double shutdown_limit);
    void reset_state();

public:
  speed_algorithm_RD_t(void);
  void evaluate(time_t now, double actual_speed, double actual_distance, 
		current_speed_limits_t limits, speed_results_t *results);
};

