/***************************************************************************
Copyright (c) 2002,2003,2004 Donald Wayne Carr
                                                                                                                             
Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:
                                                                                                                             
The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.
                                                                                                                             
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include <errno.h>
#include <math.h>

#include <pthread.h>

#include "rtcommon.h"
#include "rt_serial.h"


static int serial_fd = -1;
void spd_init_screen();

/*********************************************************************/

void horizontal_bar_graph(int row, int col, int pix)
{
  char buf[10];

  buf[0] = 0xFE;
  buf[1] = 0x7C;
  buf[2] = col;
  buf[3] = row;
  buf[4] = 0;
  buf[5] = pix;
  write(serial_fd, buf, 6);
}

/*********************************************************************/

void d4x40_cls(void)
{
  char buf[5];

  buf[0] = 0xFE; // command byte 
  buf[1] = 'X';  // Clear the screen. 
  write(serial_fd, buf, 2);
}

/***************************************************************/

int scr_goto_xy(int row, int col)
{
  char buf[5];

  buf[0] = 0xFE;  // goto xy
  buf[1] = 0x47;
  buf[2] = col; // column
  buf[3] = row; // row

  if (col > 40)
  {
    col = 40;
  }
  if (col < 1)
  {
    col = 1;
  }
  if (row > 4)
  {
    row = 4;
  }
  if (row < 1)
  {
    row = 1;
  }
  if (serial_fd == -1)
  {
    spd_init_screen();
  }
  write(serial_fd, buf, 4);
}

/***************************************************************/

int d4x40_printf(int row, int col, const char *fmt, ...)
{
  char myline[41];

  scr_goto_xy(row, col);

  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  int n = vsnprintf(myline, sizeof(myline), fmt, arg_ptr);
  //printf("writing to display: '%s'\n", myline);
  myline[sizeof(myline) - 1] = '\0';
  va_end(arg_ptr);
  
  write(serial_fd, myline, strlen(myline));
  return n;
}

/***************************************************************/

char map_char(char a_char)
{
  switch (a_char)
  {
    case 'F': return '0';
    case 'X': return '1';
    case 'U': return '2';
    case 'V': return '3';
    case 'S': return '4';
    case 'P': return '5';
    case 'Q': return '6';
    case 'N': return '7';
    case 'K': return '8';
    case 'L': return '9';
    case 'W': return 'A';
    case 'R': return 'B';
    case 'M': return 'C';
    case 'H': return 'D';
    case 'I': return '+';
    case 'G': return '-';
    default: return '?';
  }
}

/***************************************************************/
//static const char *serial_device = "/dev/ttyT8S0";
static const char *serial_device = "/dev/ttyUSB0";

void spd_init_screen()
{
  char buf[10];

  if (serial_fd != -1)
  {
    return;
  }
  printf("Opening %s\n", serial_device);
  serial_fd = rt_open_serial(serial_device, 19200, 0);
  printf("serial_fd = %d\n", serial_fd);
  //serial_fd = rt_open_serial("/dev/ttyUSB0", 19200, 0);
  //serial_fd = rt_open_serial("/dev/ttyTS0", 19200, 0); // COM3 on the board.
  //serial_fd = rt_open_serial("/dev/ttyT8S5", 19200, 0); // COM3 on the board.
  buf[0] = 0xFE;
  buf[1] = 'h'; // Horizontal bar graph mode. 
  write(serial_fd, buf, 2);
  buf[1] = 'E'; // Clear keypad buffer. 
  write(serial_fd, buf, 2);
  buf[1] = 'D'; // Auto Line Wrap Off. 
  write(serial_fd, buf, 2);
  buf[1] = 'R'; // Auto Scroll Off. 
  write(serial_fd, buf, 2);
  buf[1] = 'A'; // Auto transmit keypress on. 
  write(serial_fd, buf, 2);
  buf[1] = 'X'; // Clear the screen. 
  write(serial_fd, buf, 2);


};

/*********************************************************************/

int main(int argc, char *argv[])
{
  char buf[10];
 
  if (argc > 1)
  {
    serial_device = argv[1];
  }
  spd_init_screen();
  
  buf[0] = 0xFE;
  buf[1] = 0x57;  
  buf[2] = 0x01;
  write(serial_fd, buf, 3);
  sleep(1);
  buf[1] = 0x56; 
  write(serial_fd, buf, 3);
  printf("printing line 1 . . .\n");
  d4x40_printf(1, 1, "Hello SITEUR: %lf, %c buzzer off", 99.9, 'X');
  sleep(1);
  buf[1] = 0x57; 
  write(serial_fd, buf, 3);
  printf("printing line 2 . . .\n");
  d4x40_printf(2, 1, "This is cool, buzzer ON");
  sleep(1);
  buf[1] = 0x56; 
  write(serial_fd, buf, 3);
  printf("printing line 3 . . .\n");
  d4x40_printf(3, 1, "This is line three, buzzer off");
  sleep(1);
  buf[1] = 0x57;
  write(serial_fd, buf, 3);
  printf("printing line 4 . . .\n");
  d4x40_printf(4, 1, "This is line four, buzzer ON");
  sleep(1);
  buf[1] = 0x56;
  write(serial_fd, buf, 3);

  sleep(1);
  d4x40_cls();

  void d4x40_cls(void);
  for (int i=0; i <= 100; i++)
  {
    horizontal_bar_graph(3, 8, i);
    usleep(8000);
  }
  sleep(1);
  for (int i=100; i >= 0; i--)
  {
    horizontal_bar_graph(3, 8, i);
    usleep(8000);
  }

  sleep(1);
  d4x40_printf(4, 1, "Jugar con el Teclado!!!");
  scr_goto_xy(1, 1);

  while (true)
  {
    char buf[5];
    printf("Ok, type something on the funny little touchpad\n");
    int n = read(serial_fd, buf, sizeof(buf) - 1);
    if (n > 0)
    {
      char mych = map_char(buf[n-1]);
      printf("Char recieved: %c, mapped to %c\n", buf[n-1], mych);
      write(serial_fd, &mych, 1); 
    }
  } 
  close(serial_fd);

}


