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
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include <errno.h>
#include <math.h>

#include <pthread.h>

#include "rtcommon.h"
#include "rt_serial.h"
#include "spd_comm.h"
#include "ap_config.h"

#include "odo_mode.h"

static int serial_fd1 = -1;

/***************************************************************/
 char d4x40_trans_char(char ch)
{
  /* Version 2 */
  switch (ch)
  {
    case 'X': return('1'); /* 1 R1C1 */
    case 'U': return('2'); /* 2 R1C2 */
    case 'V': return('3'); /* 3 R1C3 */
    case 'S': return('4'); /* 4 R2C1 */
    case 'P': return('5'); /* 5 R2C2 */
    case 'Q': return('6'); /* 6 R2C3 */
    case 'N': return('7'); /* 7 R3C1 */
    case 'K': return('8'); /* 8 R3C2 */
    case 'L': return('9'); /* 9 R3C3 */
    case 'F': return('0'); /* 0 R4C2 */

    case 'W': return('f'); /* A R1C4 */
    case 'R': return('s'); /* B R2C4 */
    case 'M': return('a'); /* C R3C4 */
    case 'H': return('m'); /* D R4C4 */

    case 'I': return('q'); /* + R4C1 */
    case 'G': return('c'); /* - R4C3 */
  }
  return ' ';
}

static int key_hit = -1;

void *check_for_keys(void *param)
{
  key_hit = -1;
  tcflush(serial_fd1, TCIOFLUSH);
  while (true)
  {
    char buf[5];
    int n = read(serial_fd1, buf, sizeof(buf) - 1);
    if (n > 0)
    {
      key_hit = buf[n-1];
    }
  }
  return NULL; // should never get here
}

/***************************************************************/

int d4x40_printf(int row, int col, const char *fmt, ...)
{
  char buf[5];
  char myline[41];

  if (col > 80)
  {
    return 0;
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
  if (serial_fd1 == -1)
  {
    odo_init_screen();
  }

  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  int n = vsnprintf(myline, sizeof(myline), fmt, arg_ptr);
  //printf("writing to display: '%s'\n", myline);
  myline[sizeof(myline) - 1] = '\0';
  va_end(arg_ptr);

  
  buf[0] = 0xFE;  // goto xy
  buf[1] = 0x47;
  buf[2] = col; // column
  buf[3] = row; // row

  write(serial_fd1, buf, 4);
  write(serial_fd1, myline, strlen(myline));
  return n;
}

/***************************************************************/

void initialize_all(void)
{
  char buf[10];

  buf[0] = 0xFE;
  buf[1] = 'h'; // Horizontal bar graph mode. 
  write(serial_fd1, buf, 2);
  buf[1] = 'E'; // Clear keypad buffer. 
  write(serial_fd1, buf, 2);
  buf[1] = 'D'; // Auto Line Wrap Off. 
  write(serial_fd1, buf, 2);
  buf[1] = 'R'; // Auto Scroll Off. 
  write(serial_fd1, buf, 2);
  buf[1] = 'A'; // Auto transmit keypress on. 
  write(serial_fd1, buf, 2);
  buf[1] = 'X'; // Clear the screen. 
  write(serial_fd1, buf, 2);
}

/*********************************************************************/

void odo_redraw_all(void)
{
  initialize_all();
}

/*******************************************************************/

int xodo_wait_key(const char *msg)
{
  return -1;
}

/*******************************************************************/

int odo_wait_key(const char *msg)
{
  return -1;
}

/*********************************************************************/

void odo_init_screen()
{
  char buf[10];
  //unsigned char buf2[10]; 
  //unsigned char buf3[10]; 
  //char buf4[10]; 

  if (serial_fd1 != -1)
  {
    return;
  }

  const char *serial_device = ap_config.get_config("4x40_serial_dev", "/dev/ttyT8S0");
  int baudrate = ap_config.get_int("4x40_baudrate", 19200);

  //serial_fd1 = rt_open_serial("/dev/ttyUSB0", 19200, 0);
  //serial_fd1 = rt_open_serial("/dev/ttyTS0", 19200, 0); // COM3 on the board.
  serial_fd1 = rt_open_serial(serial_device, baudrate, 0); 

  buf[0] = 0xFE;
  buf[1] = 'h'; // Horizontal bar graph mode. 
  write(serial_fd1, buf, 2);
  buf[1] = 'E'; // Clear keypad buffer. 
  write(serial_fd1, buf, 2);
  buf[1] = 'D'; // Auto Line Wrap Off. 
  write(serial_fd1, buf, 2);
  buf[1] = 'R'; // Auto Scroll Off. 
  write(serial_fd1, buf, 2);
  buf[1] = 'A'; // Auto transmit keypress on. 
  write(serial_fd1, buf, 2);
  buf[1] = 'X'; // Clear the screen. 
  write(serial_fd1, buf, 2);

  write(serial_fd1, "L1 PerifericoSur-Tesoro 45%", 27);

  buf[1] = '5'; // Read Serial Number. 
  write(serial_fd1, buf, 2);
  buf[1] = '6'; // Read Version Number. 
  write(serial_fd1, buf, 2);

  /***
  buf2[0] = 0xFE;  
  buf2[1] = 0x7C; // horizontal bar graph. 
  buf2[2] = 8;
  buf2[3] = 3;
  buf2[4] = 0;

  buf3[0] = 0xFE;
  buf3[1] = 0x7C; // horizontal bar graph. 
  buf3[2] = 8;
  buf3[3] = 4;
  buf3[4] = 0;

  buf4[0] = 0xFE;  
  buf4[1] = 0x7C; // horizontal bar graph. 
  buf4[2] = 8 + 23;
  buf4[3] = 4;
  buf4[4] = 1;
  ***/

  buf[0] = 0xFE;  // goto xy
  buf[1] = 0x47;
  buf[2] = 1; // column
  buf[3] = 3; // row

  initialize_all();

  pthread_t thr;
  int retval = pthread_create(&thr, NULL, check_for_keys, NULL);
  if (retval != 0)
  {
    perror("can't create thread");
    odo_endwin(); exit(0);
  }
}

/**********************************************************************/

void odo_print_current(double speed, double distance, long total_count, long current_count)
{ 
  d4x40_printf(1,2,"Speed: %12.1lf", speed);
  d4x40_printf(2,2,"Dist.: %12.1lf", distance);
  d4x40_printf(3,2,"Count: %10d", total_count);
  d4x40_printf(4,2,"Count: %10d", current_count);
}

/*********************************************************************/

int odo_endwin(void)
{
  return 0;
}

/*********************************************************************/

int d4x40_getch(void)
{

  if (serial_fd1 == -1)
  {
    odo_init_screen();
  }
  if (key_hit != -1)
  {
    int temp = key_hit;
    key_hit = -1;
    int tch = d4x40_trans_char(temp);
    printf("got a key: %d %c\n", temp, tch);
    return tch;
  }
  return -1;
}



int odo_getch(void)
{
  return d4x40_getch();
}

/*********************************************************************/


