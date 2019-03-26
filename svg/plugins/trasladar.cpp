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

class trasladar_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new trasladar_t;
}

const char *trasladar_t::get_name(void)
{
  return "trasladar";
}

void trasladar_t::generate_doc(doc_object_base_t *dob)
{
  /***
  dob->start("traslador", "Simple object that translate");
  dob->notes("Testing only");
  dob->end();
  ***/
}

void trasladar_t::generate(plugin_data_t d, int argc, char **argv)
{
//trasladar|LEVEL|mediumslateblue|20|20|280|75
  const char *tag = argv[1];
  const char *color = argv[2];
  double width = atof(argv[3]);
  double x1 = atof(argv[4]);
  double x2 = atof(argv[5]);
  double y = atof(argv[6]);

  
  fprintf(d.svg_fp, "<!--  START insert for trasladar (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  START insert for trasladar (%03d)\n", n_instance);
  fprintf(d.svg_fp, "<rect  id=\"trasladar_%03d\" fill=\"%s\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" stroke=\"none\" stroke-width=\"0\" />\n",
                     n_instance, color, x1, y, width, width);

  char js_object_name[30];
  snprintf(js_object_name, sizeof(js_object_name), "trasladar_obj_%03d", n_instance);

  fprintf(d.js_fp, "var %s = new trasladar_t(\"trasladar_%03d\", %lg, %lg);\n", js_object_name, n_instance, x1, x2); 

  fprintf(d.svg_fp, "<!--  END insert for trasladar (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  END insert for trasladar (%03d)\n", n_instance);
  add_js_library("trasladar.js");
  add_animation_object(tag, js_object_name);

  n_instance++;
}

