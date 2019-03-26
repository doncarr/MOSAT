
#ifndef __IODB_CPP_INC__
#define __IODB_CPP_INC__

extern "C"
{
#include <oemdefs.h>
#include <iodb.h>
}

class sxiodb_t
{
public:
  int ai_read(UINT16 start_addr, UINT16 n_to_read, UINT16 *analog_values, const char *str) 
     {return IODBRead(ANALOG_IN, start_addr, n_to_read, (void *)analog_values, str);}; 
  int ao_write(UINT16 start_addr, UINT16 n_to_read, UINT16 *analog_values, const char *str) 
     {return IODBWrite(ANALOG_OUT, start_addr, n_to_read, (void *)analog_values, str);}; 
  int di_read(UINT16 start_addr, UINT16 n_to_read, bool *discrete_values, const char *str) 
     {return IODBRead(DISCRETE_IN, start_addr, n_to_read, (void *)discrete_values, str);}; 
  int do_write(UINT16 start_addr, UINT16 n_to_read, bool *discrete_values, const char *str) 
     {return IODBWrite(DISCRETE_OUT, start_addr, n_to_read, (void *)discrete_values, str);}; 
};

#endif


