/************************************************************************
This software is part of React, a control engine
Copyright (C) 2005,2006 Donald Wayne Carr 

This program is free software; you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation; either version 2 of the License, or 
(at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You should have received a copy of the GNU General Public License along 
with this program; if not, write to the Free Software Foundation, Inc., 
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
***********************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "conio.h"

static char last_char = '\0';
static bool have_char = false;
static bool flag = true;
static  struct termio ttysave;
static  struct termio tty;

/******************/

int mygetch(void)
{
  // gets one key stroke.
  if (have_char)
  {
    have_char = false;
    return (int) last_char;
  }
  char ch;
  int total;
  /****
  if (flag)
  {
    printf("Initializing conio\n");
    flag = false;
    if( ioctl( 0, TCGETA, &tty) == -1 )
    {
      perror("ioctl");
      exit(1);
    }
    ttysave = tty;
    tty.c_lflag &= ~(ICANON | ECHO | ISIG );
    tty.c_cc[VMIN] =1;
    tty.c_cc[VTIME] =0;
    if( ioctl(0, TCSETAF, &tty ) == -1 )
    {
      coniorestore();
      perror("ioctl");
      exit(2);
    }
  }
  ***/
  switch( total = read( 0, &ch, 1 ))
  {
    case -1:
      coniorestore();
      perror("read");
      exit(3);
    case 0:
      coniorestore();
      fputs("EOF error!", stderr );
      exit(4);
    default:
      ;
  }
  //coniorestore(&ttysave);
  return (int) ch;
}

/******************/

bool kb_hit(void)
{
  if (have_char == true)
  {
    return true;
  }
  int total;
  bool retval = false;
  char temp_char;

  switch( total = read( 0, &temp_char, 1 ))
  {
    case -1:
      coniorestore();
      perror("read");
      exit(3);
    case 0:
      retval = false;
      break;
    default:
      retval = true;
      break;
  }

  //coniorestore(&ttysave);
  if (retval == true)
  {
    have_char = true;
    last_char = temp_char;
  }
  return retval;
}

/*********************/

void conioinit(void)
{
  if (flag)
  {
    printf("Initializing conio\n");
    flag = false;
    if( ioctl( 0, TCGETA, &tty) == -1 )
    {
      perror("ioctl");
      exit(1);
    }
    ttysave = tty;
    tty.c_lflag &= ~(ICANON | ECHO | ISIG );
    tty.c_cc[VMIN] =0;
    tty.c_cc[VTIME] =1;
    if( ioctl(0, TCSETAF, &tty ) == -1 )
    {
      coniorestore();
      perror("ioctl");
      exit(2);
    }
  }
}

/*********************/

void coniorestore(void)
{
  printf("conio_restore called . . \n");
  if (!flag)
  {
    printf("Restoring conio to original state . . \n");
    if( ioctl(0, TCSETAF, &ttysave) == -1 )
    {
      perror("ioctl");
      exit(5);
    }
  }
}


/******************

int main(int argc, char *argv[])
{
  char ch;
  while (1)
  {
    if (kb_hit())
    {
      int ch = mygetch();
      printf("You pressed: %c\n", ch);
      if (ch == 'q')
      {
	break;
      }
    }
  }
  coniorestore();

}
*******/
