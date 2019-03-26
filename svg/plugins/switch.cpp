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

class switch_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new switch_t;
}

const char *switch_t::get_name(void)
{
  return "switch";
}

void switch_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("switch", "A fancy two position rotary switch that sends outputs to the control system");
  dob->param("Discrete TAG");
  dob->param("On Color");
  dob->param("Off Color");
  dob->param("X of upper left corner");
  dob->param("Y of upper left corner");
  dob->param("Width");
  dob->example("switch|SPRAY_PUMP_ON|lime|red|5|115|30|");
  dob->notes("Uses tag attributes: 'lo_desc', 'hi_desc'");
  dob->end();
}


void switch_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 7)
  {
    printf("%s, line %d: There must be 7 arguments to switch\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  const char *tag = argv[1];
  const char *on_color = argv[2];
  const char *off_color = argv[3];
  double x1 = atof(argv[4]);
  double y1 = atof(argv[5]);
  double width = atof(argv[6]);
  //double font_size = width * 0.6;
  double scale_factor = width / 100;
  double cx = x1 + (width / 2.0);
  double cy = y1 + (65.0 * scale_factor);

  char js_object_name[30];
  char js_group_name[30];
  char js_on_name[30];
  char js_off_name[30];
  char js_click1_name[30];
  char js_click2_name[30];
  char js_handler_name[30];
  char js_on_text_name[30];
  char js_off_text_name[30];
  snprintf(js_object_name, sizeof(js_object_name), "switch_obj_%03d", n_instance);
  snprintf(js_click1_name, sizeof(js_click1_name), "switch_click1_%03d", n_instance);
  snprintf(js_click2_name, sizeof(js_click2_name), "switch_click2_%03d", n_instance);
  snprintf(js_handler_name, sizeof(js_handler_name), "switch_handler_%03d", n_instance);
  snprintf(js_group_name, sizeof(js_group_name), "switch_group_%03d", n_instance);
  snprintf(js_on_name, sizeof(js_on_name), "switch_on_%03d", n_instance);
  snprintf(js_off_name, sizeof(js_off_name), "switch_off_%03d", n_instance);

  snprintf(js_on_text_name, sizeof(js_on_text_name), "switch_on_text_%03d", n_instance);
  snprintf(js_off_text_name, sizeof(js_off_text_name), "switch_off_text_%03d", n_instance);

  fprintf(d.svg_fp, "<!--  START insert for switch (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  START insert for switch (%03d)\n", n_instance);
  /***********************/

//  <rect id="rect_001" x="0" y="0" rx="3" ry="3" width="100" height="105" fill="lightgray" stroke="darkgray" stroke-width="1"/>

  fprintf(d.svg_fp, "<rect  id=\"%s\" x=\"%lg\" y=\"%lg\" rx=\"%lg\" ry=\"%lg\" width=\"%lg\" height=\"%lg\" fill=\"red\" stroke=\"black\" stroke-width=\"%lg\" filter=\"url(#filter1)\"/>\n", 
    js_off_name, x1 + (2.0 * scale_factor), y1 + (2.0 * scale_factor), 
    4.0 * scale_factor, 4.0 * scale_factor, 48.0 * scale_factor, 20.0 * scale_factor, scale_factor);
  fprintf(d.svg_fp, "<text id=\"%s\" x=\"%lg\" y=\"%lg\" font-size=\"%lg\" fill=\"black\" text-anchor=\"middle\">Off</text>\n", js_off_text_name,
     x1 + 25.0 * scale_factor, y1 + 18.6 * scale_factor, 18.0 * scale_factor);

  fprintf(d.svg_fp, "<rect  id=\"%s\" x=\"%lg\" y=\"%lg\"  rx=\"%lg\" ry=\"%lg\" width=\"%lg\" height=\"%lg\" fill=\"gray\" stroke=\"black\" stroke-width=\"%lg\" filter=\"url(#filter1)\"/>\n", 
    js_on_name, x1 + (52.0 * scale_factor), y1 + (2.0 * scale_factor), 
    4.0 * scale_factor, 4.0 * scale_factor, 48.0 * scale_factor, 20.0 * scale_factor, scale_factor);
  fprintf(d.svg_fp, "<text id=\"%s\" x=\"%lg\" y=\"%lg\" font-size=\"%lg\" fill=\"black\" text-anchor=\"middle\">On</text>\n", js_on_text_name,
     x1 + 75.0 * scale_factor, y1 + 18.6 * scale_factor, 18.0 * scale_factor);


  fprintf(d.svg_fp, "<circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" "
              "stroke=\"darkgray\" stroke-width=\"1\" fill=\"gray\" filter=\"url(#filter1)\"/>\n",
              cx, cy, 40.0 * scale_factor);
  fprintf(d.svg_fp, "<circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" "
                    "stroke=\"none\" fill=\"white\"/>\n",
              cx, cy, 38.0 * scale_factor);
  fprintf(d.svg_fp, "<circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" "
                "stroke=\"none\" fill=\"url(#grayRadialSwitch)\"/>\n",
              cx, cy, 37.0 * scale_factor);

  fprintf(d.svg_fp, "<g id=\"%s\" transform=\"rotate(-45 %lg,%lg)\">\n", 
                   js_group_name, cx, cy);
  fprintf(d.svg_fp, "  <circle id=\"c1\" cx=\"%lg\" cy=\"%lg\" r=\"%lg\" "
             "stroke=\"none\" fill=\"url(#grayLinearSwitch1)\"/>\n",
              cx, cy, 30.0 * scale_factor);
  fprintf(d.svg_fp, "  <rect id=\"r1\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" "
               "rx=\"%lg\" ry=\"%lg\" fill=\"url(#grayLinearSwitch2)\" stroke=\"none\"/>\n", 
        x1 + (45.0 * scale_factor), y1 + (21.0 * scale_factor), 10.0 * scale_factor, 
           87.0 * scale_factor, 4.0 * scale_factor, 4.0 * scale_factor);
  fprintf(d.svg_fp, "  <circle id=\"p1\" cx=\"%lg\" cy=\"%lg\" r=\"%lg\" "
                    "stroke=\"none\" fill=\"white\"/>\n",
                 cx, y1 + (27.0 * scale_factor), 3.0 * scale_factor);
  fprintf(d.svg_fp, "</g>\n");
  fprintf(d.svg_fp, "<rect id=\"rect_001\" x=\"%lg\"  y=\"%lg\" width=\"%lg\" height=\"%lg\" "
                    "onclick=\"%s()\" visibility=\"hidden\" pointer-events=\"all\" "
                    "onmouseover=\"this.style.cursor='pointer';\"/>\n", 
          x1, y1, 42.0 * scale_factor, 40.0 * scale_factor, js_click1_name);
  fprintf(d.svg_fp, "<rect id=\"rect_001\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" "
                    "onclick=\"%s()\" visibility=\"hidden\" pointer-events=\"all\" "
                    "onmouseover=\"this.style.cursor='pointer';\"/>\n", 
          x1 + (58.0 * scale_factor), y1, (42.0 * scale_factor), 
              40.0 * scale_factor, js_click2_name);

  /***********************/
  
  fprintf(d.js_fp, "function %s()\n", js_click1_name); 
  fprintf(d.js_fp, "{\n");
  fprintf(d.js_fp, "  %s.off_clicked();\n", js_object_name);
  fprintf(d.js_fp, "}\n");
  fprintf(d.js_fp, "function %s()\n", js_click2_name); 
  fprintf(d.js_fp, "{\n");
  fprintf(d.js_fp, "  %s.on_clicked();\n", js_object_name);
  fprintf(d.js_fp, "}\n");
  fprintf(d.js_fp, "function %s()\n", js_handler_name); 
  fprintf(d.js_fp, "{\n");
  fprintf(d.js_fp, "  %s.interval_handler();\n", js_object_name);
  fprintf(d.js_fp, "}\n");

  fprintf(d.js_fp, "var %s = new switch_t(\"%s\", \"%s\", \"%s\", \"%s\", "
                   "\"%s\", \"%s\", %lg, %lg, \"%s()\", \"%s\", \"%s\");\n", 
      js_object_name, tag, js_group_name, js_on_name, js_off_name, 
      on_color, off_color, cx, cy, js_handler_name, js_on_text_name, js_off_text_name); 

  fprintf(d.svg_fp, "<!--  END insert for switch (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  END insert for switch (%03d)\n", n_instance);

  add_js_library("switch.js");
  add_svg_library("switch_grad.svg");
  add_animation_object(tag, js_object_name);
  add_svg_library("filter1.svg");



  n_instance++;
}

