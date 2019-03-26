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

class svg_header_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new svg_header_t;
}

const char *svg_header_t::get_name(void)
{
  return "svg_header";
}

/*********************************/

void svg_header_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("svg_header", "Inserts the SVG header");
  dob->param("SVG Title");
  dob->param("X1 of Viewbox");
  dob->param("Y1 of Viewbox");
  dob->param("X2 of Viewbox");
  dob->param("Y2 of Viewbox");
  dob->notes("Other types of headers are possible. "
    "For example you might want to make a header suitable for "
    "embedding in an HTML file, one that takes up only a small "
    "part of the screen, etc.");
  dob->example("svg_header|Spray Demo|0|0|300|150|");
  dob->end();
}

void svg_header_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 6)
  {
    printf("There must be 6 arguments to svg_header\n");
    exit(-1);
  }

  const char *title = argv[1];
  const char *x1 = argv[2];
  const char *y1 = argv[3];
  const char *x2 = argv[4];
  const char *y2 = argv[5];

  fprintf(d.svg_top_of_file_fp, "<!--  START insert svg header -->\n");

  fprintf(d.svg_top_of_file_fp, "<svg xmlns=\"http://www.w3.org/2000/svg\"\n");
  fprintf(d.svg_top_of_file_fp, "     xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n");
  fprintf(d.svg_top_of_file_fp, "     width=\"100%%\" height=\"100%%\" viewBox=\"%s %s %s %s\" onload=\"load()\">\n",
          x1, y1, x2, y2);
  fprintf(d.svg_top_of_file_fp, "\n");
  fprintf(d.svg_top_of_file_fp, "  <title>%s</title>\n", title);
  fprintf(d.svg_top_of_file_fp, "  <metadata>author: Don Carr</metadata>\n");
 
  fprintf(d.svg_top_of_file_fp, "<!--  END insert svg header -->\n");
  n_instance++;
}

