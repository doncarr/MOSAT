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

class triangle_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new triangle_t;
}

const char *triangle_t::get_name(void)
{
  return "triangle";
}

void triangle_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("triangle", "A simple triangle to indicate flow in or out");
  dob->param("Center X of point of triange");
  dob->param("Center Y of point of triange");
  dob->param("base width of triange");
  dob->param("Fill color of triange");
  dob->param("Rotation (typically 0, 90, 180, or 270)");
  dob->example("triangle|252|25|5|black|90|");
  dob->end();
}

void triangle_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 6)
  {
    printf("%s, line %d: There must be 6 arguments to triangle\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  double cx = atof(argv[1]);
  double cy = atof(argv[2]);
  double width = atof(argv[3]);
  const char *color = argv[4];
  int angle = atoi(argv[5]);
  double height = width * 1.2;

  // triangle|cx|cy|width|color|angle|
  
  char str[200];
  if (angle == 0) str[0] = '\0';
  else snprintf(str, sizeof(str), "transform=\"rotate(%d %lg,%lg)\"", angle, cx, cy);
  if (!d.silent) printf("Triangle: %s\n", str);


  fprintf(d.svg_fp, "<!--  START insert for triangle (%03d) -->\n", n_instance);

  fprintf(d.svg_fp, "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg \" stroke=\"black\" stroke-width=\"%lg\" fill=\"%s\" %s/>\n",
             cx - (width / 2.0), cy, cx + (width / 2.0), cy, cx, cy - height, 0.05 * width, color, str);

  fprintf(d.svg_fp, "<!--  END insert for triangle (%03d) -->\n", n_instance);
  n_instance++;
}

