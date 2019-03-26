
#include <stdio.h>
#include <string.h>

#include "mtype.h"
#include "modio.h"
#include "hprtu.h"
#include "hp2mod.h"
#include "trace.h"

/**********************************************************************/

// This function determines the reply size from the first three bytes
// of a standard modbus reply.
//   1st byte: unit id
//   2nd byte: opcode
//   3rd byte: byte count or error code

int MODSerial::reply_size(uint8 *buf)
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
  /***********/

/*************************/

int MODSerial::read_di(uint16 start_address, int n_to_read, bool *vals)
{
  //uint8 msg[8];
  uint8 msg[6];
  uint8 reply[512];

  msg[0] = address;
  msg[1] = READ_INPUT_TABLE;
  uint16 tmp = start_address;
  swap16(&tmp);
  memcpy(msg+2, &tmp, 2);
  tmp = n_to_read;
  swap16(&tmp);
  memcpy(msg+4, &tmp, 2);
  int n = send(msg, sizeof(msg));
  if (n != sizeof(msg))
  {
    react_trace.dprintf(5, "Size sent not same as requested. \n");
    return -1;
  }

  //int receive_size = 5 + (n_to_read / 8);
  int receive_size = 3 + (n_to_read / 8);
  if ((n_to_read % 8) != 0)
  {
    receive_size++;
  }

  n = receive(reply, sizeof(reply));

  if (n < 0)
  {
    return -1;
  }
  else if (n != receive_size)
  {
    react_trace.dprintf(5, "**** Wrong reply size for read di yy\n");
    react_trace.dprintf(5, "**** n = %d, expected %d\n", n, receive_size);
    return -1;
  }
  for (int i = 0; i < n_to_read; i++)
  {
    int byte = (i / 8) + 3;
    int bit = i % 8;
    vals[i] = (reply[byte] & (1 << bit)) != 0;
  }
  return n_to_read;
}

/**********************************************************************/

int MODSerial::read_di_register(unsigned short start_register, int n_di, bool *vals)
{
  if (n_di > 128)
  {
    n_di = 128;
  }
  int n_registers = n_di / 16;
  if ((n_di % 16) != 0)
  {
    n_registers++;
  }
  uint16 reg_vals[8];
  int n = this->read_reg(start_register, n_registers,  reg_vals);
  if (n != n_registers)
  {
    return -1;
  }
  for (int i = 0; i < n_di; i++)
  {
    int word = i / 16;
    int bit = i % 16;
    vals[i] = (reg_vals[word] & (1 << bit)) != 0;
  }
  return n_di;
}

/**********************************************************************/

int MODSerial::read_di_req(request_t *req)
{
  int count;
  uint16 start_point, num_points, tmp;

  start_point = req[0].mod_number;
  react_trace.dprintf(0, "DI %d: mod_number = %d\n", 0, (int) req[0].mod_number);
  for (count = 1; (req[count].mod_type == MOD_DISCRETE_IN) &&
		  (req[count].mod_number - req[count-1].mod_number <= 20) &&
		  ((req[count].mod_number - start_point + 1) <= 128);
		    count++)
  {
    react_trace.dprintf(0, "DI %d: mod_number = %d\n", count, (int) req[count].mod_number);
  }
  react_trace.dprintf(0, "DI %d: mod_number = %d\n", count, (int) req[count].mod_number);

  num_points = req[count-1].mod_number - start_point + 1;
  react_trace.dprintf(0, "Read DI: Start = %d, Num = %d\n", int(start_point),
	  int(num_points));
  //uint8 msg[8];
  uint8 msg[6];
  uint8 reply[512];
  msg[0] = address;
  msg[1] = READ_INPUT_TABLE;
  tmp = start_point;
  tmp--;
  swap16(&tmp);
  memcpy(msg+2, &tmp, 2);
  tmp = num_points;
  swap16(&tmp);
  memcpy(msg+4, &tmp, 2);
  int n = send(msg, sizeof(msg));
  if (n != sizeof(msg))
  {
    return -1;
  }
  //int receive_size = 5 + (num_points / 8);
  int receive_size = 3 + (num_points / 8);
  if ((num_points % 8) != 0)
  {
    receive_size++;
  }

  n = receive(reply, sizeof(reply));

  if (n < 0)
  {
    return -1;
  }
  else if (n != receive_size)
  {
    react_trace.dprintf(5, "**** Wrong reply size for read di xx\n");
    return -1;
  }

  for (int i = 0; i < count; i++)
  {
    int offset = req[i].mod_number - start_point;
    int byte = (offset / 8) + 3;
    int bit = offset % 8;
    req[i].data = (reply[byte] & (1 << bit)) ? 1 : 0;
  }
  return count;
}

/**********************************************************************/

int MODSerial::read_do(uint16 start_address, int n_to_read, bool *vals)
{
  //uint8 msg[8];
  uint8 msg[6];
  uint8 reply[512];
  msg[0] = address;
  msg[1] = READ_OUTPUT_TABLE;
  uint16 tmp = start_address;
  swap16(&tmp);
  memcpy(msg+2, &tmp, 2);
  tmp = n_to_read;
  swap16(&tmp);
  memcpy(msg+4, &tmp, 2);

  int n = send(msg, sizeof(msg));

  if (n != sizeof(msg))
  {
    return -1;
  }

  //int receive_size = 5 + (n_to_read / 8);
  int receive_size = 3 + (n_to_read / 8);
  if ((n_to_read % 8) != 0)
  {
    receive_size++;
  }

  n = receive(reply, sizeof(reply));

  if (n < 0)
  {
    return -1;
  }
  if (n != receive_size)
  {
    react_trace.dprintf(5, "*** Incorrect reply size for read di\n");
    return -1;
  }

  for (int i = 0; i < n_to_read; i++)
  {
    int byte = (i / 8) + 3;
    int bit = i % 8;
    vals[i] = (reply[byte] & (1 << bit)) != 0;
  }
  return n_to_read;
}

/************************************************************************/

int MODSerial::read_do_req(request_t *req)
{
  int count;
  uint16 start_point, num_points, tmp;

  start_point = req[0].mod_number;
  for (count = 1; (req[count].mod_type == MOD_DISCRETE_OUT) &&
		  (req[count].mod_number - req[count-1].mod_number <= 20) &&
		  ((req[count].mod_number - start_point + 1) <= 128);
		    count++);

  num_points = req[count-1].mod_number - start_point + 1;
  react_trace.dprintf(0, "Read DO: Start = %d, Num = %d\n", int(start_point),
	  int(num_points));

  //uint8 msg[8];
  uint8 msg[6];
  uint8 reply[512];
  msg[0] = address;
  msg[1] = READ_OUTPUT_TABLE;
  tmp = start_point;
  tmp--;
  swap16(&tmp);
  memcpy(msg+2, &tmp, 2);
  tmp = num_points;
  swap16(&tmp);
  memcpy(msg+4, &tmp, 2);

  int n = send(msg, sizeof(msg));

  if (n != sizeof(msg))
  {
    return -1;
  }
  //int receive_size = 5 + (num_points / 8);
  int receive_size = 3 + (num_points / 8);
  if ((num_points % 8) != 0)
  {
    receive_size++;
  }

  n = receive(reply, sizeof(reply));

  if (n < 0)
  {
    return -1;
  }
  if (n != receive_size)
  {
    react_trace.dprintf(5, "*** Incorrect reply size for read di\n");
    return -1;
  }
  count++;
  count--;
  for (int i = 0; i < count; i++)
  {
    int offset = req[i].mod_number - start_point;
    int byte = (offset / 8) + 3;
    int bit = offset % 8;
    req[i].data = (reply[byte] & (1 << bit)) ? 1 : 0;
  }
  return count;
}

/************************************************************************/

int MODSerial::read_ai(uint16 start_address, int n_to_read, uint16 *vals)
{
  react_trace.dprintf(0, "Read AI: Start = %d, Num = %d\n", int(start_address),
	  int(n_to_read));

  //uint8 msg[8];
  uint8 msg[6];
  uint8 reply[512];
  for (int i=0; i < n_to_read; i++)
  {
    vals[i] = 0;
  }

  for (unsigned i=0; i < (sizeof(reply) / sizeof(reply[0])); i++)
  {
    reply[i] = 0;
  }

  msg[0] = address;
  msg[0] = address;
  msg[1] = READ_ANALOG_INPUTS;
  react_trace.dprintf(0, "Opcode = %d, ", (int) msg[1]);
  uint16 tmp = start_address;
  react_trace.dprintf(0, "Start = %d, ", (int) tmp);
  swap16(&tmp);
  memcpy(msg+2, &tmp, 2);
  tmp = n_to_read;
  react_trace.dprintf(0, "n = %d\n", (int) tmp);
  swap16(&tmp);
  memcpy(msg+4, &tmp, 2);
  int n = send(msg, sizeof(msg));
  if (n != sizeof(msg))
  {
    return -1;
  }
  //int receive_size = 5 + (n_to_read * 2);
  int receive_size = 3 + (n_to_read * 2);

  printf("================================================ READ AI ======================\n");
  n = receive(reply, sizeof(reply));
  if (n != receive_size)
  {
    react_trace.dprintf(6, "**** Incorrect reply size for read analog input\n");
    return -1;
  }
  printf("================================================ READ AI, n = %d ======================\n", n);

  for (int i=0; i < n_to_read; i++)
  {
    memcpy(&vals[i], &reply[3+(2*i)], 2);
    swap16(&vals[i]);
  }
  return n_to_read;
}

/************************************************************************/

int MODSerial::read_reg(uint16 start_address, int n_to_read, uint16 *vals)
{
  react_trace.dprintf(0, "Read REG: Start = %d, Num = %d", int(start_address),
	  int(n_to_read));

  //uint8 msg[8];
  uint8 msg[6];
  uint8 reply[512];
  msg[0] = address;
  msg[1] = READ_REGISTERS;
  uint16 tmp = start_address;
  swap16(&tmp);
  memcpy(msg+2, &tmp, 2);
  tmp = n_to_read;
  swap16(&tmp);
  memcpy(msg+4, &tmp, 2);

  int n = send(msg, sizeof(msg));

  if (n != sizeof(msg))
  {
    return -1;
  }

  //int receive_size = 5 + (n_to_read * 2);
  int receive_size = 3 + (n_to_read * 2);
  n = receive(reply, sizeof(reply));
  if (n != receive_size)
  {
    react_trace.dprintf(5, "**** Wrong reply size for read register\n");
    return -1;
  }

  for (int i=0; i < n_to_read; i++)
  {
    memcpy(&vals[i], &reply[3+(2*i)], 2);
    swap16(&vals[i]);
  }
  return n_to_read;
}

/************************************************************************/

int MODSerial::read_reg_req(request_t *req)
{
  int count;
  uint16 start_point, num_points, tmp;

  start_point = req[0].mod_number;
  react_trace.dprintf(0, "REG %d: mod_number = %d\n", 0, (int) req[0].mod_number);
  for (count = 1; (req[count].mod_type == MOD_REGISTER) &&
		 (req[count].mod_number - req[count-1].mod_number <= 20) &&
		 ((req[count].mod_number - start_point + 1) <= 120);
		    count++)
  {
    react_trace.dprintf(0, "REG %d: mod_number = %d\n", count, (int) req[count].mod_number);
  }
  react_trace.dprintf(0, "REG %d: mod_number = %d\n", count, (int) req[count].mod_number);

  num_points = req[count-1].mod_number - start_point + 1;
  react_trace.dprintf(0, "Read REG: Start = %d, Num = %d", int(start_point),
	  int(num_points));

  //uint8 msg[8];
  uint8 msg[6];
  uint8 reply[512];
  msg[0] = address;
  msg[1] = READ_REGISTERS;
  tmp = start_point;
  tmp--;
  swap16(&tmp);
  memcpy(msg+2, &tmp, 2);
  tmp = num_points;
  swap16(&tmp);
  memcpy(msg+4, &tmp, 2);

  int n = send(msg, sizeof(msg));

  if (n != sizeof(msg))
  {
    return -1;
  }

  //int receive_size = 5 + (num_points * 2);
  int receive_size = 3 + (num_points * 2);
  n = receive(reply, sizeof(reply));
  if (n != receive_size)
  {
    react_trace.dprintf(5, "**** Wrong reply size for read register\n");
    return -1;
  }
  for (int i = 0; i < count; i++)
  {
    int offset = req[i].mod_number - start_point;
    memcpy(&req[i].data, &reply[3+(2*offset)], 2);
    swap16(&req[i].data);
  }
  return count;
}

/************************************************************************/

int MODSerial::write_reg(uint16 num, uint16 val)
{
  //uint8 msg[8];
  uint8 msg[6];
  uint8 reply[512];
  react_trace.dprintf(0, "\nWriting to MODBUS REG: n = %u, val = %u\n",
		      num, val);
  msg[0] = address;
  msg[1] = PRESET_SINGLE_REGISTER;
  //num--;
  swap16(&num);
  memcpy(msg+2, &num, 2);
  swap16(&val);
  memcpy(msg+4, &val, 2);

  int n = send(msg, sizeof(msg));
  if (n != sizeof(msg))
  {
    return -1;
  }

  n = receive(reply, sizeof(reply));

  if (n < 0)
  {
    return -1;
  }
  else if (n != sizeof(msg))
  {
    react_trace.dprintf(5, "Wrong reply size for write register\n");
    return -1;
  }
  if (0 != memcmp(msg, reply, sizeof(msg) - 2))
  {
    react_trace.dprintf(5, "Reply does not match for write register\n");
    return -1;
  }
  return 0;
}

/************************************************************************/

int MODSerial::send_do(uint16 num, uint8 val)
{
  uint8 byte;
  //uint8 msg[8];
  uint8 msg[6];
  uint8 reply[512];
  react_trace.dprintf(0, "\nSending MODBUS Device DO: n = %d, val = %d\n",
		      (int) num, int(val));
  msg[0] = address;
  msg[1] = FORCE_SINGLE_OUTPUT;
  //num--;
  swap16(&num);
  memcpy(msg+2, &num, 2);
  byte = val ? 255 : 0;
  msg[4] = byte;
  msg[5] = 0;
  int n = send(msg, sizeof(msg));

  if (n != sizeof(msg))
  {
    return -1;
  }

  n = receive(reply, sizeof(reply));

  if (n < 0)
  {
    return -1;
  }
  else if (n != sizeof(msg))
  {
    react_trace.dprintf(5, "*** Wrong reply size for discrete out\n");
    return -1;
  }

  if (0 != memcmp(msg, reply, sizeof(msg) - 2))
  {
    react_trace.dprintf(5, "*** Reply from MODBUS Device does not match for discrete out\n");
    return -1;
  }

  return 0;
}

/***********************************************************************/

int  MODSerial::write_multiple_regs(uint16 start_reg, int n, uint16 *vals)
{
  react_trace.dprintf(0, "write multiple regs: Start = %d, Num = %d", 
              int(start_reg), n);
  //uint8 msg[8];
  //int msg_size = 6 + 2 * n; 
  uint8 msg[512];
  uint8 reply[512];
  msg[0] = address;
  msg[1] = PRESET_MULTIPLE_REGISTERS;
  uint16 tmp = start_reg;
  swap16(&tmp);
  memcpy(msg+2, &tmp, 2);
  tmp = n;
  swap16(&tmp);
  memcpy(msg+4, &tmp, 2);
  msg[6] = uint8(2 * n);
  uint8 *p = msg + 7;
  for (int i=0; i < n; i++)
  {
    tmp = vals[i];
    swap16(&tmp);
    memcpy(p, &tmp, 2);
    p += 2;
  }

  //int n_sent = send(msg, (2 * n) + 7);

  int n_recv = receive(reply, sizeof(reply));
  if (n_recv != 6)
  {
    react_trace.dprintf(5, "*** Wrong reply size for write multiple regs\n");
    return -1;
  }

  return 0;

}

/***********************************************************************/

int  MODSerial::send_multiple_dos(uint16 start_do, int n, uint8 *vals)
{
  react_trace.dprintf(0, "write multiple dos: Start = %d, Num = %d", 
              int(start_do), n);
  int data_bytes = n / 8; 
  if ((n % 8) != 0) data_bytes++;
  uint8 msg[512];
  uint8 reply[512];
  msg[0] = address;
  msg[1] = FORCE_MULTIPLE_OUTPUTS;
  uint16 tmp = start_do;
  swap16(&tmp);
  memcpy(msg+2, &tmp, 2);
  tmp = n;
  swap16(&tmp);
  memcpy(msg+4, &tmp, 2);
  msg[6] = uint8(data_bytes);

  for (int i=0; i < data_bytes; i++)
  {
    msg[7 + i] = 0;
  }
  for (int i=0; i < n; i++)
  {
    if (vals[i])
    {
      int byte = i / 8;
      int bit = i % 8;;
      uint8 mask = 1 << bit;
      msg[7 + byte] |= mask;
    }
  }

  //int n_sent = send(msg, data_bytes + 7);

  int n_recv = receive(reply, sizeof(reply));
  if (n_recv != 6)
  {
    react_trace.dprintf(5, "*** Wrong reply size for write multiple regs\n");
    return -1;
  }


  return 0;
}
 
/***********************************************************************/

void MODSerial::set_debug_level(int a_debug_level)
{
  react_trace.set_level(a_debug_level);
}

/***********************************************************************/

int MODSerial::send(uint8 *buf, int size)
{
  //add_CRC(buf, size, 0xffff);
  react_trace.print_buf(0, "\nSending to MODBUS Device ....\n", buf, size);
  //RXFlush();
  //return PutBuffer(buf, size);
  modio->put_buffer(buf, size);
  return modio->send_message();
}

/***********************************************************************/

int MODSerial::receive(uint8 *buf, int size)
{
  int done = false;

  react_trace.dprintf(0, "Waiting message . . . .\n");
  modio->wait_message();
  react_trace.dprintf(0, "Got message . . . .\n");
  while (!done)
  {
    int n = modio->get_buffer(buf, 1);
    if (n != 1)
    {
      react_trace.dprintf(5, "**** Timeout on msg receive\n");
      return -1;
    }
    if (buf[0] != address)
    {
      react_trace.dprintf(5, "**** Incorrect address, got %u, expected %u\n",
                buf[0], address);
      done = true;
      continue;
    }
    done = true;
  }
  int n = modio->get_buffer(buf + 1, 2);
  if (n != 2)
  {
    react_trace.dprintf(5, "**** Timeout on msg receive\n");
    return -1;
  }
  int count;
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
      react_trace.print_buf(5, "*** Bad MODBUS Device reply\n", buf, 3);
      const char *errstr = "";
      switch (buf[2])
      {
        case 0x1:
	  errstr = "Illegal Function";
	  break;
        case 0x2:
	  errstr = "Illegal Data Address";
	  break;
        case 0x3:
	  errstr = "Illegal Data Value";
	  break;
        case 0x4:
	  errstr = "Slave Device failure";
	  break;
        case 0x5:
	  errstr = "Acknowledge";
	  break;
        case 0x6:
	  errstr = "Slave Device Busy";
	  break;
        case 0x8:
	  errstr = "Memory Parity Error";
	  break;
        case 0xA:
	  errstr = "Gateway Path Unavailable";
	  break;
        case 0xB:
	  errstr = "Gateway Target Device Failed to Respond";
	  break;
	default:
	  errstr = "Unknow Error Code";
	  break;
      }
      react_trace.dprintf(5, "Error code: %s\n", errstr);
      //RXFlush();
      return -1;
  }
  count = count - 2;
  if (count > size)
  {
    react_trace.dprintf(5, "*** MODBUS Device buffer not big enough to hold Reply.\n");
    return -1;
  }

  n = modio->get_buffer(buf + 3, count - 3);

  if (n != (count - 3))
  {
    react_trace.dprintf(5, "**** Wrong number of bytes in MODBUS reply\n");
    //RXFlush();
    return -1;
  }

  react_trace.print_buf(0, "\nReceived:\n", buf, count);

  /*******
  if (!check_CRC(buf, count, 0xffff))
  {
    react_trace.dprintf(5, "**** Bad CRC in message from MODBUS Device\n");
    //RXFlush();
    return -1;
  }
  *******/

  //RXFlush();
  //return count;
  return n+3;
}

/***********************************************************************/
