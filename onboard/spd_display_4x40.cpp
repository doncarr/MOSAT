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
#include "rtmodbus.h"
#include "rt_serial.h"
#include "spd_display.h"

#include "ap_config.h"

static char station1[20];
static char station2[20];
static bool doors_open = false;


static int serial_fd1 = -1;

/*********************************************************************/
 
void spd_create_image(const char *base_name, const char *gtitle, bool window)
{
}

/*********************************************************************/

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

  //write(serial_fd1, "L1 PerifericoSur-Tesoro 45%", 27);

/***
  buf[1] = '5'; // Read Serial Number. 
  write(serial_fd1, buf, 2);
  buf[1] = '6'; // Read Version Number. 
  write(serial_fd1, buf, 2);
****/

/****
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

  buf[0] = 0xFE;  // goto xy
  buf[1] = 0x47;
  buf[2] = 1; // column
  buf[3] = 3; // row
****/
}

/*********************************************************************/

void draw_numbers(void)
{
  char buf[10];
  char str[10];
  char buf2[1]; 
  buf[0] = 0xFE;  // goto xy
  buf[1] = 0x47;
  buf[2] = 1; // column
  buf[3] = 2; // row

  for (int i=1; i < 6; i++)  
  {
    if (i < 5)
    {
      snprintf(str, sizeof(str), "%2d", (i * 2 * 10) - 10);
      buf[2] = 5 + (i*4);
      write(serial_fd1, buf, 4);
      write(serial_fd1, str, 2);
    }

    buf[2] = 4 + (i*4);
    write(serial_fd1, buf, 4);
    buf2[0] = 1;
    write(serial_fd1, buf2, 1);
    buf[2] = 3 + (i*4);
    write(serial_fd1, buf, 4);
    buf2[0] = 5;
    write(serial_fd1, buf2, 1);
  }
}

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
  /* Versoin 1
  switch (ch)
  {
    case 19: return('1');
    case 52: return('2');
    case 41: return('3');
    case 20: return('4');
    case 53: return('5');
    case 42: return('6');
    case 21: return('7');
    case 54: return('8');
    case 43: return('9');
    case 55: return('0');

    case 30: return('f');
    case 31: return('s');
    case 32: return('a');
    case 33: return('m');

    case 22: return('q');
    case 44: return('c');
  }
  ****/
  return ' ';
}

/***************************************************************/
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

int d4x40_getch(void)
{
  //printf("d4x40_getch();\n");
  //char ch = 'm';
  //return ch;

  if (serial_fd1 == -1)
  {
    spd_init_screen();
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

/***************************************************************/

void d4x40_cls(void)
{
  char buf[5];

  buf[0] = 0xFE; // command byte 
  buf[1] = 'X';  // Clear the screen. 
  write(serial_fd1, buf, 2);
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
    spd_init_screen();
  }

  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  int n = vsnprintf(myline, sizeof(myline), fmt, arg_ptr);
  //printf("writing to display: '%s'\n", myline);
  myline[sizeof(myline) - 1] = '\0';
  va_end(arg_ptr);

  
  buf[0] = 0xFE; 
  buf[1] = 0x47; // goto xy
  buf[2] = col; // column
  buf[3] = row; // row

  write(serial_fd1, buf, 4);
  write(serial_fd1, myline, strlen(myline));
  return n;
}

/***************************************************************/

/*******************************************************************/

int spd_prompt_user(const char *msg1, const char *msg2)
{
  int mych;
  printf("In 4x40 prompt user\n");
  d4x40_printf(1,1,"%-40s", msg1);
  d4x40_printf(2,1,"%-40s", msg2);

  while (-1 != d4x40_getch());

  while (1)
  {
    mych = d4x40_getch();
    if ((mych ==  'q') || (mych == 'Q')) {spd_endwin(); exit(0);}
    if (mych != -1)
    {
      break;
    }
    usleep(200000);
  }
  d4x40_printf(1,1,"%-40s", " ");
  d4x40_printf(2,1,"%-40s", " ");
  draw_numbers();
  return mych;
}

/*******************************************************************/

int spd_wait_key(const char *msg)
{
  int mych;
  d4x40_printf(1,1,"%-36s", msg); 

  while (-1 != d4x40_getch());
  
  while (1)
  {
    mych = d4x40_getch();
    if ((mych ==  'q') || (mych == 'Q')) {spd_endwin(); exit(0);}
    if (mych != -1)
    {
      break;
    } 
    usleep(400000);
    d4x40_printf(1,1,"%-36s", " "); 

    mych = d4x40_getch();
    //if ((mych ==  'q') || (mych == 'Q')) {spd_endwin(); exit(0);}
    if (mych != -1)
    {
      break;
    } 
    usleep(400000);
    d4x40_printf(1,1, msg); 
  }
  d4x40_printf(1,1,"%-36s", " "); 
  draw_numbers();
  return mych;
}

/*********************************************************************/

void spd_redraw_segment(void)
{
  char st[11];

  snprintf(st, sizeof(st), "%10s", station1);
  d4x40_printf(1,1,"%s", st);
  snprintf(st, sizeof(st), "%-10s", station2);
  d4x40_printf(1,31,"%s", st);
  d4x40_printf(2,39,"%c", doors_open ? 'O' : '-');

  //d4x40_printf(1,6,"%s-%s", station1, station2);
   
}

/*********************************************************************/

void spd_init_segment(const char *st1, const char *st2)
{
  safe_strcpy(station1, st1, sizeof(station1));
  safe_strcpy(station2, st2, sizeof(station2));
  spd_redraw_segment();
}
                                                                                
/*********************************************************************/

void spd_init_screen()
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
  **/

  buf[0] = 0xFE;  // goto xy
  buf[1] = 0x47;
  buf[2] = 1; // column
  buf[3] = 3; // row

  initialize_all();
  draw_numbers();

  pthread_t thr;
  int retval = pthread_create(&thr, NULL, check_for_keys, NULL);
  if (retval != 0)
  {
    perror("can't create thread");
    spd_endwin(); exit(0);
  }

/********
  char str[10];
  for (int i=0; i <= 80; i++)
  {
    buf2[5] = i;
    buf3[5] = i;
    buf4[5] = 120 - i - 5;
    write(serial_fd, buf2, 6);
    write(serial_fd, buf3, 6);
    //write(serial_fd, buf4, 6);

    snprintf(str, sizeof(str), "Act %2d", i);
    buf[3] = 3;
    write(serial_fd, buf, 4);
    write(serial_fd, str, 6);

    snprintf(str, sizeof(str), "Des %2d", i);
    buf[3] = 4;
    write(serial_fd, buf, 4);
    write(serial_fd, str, 6);

    if (((i + 10) %10) == 0)
    {
      sleep(1);
    }
    usleep(100000);
  }
  for (int i=80; i >= 0; i--)
  {
    buf2[5] = i;
    buf3[5] = i;
    buf4[5] = 120 - i - 5;
    write(serial_fd, buf2, 6);
    write(serial_fd, buf3, 6);
    //write(serial_fd, buf4, 6);
    snprintf(str, sizeof(str), "Act %2d", i);
    buf[3] = 3;
    write(serial_fd, buf, 4);
    write(serial_fd, str, 6);

    snprintf(str, sizeof(str), "Des %2d", i);
    buf[3] = 4;
    write(serial_fd, buf, 4);
    write(serial_fd, str, 6);

    usleep(100000);
  }

  ********/

  //spd_redraw_all();
};

/*********************************************************************/

int mainxx(int argc, char *argv[])
{
  spd_init_screen();

  while (1)
  {
    int buf[5];
    int n = read(serial_fd1, buf, sizeof(buf) - 1);
    if (n > 0)
    {
      for (int i=0; i < n; i++)
      {
        buf[i] &= 0x7F;
        printf("%d 0x%02x\t", (int) buf[i], (int) buf[i]);
        if (buf[i] == 0x16) {spd_endwin(); exit(0);}
      }
      printf("\n");
      buf[n] = '\0';
    }
  }
}

/*********************************************************************/

void spd_redraw_all()
{
  initialize_all();
  draw_numbers();
}

/**********************************************************************/

void horizontal_bar_graph(int row, int col, int pix)
{
  char buf[10];

  buf[0] = 0xFE;
  buf[1] = 0x7C; // horizontal bar graph.
  buf[2] = col;
  buf[3] = row;
  buf[4] = 0;
  buf[5] = pix;
  write(serial_fd1, buf, 6);
}


/**********************************************************************/

void buzzer_off(void)
{
  char buf[3];
  buf[0] = 0xFE;
  buf[1] = 0x56;
  buf[2] = 0x01;
  write(serial_fd1, buf, 3);
}

/**********************************************************************/
static bool buzzon = false;

void buzzer_toggle(void)
{
  char buf[3];
  buf[0] = 0xFE;
  buf[1] = 0x57;
  buf[2] = 0x01;
  if (!buzzon) buf[1] = 0x56;
  buzzon = !buzzon;
  write(serial_fd1, buf, 3);
}

/**********************************************************************/

void spd_print_current(double desired, double actual, int type, bool warn, 
      double pct, double total_distance, double now, double total_time,  spd_discrete_t *discretes)
{ 
  doors_open = discretes->doors_open;

  if (actual > 99) actual = 99;
  if (desired > 99) desired = 99;
  if (actual < 0) actual = 0;
  if (desired < 0) desired = 0;

  if (warn)
  {
    buzzer_toggle();
  }
  else
  {
    buzzer_off();
  }

  d4x40_printf(3,1,"De %04.1lf", desired);
  d4x40_printf(4,1,"Ac %04.1lf", actual);

  int act_pix = (int) (actual + 0.5);
  int des_pix = (int) (desired + 0.5);

  /****
  buf[0] = 0xFE;
  buf[1] = 0x7C; // horizontal bar graph. 
  buf[2] = 8;
  buf[3] = 3;
  buf[4] = 0;
  buf[5] = des_pix;
  write(serial_fd1, buf, 6);

  buf[3] = 4;
  buf[5] = act_pix;
  write(serial_fd1, buf, 6);
  ****/
  horizontal_bar_graph(3, 8, des_pix);
  horizontal_bar_graph(4, 8, act_pix);

  const char *accel_str;
  if (type == 0)
  {
    accel_str = "<<<";
  }
  else if (type == 2)
  {
    accel_str = ">>>";
  }
  else
  {
    accel_str = " - ";
  }
  d4x40_printf(3,30,"%s", accel_str);

  d4x40_printf(2,30,"%3.lf%%", pct);
   
  horizontal_bar_graph(1, 11, (int) (pct + 0.5));
  spd_redraw_segment(); // bargraph erases the rest of the line!

  double left = total_distance * (1.0 - (pct/100.0));
  if (left < 1.0) left = 0.0;

  //mvprintw(12,2,"Tiempo: %3.0lf, Distancia: %6.1lf", 
   //        now, (pct/100.0) * total_distance);
  //if (COLS < 70) mvprintw(13,2, "");
  //else printw(", ");
  //printw("Falta: %4.0lf, Porciento: %3.0lf%%", left, pct);

}

/*********************************************************************/

void spd_show_loading(int time)
{
  //d4x40_printf(1,1,"Cargando y Descargando Pasajeros");
  for (int i=0; i < time; i++)
  {
    d4x40_printf(1,1,"%-30s", "En estacion");
    usleep(500000);
    d4x40_printf(1,1,"%-30s", " ");
    usleep(500000);
  }
  d4x40_printf(1,1,"%-30s", " ");
  draw_numbers();
}

/*********************************************************************/

void spd_print_auto(bool auto_mode)
{
  if (auto_mode)
  {
    d4x40_printf(2,40,"A");
  }
  else
  {
    d4x40_printf(2,40," ");
  }
}

/*********************************************************************/

int spd_endwin(void)
{
  d4x40_cls();
  d4x40_printf(1,1,"Exiting . . .");
  printf("Exiting . . .");
  sleep(1);
  close(serial_fd1);
  return 0;
}

/*********************************************************************/
int spd_beep(void)
{
 /**
  char buf[2];
  buf[0] = 0xFE; 
  buf[1] = 'V'; // GPO On
  write(serial_fd1, buf, 2);
  buf[1] = 'W'; // GPO On
  write(serial_fd1, buf, 2);
  ***/
  return 0;
}
/*********************************************************************/
int spd_getch(void)
{
  return d4x40_getch();
}

/*********************************************************************/

void spd_create_image(char *base_name, char *gtitle, bool window)
{
  const char *fname = "gnuplotoptions.txt";
  FILE *fp = fopen(fname, "w");
  if (fp == NULL)
  {
    perror(fname);
    return;
  }
  fprintf(fp, "set   autoscale\n");
  fprintf(fp, "unset log\n");
  fprintf(fp, "unset label\n");
  fprintf(fp, "set xtic auto\n");
  fprintf(fp, "set ytic auto\n");
  fprintf(fp, "set grid\n");
  fprintf(fp, "set xlabel \"meters \"\n");
  fprintf(fp, "set ylabel \"km/h\"\n");

  fprintf(fp, "set title \"%s\"\n", gtitle);

  // The following line is basic black and white
  //fprintf(fp, "set terminal png size 800,600 xffffff x000000 x000000 x000000\n");
  // The following is default, probably black and white with red data
  //fprintf(fp, "set terminal png size 800,600\n");
  //The follow line is light blue background, blue font, magenta data line
  if (!window)
  {
    //fprintf(fp, "set terminal png size 1000,300 xadd8e6 x0000ff xdda0dd x9500d3\n");
    fprintf(fp, "set terminal png small color\n");
  }
  fprintf(fp, "%s\n", "set output\n");
  fprintf(fp, "plot \"%s.txt\" using 1:2 with lines lw 2 title \"actual\"", base_name);
  fprintf(fp, ", \"%s.txt\" using 1:3 with lines lw 2 title \"deseable\"\n", base_name);

  fclose(fp);
  char command[500];
  if (window)
  {
    snprintf(command, sizeof(command), "gnuplot -persist %s", fname);
  }
  else
  {
    snprintf(command, sizeof(command), "gnuplot %s > %s.png", fname, base_name);
  }
  system(command);
}

/*********************************************************************/

void spd_show_performance(const onboard_performance_t *perf)
{
  d4x40_cls();
  d4x40_printf(1, 1, "----------- Esta Segmento -------------");
  d4x40_printf(2, 1, "Typ: 98, Act: 104, -6 segundos      ");
  d4x40_printf(3, 1, "--------------- Total -----------------");
  d4x40_printf(4, 1, "Typ: 22:35, Act: 23:09, -34 segundos ");
}
    
/*********************************************************************/

