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

class pump_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new pump_t;
}

const char *pump_t::get_name(void)
{
  return "pump";
}

void pump_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("pump", "A simple pump that can change colors for on/off");
  dob->param("Discrete Tag (can be blank or null)");
  dob->param("On Color");
  dob->param("Off Color");
  dob->param("Center X for the pump");
  dob->param("Center Y for the pump");
  dob->param("Type (1 = clockwise outlet, 2 = counterclockwise outlet)");
  dob->param("Angle of rotation, not including base (typcially 0, 90, 180, or 270)");
  dob->param("Generate popup 1 = yes, 0 = no");
  dob->example("pump|PUMP_ON|lime|gray|65|130|12|1|90|");
  dob->end();
}



void pump_t::generate(plugin_data_t d, int argc, char **argv)
{
  if ((argc != 10) && (argc != 9))
  {
    printf("%s, line %d: There must be 9 or 10 arguments to pump\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  const char *the_tag = argv[1];
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
  bool gen_popup = false;
  double scale_factor = width / 100.0;
//  double cx = x1 + (width / 2.0);
//  double cy = y1 + (width / 2.0);
// new
  double x1 = cx - (width / 2.0);
  double y1 = cy - (width / 2.0);
//
  double x2 = x1 + width;
  double y2 = y1 + width;


  if (d.popup_on && (argc > 9))
  {
    gen_popup = (argv[9][0] == '1');
  }



//tag|on_color|off_collor|x1|y1|width|type(1-2)|rotation(0,90,180,270)|

  char js_object_name[60];
  char js_group_name[60];
  snprintf(js_object_name, sizeof(js_object_name), "pump_obj_%03d", n_instance);
  snprintf(js_group_name, sizeof(js_group_name), "pump_group_obj_%03d", n_instance);

  if (type > 2) type = 2;
  if (type < 1) type = 1;

  char str[200];
  if (angle == 0) str[0] = '\0';
  else snprintf(str, sizeof(str), "transform=\"rotate(%d %lg,%lg)\"", angle, cx, cy);

  
  fprintf(d.svg_fp, "<!--  START insert for pump (%03d) -->\n", n_instance);

  
  fprintf(d.svg_fp, "<g  id=\"%s\" fill=\"%s\" stroke=\"black\" stroke-width=\"%lg\">\n",
        js_group_name, on_color, 0.5 * scale_factor);
  fprintf(d.svg_fp, "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg\" />\n",
             cx, cy, x1, y2 + (5.0 * scale_factor), x2, y2 + (5 * scale_factor) );
  if (type == 1)
  { 
    fprintf(d.svg_fp, "<path \n");
    fprintf(d.svg_fp, "%s", str);
    fprintf(d.svg_fp, "  d=\"M%lg %lg A%lg %lg 0 1 0 %lg %lg L%lg %lg L%lg %lg Z\"/>\n",
            x1, cy, 50.0 * scale_factor, 50.0 * scale_factor, 
            x1 + (30.0 * scale_factor), y1 + (4.1742430 * scale_factor),
            x1 + (30.0 * scale_factor), y1 - (10.0 * scale_factor),
            x1, y1 - (10.0 * scale_factor));
  }
  else if (type == 2)
  {
    fprintf(d.svg_fp, "<path \n");
    fprintf(d.svg_fp, "%s", str);
    fprintf(d.svg_fp, "  d=\"M%lg %lg A%lg %lg 0 1 1 %lg %lg L%lg %lg L%lg %lg Z\"/>\n",
            x2, cy, 50.0 * scale_factor, 50.0 * scale_factor, 
            x1 + (70.0 * scale_factor), y1 + (4.1742430 * scale_factor),
            x1 + (70.0 * scale_factor), y1 - (10.0 * scale_factor),
            x2, y1 - (10.0 * scale_factor));
  }

  fprintf(d.svg_fp, "</g>\n");
  fprintf(d.svg_fp, "<circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" stroke=\"black\" stroke-width=\"%lg\" fill=\"none\"/>\n",
            cx, cy, 15.0 * scale_factor, 0.5 * scale_factor);

  fprintf(d.js_fp, "var %s = new pump_t(\"%s\", \"%s\", \"%s\");\n", 
             js_object_name, js_group_name, on_color, off_color);

  if (gen_popup)
  {
    double px, py;
    find_a_place_nearby(&px, &py, x1, y1, width, width); 
    fprintf(d.svg_fp, "<rect x=\"%lg\"  y=\"%lg\" width=\"%lg\" height=\"%lg\"\n"
       "  onclick=\"show_popup(%lg,%lg,'ON', 'Off', '%s')\" visibility=\"hidden\"\n"
       "  pointer-events=\"all\" onmouseover=\"this.style.cursor='pointer';\"/>\n",
         x1, y1, width, width, px, py, the_tag); 
  }
  fprintf(d.js_fp, "// --  END insert for pump (%03d)\n", n_instance);
  fprintf(d.svg_fp, "<!--  END insert for pump (%03d) -->\n", n_instance);
  if ((strlen(the_tag) > 0) && (0 != strcasecmp(the_tag, "null")))
  {
    add_js_library("pump.js");
    add_animation_object(the_tag, js_object_name);
  }

  n_instance++;
}

