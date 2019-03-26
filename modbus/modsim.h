
#ifndef __MODSIM_INC__
#define __MODSIM_INC__

class ModSerialSim 
{
private:
  mod_io_t *modio;
  int extra;
  bool inc_mode;
  int extra_ctr;
public:
  //ModSerialSim(GFComLink *dp) : GFSerial(dp) {timeout = 500;};
  ModSerialSim(mod_io_t *io) {modio = io; extra = 65520; inc_mode = true; extra_ctr = 0;};
  int process_request(void);
  void read_output_table(uint8 *buf);
  void read_input_table(uint8 *buf);
  void read_registers(uint8 *buf);
  void read_analog_inputs(uint8 *buf);
  void force_single_output(uint8 *buf);
  void preset_single_register(uint8 *buf);
  void read_exception_status(uint8 *buf);
  void loopback(uint8 *buf);
  void force_multiple_outputs(uint8 *buf);
  void preset_multiple_registers(uint8 *buf);
  void report_device_type(uint8 *buf);
  void read_scratch_pad(uint8 *buf);
};

#endif
