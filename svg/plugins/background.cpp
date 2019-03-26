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

class background_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new background_t;
}

const char *background_t::get_name(void)
{
  return "background";
}

void background_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("background" ,"Set the background color");
  dob->param("The background color.\n");
  dob->example("background|thistle|");
  dob->notes("This will typically be the second line, right after the svg_header.");
  dob->end();
}

void background_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 2)
  {
    printf("%s, line %d: There must be 2 arguments to background\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  const char *color = argv[1];
  fprintf(d.svg_top_of_file_fp, "<!--  START insert background color -->\n");
 
  fprintf(d.svg_top_of_file_fp, "<g id=\"main_group\">\n");
  fprintf(d.svg_top_of_file_fp, "<rect fill=\"%s\" x=\"0%%\" y=\"0%%\" width=\"100%%\" height=\"100%%\" stroke=\"none\" stroke-width=\"0\"/>\n", color);
  fprintf(d.svg_top_of_file_fp, "<!--  END insert background color -->\n");
  n_instance++;
}

