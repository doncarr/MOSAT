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

class slider_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new slider_t;
}

const char *slider_t::get_name(void)
{
  return "slider";
}

void slider_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("slider", "Analog slider to send analog values to the control system, also reads back value");
  dob->param("Analog Tag");
  dob->param("Bar color");
  dob->param("X of upper left corner");
  dob->param("Y of upper left corner");
  dob->param("Width");
  dob->example("slider|PVALVE|blue|110|135.5|180|");
  dob->notes("You can grab the button in the middle and drag left or right to set the value");
  dob->end();
}

void slider_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 6)
  {
    printf("%s, line %d: There must be 6 arguments to slider\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  const char *the_tag = argv[1];
  const char *the_color = argv[2];
  double x1 = atof(argv[3]);
  double y1 = atof(argv[4]);
  double width = atof(argv[5]);
  double height = width * 0.03;
  const char *slider_color = "gray";
  double stroke_width = width * 0.003;
  double circle_radius = height * 0.9;

  char js_object_name[50];
  char js_circle_name[50];
  char js_rect_name[50];
  char js_text_name[50];
  snprintf(js_object_name, sizeof(js_object_name), "slider_object_%03d", n_instance);
  snprintf(js_circle_name, sizeof(js_circle_name), "slider_circle_%03d", n_instance);
  snprintf(js_rect_name, sizeof(js_rect_name), "slider_rect_%03d", n_instance);
  snprintf(js_text_name, sizeof(js_text_name), "slider_text_%03d", n_instance);

  char slider_timeout_name[50];
  char slider_mousemove_name[50];
  char slider_mousedown_name[50];
  char slider_mouseup_name[50];
  char slider_barclick_name[50];
  snprintf(slider_timeout_name, sizeof(slider_timeout_name), "slider_timeout_%03d", n_instance);
  snprintf(slider_mousemove_name, sizeof(slider_mousemove_name), "slider_mousemove_%03d", n_instance);
  snprintf(slider_mousedown_name, sizeof(slider_mousedown_name), "slider_mousedown_%03d", n_instance);
  snprintf(slider_mouseup_name, sizeof(slider_mouseup_name), "slider_mouseup_%03d", n_instance);
  snprintf(slider_barclick_name, sizeof(slider_barclick_name), "slider_barclick_%03d", n_instance);

  add_svg_library("white_gradients.svg");
  if (0 == strcasecmp(the_color, "yellow"))
  {
    slider_color = "yellow";
    add_svg_library("yellow_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "black"))
  {
    slider_color = "black";
    add_svg_library("black_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "orange"))
  {
    slider_color = "orange";
    add_svg_library("orange_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "brown"))
  {
    slider_color = "brown";
    add_svg_library("brown_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "white"))
  {
    slider_color = "white";
    add_svg_library("white_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "red"))
  {
    slider_color = "red";
    add_svg_library("red_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "blue"))
  {
    slider_color = "blue";
    add_svg_library("blue_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "green"))
  {
    slider_color = "green";
    add_svg_library("green_gradients.svg");
  }
  else // Anything else, use gray
  {
    slider_color = "gray";
    add_svg_library("gray_gradients.svg");
  }
  if (!d.silent) printf("Slider, tag: %s, color: %s\n", the_tag, slider_color);



  fprintf(d.svg_fp, "<!--  START insert for slider (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  START insert for slider (%03d)\n", n_instance);
  /***********************/

  fprintf(d.svg_fp, "<rect  x=\"%lg\" y=\"%lg\" rx=\"%lg\" ry=\"%lg\" width=\"%lg\" height=\"%lg\"\n", 
    x1, y1, height * 0.4, height * 0.4, width, height);
  fprintf(d.svg_fp, "  fill=\"url(#whiteLinearH)\" stroke=\"black\" stroke-width=\"%lg\"\n", stroke_width);
  fprintf(d.svg_fp, "  pointer-events=\"all\" onmouseover=\"this.style.cursor='pointer';\"\n");
  fprintf(d.svg_fp, "  onmousedown=\"%s(evt)\"/>\n", slider_mousedown_name); 

  fprintf(d.svg_fp, "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\"\n", 
         x1, y1 + (height), x1, y1 + (height * 1.55));
  fprintf(d.svg_fp, "     stroke=\"black\" stroke-width=\"%lg\"/>\n", 
              stroke_width);

  fprintf(d.svg_fp, "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\"\n", 
         x1 + (width * 0.5), y1 + (height*1.2), x1 + (width * 0.5), y1 + (height * 1.55));
  fprintf(d.svg_fp, "     stroke=\"black\" stroke-width=\"%lg\"/>\n", 
              stroke_width);

  fprintf(d.svg_fp, "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\"\n", 
         x1 + width, y1 + (height), x1 + width, y1 + (height * 1.55));
  fprintf(d.svg_fp, "     stroke=\"black\" stroke-width=\"%lg\"/>\n", 
              stroke_width);

  fprintf(d.svg_fp, "<rect  id=\"%s\" x=\"%lg\" y=\"%lg\" rx=\"%lg\" ry=\"%lg\" width=\"%lg\" height=\"%lg\"\n", 
    js_rect_name, x1, y1, height * 0.5, height * 0.4, width * 0.5, height);
  fprintf(d.svg_fp, "  fill=\"url(#%sLinearH)\" stroke=\"black\" stroke-width=\"%lg\"\n", slider_color, stroke_width);
  fprintf(d.svg_fp, "  pointer-events=\"all\" onmouseover=\"this.style.cursor='pointer';\"\n");
  fprintf(d.svg_fp, "  onmousedown=\"%s(evt)\"/>\n", slider_mousedown_name); 

  fprintf(d.svg_fp, "<circle id=\"%s\" cx=\"%lg\" cy=\"%lg\" r=\"%lg\" stroke=\"black\" fill=\"url(#whiteRadial)\" stroke-width=\"%lg\"\n",
     js_circle_name, x1 + (width * 0.5), y1 + (height * 0.5), 
         circle_radius, stroke_width);
  fprintf(d.svg_fp, "  pointer-events=\"all\" onmouseover=\"this.style.cursor='pointer';\"\n");
  //fprintf(d.svg_fp, "  onmousedown=\"%s(evt)\" onmouseup=\"%s(evt)\" onmousemove=\"%s(evt)\"/>\n", 
  //       slider_mousedown_name, slider_mouseup_name, slider_mousemove_name);
  fprintf(d.svg_fp, "  onmousedown=\"%s(evt)\"/>\n", 
         slider_mousedown_name);

  fprintf(d.svg_fp, "  <text id=\"%s\" x=\"%lg\" y=\"%lg\" font-size=\"%lg\" fill=\"black\"\n",
                            js_text_name, x1 + (width / 2.0), y1 - height, height * 1.2);
  fprintf(d.svg_fp, "       alignment-baseline=\"middle\" text-anchor=\"middle\">50.0</text>\n");



 //ball.addEventListener("mousedown", mouseDown, false);
  //          ball.addEventListener("mouseup", mouseUp, false);
   //         ball.addEventListener("mousemove", mouseMove, false);

  /**********************
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
  *****/

  fprintf(d.js_fp, "function %s()\n", slider_timeout_name);
  fprintf(d.js_fp, "{\n");
  //fprintf(d.js_fp, "  console.log(\"slider timeout\");\n");
  fprintf(d.js_fp, "  %s.timeout();\n", js_object_name);
  fprintf(d.js_fp, "}\n");

  fprintf(d.js_fp, "function %s(evt)\n", slider_mousedown_name);
  fprintf(d.js_fp, "{\n");
  //fprintf(d.js_fp, "  console.log(\"mouse down\");\n");
  //fprintf(d.js_fp, "  console.log(\"X: \" + evt.clientX + \" Y: \" + evt.clientY);\n");
  fprintf(d.js_fp, "  %s.mousedown(evt);\n", js_object_name);
  fprintf(d.js_fp, "}\n");

  fprintf(d.js_fp, "function %s(evt)\n", slider_mouseup_name);
  fprintf(d.js_fp, "{\n");
  fprintf(d.js_fp, "  %s.mouseup(evt);\n", js_object_name);
  fprintf(d.js_fp, "}\n");

  /***
  fprintf(d.js_fp, "function %s(evt)\n", slider_barclick_name);
  fprintf(d.js_fp, "{\n");
  fprintf(d.js_fp, "  %s.barclick(evt);\n", js_object_name);
  fprintf(d.js_fp, "}\n");
  ***/

  fprintf(d.js_fp, "function %s(evt)\n", slider_mousemove_name);
  fprintf(d.js_fp, "{\n");
  fprintf(d.js_fp, "  %s.mousemove(evt);\n", js_object_name);
  fprintf(d.js_fp, "}\n");

  fprintf(d.js_fp, "document.addEventListener(\"mouseup\", %s, false);\n", slider_mouseup_name);
  fprintf(d.js_fp, "document.addEventListener(\"mousemove\", %s, false);\n", slider_mousemove_name);

  if ((strlen(the_tag) > 0) && (0 != strcasecmp(the_tag, "null")))
  {
    fprintf(d.js_fp, "var %s = new slider_t(\"%s\", \"%s\", \"%s\", \"%s\", %lg, %lg, %lg, %lg, \"%s()\");\n", 
       js_object_name, the_tag, js_circle_name, js_rect_name, 
       js_text_name, x1, y1, width, height, slider_timeout_name); 

    add_js_library("transform.js");
    add_js_library("slider.js");

    add_animation_object(the_tag, js_object_name);
  }

  fprintf(d.svg_fp, "<!--  END insert for slider (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  END insert for slider (%03d)\n", n_instance);

  n_instance++;
}

