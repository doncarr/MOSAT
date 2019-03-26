

/***********************************************************************/

int main(int argc, char *argv[])
{
  bool on;
  double hot;
  double cold;
  ac_unit_sim_t unit(60.0, 0.35,
                10.0, 0.0);

  for (int i=0; true; i++)
  {
    on = unit.update(double(i), &hot, &cold);
    printf("%s: %0.1lf, %0.1lf\n", on ? "on" : "off", cold, hot);
    sleep(1);
  }
}

/***********************************************************************/

