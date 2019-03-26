
#ifndef __rt_modbus_opcodes_INC__
#define __rt_modbus_opcodes_INC__

const uint8 READ_OUTPUT_TABLE = 1;
const uint8 READ_INPUT_TABLE = 2;
const uint8 READ_REGISTERS = 3;
const uint8 READ_ANALOG_INPUTS = 4;
const uint8 FORCE_SINGLE_OUTPUT = 5;
const uint8 PRESET_SINGLE_REGISTER = 6;
const uint8 READ_EXCEPTION_STATUS = 7;
const uint8 LOOPBACK = 8;
const uint8 FORCE_MULTIPLE_OUTPUTS = 15;
const uint8 PRESET_MULTIPLE_REGISTERS = 16;
const uint8 REPORT_DEVICE_TYPE = 17;
const uint8 READ_SCRATCH_PAD = 67;

const uint8 MOD_UNDEFINED = 0;
const uint8 __MOD_REGISTER_OUT = 1; /* No longer used */
const uint8 MOD_REGISTER = 2;
const uint8 MOD_ANALOG_IN = 3;
const uint8 MOD_DISCRETE_IN = 4;
const uint8 MOD_DISCRETE_OUT = 5;


#endif
