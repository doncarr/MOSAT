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

class valve3way_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new valve3way_t;
}

const char *valve3way_t::get_name(void)
{
  return "valve3way";
}

void valve3way_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("valve3way", "Simple three way valve animated by a discrete value");
  dob->param("Discrete tag");
  dob->param("On Color");
  dob->param("Off Color");
  dob->param("Center X");
  dob->param("Center Y");
  dob->param("Width");
  dob->param("Type (1 = square handle, 2 = rounded handle)");
  dob->param("Rotation Angle (typically 0, 90, 180, or 270)");
  dob->param("On outlet: 1=left, 2=right, 3=bottom");
  dob->param("Off outlet:  1=left, 2=right, 3=bottom");
  dob->param("Generate popup 1 = yes, 0 = no");
  dob->example("valve3way|VALVE3WAY_DIR1|magenta|gray|70|35|8|2|90|2|3|1|");

  dob->notes("This is for a 3 way valve controlled by a boolean that directs fluid to one of "
             "two possible outlets, from one inlet. This means you have to define "
             "the two outlets, and the third, by implication is the inlet.");             

  dob->notes("When you set 'on outlet' and 'off outlet', you are setting where the fluid goes when the "
            "when the valve is 'on' or 'off'. "
            "These directions are looking at the valve with the handle up, so, for example, if you rotate, "
            "the valve 90 degrees, 'right' is on the bottom, 'left' is on the top, and 'bottom' is on the left");

  dob->end();
}

void valve3way_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 12)
  {
    printf("There must be 12 arguments to 'valve3way'\n");
    exit(-1);
  }

  const char *tag = argv[1];
  const char *on_color = argv[2];
  const char *off_color = argv[3];
  double cx = atof(argv[4]);
  double cy = atof(argv[5]);
  double width = atof(argv[6]);
  int type = atoi(argv[7]);
  int angle = atoi(argv[8]);
  int on_code = atoi(argv[9]);
  int off_code = atoi(argv[10]);

  double scale_factor = width / 50.0;
  double x1 = cx - (width / 2.0);
  double x2 = cx + (width / 2.0);
  double y1 = cy - (30.0 * scale_factor);
  double width_factor = 17.5;

  bool gen_popup = false;

  if (d.popup_on && (argc > 11))
  {
    gen_popup = (argv[11][0] == '1');
  }

  
//tag|on_color|off_collor|x1|y1|width|type(1-2)|rotation(0,90,180,270)|

  if (type > 2) type = 2;
  if (type < 1) type = 1;

  if ((on_code < 1) || (on_code > 3))
  {
    printf("The on_code must be 1, 2, or 3: %d\n", on_code);
    exit(-1);
  }
  if ((off_code < 1) || (off_code > 3))
  {
    printf("The off_code must be 1, 2, or 3: %d\n", off_code);
    exit(-1);
  }

  char js_object_name[30];
  char js_on_name[30];
  char js_off_name[30];
  snprintf(js_object_name, sizeof(js_object_name),   "valve3way_obj_%03d",  n_instance);
  snprintf(js_on_name,   sizeof(js_object_name),   "valve3way_out%d_%03d", on_code, n_instance);
  snprintf(js_off_name,   sizeof(js_object_name),   "valve3way_out%d_%03d", off_code, n_instance);

  char transform_str[200];
  if (angle == 0) transform_str[0] = '\0';
  else snprintf(transform_str, sizeof(transform_str), "transform=\"rotate(%d %lg,%lg)\"", angle, cx, cy);

  fprintf(d.svg_fp, "<!--  START insert for valve3way (%03d) -->\n", n_instance);


  fprintf(d.svg_fp, "<g stroke=\"black\" stroke-width=\"%lg\" %s>\n", 
       0.5 * scale_factor, transform_str);
  if (type == 1)
  {
    fprintf(d.svg_fp, "  <path  fill=\"%s\" \n", on_color);
    fprintf(d.svg_fp, "     d=\"M%lg %lg A%lg %lg 0 1 1 %lg %lg Z\"/>\n",
           x1 + (10.0 * scale_factor), y1 + (10.0 * scale_factor), 
           5.0 * scale_factor, 3.0 * scale_factor, 
           x1 + (40.0 * scale_factor), y1 + (10.0 * scale_factor));
  }
  else if (type == 2)
  {
    fprintf(d.svg_fp, "  <rect fill=\"%s\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\"/>\n",
                 on_color, x1 + (10.0 * scale_factor), y1, 
                 30.0 * scale_factor, 10.0 * scale_factor); 
  }
  fprintf(d.svg_fp, "  <polygon id=\"valve3way_out1_%03d\" fill=\"%s\" points=\"%lg,%lg %lg,%lg %lg,%lg\"/>\n",
                n_instance, on_color,
                x1, cy + (width_factor * scale_factor), //y1 + (10.0 * scale_factor),
                x1, cy - (width_factor * scale_factor), //y2, 
                cx, cy);
  fprintf(d.svg_fp, "  <polygon id=\"valve3way_out2_%03d\" fill=\"%s\" points=\"%lg,%lg %lg,%lg %lg,%lg\"/>\n",
                n_instance, on_color,
                x2, cy + (width_factor * scale_factor), //y1 + (10.0 * scale_factor),
                x2, cy - (width_factor * scale_factor), //y2, 
                cx, cy);
  fprintf(d.svg_fp, "  <polygon id=\"valve3way_out3_%03d\" fill=\"%s\" points=\"%lg,%lg %lg,%lg %lg,%lg\"/>\n",
                n_instance, on_color,
                cx - (width_factor * scale_factor), cy + (width/2.0),
                cx + (width_factor * scale_factor), cy + (width/2.0),
                cx, cy);
  fprintf(d.svg_fp, "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" \n",
               cx, y1 + (10.0 * scale_factor), cx, cy);
  fprintf(d.svg_fp, "     stroke =\"black\" stroke-width=\"%lg\"/>\n",
           1.5 * scale_factor);
  fprintf(d.svg_fp, "</g>\n");

 fprintf(d.js_fp, "var %s = new valve3way_t(\"%s\", \"%s\", \"%s\", \"%s\");\n",
             js_object_name, js_on_name, js_off_name, on_color, off_color);

  if (gen_popup)
  {
    double px, py;
    find_a_place_nearby(&px, &py, x1, y1, width, width);
    fprintf(d.svg_fp, "<rect x=\"%lg\"  y=\"%lg\" width=\"%lg\" height=\"%lg\"\n"
      "   onclick=\"show_popup(%lg,%lg,'Open', 'Close', '%s')\" visibility=\"hidden\"\n"
      "   pointer-events=\"all\" onmouseover=\"this.style.cursor='pointer';\"/>\n",
         x1, y1, width, width, px, py, tag);
  }


  fprintf(d.js_fp, "// --  END insert for valve3way (%03d)\n", n_instance);
  fprintf(d.svg_fp, "<!--  END insert for valve3way (%03d) -->\n", n_instance);
  n_instance++;

  add_js_library("valve3way.js");
  add_animation_object(tag, js_object_name);
}

