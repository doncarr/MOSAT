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

/*******************************************/

static int n_instance = 1;

class groupbox_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

/*******************************************/

extern "C" gen_plugin_base_t *get_object(void)
{
  return new groupbox_t;
}

/*******************************************/

const char *groupbox_t::get_name(void)
{
  return "groupbox";
}

/*******************************************/
void groupbox_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("groupbox", "A really cool group box with rounded edges and shadow");
  dob->param("X of upper left corner");
  dob->param("Y of upper left corner");
  dob->param("Width");
  dob->param("Height");
  dob->param("Radius for rounded corners");
  dob->param("Stroke-width for border");
  dob->param("Color");
  dob->example("groupbox|10|20|50|75|3|1|gray|");
  dob->notes("Can be any SVG color");
  dob->end();
}



void groupbox_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 8)
  {
    printf("%s, line %d: There must be 8 arguments to groupbox\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  double x1 = atof(argv[1]);
  double y1 = atof(argv[2]);
  double width = atof(argv[3]);
  double height = atof(argv[4]);
  double radius = atof(argv[5]);
  double stroke_width = atof(argv[6]);
  const char *color = argv[7];

  fprintf(d.svg_fp, "<!-- START insert for groupbox (%03d) -->\n", n_instance);

                            // "<rect  x=\"%lg\" y=\"%lg\" rx=\"%lg\" ry=\"%lg\" width=\"%lg\" height=\"%lg\""
  fprintf(d.svg_fp, "<rect x='%lg' y='%lg' width='%lg' height='%lg' " 
                    "rx='%lg' ry='%lg' "
                    "stroke='black' stroke-width='%lg' "
                    "fill='%s' filter='url(#filter1)'/>\n",
                    x1, y1, width, height, radius, radius, stroke_width, color);

  fprintf(d.svg_fp, "<!--  END insert for groupbox (%03d) -->\n", n_instance);
  add_svg_library("filter1.svg");

  n_instance++;
}

/*******************************************/

