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

class simple_rect_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new simple_rect_t;
}

const char *simple_rect_t::get_name(void)
{
  return "simple_rect";
}

void simple_rect_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("simple_rect", "Simple retangle that grows up to show analog value and has pv text below");
  dob->param("Analog Tag");
  dob->param("Bar Color");
  dob->param("X of upper left corner");
  dob->param("Y of upper left corner");
  dob->param("Height");
  dob->param("Width (optional, 0.2 * height if not given)");
  dob->param("Font Size for PV (optional, 0.1 * height if not given)");
  dob->example("simple_rect|PRESSURE|mediumslateblue|110|45|70|");
  dob->example("simple_rect|PRESSURE|mediumslateblue|100|35|80|15|8|");
  dob->notes("Uses the following attributes of the tag: "
       "'scale_hi', 'scale_lo', 'eu', 'decimal_places'");
  dob->notes("Automatically calculates width");
  dob->end();
}

void simple_rect_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc < 6)
  {
    printf("%s, line %d: There must be AT LEAST 6 arguments to simple_rect\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  const char *tag = argv[1];
  const char *color = argv[2];
  double x = atof(argv[3]);
  double y = atof(argv[4]);
  double height = atof(argv[5]);
  
  double width = height * 0.2;
  if (argc > 6) width = atof(argv[6]);
  double font_size = height * 0.1;
  if (argc > 7) font_size = atof(argv[7]);
  double cx = x + (width/2.0);
  double cy = y + (height/2.0);
  double stroke_width = height / 100.0;

  
  fprintf(d.svg_fp, "<!--  START insert for simple_rect (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  START insert for simple_rect (%03d)\n", n_instance);
  fprintf(d.svg_fp, "<rect  fill=\"cornsilk\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" stroke=\"none\"/>\n",
                     x, y, width, height);
  fprintf(d.svg_fp, "<rect  id=\"simple_rect_%03d\" fill=\"%s\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" stroke=\"none\" transform=\"rotate(180 %lg %lg)\"/>\n",
                     n_instance, color, (x + stroke_width / 2.0), 
                     y, width-stroke_width, height, cx, cy);
  fprintf(d.svg_fp, "<rect  fill=\"none\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" stroke=\"black\" stroke-width=\"%lg\"/>\n",
                     x, y, width, height, stroke_width);
  fprintf(d.svg_fp, "<text id=\"simple_rect_pv_%03d\" x=\"%lg\" y=\"%lg\" font-family=\"Verdana\" font-size=\"%lg\" fill=\"black\" text-anchor=\"middle\">0</text>\n",
                     n_instance, cx, y + height + (font_size * 1.1), font_size); 

  char js_object_name[30];
  snprintf(js_object_name, sizeof(js_object_name), "simple_rect_obj_%03d", n_instance);

  fprintf(d.js_fp, "var %s = new simple_rect_t(\"simple_rect_%03d\", \"simple_rect_pv_%03d\", %lg);\n", js_object_name, n_instance, n_instance, height); 

  fprintf(d.svg_fp, "<!--  END insert for simple_rect (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  END insert for simple_rect (%03d)\n", n_instance);
  add_js_library("simple_rect.js");
  add_animation_object(tag, js_object_name);

  n_instance++;
}

