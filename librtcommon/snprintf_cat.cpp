#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "rtcommon.h"

/**********************************************************/

// The folowing string length function will not blindly walk through
// memory looking for the terminating '\0', and, will return -1 
// if there is not a terminating '\0' within the size given.

static int strlen_max(const char *s, int max)
{
  int len = 0;
  while (*s != '\0')
  {
    if (len == max - 1)
    {
      return -1;
    }
    len++;
    s++;
  }
  return len;
}

/**********************************************************/
// This function is to add the results of snprintf to the end 
// of the existing string. The existing string MUST be terminited
// within the maximum size of the buffer, or and error message
// will be placed in the string and -1 will be returned. If the
// string is not large enough to hold results of printf, we will
// also place an error message in the string. 

int snprintf_cat(char *buf, int size, const char *fmt, ...)
{
  va_list arg_ptr;

  va_start(arg_ptr, fmt);

  //int len = strlen(buf);
  int len = strlen_max(buf, size);
  printf("** len = %d, max = %d\n", len, size); 
  
  //if (len >= size)
  if (len < 0)
  {
    snprintf(buf, size, "Severe program error (%s, %d): buffer not termninated", __FILE__, __LINE__);
    va_end(arg_ptr);
    return -1;
  }
  int available = size - len;
  int retval = vsnprintf(buf + len, available, fmt, arg_ptr);
  if (retval >= (available))
  {
    snprintf(buf, size, "Severe program error (%s, %d): buffer not big enough", __FILE__, __LINE__);
  }

  va_end(arg_ptr);
  return retval;
}

/**********************************************************/

