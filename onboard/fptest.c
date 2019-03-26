

#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv) 
{
  double d1, d2;
  d1  = strtod("25.0", NULL);
  printf("25.0 = %10.10f\n",d1);
  d2  = atof("25.0");
  printf("25.0 = %10.10f\n",d2);
  return 0;
}
