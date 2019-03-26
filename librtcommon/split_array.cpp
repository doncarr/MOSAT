/***************************************************************************
Copyright (c) 2010 Donald Wayne Carr 

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


/***********************************************************************

split_array.c

Contains function to split a json array into strings.

**********************************************************************/

#include <stdio.h>
#include <string.h>
#include "rtcommon.h"
#include "arg.h"


int split_array(char *array, char *argv[], int max_args)
{
  ltrim(array);
  rtrim(array);
  if (array[0] != '[') return 0;
  if (array[strlen(array)-1] != ']') return 0;

  array[strlen(array)-1] = '\0';

  return get_delim_args(array+1, argv, ',', max_args);
}


/***********************************************************************/
