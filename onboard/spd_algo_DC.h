
class speed_algorithm_DC_t : public speed_algorithm_base_t 
{
private:
  speed_state_t last_state;
  time_t warn_start_time;
  speed_state_t check_speed(time_t now, double actual, 
    double the_lo_limit, double the_hi_limit, double shutdown_limit);

public:
  speed_algorithm_DC_t(void);
  void evaluate(time_t now, double actual_speed, double actual_distance, 
		current_speed_limits_t limits, speed_results_t *results);
};

