
#include <stdio.h>
#include <ctype.h>

#include "mtype.h"

/*********************************************************************/

uint8 hex_to_bin(uint8 ch)
{
  if ((ch >= '0') && (ch <= '9'))
  {
    return ch - '0';
  }
  if ((ch >= 'A') && (ch <= 'F'))
  {
    return ch - 'A' + 10;
  }
  if ((ch >= 'a') && (ch <= 'f'))
  {
    return ch - 'a' + 10;
  }
  return 0;
}

/*********************************************************************/

uint8 bin_to_hex(uint8 bin)
{
  if (bin < 10)
  {
    return '0' + bin;
  }
  if ((bin >= 10) && (bin < 16))
  {
    return 'A' + bin - 10;
  }
  return '0';
}

/*********************************************************************/

int hexascii_to_binary(uint8 bin_buf[], uint8 hex_buf[], int n_hex)
{
  int i, j;

  for (i=0, j=0; i < n_hex; i+= 2, j++)
  {
    bin_buf[j] = (hex_to_bin(hex_buf[i]) << 4) | hex_to_bin(hex_buf[i+1]);
  }
  return j;
}

/*********************************************************************/

int binary_to_hexascii(uint8 hex_buf[], uint8 bin_buf[], int n_bin)
{
  int i, j;
  for (i=0, j=0; j < n_bin; i+= 2, j++)
  {
    hex_buf[i] = bin_to_hex(bin_buf[j] >> 4);
    hex_buf[i+1] = bin_to_hex(bin_buf[j] & 0x0F);
  }
  hex_buf[i] = '\0';
  return i;
}

/********************************************************************/


