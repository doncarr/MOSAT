#include <stdio.h>
#include <string.h>


#include "mtype.h"
#include "modio.h"
#include "hprtu.h"

#include "hp2mod.h"

#include "rtmodbus_utility.h"

/********************************************************************/

int odd(uint16 byte)
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


uint16 CRC_16(uint8 *buf, int n, uint16 start_mask)
//int crc_16( string )     /* calculates crc 16 */
//unsigned string[];
{
  uint16  crc, byte;

   //crc = 0xffff;
   //crc = 0x0000;
   crc = start_mask;

   for (int i = 0; i < n; i++)
   {
       //while ( *string != 0xffff) {
     byte = (uint16(buf[i]) ^ crc) & 0xff;
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


int check_CRC(uint8 *buf, int size, uint16 start_mask)
{
  uint16 calc_crc, actual_crc;

  memcpy(&actual_crc, &buf[size - 2], 2);
  calc_crc = CRC_16(buf, size - 2, start_mask);
  return (calc_crc == actual_crc);
}

/*********************************************************************/

void add_CRC(uint8 *buf, int size, uint16 start_mask)
{
  uint16 crc;

  crc = CRC_16(buf, size - 2, start_mask);
  memcpy(&buf[size-2], &crc, 2);
  return;
}

/*********************************************************************/


/**********************************************************************/

// This function determines the reply size from the first three bytes
// of a standard modbus reply.
//   1st byte: unit id
//   2nd byte: opcode
//   3rd byte: byte count or error code

int rt_modbus_reply_size_min(uint8 *buf)
{
  switch (buf[1])
  {
    case READ_OUTPUT_TABLE:
    case READ_INPUT_TABLE:
    case READ_REGISTERS:
    case READ_ANALOG_INPUTS:
    case READ_SCRATCH_PAD:
      return 3;
      break;
    case FORCE_SINGLE_OUTPUT:
    case PRESET_SINGLE_REGISTER:
    case FORCE_MULTIPLE_OUTPUTS:
    case PRESET_MULTIPLE_REGISTERS:
    case LOOPBACK:
      return 8;
      break;
    case READ_EXCEPTION_STATUS:
      return 5;
      break;
    case REPORT_DEVICE_TYPE:
      return 10;
      break;
    default:
      if (buf[1] > 0x80) // Ok, this is an error return!
      {
        return 3;
      }
      else // oops, this is completely unexpected we can't determine the size.
      {
        return -1;
      }
  }
}

/********************************************************************/

int rt_modbus_reply_size_total(uint8 *buf)
{
  switch (buf[1])
  {
    case READ_OUTPUT_TABLE:
    case READ_INPUT_TABLE:
    case READ_REGISTERS:
    case READ_ANALOG_INPUTS:
    case READ_SCRATCH_PAD:
      return 5 + buf[2];
      break;
    case FORCE_SINGLE_OUTPUT:
    case PRESET_SINGLE_REGISTER:
    case FORCE_MULTIPLE_OUTPUTS:
    case PRESET_MULTIPLE_REGISTERS:
    case LOOPBACK:
      return 8;
      break;
    case READ_EXCEPTION_STATUS:
      return 5;
      break;
    case REPORT_DEVICE_TYPE:
      return 10;
      break;
    default:
      if (buf[1] > 0x80) // Ok, this is an error!
      {
        return 3;
      }
      else // oops, this is completely unexpected we can't determine the size.
      {
        return -1;
      }
  }
}

/*********************************************************************/
