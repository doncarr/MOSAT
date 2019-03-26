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



class jl_algorithm_t : public base_algorithm_t
{
private:

  /* put you own variables you need here */

public:
  jl_algorithm_t(void);
  void set_notify_object(alg_notify_object_t *notify_object, int estop_key);
  void initialize_sizes(int n_sections, int max_trains);
  void set_section_keys(int keys[], int n_keys);
  void set_entry_sections(int entry_sections[], int keys[], int num_entrys);
  void initial_state(int train_locations[], int n_locations);

  void crossing_event(int section);
  void entry_event(int section);
  void exit_event(int section);
  bool request_entry(int section);
  void cancel_entry(int section);
};



