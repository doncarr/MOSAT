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


class stoplight_reader_t
{
public:
  bool read_stoplight(int s) = NULL;
  bool read_alg_setting(int s, int alg) = NULL;
};

class stoplight_mgr_t : stoplight_reader_t
{
private:
  bool actual_stop_light[60];
  bool alg_light_settings[60][n_algs];
public:
  stoplight_mgr_t(int n_sections, int n_algorithms);
};

stoplight_mgr_t::

int main(int argc, char *argv[])
{
}
