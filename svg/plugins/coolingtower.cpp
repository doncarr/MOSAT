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

class cooling_tower_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new cooling_tower_t;
}

const char *cooling_tower_t::get_name(void)
{
  return "cooling_tower";
}

void cooling_tower_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("cooling_tower", "A simple trapazoid schematic of a cooling tower");
  dob->param("X of upper left corner");
  dob->param("Y of upper left corner");
  dob->param("Width at top");
  dob->param("Fill Color");
  dob->param("Text centered in middle of cooling tower (can be blank)");
  dob->param("Text color");
  dob->example("cooling_tower|205|40|50|gray|CT-902|black|");
  dob->notes("No animation, includes shadow");
  dob->end();
}

void cooling_tower_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 7)
  {
    printf("%s, line %d: There must be 7 arguments to cooling_tower\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  double x1 = atof(argv[1]);
  double y1 = atof(argv[2]);
  double width = atof(argv[3]);
  const char *color = argv[4];
  const char *the_text = argv[5];
  const char *text_color = argv[6];
  double scale_factor = width / 50.0;
  double y2 = y1 + 40 * scale_factor;
  
  fprintf(d.svg_fp, "<!--  START insert for cooling_tower (%03d) -->\n", n_instance);

  fprintf(d.svg_fp, "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg\" stroke=\"black\" stroke-width=\"%lg\" fill=\"%s\" filter=\"url(#filter1)\"/>\n",
             x1, y1, x1 + width, y1, x1 + (40 * scale_factor), y2, x1 + (10.0 * scale_factor), y2, 0.5 * scale_factor, color);
  fprintf(d.svg_fp, "<text x=\"%lg\" y=\"%lg\" font-size=\"%lg\" fill=\"%s\" text-anchor=\"middle\">%s</text>\n",
           x1 + (width / 2.0), y1 + (8.0 * scale_factor), 5.0 * scale_factor, text_color, the_text);
  fprintf(d.svg_fp, "<!--  END insert for cooling_tower (%03d) -->\n", n_instance);
  add_svg_library("filter1.svg");
  n_instance++;
}

