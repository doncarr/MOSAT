
#ifndef __TS_KEYPAD_INC__
#define __TS_KEYPAD_INC__

#ifdef __cplusplus
extern "C" {
#endif
int keypad_getchar(void);
int keypad_getchar_x(int a, char *p, double d);
#ifdef __cplusplus
}
#endif

#endif
