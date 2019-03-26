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

/**********************************************************/

#include "gen_display.h"

static int n_instance = 1;

class demlux_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new demlux_t;
}

const char *demlux_t::get_name(void)
{
  return "demlux";
}

void demlux_t::generate_doc(doc_object_base_t *dob)
{
  /***
  dob->start("demlux", "Logo created for &#273;&ecirc;mlux");
  dob->param("X of upper left corner");
  dob->param("Y of upper left corner");
  dob->param("Height");
  dob->end();
  ****/
}

void demlux_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 4)
  {
    printf("%s, line %d: There must be 4 arguments to demlux\n",
           d.file_name, d.line_number);
    exit(-1);
  }
  double x = atof(argv[1]);
  double y = atof(argv[2]);
  double height = atof(argv[3]);
  
  fprintf(d.svg_fp, "<!--  START insert for demlux (%03d) -->\n", n_instance);
  fprintf(d.svg_fp, "<defs>\n");
  fprintf(d.svg_fp, "<linearGradient id=\"MyFirstGradient\" x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\"\n"
                    "   gradientUnits=\"userSpaceOnUse\">\n",
                               x, y, x + height * 5.0, y + height);
  fprintf(d.svg_fp, " <stop id=\"stop1\" style=\"stop-color:darkblue;\" offset=\"0\" />\n");
  fprintf(d.svg_fp, " <stop id=\"stop2\" style=\"stop-color:yellow;\" offset=\"0.5\" />\n");
  fprintf(d.svg_fp, " <stop id=\"stop3\" style=\"stop-color:darkblue;\" offset=\"1\" />\n");
  fprintf(d.svg_fp, "  </linearGradient>\n");
  fprintf(d.svg_fp, "  </defs>\n");
  fprintf(d.svg_fp, "<text x='%lg' y='%lg' font-family='Verdana' font-size='%lg' text-anchor='start'\n"
                    " color='url(#MyFirstGradient)'>đêmlux</text>\n", 
          x, y, height);
  fprintf(d.svg_fp, "<!--  END insert for demlux (%03d) -->\n", n_instance);
  add_svg_library("black_gradients.svg");
  n_instance++;
}

