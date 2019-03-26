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

class spray_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

/*******************************************/

extern "C" gen_plugin_base_t *get_object(void)
{
  return new spray_t;
}

/*******************************************/

const char *spray_t::get_name(void)
{
  return "spray";
}

/*******************************************/

void spray_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("spray", "Spray patern with dashed lines that grow with analog value");
  dob->param("Analog Tag");
  dob->param("X of center/start of spray");
  dob->param("Y of center/start of spray");
  dob->param("Number of lines in the spray");
  dob->param("Line width of the spray (at 100% of range)");
  dob->param("Width of top of spray");
  dob->param("Width of bottom of spray");
  dob->param("Spray Height");
  dob->example("spray|PRESSURE|180|15|7|1|4|70|90|");
  dob->end();
}

/*******************************************/
void spray_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 9)
  {
    printf("%s, line %d: There must be 9 arguments to spray\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  int i, nlines;
  double topx1, bottomx1, top_increment, bottom_increment;
  double x1, y1, x2, y2;
  double x, y, line_width, top_spray_width, bottom_spray_width, spray_height;

  fprintf(d.svg_fp, "<!-- START insert for spray (%03d) -->\n", n_instance);
  fprintf(d.svg_fp, "<!-- Original generator version written by Ricardo Tucker Carr -->\n");

  const char *the_tag = argv[1];
  x = atof(argv[2]);
  y = atof(argv[3]);
  nlines = atoi(argv[4]);
  line_width = atof(argv[5]);
  top_spray_width = atof(argv[6]);
  bottom_spray_width = atof(argv[7]);
  spray_height = atof(argv[8]);

  if (!d.silent)
  {
    fprintf(stderr, "x = %lg\n", x);
    fprintf(stderr, "y = %lg\n", y);
    fprintf(stderr, "nlines = %d\n", nlines);
    fprintf(stderr, "line_width = %lg\n", line_width);
    fprintf(stderr, "top_spray_width = %lg\n", top_spray_width);
    fprintf(stderr, "bottom_spray_width = %lg\n", bottom_spray_width);
    fprintf(stderr, "spray_height = %lg\n", spray_height);
  } 
  topx1 = x-(top_spray_width/2.0);
  bottomx1 = x-(bottom_spray_width/2.0);
  top_increment = top_spray_width/(nlines-1.0);
  bottom_increment = bottom_spray_width/(nlines-1.0);
  y1 = y;
  y2 = y+spray_height;

  char js_object_name[60];
  char js_group_name[60];
  snprintf(js_object_name, sizeof(js_object_name), "spray_obj_%03d", n_instance);
  snprintf(js_group_name, sizeof(js_group_name), "spray_group_obj_%03d", n_instance);

  fprintf(d.svg_fp, "<g id=\"%s\" fill=\"none\" stroke=\"white\" stroke-width=\"%lg\" stroke-dasharray=\"%lg,%lg\">\n",
           js_group_name, line_width, line_width, 2.0 * line_width);
  for (i = 0; i < nlines; i++)
  {
    x1 = topx1+(i*top_increment);
    x2= bottomx1+(i*bottom_increment);
    //printf(" %lg, %lg,  %lg, %lg\n", x1, y1, x2, y2);
    fprintf(d.svg_fp, "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\"/>\n", x1, y1, x2, y2);
  }
  fprintf(d.svg_fp, "</g>\n");
  fprintf(d.js_fp, "var %s = new spray_t(\"%s\");\n",
             js_object_name, js_group_name);
  fprintf(d.svg_fp, "<!--  END insert for spray (%03d) -->\n", n_instance);
  if ((strlen(the_tag) > 0) && (0 != strcasecmp(the_tag, "null")))
  {
    add_js_library("spray.js");
    add_animation_object(the_tag, js_object_name);
  }

  n_instance++;
}

/*******************************************/

