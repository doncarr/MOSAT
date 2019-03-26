
#include <stdio.h>
#include <unistd.h>
#include "d2x24.h"

int main(int argc, char *argv[])
{
  while(1)
  {
    d2x24_printf(0, "%s", "1234567890!@#$%*()_+<>?\\");
    d2x24_printf(1, "%s", "abcdefghigklmnopqrstuvwx");
    sleep(2);
    d2x24_printf(0, "%s", "ABCDEFGHIJKLMNOPQRSTUVWX");
    d2x24_printf(1, "%s", "yzYZ,./[]{}|~\"1234567890");
    sleep(2);
  }
}
