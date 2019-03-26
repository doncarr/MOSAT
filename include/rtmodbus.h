
#ifndef __rtmodbus_INC__
#define __rtmodbus_INC__

/***********
  Important! This headerfile is to be included by programs that do NOT
  know the internals of modbus or our implementation. It is important
  that it NOT include any other of the modbus header files, or require
  any of the other modbus header files
***********/

#define RT_MODBUS_SERIAL (0)
#define RT_MODBUS_TCPIP (1)

class rtmodbus_t 
{
public:
  virtual int read_di(unsigned short start_address, int n, bool *vals) = 0;
  // The following is non-standard, but, Phoenix Contact does it, and probably others
  virtual int read_di_register(unsigned short start_register, int n_di, bool *vals) = 0;
  virtual int read_do(unsigned short start_address, int n, bool *vals) = 0;
  virtual int read_ai(unsigned short start_address, int n, unsigned short *vals) = 0;
  virtual int read_reg(unsigned short start_address, int n, unsigned short *vals) = 0;
  virtual int write_reg(unsigned short num, unsigned short val) = 0;
  virtual int send_do(unsigned short num, unsigned char val) = 0;
  virtual int write_multiple_regs(unsigned short, int n, unsigned short *vals) = 0;
  virtual int send_multiple_dos(unsigned short num, int n, unsigned char *vals) = 0;

  virtual void set_address(unsigned char an_addr) = 0; 
  virtual unsigned char get_address(void) = 0; 
  virtual void set_timeout(double a_timeout) = 0;
  virtual void set_debug_level(int a_debug_level) = 0;
  virtual ~rtmodbus_t(void) {};

  /***
  int read_di_req(request_t *req);
  int read_do_req(request_t *req);
  int read_ai_req(request_t *req);
  int read_reg_req(request_t *req);
  ***/

};

  // Here, the device is a serial port /dev/ttyNN, or the IP address with
  // Optional port, examples: "10.0.0.1" (would default to port 502,
  //                          "10.0.0.1:2000"  (would use port 2000
  // You can also use a symbolic name in place of the numeric IP address:
  //           "mycomputer.mydomain.com:1000" would first look up the host, 
  //              then use port 1000.

rtmodbus_t *rt_create_modbus(const char *device); 
rtmodbus_t *rt_create_modbus_serial(const char *device, int baudrate, float timeout, int unit_id);

#endif


