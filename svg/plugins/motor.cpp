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
static const char *shadow = " filter=\"url(#pipeshadow)\" ";

class motor_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new motor_t;
}

const char *motor_t::get_name(void)
{
  return "motor";
}

void motor_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("motor", "simple electric motor with cooling fins");
  dob->param("Discrete Tag to animate the color (can be null or blank)");
  dob->param("On Color (typically a bright color)");
  dob->param("Off Color (typically a shade of gray)");
  dob->param("Center X of where shaft attaches");
  dob->param("Center Y of where shaft attaches");
  dob->param("Width of motor");
  dob->param("Angle of rotation around the shaft attach point");
  dob->param("Generate popup 1 = yes, 0 = no");
  dob->example("motor|PUMP1_ON|lime|grey|38|60|12|0|");
  dob->notes("Height is 54% of width");
  dob->end();
}



void motor_t::generate(plugin_data_t d, int argc, char **argv)
{
  if ((argc != 8) && (argc != 9))
  {
    printf("%s, line %d: There must be 8 or 9 arguments to motor\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  const char *the_tag = argv[1];
  const char *on_color = argv[2];
  const char *off_color = argv[3];
  //double x1 = atof(argv[4]);
  //double y1 = atof(argv[5]);
  double cx = atof(argv[4]);
  double cy = atof(argv[5]);
  double width = atof(argv[6]);
  int angle = atoi(argv[7]);
  double scale_factor = width / 100.0;
  double height = 54.0 * scale_factor;
  bool gen_popup = false;

  //double cx = x1 + (width / 2.0);
  //double cy = y1 + (height / 2.0);
  double x1 = cx;
  double y1 = cy - (height / 2.0);

  if (d.popup_on && (argc > 8))
  {
    gen_popup = (argv[8][0] == '1');
  }



//motor|tag|on_color|off_collor|x1|y1|width|rotation(0,90,180,270)|

  char js_object_name[30];
  char js_group_name[30];
  snprintf(js_object_name, sizeof(js_object_name), "motor_obj_%03d", n_instance);
  snprintf(js_group_name, sizeof(js_group_name), "motor_group_obj_%03d", n_instance);

  char str[200];
  if (angle == 0) str[0] = '\0';
  else snprintf(str, sizeof(str), "transform=\"rotate(%d %lg,%lg)\"", angle, cx, cy);

  fprintf(d.svg_fp, "<!--  START insert for motor (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  START insert for motor (%03d)\n", n_instance);

  fprintf(d.svg_fp, "<g  %s >\n", str);
  fprintf(d.svg_fp, "<rect id=\"%s\" x=\"%lg\" y=\"%lg\" rx=\"%lg\" ry=\"%lg\" \n"
                    "width=\"%lg\" height=\"%lg\" fill=\"%s\" stroke=\"black\" stroke-width=\"%lg\" %s/>\n",
       js_group_name, x1, y1, 5.0 * scale_factor, 8.0 * scale_factor, 
       width, height, on_color, 0.6 * scale_factor, shadow);
  double fin_x = x1 + 20 * scale_factor; 
  double fin_width = 75.0 * scale_factor;
  double fin_height = 3.0 * scale_factor;
  double fin_rx = 1.5 * scale_factor;
  double fin_stroke_width = 0.5 * scale_factor;
  for (int i=0; i < 6; i++)
  {
    double fin_y = y1 + (((i * 10.0) + 1.0) * scale_factor);
    fprintf(d.svg_fp, "  <rect x=\"%lg\" y=\"%lg\" rx=\"%lg\" ry=\"%lg\" width=\"%lg\" height=\"%lg\"\n"
         "        fill=\"black\" stroke=\"black\" stroke-width=\"%lg\"/>\n", 
      fin_x, fin_y, fin_rx, fin_rx, fin_width, fin_height, fin_stroke_width);
  }
  fprintf(d.svg_fp, "<line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" stroke=\"black\" stroke-width=\"%lg\"/>\n",
          fin_x, y1, fin_x, y1 + height, 1.5 * scale_factor);
  fprintf(d.svg_fp, "<line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" stroke=\"black\" stroke-width=\"%lg\"/>\n", 
          fin_x + fin_width, y1, fin_x + fin_width, y1 + height, 1.5 * scale_factor);

  fprintf(d.svg_fp, "</g>\n");


//name, on_val, off_val

  //fprintf(d.js_fp, "var  xxx = [{\"name\":\"%s\", \"property\":\"color\", \"on_val\":\"%s\", \"off_val\":\"%s\"}];\n",
   //         js_group_name, on_color, off_color);

  //for (var i=0; i < xxx.length;  i++)
  //{
   // consolo.log("name: " + this.objs[i].name + ", property: " + this.objs[i].property + ", on_val: " + this.objs[i].on_val + ", off_val: " + this.objs[i].off_val);
  //}
  //fprintf(d.js_fp, "console.log(xxx)\n");
  //fprintf(d.js_fp, "var %s = new discrete_t(\"[{'name':'%s', 'property':'color', 'on_val':'%s', 'off_val':'%s'}]\");\n", 

  //fprintf(d.js_fp, "var %s = new discrete_t(xxx);\n", 
  //          js_object_name);//, js_group_name, on_color, off_color);

  if (gen_popup)
  {
    double px, py;
    find_a_place_nearby(&px, &py, x1, y1, width, height);
    fprintf(d.svg_fp, "<rect x=\"%lg\"  y=\"%lg\" width=\"%lg\" height=\"%lg\"\n"
       "   onclick=\"show_popup(%lg,%lg,'ON', 'Off', '%s')\" visibility=\"hidden\"\n"
       "   pointer-events=\"all\" onmouseover=\"this.style.cursor='pointer';\"/>\n",
           x1, y1, width, width, px, py, the_tag);
  }

  fprintf(d.js_fp, "// --  END insert for motor (%03d)\n", n_instance);
  fprintf(d.svg_fp, "<!--  END insert for motor (%03d) -->\n", n_instance);

  if ((strlen(the_tag) > 0) && (0 != strcasecmp(the_tag, "null")))
  {
    fprintf(d.js_fp, "var %s = new pump_t(\"%s\", \"%s\", \"%s\");\n",
             js_object_name, js_group_name, on_color, off_color);
    add_js_library("pump.js");
    add_animation_object(the_tag, js_object_name);
  }

  add_svg_library("pipeshadow.svg");
  n_instance++;
}

