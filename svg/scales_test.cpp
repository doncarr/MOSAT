

#include <stdio.h>
#include <stdlib.h>

void calc_scales(double max, double *max_val, double *major_inc, double *minor_inc, int *n_major, int *n_minor, int *n_dec);


int main(int argc, char *argv[])
{
  double max, max_val, major_inc, minor_inc;
  int n_major, n_minor, n_dec;

  max = atof(argv[1]); 
  calc_scales(max, &max_val, &major_inc, &minor_inc, &n_major, &n_minor, &n_dec);

  printf("----------------------------------------\n");
  printf("N major = %d\n", n_major);
  printf("N minor = %d\n", n_minor);
  printf("N dec = %d\n", n_dec);
  printf("major_inc = %0.1lf\n", major_inc);
  printf("minor_inc = %0.1lf\n", minor_inc);
  printf("max_val = %0.1lf\n", max_val);
  printf("max = %0.1lf\n", max);

  double major_val = 0.0;
  for (int i=0; i <= n_major; i++)
  {
    printf("---- %0.*lf\n", n_dec, major_val); 
    double minor_val = major_val;
    major_val += major_inc;
    for (int j=1; j < n_minor; j++)
    {
      minor_val += minor_inc; 
      if (minor_val <= max_val) printf("- %0.*lf\n", n_dec, minor_val); 
    }
  }

}

