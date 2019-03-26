/************************************************************************
This software is part of React, a control engine
Copyright (C) 2012 Donald Wayne Carr 

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gen_display.h"

static int n_instance = 1;

class insert_svg_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new insert_svg_t;
}

const char *insert_svg_t::get_name(void)
{
  return "insert_svg";
}

void insert_svg_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("insert_svg", "Inserts raw SVG code into the SVG portion of the file");
  dob->param("Exact SVG text to be inserted");
  dob->notes("For simple things that do not require a dedicated widget");
  dob->example("insert_svg|&lt;circle cx=\"107.5\" cy=\"126\" r=\"0.7\" fill=\"back\"/&gt;|");
  dob->end();
}



void insert_svg_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 2)
  {
    printf("%s, line %d: There must be 2 arguments to insert_svg\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  const char *the_svg = argv[1];
  
  fprintf(d.svg_fp, "<!--  START insert for insert_svg (%03d) -->\n", n_instance);
  fprintf(d.svg_fp, "%s\n", the_svg); 
  fprintf(d.svg_fp, "<!--  END insert for insert_svg (%03d) -->\n", n_instance);
  n_instance++;
}

