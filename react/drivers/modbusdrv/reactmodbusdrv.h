/************************************************************************
This software is part of React, a control engine
Copyright (C) 2005,2006 Donald Wayne Carr 

This program is free software; you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation; either version 2 of the License, or 
(at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You should have received a copy of the GNU General Public License along 
with this program; if not, write to the Free Software Foundation, Inc., 
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
***********************************************************************/

#define REACT_MOD_DI (1)
#define REACT_MOD_AI (2)
#define REACT_MOD_AO (3)
#define REACT_MOD_DO (4)
#define REACT_MOD_ERROR_MAP (10)

#define REACT_MAX_MOD_IO (32)
#define REACT_MAX_MOD_DO_MAP (32)
#define REACT_MAX_MOD_AO_MAP (32)
#define REACT_MAX_MOD_AI (128)
#define REACT_MAX_MOD_DI (128)
#define REACT_MAX_MOD_DO (64)
#define REACT_MAX_MOD_AO (64)

struct mod_io_def_t
{
  int modbus_id; // With a radio, or RS485, you can have multiple modbus ids for a single serial port 
  int type;
  int opcode;
  int n;
  int modbus_offset;
  int channel_offset;   
};


struct do_send_t
{
  int ch;
  unsigned char val;
};

struct ao_send_t
{
  int ch;
  unsigned short val;
};


class reactmodbus_driver_t : public io_driver_t
{
private:
  bool read_values;
  bool wake_him_up;
  int x[20];
  rtmodbus_t *modbus;
  unsigned short ai_vals[REACT_MAX_MOD_AI];
  bool di_vals[REACT_MAX_MOD_DI];
  unsigned short tmp_ai_vals[REACT_MAX_MOD_AI];
  bool tmp_di_vals[REACT_MAX_MOD_DI];
  do_send_t do_vals_to_send[64];  
  ao_send_t ao_vals_to_send[64];  
  int n_dos_to_send;
  int n_aos_to_send;
  int modbus_error_map_start;
  int modbus_error_map_n;
  bool alt_do_opcode;
  bool alt_ao_opcode;
  uint8 default_modbus_id;
  int di_offset;
  int do_offset;
  int ai_offset;
  int ao_offset;
  sem_t output_mutex_sem; 
  sem_t read_mutex_sem; 
  sem_t read_wait_sem; 
  int n_mod_io; 
  mod_io_def_t mod_io[REACT_MAX_MOD_IO];
  int n_ao_map;
  mod_io_def_t ao_map[REACT_MAX_MOD_AO_MAP];
  int n_do_map;
  mod_io_def_t do_map[REACT_MAX_MOD_AO_MAP];
  void add_io(int modbus_id, const char *io_type, int opcode, int n_io, 
               int modbus_offset, int channel_offset);
  void set_mod_error(int retval, int unit_id);
  void read_mod_io(void);
  void check_overlap(int a_modbus_id, const char *io_type, int opcode, 
                  int n_io, int modbus_offset, int channel_offset);
public:
  reactmodbus_driver_t(react_drv_base_t *react, const char *option);
  void read(void);
  void end_read(void);
  void read_thread(void);
  bool get_di(int channel);
  double get_ai(int channel);
  void get_ai_range(int start, int end, double vals[]);
  long get_count(int channel);
  void send_do(int channel, bool val);
  void send_ao(int channel, double val);
  void map_ao(int ch, int *modbus_ch, int *modbus_id, int *opcode);
  void map_do(int ch, int *modbus_ch, int *modbus_id, int *opcode);
  void close(void);
};


