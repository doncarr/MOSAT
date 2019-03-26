#include<unistd.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<sys/time.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>

// Use 0 for busy-wait
#define POLL_FREQ	30

// Use 0 for busy-wait
#define DEBOUNCE_POLL_FREQ	256

#define DEBOUNCE_STABLE_MS	10
#define REPEAT_FREQ	30
#define REPEAT_DELAY_MS	500

// struct timeval utility macros
#define TV_ADD_US(x, y)	{ \
	(x).tv_usec += (y); \
	(x).tv_sec += (x).tv_usec / 1000000; \
	(x).tv_usec = (x).tv_usec % 1000000; \
}

#define TV_GTE(x, y) ((x).tv_sec > (y).tv_sec || ((x).tv_sec == (y).tv_sec && \
	(x).tv_usec >= (y).tv_usec)) 

#define TV_ELAPSED_US(x, y)	(((x).tv_sec - (y).tv_sec) * 1000000 + \
	((x).tv_usec - (y).tv_usec))

unsigned int get_keys(void);
unsigned int debounce(unsigned int);
void keypad_event(unsigned int, unsigned int);

volatile unsigned int *dir, *dat;
unsigned int lastkey = -1;
struct timeval repeat_time;

int main(int argc, char **argv) {
	unsigned int k = 0, pressed = 0; 
	unsigned char *start;
	int fd = open("/dev/mem", O_RDWR|O_SYNC);
	start = (unsigned char *) mmap(0, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x80840000);
	dat = (unsigned int *)(start + 0x4);
	dir = (unsigned int *)(start + 0x14);
	*dir = 0xf;

	setvbuf(stdout, NULL, _IONBF, 0);

	for(;;) {
		struct timeval now;
		unsigned int changed;

		k = get_keys();
		changed = pressed ^ k;
		if (changed) {
			k = debounce(changed);
			changed &= pressed ^ k;
		}
		if (changed) {
			keypad_event(changed & k, changed & pressed);
			pressed &= ~(changed & pressed);
			pressed |= changed & k;
		}
		
		// Do key repeat as necessary
		if (lastkey != -1) {
			gettimeofday(&now, NULL);
			if (TV_GTE(now, repeat_time)) {
				printf("%c", lastkey);
				TV_ADD_US(repeat_time, REPEAT_FREQ / 1000000);
			}
		}

		if (POLL_FREQ) usleep(1000000 / POLL_FREQ);
	}
	close(fd);
}

void keypad_event(unsigned int on, unsigned int off) {
	unsigned int i;
	unsigned char keys[] = {
	  '1', '2', '3', 'A',
	  '4', '5', '6', 'B',
	  '7', '8', '9', 'C',
	  '*', '0', '#', 'D',
	};

	lastkey = -1;
	for(i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
		if (on & (1 << i)) {
			printf("%c", keys[i]);
			lastkey = keys[i];
		}
	}
	if (REPEAT_DELAY_MS && lastkey != -1) {
		gettimeofday(&repeat_time, NULL);
		TV_ADD_US(repeat_time, REPEAT_DELAY_MS * 1000);
	}
}

unsigned int get_keys(void) {
	unsigned int pos, on = 0;

	for(pos = 0; pos < 4; pos++) {
		*dat = ~(1 << pos);
		on |= (~(*dat >> 4) & 0xf) << (4 * pos);
	}
	return on;
}

// debounce will return when masked keys have been stable for DEBOUNCE_STABLE_MS
unsigned int debounce(unsigned int mask) {
	struct timeval verystart, start, now;
	unsigned int last_val, val;
	
	last_val = get_keys(); 
	gettimeofday(&verystart, NULL);
	memcpy(&start, &verystart, sizeof(struct timeval));
	do {
		val = get_keys();
		gettimeofday(&now, NULL);
		if ((val ^ last_val) & mask) 
		  memcpy(&start, &now, sizeof(struct timeval));
		last_val = val;
		if (DEBOUNCE_POLL_FREQ) usleep(1000000 / DEBOUNCE_POLL_FREQ);
	} while (TV_ELAPSED_US(now, start) <= (DEBOUNCE_STABLE_MS * 1000));
	
	return val;
}

