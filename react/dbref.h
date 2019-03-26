
#ifndef __DBREF_INC__
#define __DBREF_INC__

class rt_bool_ref_t
{
public:
  virtual bool val(void) = 0;
  virtual ~rt_bool_ref_t(void){};
};

class rt_double_ref_t
{
public:
  virtual double val(void) = 0;
  virtual ~rt_double_ref_t(void){};
};

class rt_long_ref_t
{
public:
  virtual long val(void) = 0;
  virtual ~rt_long_ref_t(void){};
};

class rt_double_ptr_ref_t : public rt_double_ref_t
{
private:
  double *dptr;
public:
  rt_double_ptr_ref_t(double *a_dptr){dptr = a_dptr;};
  inline double val(void){return *dptr;};
  virtual ~rt_double_ptr_ref_t(void){};
};

class rt_bool_ptr_ref_t : public rt_bool_ref_t
{
private:
  bool *bptr;
public:
  rt_bool_ptr_ref_t(bool *a_bptr){bptr = a_bptr;};
  inline bool val(void){return *bptr;};
  virtual ~rt_bool_ptr_ref_t(void){};
};

class rt_long_ptr_ref_t : public rt_long_ref_t
{
private:
  long *lptr;
public:
  rt_long_ptr_ref_t(long *a_lptr){lptr = a_lptr;};
  inline long val(void){return *lptr;};
  virtual ~rt_long_ptr_ref_t(void){};
};

rt_bool_ref_t *react_get_discrete_ref_fn(char *tag);
rt_double_ref_t *react_get_analog_ref_fn(char *tag);

#endif

