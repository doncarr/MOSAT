#include "mod_server_msg.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "rt_modbus_opcodes.h"
#include "rt_modbus_crc.h"
#include "mod_atmega_implementation.h"

/**********************************************************************/

int rt_modbus_read_output_table(uint8_t *buf, int n)
{
  uint16_t start_point;
  uint16_t num_points;
  int i;
  uint8_t *p;
  int byte_count;
  uint8_t val;
  int byte;
  int bito;
  uint8_t mask;

  //trace.print_buf(0, "Read Output Table:\n", buf, 8);
  //print_binary_buf("Read Output Table: ", buf, 8);
  memcpy(&start_point, buf + 2, 2);
  swap16(&start_point);
  memcpy(&num_points, buf + 4, 2);
  swap16(&num_points);
  if (num_points > 128)
  {
    //trace.dprintf(5, "*** Too many points specified!\n");
    //trace.dprintf(5, "Start %u, Num %u\n", start_point, num_points);
    buf[1] = 128 + READ_OUTPUT_TABLE;
    buf[2] = 4;
    add_CRC(buf, 5, 0xffff);
    return 5;
  }

  byte_count = num_points / 8;

  if ((num_points % 8) > 0)
  {
    byte_count++;
  }

  buf[2] = byte_count;
  
  /*******************/
  // put call to read actual values here
  p = buf + 3;
  for (i=0; i < byte_count; i++)
  {
    *(p++) = 0x00;
  }
  
  for (i=0; i < num_points; i++)
  {
    // val = read_do(i + start_point); 
    //val = (i % 2) != 0;
    //if (val)
    if ((i % 2) != 0)
    {
      byte = i / 8;
      bito = i % 8;
      mask = 1 << bito;
      buf[byte + 3] |= mask;
    }
  }
  /*******************/

  add_CRC(buf, 5 + byte_count, 0xffff);
  //trace.print_buf(0, "Reply:\n", buf, 5 + byte_count);
  //PutBuffer(buf, 5 + byte_count);
  return 5 + byte_count;
}

/**********************************************************************/

int rt_modbus_read_input_table(uint8_t *buf, int n)
{
  uint16_t start_point;
  uint16_t num_points;
  int byte_count;
  uint8_t *p;
  int i;
  uint8_t val;
  int byte, bito;
  uint8_t mask;

  //trace.print_buf(0, "Read Input Table:\n", buf, 8);
  memcpy(&start_point, buf + 2, 2);
  swap16(&start_point);
  memcpy(&num_points, buf + 4, 2);
  swap16(&num_points);
  if (num_points > 128)
  {
    //trace.dprintf(5, "*** Too many points specified!\n");
    //trace.dprintf(5, "Start %u, Num %u\n", start_point, num_points);
    buf[1] = 128 + READ_INPUT_TABLE;
    buf[2] = 4;
    add_CRC(buf, 5, 0xffff);
    //PutBuffer(buf, 5);
    return 5;
  }
  byte_count = num_points / 8;
  if ((num_points % 8) > 0)
  {
    byte_count++;
  }
  buf[2] = byte_count;

  /*******************/
  // put call to read actual values here
  p = buf + 3;
  for (i=0; i < byte_count; i++)
  {
    *(p++) = 0x00;
  }

  for (i=0; i < num_points; i++)
  {
    // val = read_di(i + start_point); 
    val = (i % 2) == 0;
    if (val)
    {
      byte = i / 8;
      bito = i % 8;
      mask = 1 << bito;
      buf[byte + 3] |= mask;
    }
  }
  /*******************/

  add_CRC(buf, 5 + byte_count, 0xffff);
  //trace.print_buf(0, "Reply:\n", buf, 5 + byte_count);
  //PutBuffer(buf, 5 + byte_count);
  return (5 + byte_count);
}

/**********************************************************************/

int rt_modbus_read_registers(uint8_t *buf, int n)
{
  uint16_t start_point;
  uint16_t num_points;
  int byte_count;
  uint8_t *p;
  int i;
  uint16_t tmp;

  //trace.print_buf(0, "Read Registers:\n", buf, 8);
  memcpy(&start_point, buf + 2, 2);
  swap16(&start_point);
  memcpy(&num_points, buf + 4, 2);
  swap16(&num_points);
  if (num_points > 120)
  {
    //trace.dprintf(5, "*** Too many points specified!\n");
    //trace.dprintf(5, "Start %u, Num %u\n", start_point, num_points);
    buf[1] = 128 + READ_REGISTERS;
    buf[2] = 4;
    add_CRC(buf, 5, 0xffff);
    //PutBuffer(buf, 5);
    return 5;
  }
  byte_count = num_points * 2;

  buf[2] = byte_count;

  /*******************/
  // put call to read actual values here
  p = buf + 3;
  for (i=0; i < num_points; i++)
  {
    if ((start_point + i) % 2)
    {
      tmp = 0xAAAA;
    }
    else
    {
      tmp = 0x5555;
    }
    swap16(&tmp);
    memcpy(p, &tmp, 2);
    p += 2;
  }
  /*******************/

  add_CRC(buf, 5 + byte_count, 0xffff);
  //trace.print_buf(0, "Reply:\n", buf, 5 + byte_count);
  //PutBuffer(buf, 5 + byte_count);
  return (5 + byte_count);
}

/**********************************************************************/

int rt_modbus_read_analog_inputs(uint8_t *buf, int sz)
{
  uint16_t start_point;
  uint16_t num_points;
  int byte_count;
  uint8_t *p;
  uint16_t tmp;
  int i;

  //trace.print_buf(0, "Read Analog Inputs:\n", buf, 8);
  memcpy(&start_point, buf + 2, 2);
  swap16(&start_point);
  memcpy(&num_points, buf + 4, 2);
  swap16(&num_points);
  if (num_points > 120)
  {
    //trace.dprintf(5, "*** Too many points specified!\n");
    //trace.dprintf(5, "Start %u, Num %u\n", start_point, num_points);
    buf[1] = 128 + READ_ANALOG_INPUTS;
    buf[2] = 4;
    add_CRC(buf, 5, 0xffff);
    //PutBuffer(buf, 5);
    return 5;
  }

  byte_count = num_points * 2;
  buf[2] = byte_count;

  /*******************/
  // put call to read actual values here
  p = buf + 3;
  for (i=0; i < num_points; i++)
  {
    //tmp = start_point + i;
    tmp =read_ai(start_point + i); 
    swap16(&tmp);
    memcpy(p, &tmp, 2);
    p += 2;
  }
  /*******************/

  add_CRC(buf, 5 + byte_count, 0xffff);
  //trace.print_buf(0, "Reply:\n", buf, 5 + byte_count);
  //PutBuffer(buf, 5 + byte_count);
  return (5 + byte_count);
}

/**********************************************************************/

int rt_modbus_force_single_output(uint8_t *buf, int sz)
{
  uint16_t address;
  uint16_t value;
  //trace.print_buf(1, "Force Single Output:\n", buf, 8);

  //trace.print_buf(1, "Reply:\n", buf, 8);
  //PutBuffer(buf, 8);
  
  memcpy(&address, buf + 2, 2);
  swap16(&address);
  memcpy(&value, buf + 4, 2);
  swap16(&value);

  send_do(address, value != 0);


  return 8;
}

/**********************************************************************/

int rt_modbus_preset_single_register(uint8_t *buf, int sz)
{
  uint16_t address;
  uint16_t value;
  //trace.print_buf(1, "Preset Single Register:", buf, 8);

  //trace.print_buf(1, "Reply:\n", buf, 8);
  //PutBuffer(buf, 8);
  
  memcpy(&address, buf + 2, 2);
  swap16(&address);
  memcpy(&value, buf + 4, 2);
  swap16(&value);

  write_register(address, value);

  return 8;
}

/**********************************************************************/

int rt_modbus_read_exception_status(uint8_t *buf, int sz)
{

  // ************ Probably eliminate, no longer used??

  //trace.print_buf(1, "Read Exception Status:\n", buf, 4);
  buf[2] = 0;
  add_CRC(buf, 5, 0xffff);
  //trace.print_buf(1, "Reply:\n", buf, 5);
  //PutBuffer(buf, 5);
  return 5;
}

/**********************************************************************/

int rt_modbus_loopback(uint8_t *buf, int sz)
{
  //trace.print_buf(1, "Loopback:\n", buf, 8);

  //trace.print_buf(1, "Reply:\n", buf, 8);
  //PutBuffer(buf, 8);
  return 8;
}

/**********************************************************************/

int rt_modbus_force_multiple_outputs(uint8_t *buf, int sz)
{
  uint16_t start_point;
  uint16_t num_points;
  int data_bytes;
  int size;
  int i;
  int byte, bito;
  uint8_t mask;
  int sz_check;
  uint8_t val;

  data_bytes = buf[6];
  size = 9 + data_bytes;

  memcpy(&start_point, buf + 2, 2);
  swap16(&start_point);
  memcpy(&num_points, buf + 4, 2);
  swap16(&num_points);

  //printf("start = %hu, num = %hu\n", start_point, num_points);

  sz_check = num_points / 8;
  if ((num_points % 8) != 0) sz_check++;

  //printf("data bytes = %hu, calculated size = %d\n", data_bytes, sz_check);
  /*if (sz_check == data_bytes)
  {
    printf("Size matches\n");
  }
  else
  {
    printf("*** Data error size does NOT match\n");
  }*/


  /*******************/
  // put call to send the actual values here

  for (i=0; i < num_points; i++)
  {
    byte = i / 8;
    bito = i % 8;
    mask = 1 << bito;
    val = (buf[byte + 7] & mask) != 0;
    //printf("byte: %d (buf[%d]), bit %d: %c, addr: %d\n",byte, byte + 7, bit, val?'T':'F', ((int) start_point) + i);
    send_do(start_point + i, val);
  }

  /*******************/

  add_CRC(buf, 8, 0xffff);
  //trace.print_buf(1, "Force Multiple Outputs:\n", buf, size);
  //PutBuffer(buf, 8);
  //trace.print_buf(1, "Reply:\n", buf, 8);
  return 8;
}

/**********************************************************************/

int rt_modbus_preset_multiple_registers(uint8_t *buf, int sz)
{
  int data_bytes;
  int size;
  uint16_t start_point;
  uint16_t num_points;
  uint16_t tmp;
  int i;

  data_bytes = buf[6];
  size = 9 + data_bytes;

  memcpy(&start_point, buf + 2, 2);
  swap16(&start_point);
  memcpy(&num_points, buf + 4, 2);
  swap16(&num_points);
  //printf("start = %hu, num = %hu\n", start_point, num_points);

  //printf("data bytes = %hu, calculated size = %d\n", data_bytes, num_points * 2);
  /*if ((num_points * 2) == data_bytes)
  {
    printf("Size matches\n");
  }
  else
  {
    printf("*** Data error size does NOT match\n");
  }*/

  //trace.print_buf(1, "Preset Multiple Registers:\n", buf, size);
  for (i=0; i < num_points; i++)
  {
    memcpy(&tmp, buf + (i * 2) + 7, 2);
    swap16(&tmp);
    //printf("value %d: %04hx, addr: %d\n", i, tmp, start_point + i);
    write_register(start_point + i, tmp); 
  }

  add_CRC(buf, 8, 0xffff);

  //trace.print_buf(1, "Reply:\n", buf, 8);
  //PutBuffer(buf, 8);

  return 8;
}

/**********************************************************************/

int rt_modbus_report_device_type(uint8_t *buf, int sz)
{

  // ************ Probably eliminate, no longer used??

  //trace.print_buf(1, "Report Device Type:\n", buf, 4);
  buf[2] = 5;
  buf[3] = 0;
  buf[4] = 0;
  buf[5] = 0;
  buf[6] = 0;
  buf[7] = 0;
  add_CRC(buf, 10, 0xffff);
  //trace.print_buf(1, "Reply:\n", buf, 10);
  //PutBuffer(buf, 10);
  return 10;
}

/**********************************************************************/

int rt_modbus_read_scratch_pad(uint8_t *buf, int sz)
{

  // ************ Probably eliminate, no longer used??

  //trace.print_buf(1, "Read Scratch Pad:\n", buf, 8);
  buf[2] = 0;
  add_CRC(buf, 5, 0xffff);
  //trace.print_buf(1, "Reply:\n", buf, 5);
  //PutBuffer(buf, 5);
  return 5;
}

/********************************************************************/

int rt_modbus_min_bytes(uint8_t *buf)
{
  /* Determine the minimum number of bytes based on 
     the first two bytes read in a modbus message */

  switch (buf[1])
  {
    case READ_OUTPUT_TABLE:
    case READ_INPUT_TABLE:
    case READ_REGISTERS:
    case READ_ANALOG_INPUTS:
    case FORCE_SINGLE_OUTPUT:
    case PRESET_SINGLE_REGISTER:
    case LOOPBACK:
    case READ_SCRATCH_PAD:
      return 8;
    case READ_EXCEPTION_STATUS:
    case REPORT_DEVICE_TYPE:
      return 4;
    case FORCE_MULTIPLE_OUTPUTS:
    case PRESET_MULTIPLE_REGISTERS:
      return 7;
    default:
      return -1;
  }
}

/*************/

int rt_modbus_total_bytes(uint8_t *buf, int n)
{
  /* Determine the total number of bytes based on 
     the first minimum bytes read in a modbus message */
  switch (buf[1])
  {
    case READ_OUTPUT_TABLE:
    case READ_INPUT_TABLE:
    case READ_REGISTERS:
    case READ_ANALOG_INPUTS:
    case FORCE_SINGLE_OUTPUT:
    case PRESET_SINGLE_REGISTER:
    case LOOPBACK:
    case READ_SCRATCH_PAD:
      if (n != 8) return -1;
      return 8;
    case READ_EXCEPTION_STATUS:
    case REPORT_DEVICE_TYPE:
      if (n != 4) return -1;
      return 4;
    case FORCE_MULTIPLE_OUTPUTS:
    case PRESET_MULTIPLE_REGISTERS:
      if (n != 7) return -1;
      return buf[6] + 9;
    default:
      return -1;
  }
}

/********************************************************************/

int rt_modbus_process_request(uint8_t *buf, int n)
{
  switch (buf[1])
  {
    case READ_OUTPUT_TABLE:
      return rt_modbus_read_output_table(buf, n);
    case READ_INPUT_TABLE:
      return rt_modbus_read_input_table(buf, n);
    case READ_REGISTERS:
      return rt_modbus_read_registers(buf, n);
    case READ_ANALOG_INPUTS:
      return rt_modbus_read_analog_inputs(buf, n);
    case FORCE_SINGLE_OUTPUT: 
      return rt_modbus_force_single_output(buf, n);
    case PRESET_SINGLE_REGISTER: 
      return rt_modbus_preset_single_register(buf, n);
    case READ_EXCEPTION_STATUS: 
      return rt_modbus_read_exception_status(buf, n); 
    case LOOPBACK: 
      return rt_modbus_loopback(buf, n);
    case FORCE_MULTIPLE_OUTPUTS:
      return rt_modbus_force_multiple_outputs(buf, n);
    case PRESET_MULTIPLE_REGISTERS:
      return rt_modbus_preset_multiple_registers(buf, n);
    case REPORT_DEVICE_TYPE: 
      return rt_modbus_report_device_type(buf, n);
    case READ_SCRATCH_PAD: 
      return rt_modbus_read_scratch_pad(buf, n); 
    default:
      if (buf[1] < 80) buf[1] += 0x80;
      else buf[1] = 0xFF;
      buf[2] = 0x01;  // unsuported or illegal opcode 
          // Note: we will probably only ever use 0x01 to 0x04.
          // Error Codes:
          // 0x01 = Illegal or unsuported opcode
          // 0x02 = Illegal or out of range address 
          // 0x03 = Illegal data value
          // 0x04 = Slave device failure
          // 0x05 = Acknowledge
          // 0x06 = Slave device busy
          // 0x07 = Memory parity error
          // 0x0A = Gateway path unavailable
          // 0x0B = Gateway target device failed to respond
      add_CRC(buf, 5, 0xffff);
      return 5;
      //trace.dprintf(5, "Un-supported opcode!!!!\n", int(buf[1]));
  }
  return 0;
}

/**********************************************************************/

