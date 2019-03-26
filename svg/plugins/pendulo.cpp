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

class pendulo_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new pendulo_t;
}

const char *pendulo_t::get_name(void)
{
  return "pendulo";
}

void pendulo_t::generate_doc(doc_object_base_t *dob)
{
  /**
  dob->start("pendulo", "A simple pendulum");
  dob->notes("Just for testing");
  dob->end();
  **/
}


void pendulo_t::generate(plugin_data_t d, int argc, char **argv)
{
  const char *tag = argv[1];
  const char *color = argv[2];
  double x = atof(argv[3]);
  double y = atof(argv[4]);
  double height = atof(argv[5]);
  double r = height / 10.0;
  char obj_name[100];
  double font_size = height * 0.2;
  
  fprintf(d.svg_fp, "<!--  START insert for pendulo (%03d) -->\n", n_instance);

  //<path fill="green" id="red_needle"
   //   d="M69,75 A6,6 0 1,1 81,75 L75,140 Z" />

  fprintf(d.js_fp, "// --  START insert for pendulo (%03d)\n", n_instance);
  fprintf(d.svg_fp, "<path fill=\"%s\" id=\"pendulo_%03d\"\n",
           color, n_instance);
         
  fprintf(d.svg_fp, "    d=\"M%lg,%lg A%lg,%lg 0 1,1 %lg,%lg L%lg,%lg Z\" />\n",
           x - r, y, r, r,  x + r, y, x, y + height);

  fprintf(d.svg_fp, "<text id=\"pendulo_pv_%03d\" x=\"%lg\" y=\"%lg\" font-family=\"Verdana\" font-size=\"%lg\" fill=\"black\" text-anchor=\"middle\">0</text>\n",
                     n_instance, x, y - r - (font_size * 1.1), font_size);


  snprintf(obj_name, sizeof(obj_name), "pendulo_obj_%03d", n_instance);

  fprintf(d.js_fp, "var %s = new pendulo_t(\"pendulo_%03d\", \"pendulo_pv_%03d\", %lg, %lg);\n", 
               obj_name, n_instance, n_instance, x, y);

  fprintf(d.svg_fp, "<!--  END insert for pendulo (%03d) -->\n", n_instance);
   fprintf(d.js_fp, "// --  END insert for pendulo (%03d)\n", n_instance);


  add_js_library("pendulo.js");
  add_animation_object(tag, obj_name);

  n_instance++;
}

