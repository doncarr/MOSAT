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

class simple_panel_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new simple_panel_t;
}

const char *simple_panel_t::get_name(void)
{
  return "simple_panel";
}

void simple_panel_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("simple_panel", "Simple round panel light");
  dob->param("Discrete Tag");
  dob->param("On Color");
  dob->param("Off Color");
  dob->param("Center X");
  dob->param("Center Y");
  dob->param("Radius");
  dob->example("simple_panel|PUMP1_ON|darkturquoise|gray|86|109|8|");
  dob->end();
}



void simple_panel_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 7)
  {
    printf("%s, line %d: There must be 7 arguments to simple_panel\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  const char *tag = argv[1];
  const char *on_color = argv[2];
  const char *off_color = argv[3];
  double cx = atof(argv[4]);
  double cy = atof(argv[5]);
  double r = atof(argv[6]);
  double stroke_width = r * 0.1;
  double font_size = r * 0.6;

  fprintf(d.svg_fp, "<!--  START insert for simple_panel (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  START insert for simple_panel (%03d)\n", n_instance);

  fprintf(d.svg_fp, "<circle id=\"simple_panel_%03d\" cx=\"%lg\" cy=\"%lg\" r=\"%lg\" fill=\"%s\" stroke=\"black\" stroke-width=\"%lg\"/>\n",
            n_instance, cx, cy, r, on_color, stroke_width);
  fprintf(d.svg_fp, "<text id=\"simple_panel_pv_%03d\" x=\"%lg\" y=\"%lg\" font-family=\"Verdana\" font-size=\"%lg\" fill=\"black\" text-anchor=\"middle\">0</text>\n",
                     n_instance, cx, cy + r + (font_size * 1.1), font_size);


  char js_object_name[30];
  char js_pv_name[30];
  snprintf(js_object_name, sizeof(js_object_name), "simple_panel_obj_%03d", n_instance);
  snprintf(js_pv_name, sizeof(js_pv_name), "simple_panel_pv_obj_%03d", n_instance);

  fprintf(d.js_fp, "var %s = new simple_panel_t(\"simple_panel_%03d\", \"simple_panel_pv_%03d\", \"%s\", \"%s\");\n", js_object_name, n_instance, n_instance, on_color, off_color); 

  fprintf(d.svg_fp, "<!--  END insert for simple_panel (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  END insert for simple_panel (%03d)\n", n_instance);

  add_js_library("simple_panel.js");
  add_animation_object(tag, js_object_name);

  n_instance++;
}

