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

class blades_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new blades_t;
}

const char *blades_t::get_name(void)
{
  return "blades";
}

void blades_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("blades", "Agitator blades with shaft");
  dob->param("Discrete Tagname (can be null or blank if no color change required)");
  dob->param("Center X of the blades and start of shaft");
  dob->param("Center Y of the blades and start of shaft");
  dob->param("Height of the blades");
  dob->param("Length of the shaft");
  dob->param("Rotation Angle (typically 0, 90, 180, or 270");
  dob->param("On Color (typically a bright color)");
  dob->param("Off Color (typically a shade of gray)");
  dob->example("blades|PUMP1_ON|30|60|9|10|0|lime|gray|");
  dob->end();
}

void blades_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 9)
  {
    printf("%s, line %d: There must be 9 arguments to blades\n",
           d.file_name, d.line_number);
  }

  const char *the_tag = argv[1];
  double x1 = atof(argv[2]);
  double y1 = atof(argv[3]);
  double height = atof(argv[4]);
  double shaft_length = atof(argv[5]);
  int angle = atoi(argv[6]);
  const char *on_color = argv[7];
  const char *off_color = argv[8];

  double scale_factor = height / 50.0;

  double cx = x1;
  double cy = y1;

//tag|x1|y1|height|rotation(0,90,180,270)|
  char js_object_name[60];
  char js_group_name[60];
  snprintf(js_object_name, sizeof(js_object_name), "blades_obj_%03d", n_instance);
  snprintf(js_group_name, sizeof(js_group_name), "blades_group_obj_%03d", n_instance);


  char str[200];
  if (angle == 0) str[0] = '\0';
  else snprintf(str, sizeof(str), "transform=\"rotate(%d %lg,%lg)\"", angle, cx, cy);

  fprintf(d.svg_fp, "<!--  START insert for blades (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  START insert for blades (%03d)\n", n_instance);


  fprintf(d.svg_fp, "<g id=\"%s\" fill=\"%s\" stroke=\"black\" stroke-width=\"%lg\" %s>\n", 
                  js_group_name, on_color,  0.3 * scale_factor, str);

  fprintf(d.svg_fp, "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg\"/>\n",
                    cx, cy, x1 + (9.0 * scale_factor), y1 + (7.0 * scale_factor), x1 + (12.0 * scale_factor), y1 ); 

  fprintf(d.svg_fp, "<circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" stroke=\"black\" stroke-width=\"%lg\" fill=\"black\"/>\n",
                    cx, cy, 5 * scale_factor, 0.3 * scale_factor);
  fprintf(d.svg_fp, "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg\" />\n",
                    cx, cy, x1 + (8.0 * scale_factor), y1 - (20.0 * scale_factor), x1, y1 - (25 * scale_factor) );
  fprintf(d.svg_fp, "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg\" />\n",
                    cx, cy, x1 - (8.0 * scale_factor), y1 + (20.0 * scale_factor), x1, y1 + (25.0 * scale_factor));
  fprintf(d.svg_fp, "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg\" />\n",
                    cx, cy, x1 - (10.0 * scale_factor), y1 - (9.0 * scale_factor), x1 - (12.0 * scale_factor), y1 - (1.0 * scale_factor)); 
//  fprintf(d.svg_fp, "<line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" stroke=\"%s\" stroke-width=\"%lg\"/>\n",
//                    cx, cy, x1 + shaft_length, y1, "lime", 4.0 * scale_factor); 
  fprintf(d.svg_fp, "<rect x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" stroke=\"black\" stroke-width=\"%lg\"/>\n",
                    cx, cy - (2.0 * scale_factor), shaft_length, 4.0 * scale_factor, 0.5 * scale_factor); 

  fprintf(d.svg_fp, "</g>\n");

  if ((strlen(the_tag) > 0) && (0 != strcasecmp(the_tag, "null")))
  {
    fprintf(d.js_fp, "var %s = new pump_t(\"%s\", \"%s\", \"%s\");\n",
             js_object_name, js_group_name, on_color, off_color);
    add_js_library("pump.js");
    add_animation_object(the_tag, js_object_name);
  }

  fprintf(d.svg_fp, "<!--  END insert for blades (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  END insert for blades (%03d)\n", n_instance);

  n_instance++;
}
