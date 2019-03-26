
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "ts_keypad.h"
#include "d2x24.h"

/**************************************************************/

int wait_key(char *msg)
{
  int mych;
  d2x24_printf(1,"%-24s", msg);
  while (-1 != d2x24_getch());

  while (1)
  {
    mych = d2x24_getch();
    if ((mych ==  'q') || (mych == 'Q')) {d2x24_end_screen(); exit(0);}
    if (mych != -1)
    {
      break;
    }
    usleep(400000);
    d2x24_printf(1,"%-24s", " ");
    mych = d2x24_getch();
    if ((mych ==  'q') || (mych == 'Q')) {d2x24_end_screen(); exit(0);}
    if (mych != -1)
    {
      break;
    }
    usleep(400000);
    d2x24_printf(1, msg);
  }
  return mych;
}


/**************************************************************/

int main(int argc, char *argv[])
{
  int mych = ' ';
  while(1)
  {
    d2x24_printf(0, "%s", "SITEUR Guadalajara  CBTC");
    usleep(800000);
    mych = wait_key("Hit a key");
    d2x24_printf(1,"You hit %c", mych);
    d2x24_printf(0, "%s", "1234567890!@#$%*()_+<>?\\");
    usleep(800000);
    mych = wait_key("Hit a key");
    d2x24_printf(1,"You hit %c", mych);
    d2x24_printf(0, "%s", "abcdefghigklmnopqrstuvwx");
    usleep(800000);
    mych = wait_key("Hit a key");
    d2x24_printf(1,"You hit %c", mych);
    d2x24_printf(0, "%s", "ABCDEFGHIJKLMNOPQRSTUVWX");
    usleep(800000);
    mych = wait_key("Hit a key");
    d2x24_printf(1,"You hit %c", mych);
    d2x24_printf(0, "%s", "yzYZ,./[]{}|~\"1234567890");
    usleep(800000);
    mych = wait_key("Hit a key");
    d2x24_printf(1,"You hit %c", mych);
  }
}


