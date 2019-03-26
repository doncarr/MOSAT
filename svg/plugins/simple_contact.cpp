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

class simple_contact_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new simple_contact_t;
}

const char *simple_contact_t::get_name(void)
{
  return "simple_contact";
}

void simple_contact_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("simple_contact", "square box lights up and shows a conect for 'on', gray / disconnect for 'off'");
  dob->param("Discrete Tag");
  dob->param("On Color (typically bright)");
  dob->param("Of Color (typically gray)");
  dob->param("X of upper left corner");
  dob->param("Y of upper left corner");
  dob->param("Width");
  dob->example("simple_contact|HI_LEVEL|yellow|gray|20|52|9|");
  dob->end();
}

void simple_contact_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 7)
  {
    printf("%s, line %d: There must be 7 arguments to simple_contact\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  const char *tag = argv[1];
  const char *on_color = argv[2];
  const char *off_color = argv[3];
  double x = atof(argv[4]);
  double y = atof(argv[5]);
  double width = atof(argv[6]);
  double stroke_width = width * 0.10;
  double on_y2 = y + (width/2.0);
  double off_y2 = y + (width/4.0);

  fprintf(d.svg_fp, "<!--  START insert for simple_contact (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  START insert for simple_contact (%03d)\n", n_instance);

  fprintf(d.svg_fp, "<rect id=\"simple_contact_%03d\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" fill=\"%s\" stroke=\"black\" stroke-width=\"%lg\"/>\n",
            n_instance, x, y, width, width, on_color, stroke_width);

  fprintf(d.svg_fp, "<line id=\"simple_contact_line_%03d\" x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\"\n       style=\"stroke:black;stroke-width:%lg\"/>\n", 
         n_instance, x, on_y2, x + (width * 0.75), on_y2, stroke_width);

  fprintf(d.svg_fp, "<line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\"\n      style=\"stroke:black;stroke-width:%lg\"/>\n", 
         x + (width * 0.7), on_y2, x + width, on_y2, stroke_width);

  char js_object_name[30];
  snprintf(js_object_name, sizeof(js_object_name), "simple_contact_obj_%03d", n_instance);

  fprintf(d.js_fp, "var %s = new simple_contact_t(\"simple_contact_%03d\",\"simple_contact_line_%03d\", \"%s\", \"%s\", %lg, %lg);\n", 
       js_object_name, n_instance, n_instance, on_color, off_color, on_y2, off_y2); 

  fprintf(d.svg_fp, "<!--  END insert for simple_contact (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  END insert for simple_contact (%03d)\n", n_instance);

  add_js_library("simple_contact.js");
  add_animation_object(tag, js_object_name);

  n_instance++;
}

