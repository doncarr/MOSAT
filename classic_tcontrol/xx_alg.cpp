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


#include <stdio.h>

#include "alg.h"
#include "xx_alg.h"

/******************************************************************/

jl_algorithm_t::jl_algorithm_t(void)
{
}

/******************************************************************/

void jl_algorithm_t::set_notify_object(
     alg_notify_object_t *the_object, int the_estop_key)
{
}

/******************************************************************/

void jl_algorithm_t::initialize_sizes(int sec_cnt, int max_trains)
{
}

/******************************************************************/

void jl_algorithm_t::set_section_keys(int keys[], int n_keys)
{
}

/******************************************************************/

void jl_algorithm_t::set_entry_sections(
           int entry_sections[], int entry_keys[], int num_entrys)
{
}

/******************************************************************/

void jl_algorithm_t::initial_state(int train_locations[], int n_locations)
{
}

/******************************************************************/

void jl_algorithm_t::crossing_event(int section)
{
}

/******************************************************************/

void jl_algorithm_t::entry_event(int section)
{
}

/******************************************************************/

void jl_algorithm_t::exit_event(int section)
{
}

/******************************************************************/

bool jl_algorithm_t::request_entry(int section)
{
  return true;
}

/******************************************************************/

void jl_algorithm_t::cancel_entry(int section)
{
}

/******************************************************************/

