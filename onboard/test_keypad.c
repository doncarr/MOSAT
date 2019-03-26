
#include <stdio.h>
#include "ts_keypad.h"


int main(int argc, char *argv[])
{
  while(1)
  {
    int the_key = keypad_getchar();
    printf("%c", the_key);
  }
}

