#include <rt_modbus_crc.h>
#include <string.h>



/********************************************************************/

int odd(uint16_t byte)
{
  int parity;

  parity = 0;

  while( byte )
  {
    parity ^= byte;
    byte = byte >> 1;
  }
  return( parity & 1);
}

/********************************************************************/
/*
 * crc_16
 * description: This function will calculate the Cyclic redundancy check
 *              code for the supplied character (int) string. The
 *              algorithim is described in
 *
 *                   Perez, Aram, "Byte-wise CRC calcuations",
 *                          IEEE Micro, June 1983.
 *
*/

uint16_t CRC_16(uint8_t *buf, int n, uint16_t start_mask)
//int crc_16( string )     /* calculates crc 16 */
//unsigned string[];
{
  uint16_t  crc, byte;
  int i;

   //crc = 0xffff;
   //crc = 0x0000;
   crc = start_mask;

   for (i = 0; i < n; i++)
   {
       //while ( *string != 0xffff) {
     byte = ((uint16_t)(buf[i]) ^ crc) & 0xff;
     crc = crc >> 8;
     if (odd(byte))
     {
       crc ^= 0xC001;
     }
     crc ^= (byte<<6) ^ (byte<<7);
   }
   return( crc );
}

/********************************************************************/


int check_CRC(uint8_t *buf, int size, uint16_t start_mask)
{
  uint16_t calc_crc, actual_crc;

  memcpy(&actual_crc, &buf[size - 2], 2);
  calc_crc = CRC_16(buf, size - 2, start_mask);
  return (calc_crc == actual_crc);
}

/*********************************************************************/

void add_CRC(uint8_t *buf, int size, uint16_t start_mask)
{
  uint16_t crc;

  crc = CRC_16(buf, size - 2, start_mask);
  memcpy(&buf[size-2], &crc, 2);
  return;
}

/*********************************************************************/
