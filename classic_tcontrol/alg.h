/***************************************************************************
Copyright (c) 2002,2003,2004 Donald Wayne Carr 

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS IN THE SOFTWARE.
*************************************************************************/


#define MAX_SECTIONS (60)
#define MAX_TRAINS (30)
#define MAX_ALGORITHMS (5)
#define MAX_ELEC_SUBSTATIONS (20)


// Object passed to trains so that they can control the stop lights.
class alg_notify_object_t
{
public:
  virtual void estop(int key, const char *reason) = 0;
  virtual void permit_section_entry(int section, int key) = 0;
  virtual void prohibit_section_entry(int section, int key) = 0;
  virtual void permit_system_entry(int section, int key) = 0;
  virtual void prohibit_system_entry(int section, int key) = 0;
  virtual ~alg_notify_object_t(void) {};
};

// Base class for algorithms. All algorithms must inherit from this
// class and implement all functions.

class base_algorithm_t
{
public:
  virtual void set_notify_object(
            alg_notify_object_t *notify_object, int estop_key) = 0;
  virtual void initialize_sizes(int n_sections, int max_trains) = 0;
  virtual void set_section_keys(int keys[], int n_keys) = 0;
  virtual void set_entry_sections(
         int entry_sections[], int entry_keys[], int num_entrys) = 0;
  virtual void initial_state(int train_locations[], int n_locations) = 0;

  virtual void crossing_event(int section) = 0;
  virtual void entry_event(int section) = 0;
  virtual void exit_event(int section) = 0;
  virtual bool request_entry(int section) = 0;
  virtual void cancel_entry(int section) = 0;
  virtual ~base_algorithm_t(void){};
};



