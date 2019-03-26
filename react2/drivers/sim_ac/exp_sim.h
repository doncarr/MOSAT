


class exp_simulator_t
{
private:
  double tau; // time (in seconds) it take to get to 63.2121% of steady state value.
       // Note: 0.632121 = 1.0 - exp(-1) 
  double current_value; // In what ever units.
  double steady_state_value; // Same units as current value
public:
  exp_simulator_t(void);
  void set_tau(double a_tau){tau = a_tau;};
  void set_steady_state(double a_steady_state) {steady_state_value = a_steady_state;};
  void set_current_value(double a_value){current_value = a_value;};
  double get_current_value(double a_delta_t);
};


