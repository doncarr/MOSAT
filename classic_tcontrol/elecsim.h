

class elecsim_t
{
private:
  int n_substations;
  double distances[MAX_ELEC_SUBSTATIONS];
public:
  void dosim(int ntrains, double distances[], bool runing[], double usage[]);
  void set_substations(int n, double distances[]);
};

