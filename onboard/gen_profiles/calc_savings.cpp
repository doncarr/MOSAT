

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const double accel = 1.0;

int main(int argc, char *argv[])
{
  if (argc < 4)
  {
     printf("You must enter two speeds and a distance\n");
     exit(0);
  }

  double v1 = atof(argv[1]) / 3.6;
  double v2 = atof(argv[2]) / 3.6;
  printf("v1 = %0.1lf m/s, v2 = %0.1lf m/s\n", v1, v2);
  double d = atof(argv[3]);
  double time_accelerating = (v2-v1) / accel;  
  double distance_accelerating = ((v2*v2) - (v1*v1)) / (2.0 * accel);  
  double time_at_v2 = (d - (2.0 * distance_accelerating))  / v2;
  double time_no_accel = d / v1;
  double time_with_accel = (2.0 * time_accelerating) + time_at_v2;
  printf("t1 = %0.1lf, t2 = %0.1lf, savings = %0.1lf\n", time_no_accel, time_with_accel, time_no_accel - time_with_accel);
  double yearly_savings = 150 * (time_no_accel - time_with_accel) * 365.0;
  yearly_savings /= (60.0 * 60.0 * 24);
  printf("Yearly savings (all trains) = %0.1lf days\n", yearly_savings);
  printf("Time coasting at v2 = %0.1lf\n", time_at_v2);
  double max_v2 = sqrt((d*accel) + (v1*v1));
  double min_time = 2.0 * ((max_v2 - v1) / accel);
  printf("max V2 = %0.1lf km/h, min time = %0.1lf, max_savings = %0.1lf\n", max_v2 * 3.6, min_time, time_no_accel - min_time);
}
