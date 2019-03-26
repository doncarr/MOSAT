
#include <stdio.h>

#include "mtype.h"
#include "modio.h"
#include "hprtu.h"
#include "hp2mod.h"

int rt_modbus_reply_size(uint8 *buf)
{
  int count = 0;
  switch (buf[1])
  {
    case READ_OUTPUT_TABLE:
    case READ_INPUT_TABLE:
    case READ_REGISTERS:
    case READ_ANALOG_INPUTS:
      count = 5 + buf[2];
      break;
    case FORCE_SINGLE_OUTPUT:
    case PRESET_SINGLE_REGISTER:
      count = 8;
      break;
    case READ_EXCEPTION_STATUS:
      count = 5;
      break;
    case LOOPBACK:
    case FORCE_MULTIPLE_OUTPUTS:
    case PRESET_MULTIPLE_REGISTERS:
      count = 8;
      break;
    case REPORT_DEVICE_TYPE:
      count = 10;
      break;
    case READ_SCRATCH_PAD:
      count = 5 + buf[2];
      break;
    default:
      // Ok, this is an error!
      if (buf[1] > 0x80)
      {
        return 3;
      }
      else
      {
        return -1;
      }
  }
  return count;
}



