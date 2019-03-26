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

class valve_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new valve_t;
}

const char *valve_t::get_name(void)
{
  return "valve";
}

void valve_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("valve", "Simple two way valve animated by a discrete value");
  dob->param("Discrete tag");
  dob->param("On Color");
  dob->param("Off Color");
  dob->param("Center X");
  dob->param("Center Y");
  dob->param("Width");
  dob->param("Type (1 = square handle, 2 = rounded handle)");
  dob->param("Rotation Angle (typically 0, 90, 180, or 270)");
  dob->param("Generate popup 1 = yes, 0 = no");
  dob->example("valve|VALVE3_ON|lime|gray|240|50|8|2|270|1|");
  dob->notes("We need to continue adding types of valves.");
  dob->end();
}

void valve_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 10)
  {
    printf("There must be 10 arguments to 'valve'\n");
    exit(-1);
  }

  const char *tag = argv[1];
  const char *on_color = argv[2];
  const char *off_color = argv[3];
//  double x1 = atof(argv[4]);
//  double y1 = atof(argv[5]);
// new
  double cx = atof(argv[4]);
  double cy = atof(argv[5]);
//
  double width = atof(argv[6]);
  int type = atoi(argv[7]);
  int angle = atoi(argv[8]);
  double scale_factor = width / 50.0;
//  double cx = x1 + (width / 2.0);
//  double cy = y1 + (30.0 * scale_factor);
// new
  double x1 = cx - (width / 2.0);
  double y1 = cy - (30.0 * scale_factor);
//
  double x2 = x1 + width;
  double y2 = y1 + width;
  bool gen_popup = false;

  
  if (d.popup_on && (argc > 9))
  {
    gen_popup = (argv[9][0] == '1');
  }



//tag|on_color|off_collor|x1|y1|width|type(1-2)|rotation(0,90,180,270)|

  if (type > 2) type = 2;
  if (type < 1) type = 1;

  char js_object_name[30];
  char js_group_name[30];
  snprintf(js_object_name, sizeof(js_object_name), "valve_obj_%03d", n_instance);
  snprintf(js_group_name, sizeof(js_group_name), "valve_group_obj_%03d", n_instance);

  char str[200];
  if (angle == 0) str[0] = '\0';
  else snprintf(str, sizeof(str), "transform=\"rotate(%d %lg,%lg)\"", angle, cx, cy);

  fprintf(d.svg_fp, "<!--  START insert for valve (%03d) -->\n", n_instance);


  fprintf(d.svg_fp, "<g id=\"%s\" fill=\"%s\" stroke=\"black\" stroke-width=\"%lg\" %s>\n", 
       js_group_name, on_color, 0.5 * scale_factor, str);
  if (type == 1)
  {
    fprintf(d.svg_fp, "  <path  \n");
    fprintf(d.svg_fp, "     d=\"M%lg %lg A%lg %lg 0 1 1 %lg %lg Z\"/>\n",
           x1 + (10.0 * scale_factor), y1 + (10.0 * scale_factor), 
           5.0 * scale_factor, 5.0 * scale_factor, 
           x1 + (40.0 * scale_factor), y1 + (10.0 * scale_factor));
  }
  else if (type == 2)
  {
    fprintf(d.svg_fp, "  <rect x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\"/>\n",
                 x1 + (10.0 * scale_factor), y1, 
                 30.0 * scale_factor, 10.0 * scale_factor); 
  }
  fprintf(d.svg_fp, "  <polygon points=\"%lg,%lg %lg,%lg %lg,%lg\"/>\n",
                x1, y1 + (10.0 * scale_factor),
                x1, y2, cx, cy);
  fprintf(d.svg_fp, "  <polygon points=\"%lg,%lg %lg,%lg %lg,%lg\"/>\n",
                x2, y1 + (10.0 * scale_factor),
                x2, y2, cx, cy);
  fprintf(d.svg_fp, "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" \n",
               cx, y1 + (10.0 * scale_factor), cx, cy);
  fprintf(d.svg_fp, "     stroke =\"black\" stroke-width=\"%lg\"/>\n",
           1.5 * scale_factor);
  fprintf(d.svg_fp, "</g>\n");

  fprintf(d.js_fp, "var %s = new pump_t(\"%s\", \"%s\", \"%s\");\n",
             js_object_name, js_group_name, on_color, off_color);

  if (gen_popup)
  {
    double px, py;
    find_a_place_nearby(&px, &py, x1, y1, width, width);
    fprintf(d.svg_fp, "<rect x=\"%lg\"  y=\"%lg\" width=\"%lg\" height=\"%lg\"\n"
      "   onclick=\"show_popup(%lg,%lg,'Open', 'Close', '%s')\" visibility=\"hidden\"\n"
      "   pointer-events=\"all\" onmouseover=\"this.style.cursor='pointer';\"/>\n",
         x1, y1, width, width, px, py, tag);
  }

  fprintf(d.js_fp, "// --  END insert for valve (%03d)\n", n_instance);
  fprintf(d.svg_fp, "<!--  END insert for valve (%03d) -->\n", n_instance);
  n_instance++;

  add_js_library("pump.js");
  add_animation_object(tag, js_object_name);
}

