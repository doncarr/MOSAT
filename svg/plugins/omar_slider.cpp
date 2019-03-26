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

class omar_slider_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new omar_slider_t;
}

const char *omar_slider_t::get_name(void)
{
  return "omar_slider";
}

void omar_slider_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("omar_slider", "Inserts Omar's simple slider");
  dob->param("X of upper left corner");
  dob->param("Y of upper left corner");
  dob->param("Width of Slider");
  dob->param("Height of Slider");
  dob->end();
}

void omar_slider_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 5)
  {
    printf("%s, line %d: There must be 7 arguments to omar_slider\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  double x = atof(argv[1]);
  double y = atof(argv[2]);
  double width = atof(argv[3]);
  double height = atof(argv[4]);
  double grab_width = width * (40.0/30.0);
  double grab_height = height * (20.0/450.0);
  double grab_offset = 0.5 * (grab_width - width);
  
  fprintf(d.svg_fp, "<!--  START insert for omar_slider (%03d) -->\n", n_instance);

 
  fprintf(d.svg_fp, "<rect height=\"%lg\"   width=\"%lg\"   x=\"%lg\"    y=\"%lg\"    id=\"sliderFrame\" style=\"fill:#00ff00\" />\n",
    height, width, x, y);
  fprintf(d.svg_fp, "<rect height=\"%lg\"  width=\"%lg\"  x=\"%lg\"  y=\"%lg\"  id=\"handle\"  style=\"fill:#0000ff\" /> \n", 
    grab_height, grab_width, x - grab_offset, y + height / 5);        
  //fprintf(d.svg_fp, "<text id='monitor' x=\"150\" y=\"250\">50</text>\n");
  fprintf(d.svg_fp, "<!--  END insert for omar_slider (%03d) -->\n", n_instance);
  n_instance++;
}

