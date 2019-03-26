
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>

#include "d2x24.h"
#include "ts_keypad.h"


#define GPIOBASE	0x80840000
#define PADR	0
#define PADDR	(0x10 / sizeof(unsigned int))
#define PHDR	(0x40 / sizeof(unsigned int))
#define PHDDR	(0x44 / sizeof(unsigned int))

// These delay values are calibrated for the EP9301 
// CPU running at 166 Mhz, but should work also at 200 Mhz
#define SETUP	15
#define PULSE	36
#define HOLD	22

#define COUNTDOWN(x)	asm volatile ( \
  "1:\n"\
  "subs %1, %1, #1;\n"\
  "bne 1b;\n"\
  : "=r" ((x)) : "r" ((x)) \
);
//#define COUNTDOWN(x)

#define TV_ELAPSED_US(x, y)     ((((x).tv_sec - (y).tv_sec) * 1000000) + \
        ((x).tv_usec - (y).tv_usec))

static int d2x24_init = 0;
static struct timeval last_display_time;

volatile unsigned int *gpio;
volatile unsigned int *phdr;
volatile unsigned int *phddr;
volatile unsigned int *padr;
volatile unsigned int *paddr;

void command(unsigned int);
void writechars(char *);
unsigned int lcdwait(void);
void lcdinit(void);

/* This program takes lines from stdin and prints them to the
 * 2 line LCD connected to the TS-7200 LCD header.  e.g
 *
 *    echo "hello world" | lcdmesg
 * 
 * It may need to be tweaked for different size displays
 */

int xxmain(int argc, char **argv) {
	int i = 0;

	lcdinit();
	if (argc == 2) {
		writechars(argv[1]);
	}
	if (argc > 2) {
		lcdwait();
		command(0xa8); // set DDRAM addr to second row
		writechars(argv[2]);
	}
	if (argc >= 2) return 0;

	while(!feof(stdin)) {
		char buf[512];

		lcdwait();
		if (i) {
			// XXX: this seek addr may be different for different
			// LCD sizes!  -JO
			command(0xa8); // set DDRAM addr to second row
		} else {
			command(0x2); // return home
		}
		i = i ^ 0x1;
		if (fgets(buf, sizeof(buf), stdin) != NULL) {
			unsigned int len;
			buf[0x27] = 0;
			len = strlen(buf);
			if (buf[len - 1] == '\n') buf[len - 1] = 0;
			writechars(buf);
		}
	}
	return 0;
}

void lcdinit(void) {
        gettimeofday(&last_display_time, NULL);
	int fd = open("/dev/mem", O_RDWR|O_SYNC);
        if (fd == -1)
        {
          perror("/dev/mem");
          return;
        }

	gpio = (unsigned int *)mmap(0, getpagesize(), 
	  PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIOBASE);
	phdr = &gpio[PHDR];
	padr = &gpio[PADR];
	paddr = &gpio[PADDR];
	phddr = &gpio[PHDDR];
	*paddr = 0x0;  // All of port A to inputs
	*phddr |= 0x38; // bits 3:5 of port H to outputs
	*phdr &= ~0x18; // de-assert EN, de-assert RS
	usleep(15000);
	command(0x38); // two rows, 5x7, 8 bit
	usleep(4100);
	command(0x38); // two rows, 5x7, 8 bit
	usleep(100);
	command(0x38); // two rows, 5x7, 8 bit
	command(0x6); // cursor increment mode
	lcdwait();
	command(0x1); // clear display
	lcdwait();
	command(0xc); // display on, blink off, cursor off
	lcdwait();
	command(0x2); // return home
}

unsigned int lcdwait(void) {
	int i, dat, tries = 0;
	unsigned int ctrl = *phdr;

	*paddr = 0x0; // set port A to inputs
	ctrl = *phdr;
	
	do {
		// step 1, apply RS & WR
		ctrl |= 0x30; // de-assert WR
		ctrl &= ~0x10; // de-assert RS
		*phdr = ctrl;

		// step 2, wait
		i = SETUP;
		COUNTDOWN(i);

		// step 3, assert EN
		ctrl |= 0x8;
		*phdr = ctrl;

		// step 4, wait
		i = PULSE;
		COUNTDOWN(i);

		// step 5, de-assert EN, read result
		dat = *padr;
		ctrl &= ~0x8; // de-assert EN
		*phdr = ctrl;

		// step 6, wait
		i = HOLD;
		COUNTDOWN(i);
	} while (dat & 0x80 && tries++ < 1000);
	return dat;
}

void command(unsigned int cmd) {
	int i;
	unsigned int ctrl = *phdr;

	*paddr = 0xff; // set port A to outputs
	
	// step 1, apply RS & WR, send data
	*padr = cmd;
	ctrl &= ~0x30; // de-assert RS, assert WR
	*phdr = ctrl;

	// step 2, wait
	i = SETUP;
	COUNTDOWN(i);

	// step 3, assert EN
	ctrl |= 0x8;
	*phdr = ctrl;

	// step 4, wait
	i = PULSE;
	COUNTDOWN(i);

	// step 5, de-assert EN	
	ctrl &= ~0x8; // de-assert EN
	*phdr = ctrl;

	// step 6, wait 
	i = HOLD;
	COUNTDOWN(i);
}

void writechars(char *dat) {
	int i;
	unsigned int ctrl = *phdr;

	do {
		lcdwait();
		*paddr = 0xff; // set port A to outputs

		// step 1, apply RS & WR, send data
		*padr = *dat++;
		ctrl |= 0x10; // assert RS
		ctrl &= ~0x20; // assert WR
		*phdr = ctrl;

		// step 2
		i = SETUP;
		COUNTDOWN(i);

		// step 3, assert EN
		ctrl |= 0x8;
		*phdr = ctrl;

		// step 4, wait 800 nS
		i = PULSE;
		COUNTDOWN(i);

		// step 5, de-assert EN	
		ctrl &= ~0x8; // de-assert EN
		*phdr = ctrl;

		// step 6, wait
		i = HOLD;
		COUNTDOWN(i);
	} while(*dat);
}


int d2x24_printf(int line, char *fmt, ...)
{
  struct timeval now;
  if (!d2x24_init)
  {
    d2x24_init = 1;
    lcdinit();
  }
  gettimeofday(&now, NULL);
  int elapsed_time = TV_ELAPSED_US(now, last_display_time);
  if (elapsed_time < 1000)
  {
    //printf("sleeping: %d\n", MIN_TIME_BETWEEN - elapsed_time);
    usleep(1000 - elapsed_time);
  }


  char myline[25];
  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  int n = vsnprintf(myline, sizeof(myline), fmt, arg_ptr);
  //printf("writing to display: '%s'\n", myline);
  myline[sizeof(myline) - 1] = '\0';
  va_end(arg_ptr);
  if (line == 0) // Make sure line is 0 or 1
  {
    //printf("FIRST Line\n\n");
    command(0x2); // return home
  }
  else
  {
    //printf("Second Line Line\n\n");
    command(0xa8); // set DDRAM addr to second row
  }
  writechars(myline);
  //mvprintw(line,0,"%-24s", myline);
  gettimeofday(&last_display_time, NULL);
  return n;
}

/*********************************************************/

void d2x24_init_screen(void)
{
  lcdinit();
}

/*********************************************************/

int d2x24_end_screen()
{
  return 0; 
}

/*********************************************************/

int d2x24_getch(void)
{
  int return_key;
  //printf("Waiting key . .\n");
  int key = keypad_getchar();
  //printf("Pressed %c . .\n", key);
  switch (key)
  {
    case 'A': return_key = 'f'; break;
    case 'B': return_key = 's'; break;
    case '*': return_key = 'q'; break;
    case 'C': return_key = 'a'; break;
    case 'D': return_key = 'm'; break;
    default: return_key = key;
  }
  //printf("Pressed %c %d, returning %c %d\n", key, key, return_key, return_key);
  return return_key; 
}

/*********************************************************/

int d2x24_beep(void)
{
  return 0; 
}

/*********************************************************/

