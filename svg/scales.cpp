

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void calc_scales(double max, double *max_val, double *max_major, double *major_inc, double *minor_inc, int *n_major, int *n_minor, int *n_dec)
{
  double the_log = log10(max);
  int the_floor = floor(the_log);
  //printf("log = %lf\n", the_log);
  //printf("floor = %d\n", the_floor);
  double z99 = max / pow(10.0, double(the_floor - 1)); 
  //printf("z99 = %lf\n", z99);
  int the_scale;
  int top_major;
  if (z99 >= 97.0) 
  {
    the_scale = 100;
    top_major = 100;
    *n_major = 5;
    *n_minor = 4;
  } 
  else if (z99 >= 80.0) 
  {
    the_scale = ceil(z99);
    top_major = 80;
    *n_major = 4;
    *n_minor = 4;
  }
  else if (z99 >= 60.0) 
  {
    the_scale = ceil(z99);
    top_major = 60;
    *n_major = 3;
    *n_minor = 4;
  }
  else if (z99 >= 50.0) 
  {
    the_scale = ceil(z99);
    top_major = 50;
    *n_major = 5;
    *n_minor = 4;
  }
  else if (z99 >= 40.0) 
  {
    the_scale = ceil(z99);
    top_major = 40;
    *n_major = 4;
    *n_minor = 4;
  }
  else if (z99 >= 35.0) 
  {
    the_scale = ceil(z99);
    top_major = 35;
    *n_major = 7;
    *n_minor = 2;
  }
  else if (z99 >= 30.0) 
  {
    the_scale = ceil(z99);
    top_major = 30;
    *n_major = 3;
    *n_minor = 4;
  }
  else if (z99 >= 25.0) 
  {
    the_scale = ceil(z99);
    top_major = 25;
    *n_major = 5;
    *n_minor = 2;
  }
  else if (z99 >= 20.0) 
  {
    the_scale = ceil(z99);
    top_major = 20;
    *n_major = 4;
    *n_minor = 5;
  }
  else if (z99 >= 18.0) 
  {
    the_scale = ceil(z99);
    top_major = 18;
    *n_major = 6;
    *n_minor = 3;
  }
  else if (z99 >= 15.0) 
  {
    the_scale = ceil(z99);
    top_major = 15;
    *n_major = 5;
    *n_minor = 3;
  }
  else if (z99 >= 14.0) 
  {
    the_scale = ceil(z99);
    top_major = 14;
    *n_major = 7;
    *n_minor = 2;
  }
  else if (z99 >= 12.0) 
  {
    the_scale = ceil(z99);
    top_major = 12;
    *n_major = 6;
    *n_minor = 2;
  }
  else
  {
    the_scale = ceil(z99);
    top_major = 10;
    *n_major = 5;
    *n_minor = 2;
  }
  //printf("top = %d\n", the_scale);
  //printf("top major = %d\n", top_major);
  //printf("N major = %d\n", *n_major);
  //printf("N minor = %d\n", *n_minor);
  *n_dec = 0; 
  if (the_floor < 1)
  {
    *n_dec = (-the_floor) + 1;
  }

  *major_inc = (double (top_major) / double(*n_major)) *  pow(10.0, double(the_floor - 1)); 
  *minor_inc = (double (top_major) / double(*n_major * *n_minor)) *  pow(10.0, double(the_floor - 1)); 
  *max_val = double(the_scale) *  pow(10.0, double(the_floor - 1)); 
  *max_major = double(top_major) *  pow(10.0, double(the_floor - 1)); 

}

