
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  int retval = system(argv[1]);
  printf("return val = %d\n", retval);
}
