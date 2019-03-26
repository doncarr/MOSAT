
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mtype.h"
#include "modio.h"
#include "modsim.h"
#include "hprtu.h"
#include "hp2mod.h"
#include "trace.h"

/********************************************************************/

int ModSerialSim::process_request(void)
{
  uint8 buf[200];

  if (-1 == modio->wait_message())
  {
    return -1;
  }
  int n = modio->get_buffer(buf, 2);
  if (n != 2)
  {
    return 0;
  }
  //int count;
  react_trace.dprintf(1, "Got Message ....\n");
  switch (buf[1])
  {
    case READ_OUTPUT_TABLE:
      read_output_table(buf);
      break;
    case READ_INPUT_TABLE:
      read_input_table(buf);
      break;
    case READ_REGISTERS:
      read_registers(buf);
      break;
    case READ_ANALOG_INPUTS:
      read_analog_inputs(buf);
      break;
    case FORCE_SINGLE_OUTPUT:
      force_single_output(buf);
      break;
    case PRESET_SINGLE_REGISTER:
      preset_single_register(buf);
      break;
    case READ_EXCEPTION_STATUS:
      read_exception_status(buf);
      break;
    case LOOPBACK:
      loopback(buf);
      break;
    case FORCE_MULTIPLE_OUTPUTS:
      force_multiple_outputs(buf);
      break;
    case PRESET_MULTIPLE_REGISTERS:
      preset_multiple_registers(buf);
      break;
    case REPORT_DEVICE_TYPE:
      report_device_type(buf);
      break;
    case READ_SCRATCH_PAD:
      read_scratch_pad(buf);
      break;
    default:
      buf[1] = 128;
      buf[2] = 1;
      //add_CRC(buf, 5, 0xffff);
      //modio->put_buffer(buf, 5);
      modio->put_buffer(buf, 3);
      modio->send_message();
      react_trace.dprintf(5, "Un-supported opcode: %d!!!!\n", int(buf[1]));
      //RXFlush();
      return 0;
  }
  //RXFlush();
  return 0;
}

/**********************************************************************/

void ModSerialSim::read_output_table(uint8 *buf)
{
  int n = modio->get_buffer(buf + 2, 6);
  if (n != 6)
  {
    react_trace.dprintf(5, "*** Incomplete Read Output Table message!\n");
    buf[1] = 128 + READ_OUTPUT_TABLE;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  /***
  if (!check_CRC(buf, 8, 0xffff))
  {
    react_trace.dprintf(5, "****** Bad CRC (Read Output Table)\n");
  }
  ****/

  react_trace.print_buf(0, "Read Output Table:\n", buf, 6);
  //print_binary_buf("Read Output Table: ", buf, 8);
  uint16 start_point;
  uint16 num_points;
  memcpy(&start_point, buf + 2, 2);
  swap16(&start_point);
  memcpy(&num_points, buf + 4, 2);
  swap16(&num_points);
  if (num_points > 128)
  {
    react_trace.dprintf(5, "*** Too many points specified!\n");
    react_trace.dprintf(5, "Start %u, Num %u\n", start_point, num_points);
    buf[1] = 128 + READ_OUTPUT_TABLE;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  int byte_count = num_points / 8;
  if ((num_points % 8) > 0)
  {
    byte_count++;
  }
  react_trace.dprintf(0, "N DOs: %d, N Bytes: %d\n", num_points, byte_count);
  buf[2] = byte_count;
  uint8 *p = buf + 3;
  for (int i=0; i < byte_count; i++)
  {
    *(p++) = 0xAA;
  }
  react_trace.print_buf(0, "Reply:\n", buf, 3 + byte_count);
  //add_CRC(buf, 5 + byte_count, 0xffff);
  //modio->put_buffer(buf, 5 + byte_count);
  modio->put_buffer(buf, 3 + byte_count);
  modio->send_message();
}

/**********************************************************************/

static int _mycount = 0;
static bool _ones = false;

void ModSerialSim::read_input_table(uint8 *buf)
{
    //uint16 tmp;
    _mycount++;
    if (_mycount > 15)
    {
       _mycount = 0;
       _ones = !_ones;
    }
    //if (_ones) tmp = 0xFFFF; else tmp = 0x0000;

  int n = modio->get_buffer(buf + 2, 6);
  if (n != 6)
  {
    react_trace.dprintf(5, "*** Incomplete Read Input Table message!\n");
    buf[1] = 128 + READ_INPUT_TABLE;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  /*********
  if (!check_CRC(buf, 8, 0xffff))
  {
    react_trace.dprintf(5, "****** Bad CRC (Read Input Table)\n");
  }
  *********/
  react_trace.print_buf(0, "Read Input Table:\n", buf, 6);
  uint16 start_point;
  uint16 num_points;
  memcpy(&start_point, buf + 2, 2);
  swap16(&start_point);
  memcpy(&num_points, buf + 4, 2);
  swap16(&num_points);
  if (num_points > 128)
  {
    react_trace.dprintf(5, "*** Too many points specified!\n");
    react_trace.dprintf(5, "Start %u, Num %u\n", start_point, num_points);
    buf[1] = 128 + READ_INPUT_TABLE;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  int byte_count = num_points / 8;
  if ((num_points % 8) > 0)
  {
    byte_count++;
  }
  react_trace.dprintf(0, "N DIs: %d, N Bytes: %d\n", num_points, byte_count);
  buf[2] = byte_count;
  uint8 *p = buf + 3;
  for (int i=0; i < byte_count; i++)
  {
    if (_ones)
    { 
      printf("-------- ONES!!\n");
      *(p++) = 0xFF;
    }
    else
    {
      printf("^^^^^^ NOT ones\n");
      *(p++) = 0x00;
    }
  }
  react_trace.print_buf(0, "Reply:\n", buf, 3 + byte_count);
  //add_CRC(buf, 5 + byte_count, 0xffff);
  //modio->put_buffer(buf, 5 + byte_count);
  modio->put_buffer(buf, 3 + byte_count);
  modio->send_message();
}

/**********************************************************************/

void ModSerialSim::read_registers(uint8 *buf)
{
  int n = modio->get_buffer(buf + 2, 6);
  if (n != 6)
  {
    react_trace.dprintf(5, "*** Incomplete Read Registers message!\n");
    buf[1] = 128 + READ_REGISTERS;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  /*****
  if (!check_CRC(buf, 8, 0xffff))
  {
    react_trace.dprintf(5, "****** Bad CRC (Read Registers)\n");
  }
  ********/
  react_trace.print_buf(0, "Read Registers:\n", buf, 6);
  uint16 start_point;
  uint16 num_points;
  memcpy(&start_point, buf + 2, 2);
  swap16(&start_point);
  memcpy(&num_points, buf + 4, 2);
  swap16(&num_points);
  if (num_points > 128)
  {
    react_trace.dprintf(5, "*** Too many points specified!\n");
    react_trace.dprintf(5, "Start %u, Num %u\n", start_point, num_points);
    buf[1] = 128 + READ_REGISTERS;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->get_buffer(buf, 5);
    modio->get_buffer(buf, 3);
    return;
  }
  int byte_count = num_points * 2;

  buf[2] = byte_count;
  uint8 *p = buf + 3;
    uint16 tmp;
  if (_ones) tmp = 0xFFFF; else tmp = 0x0000;
  for (int i=0; i < num_points; i++)
  {
/***
    if ((start_point + i) % 2)
    {
      tmp = 0xAAAA;
    }
    else
    {
      tmp = 0x5555;
    }
****/
    swap16(&tmp);
    memcpy(p, &tmp, 2);
    p += 2;
  }
  react_trace.print_buf(0, "Reply:\n", buf, 3 + byte_count);
  //add_CRC(buf, 5 + byte_count, 0xffff);
  //modio->put_buffer(buf, 5 + byte_count);
  modio->put_buffer(buf, 3 + byte_count);
  modio->send_message();
}

/**********************************************************************/
//static int extra = 65520;
//static bool inc_mode = true;
//static int extra_ctr = 0;

void ModSerialSim::read_analog_inputs(uint8 *buf)
{
  int n = modio->get_buffer(buf + 2, 6);
  if (n != 6)
  {
    react_trace.dprintf(5, "*** Incomplete Read Analog Inputs message!\n");
    buf[1] = 128 + READ_ANALOG_INPUTS;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  /******
  if (!check_CRC(buf, 8, 0xffff))
  {
    react_trace.dprintf(5, "****** Bad CRC (Read Analog Inputs)\n");
  }
  ********/
  react_trace.print_buf(0, "Read Analog Inputs:\n", buf, 6);
  uint16 start_point;
  uint16 num_points;
  memcpy(&start_point, buf + 2, 2);
  swap16(&start_point);
  memcpy(&num_points, buf + 4, 2);
  swap16(&num_points);
  if (num_points > 128)
  {
    react_trace.dprintf(5, "*** Too many points specified!\n");
    react_trace.dprintf(5, "Start %u, Num %u\n", start_point, num_points);
    buf[1] = 128 + READ_ANALOG_INPUTS;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  int byte_count = num_points * 2;

  buf[2] = byte_count;
  uint8 *p = buf + 3;
  for (int i=0; i < num_points; i++)
  {
    uint16 tmp = extra;//start_point + i + (uint16) extra;
    swap16(&tmp);
    memcpy(p, &tmp, 2);
    p += 2;
  }

  extra_ctr++;
  if (inc_mode)
  {
    extra++;
    if (extra > 65535)
    {
      extra = 0; // Simulate a rollover
    }
    if (extra_ctr > 2000)
    //if (extra_ctr > 11)
    {
      extra_ctr = 0;
      inc_mode = false;
    }
  }
  else
  {
    if (extra_ctr > 20)
    {
      extra_ctr = 0;
      inc_mode = true;
    }
  }

  react_trace.print_buf(0, "Reply:\n", buf, 3 + byte_count);
  //add_CRC(buf, 5 + byte_count, 0xffff);
  //modio->put_buffer(buf, 5 + byte_count);
  modio->put_buffer(buf, 3 + byte_count);
  modio->send_message();
}

/**********************************************************************/

void ModSerialSim::force_single_output(uint8 *buf)
{
  int n = modio->get_buffer(buf + 2, 6);
  if (n != 6)
  {
    react_trace.dprintf(5, "*** Incomplete Force Single Output message!\n");
    buf[1] = 128 + FORCE_SINGLE_OUTPUT;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  /***
  if (!check_CRC(buf, 8, 0xffff))
  {
    react_trace.dprintf(5, "****** Bad CRC (Force Single Output)\n");
  }
  ***/
  react_trace.print_buf(1, "Force Single Output:\n", buf, 6);

  react_trace.print_buf(1, "Reply:\n", buf, 6);
  modio->put_buffer(buf, 8);
  modio->send_message();
}

/**********************************************************************/

void ModSerialSim::preset_single_register(uint8 *buf)
{
  int n = modio->get_buffer(buf + 2, 6);
  if (n != 6)
  {
    react_trace.dprintf(5, "*** Incomplete Preset Single Register message!\n");
    buf[1] = 128 + PRESET_SINGLE_REGISTER;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 5);
    modio->send_message();
    return;
  }
  /****
  if (!check_CRC(buf, 8, 0xffff))
  {
    react_trace.dprintf(5, "****** Bad CRC (Preset Single Register)\n");
  }
  *****/
  react_trace.print_buf(1, "Preset Single Register:", buf, 6);

  react_trace.print_buf(1, "Reply:\n", buf, 6);
  modio->put_buffer(buf, 8);
  modio->send_message();
}

/**********************************************************************/

void ModSerialSim::read_exception_status(uint8 *buf)
{
  int n = modio->get_buffer(buf + 2, 2);
  if (n != 2)
  {
    react_trace.dprintf(5, "*** Incomplete Read Exception Status message!\n");
    buf[1] = 128 + READ_EXCEPTION_STATUS;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  react_trace.print_buf(1, "Read Exception Status:\n", buf, 2);
  buf[2] = 0;
  react_trace.print_buf(1, "Reply:\n", buf, 3);
  //add_CRC(buf, 5, 0xffff);
  //modio->put_buffer(buf, 5);
  modio->put_buffer(buf, 3);
  modio->send_message();
}

/**********************************************************************/

void ModSerialSim::loopback(uint8 *buf)
{
  int n = modio->get_buffer(buf + 2, 6);
  if (n != 6)
  {
    react_trace.dprintf(5, "*** Incomplete Loopback message!\n");
    buf[1] = 128 + LOOPBACK;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  /******
  if (!check_CRC(buf, 8, 0xffff))
  {
    react_trace.dprintf(5, "****** Bad CRC (Loopback)\n");
  }
  *******/
  react_trace.print_buf(1, "Loopback:\n", buf, 6);

  react_trace.print_buf(1, "Reply:\n", buf, 6);
  modio->put_buffer(buf, 8);
  modio->send_message();
}

/**********************************************************************/

void ModSerialSim::force_multiple_outputs(uint8 *buf)
{
  int n = modio->get_buffer(buf + 2, 5);
  if (n != 5)
  {
    react_trace.dprintf(5, "*** Incomplete Force Multiple Outputs message!\n");
    buf[1] = 128 + FORCE_MULTIPLE_OUTPUTS;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  int data_bytes = buf[6];
  if (data_bytes == 0)
  {
    data_bytes = 256;
  }
  int size = 9 + data_bytes;

  n = modio->get_buffer(buf + 7, size - 7);
  if (n != (size - 7))
  {
    react_trace.dprintf(5,
       "*** Not Enough data for Force Multiple Outputs message!\n");
    buf[1] = 128 + FORCE_MULTIPLE_OUTPUTS;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  /******
  if (!check_CRC(buf, size, 0xffff))
  {
    react_trace.dprintf(5, "****** Bad CRC (Force Multiple Outputs)\n");
  }
  *****/
  react_trace.print_buf(1, "Force Multiple Outputs:\n", buf, size);
  //add_CRC(buf, 8, 0xffff);
  //modio->put_buffer(buf, 8);
  modio->put_buffer(buf, 6);
  modio->send_message();
  react_trace.print_buf(1, "Reply:\n", buf, 6);
}

/**********************************************************************/

void ModSerialSim::preset_multiple_registers(uint8 *buf)
{
  int n = modio->get_buffer(buf + 2, 5);
  if (n != 5)
  {
    react_trace.dprintf(5, "*** Incomplete Preset Multiple Registers message!\n");
    buf[1] = 128 + PRESET_MULTIPLE_REGISTERS;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  int data_bytes = buf[6];
  if (data_bytes == 0)
  {
    data_bytes = 256;
  }
  int size = 9 + data_bytes;

  n = modio->get_buffer(buf + 7, size - 7);
  if (n != (size - 7))
  {
    react_trace.dprintf(5,
    "*** Not Enough data for Preset Multiple Registers message!\n");
    buf[1] = 128 + PRESET_MULTIPLE_REGISTERS;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  /****
  if (!check_CRC(buf, size, 0xffff))
  {
    react_trace.dprintf(5, "****** Bad CRC (Preset Multiple Registers)\n");
  }
  ****/
  react_trace.print_buf(1, "Preset Multiple Registers:\n", buf, size);
  react_trace.print_buf(1, "Reply:\n", buf, 6);
  //add_CRC(buf, 8, 0xffff);
  //modio->put_buffer(buf, 8);
  modio->put_buffer(buf, 6);
  modio->send_message();
}

/**********************************************************************/

void ModSerialSim::report_device_type(uint8 *buf)
{
  int n = modio->get_buffer(buf, 2);
  if (n != 2)
  {
    react_trace.dprintf(5, "*** Incomplete Report Device Type message!\n");
    buf[1] = 128 + REPORT_DEVICE_TYPE;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  /********
  if (!check_CRC(buf, 4, 0xffff))
  {
    react_trace.dprintf(5, "****** Bad CRC (Report Device Type)\n");
  }
  ********/
  react_trace.print_buf(1, "Report Device Type:\n", buf, 4);
  buf[2] = 5;
  buf[3] = 0;
  buf[4] = 0;
  buf[5] = 0;
  buf[6] = 0;
  buf[7] = 0;
  react_trace.print_buf(1, "Reply:\n", buf, 8);
  //add_CRC(buf, 10, 0xffff);
  //modio->put_buffer(buf, 10);
  modio->put_buffer(buf, 8);
  modio->send_message();

}

/**********************************************************************/

void ModSerialSim::read_scratch_pad(uint8 *buf)
{
  int n = modio->get_buffer(buf, 6);
  if (n != 6)
  {
    react_trace.dprintf(5, "*** Incomplete Read Scratch Pad message!\n");
    buf[1] = 128 + READ_SCRATCH_PAD;
    buf[2] = 4;
    //add_CRC(buf, 5, 0xffff);
    //modio->put_buffer(buf, 5);
    modio->put_buffer(buf, 3);
    modio->send_message();
    return;
  }
  /******
  if (!check_CRC(buf, 8, 0xffff))
  {
    react_trace.dprintf(5, "****** Bad CRC (Read Scratch Pad)\n");
  }
  ****/
  react_trace.print_buf(1, "Read Scratch Pad:\n", buf, 6);
  buf[2] = 0;
  react_trace.print_buf(1, "Reply:\n", buf, 3);
  //add_CRC(buf, 5, 0xffff);
  //modio->put_buffer(buf, 5);
  modio->put_buffer(buf, 3);
  modio->send_message();
}

/**********************************************************************/

