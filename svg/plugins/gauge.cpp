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

class gauge_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new gauge_t;
}

const char *gauge_t::get_name(void)
{
  return "gauge";
}

void gauge_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("gauge", "A circular gage with needle, automatic scale, pv text, engineering units\n");
  dob->param("Type (1 = with simulated black plastic cover and screws, 2 = round gauge only"); 
  dob->param("Color of needle, pv text, and engieering units"); 
  dob->param("X of upper left corner"); 
  dob->param("Y of upper left corner"); 
  dob->param("Width"); 
  dob->example("gauge|1|PRESSURE|green|125|10|75|");
  dob->notes("Uses the following attributes of the tag: "
       "'scale_hi', 'scale_lo', 'eu', 'decimal_places', "
       "'lo_alarm', 'lo_caution', 'hi_caution', 'hi_alarm', "
       " 'lo_alarm_enable', 'lo_caution_enable', "
       "'hi_caution_enable', 'hi_alarm_enable'");
  dob->notes("Height = Width");
  dob->end();
}

void gauge_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 7)
  {
    printf("%s, line %d: There must be 7 arguments to gauge\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  int gtype = atol(argv[1]);
  const char *the_tag = argv[2];
  const char *the_color = argv[3];
  double x = atof(argv[4]);
  double y = atof(argv[5]);
  double width = atof(argv[6]);
  //double width = height * 0.2;
  //double font_size = height * 0.1;
  //double cx = x + (width/2.0);
  //double cy = y + (height/2.0);
  //double stroke_width = height / 100.0;

  const char *needle_color;
  if (0 == strcasecmp(the_color, "yellow"))
  {
    needle_color = "url(#yellowLinearV)";
    add_svg_library("yellow_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "black"))
  {
    needle_color = "url(#blackLinearV)";
    add_svg_library("black_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "orange"))
  {
    needle_color = "url(#orangeLinearV)";
    add_svg_library("orange_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "brown"))
  {
    needle_color = "url(#brownLinearV)";
    add_svg_library("brown_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "white"))
  {
    needle_color = "url(#whiteLinearV)";
    add_svg_library("white_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "red"))
  {
    needle_color = "url(#redLinearV)";
    add_svg_library("red_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "blue"))
  {
    needle_color = "url(#blueLinearV)";
    add_svg_library("blue_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "green"))
  {
    needle_color = "url(#greenLinearV)";
    add_svg_library("green_gradients.svg");
  }
  else // Anything else, use the color 
  {
    needle_color = the_color;
  }


  double cx = x + width / 2.0;
  double cy = y + width / 2.0;
  
  fprintf(d.svg_fp, "<!--  START insert for gauge (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  START insert for gauge (%03d)\n", n_instance);


  if (gtype == 1)
  {
    fprintf(d.svg_fp, "<rect x=\"%lg\" y=\"%lg\" rx=\"%lg\" ry=\"%lg\" width=\"%lg\" height=\"%lg\"\n",
               x, y, width * (1.0/15.0), width * (1.0/15.0), width, width);
    fprintf(d.svg_fp, "style=\"fill:rgb(54,60,60);stroke:black;stroke-width:%lg;opacity:1.0\"/>\n", width *(2.0/150));
  }

  fprintf(d.svg_fp, "<circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" fill=\"white\"/>\n", 
             cx, cy, width * (70.0/150.0));
  fprintf(d.svg_fp, "  <circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" fill=\"cornsilk\"\n", 
                          cx, cy, width * (65.0/150.0));
  fprintf(d.svg_fp, "         style=\"stroke:rgb(30,30,30);stroke-width:%lg\"/>\n", width / 150.0);


  if (gtype != 1)
  {
    fprintf(d.svg_fp, 
     "<circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" fill=\"none\""
     " stroke-width=\"%lg\" stroke = \"black\"/>\n", 
     //" stroke-width=\"%lg\" stroke = \"black\" filter=\"url(#filter1)\"/>\n", 
       cx, cy, width * (72.0/150.0), width * 4/150);
  }
  else
  {
    double screw_cx = width * 0.1;
    double screw_cy = width * 0.1; 
    double screw_x1 = width * (13.0/150.0);
    double screw_y1 = width * 0.1;
    double screw_x2 = width * (17.0/150.0);
    double screw_y2 = width * 0.1;

    double screw_r = width * (3.0/150.0);
    double screw_stroke_width = width * (1.2/150.0);

    fprintf(d.svg_fp, "  <circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" stroke=\"none\" stroke-width=\"0\" fill=\"black\"/>\n",
                  x + screw_cx, y + screw_cy, screw_r);
    fprintf(d.svg_fp, "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" transform=\"rotate(20 %lg %lg)\"\n", 
                      x + screw_x1, y + screw_y1, 
                      x + screw_x2, y + screw_y2, x + 
                      screw_cx, y + screw_cy);
    fprintf(d.svg_fp, "      style=\"stroke:rgb(64,65,65);stroke-width:%lg\"/>\n", screw_stroke_width);
    fprintf(d.svg_fp, "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" transform=\"rotate(110 %lg %lg)\"\n",
                      x + screw_x1, y + screw_y1, x + screw_x2, y + screw_y2, x + screw_cx, y + screw_cy);
    fprintf(d.svg_fp, "      style=\"stroke:rgb(64,65,65);stroke-width:%lg\"/>\n", screw_stroke_width);

    screw_cx = width - screw_cx;
    screw_x1 = width - screw_x1;
    screw_x2 = width - screw_x2;

    fprintf(d.svg_fp, "  <circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" stroke=\"none\" stroke-width=\"0\" fill=\"black\"/>\n",
                  x + screw_cx, y + screw_cy, screw_r);
    fprintf(d.svg_fp, "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" transform=\"rotate(-10 %lg %lg)\"\n", 
                      x + screw_x1, y + screw_y1, x + screw_x2, y + screw_y2, x + screw_cx, y + screw_cy);
    fprintf(d.svg_fp, "      style=\"stroke:rgb(64,65,65);stroke-width:%lg\"/>\n", screw_stroke_width);
    fprintf(d.svg_fp, "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" transform=\"rotate(80 %lg %lg)\"\n",
                      x + screw_x1, y + screw_y1, x + screw_x2, y + screw_y2, x + screw_cx, y + screw_cy);
    fprintf(d.svg_fp, "      style=\"stroke:rgb(64,65,65);stroke-width:%lg\"/>\n", screw_stroke_width);

    screw_cy = width - screw_cy;
    screw_y1 = width - screw_y1;
    screw_y2 = width - screw_y2;

    fprintf(d.svg_fp, "  <circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" stroke=\"none\" stroke-width=\"0\" fill=\"black\"/>\n",
                  x + screw_cx, y + screw_cy, screw_r);
    fprintf(d.svg_fp, "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" transform=\"rotate(17 %lg %lg)\"\n", 
                      x + screw_x1, y + screw_y1, x + screw_x2, y + screw_y2, x + screw_cx, y + screw_cy);
    fprintf(d.svg_fp, "      style=\"stroke:rgb(64,65,65);stroke-width:%lg\"/>\n", screw_stroke_width);
    fprintf(d.svg_fp, "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" transform=\"rotate(107 %lg %lg)\"\n",
                      x + screw_x1, y + screw_y1, x + screw_x2, y + screw_y2, x + screw_cx, y + screw_cy);
    fprintf(d.svg_fp, "      style=\"stroke:rgb(64,65,65);stroke-width:%lg\"/>\n", screw_stroke_width);

    screw_cx = width - screw_cx;
    screw_x1 = width - screw_x1;
    screw_x2 = width - screw_x2;

    fprintf(d.svg_fp, "  <circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" stroke=\"none\" stroke-width=\"0\" fill=\"black\"/>\n",
                  x + screw_cx, y + screw_cy, screw_r);
    fprintf(d.svg_fp, "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" transform=\"rotate(3 %lg %lg)\"\n", 
                      x + screw_x1, y + screw_y1, x + screw_x2, y + screw_y2, x + screw_cx, y + screw_cy);
    fprintf(d.svg_fp, "      style=\"stroke:rgb(64,65,65);stroke-width:%lg\"/>\n", screw_stroke_width);
    fprintf(d.svg_fp, "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" transform=\"rotate(93 %lg %lg)\"\n",
                      x + screw_x1, y + screw_y1, x + screw_x2, y + screw_y2, x + screw_cx, y + screw_cy);
    fprintf(d.svg_fp, "      style=\"stroke:rgb(64,65,65);stroke-width:%lg\"/>\n", screw_stroke_width);
  }

  char needle_d[200];
  double needle_r = width * (6.0/150.0);
  snprintf(needle_d, sizeof(needle_d), "d=\"M%lg %lg A%lg %lg 0 1 1 %lg %lg L%lg %lg Z\"", 
                          cx - needle_r, cy, needle_r, needle_r, cx + needle_r, cy, 
                                       cx, y + (width * (140.0/150.0))),
  fprintf(d.svg_fp, "<!-- d: '%s' -->\n", needle_d); 

  /**
  fprintf(d.svg_fp, "  <path fill=\"blue\" id=\"gauge_sneedle1_%03d\" \n", n_instance);
  fprintf(d.svg_fp, "      %s style=\"fill-opacity:0.3\"\n", needle_d);
  fprintf(d.svg_fp, "         onmouseover=\"this.title='Testing, what will happen?';\"/>\n");

  fprintf(d.svg_fp, "  <path fill=\"blue\" id=\"gauge_sneedle2_%03d\" \n", n_instance);
  fprintf(d.svg_fp, "      %s style=\"fill-opacity:0.3\"\n", needle_d);
  fprintf(d.svg_fp, "         onmouseover=\"this.style.cursor='crosshair';\"/>\n");
  ***/


  fprintf(d.svg_fp, "  <path fill=\"%s\" id=\"gauge_needle_%03d\" \n", needle_color, n_instance);
  fprintf(d.svg_fp, "      %s />\n", needle_d);


  fprintf(d.svg_fp, "  <circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" stroke=\"none\" stroke-width=\"0\" fill=\"black\"/>\n",
            cx, cy, width * (4.0/150.0));

  fprintf(d.svg_fp, "  <text id=\"gauge_pv_%03d\" x=\"%lg\" y=\"%lg\" font-size=\"%lg\" style=\"fill:%s;\" \n",
                            n_instance, cx, cy + (width * (25.0/150.0)), width * (20.0/150.0), the_color);
  fprintf(d.svg_fp, "       alignment-baseline=\"middle\" text-anchor=\"middle\">300</text>\n");
  fprintf(d.svg_fp, "\n");

/**
  fprintf(d.svg_fp, "  <text id=\"gauge_range_%03d\" x=\"%lg\" y=\"%lg\" font-size=\"%lg\" style=\"fill:blue;\" \n",
                            n_instance, cx, cy + (width * (45.0/150.0)), width * (15.0/150.0));
  fprintf(d.svg_fp, "       alignment-baseline=\"middle\" text-anchor=\"middle\">300</text>\n");
***/

  fprintf(d.svg_fp, "  <text id=\"gauge_eu_%03d\" x=\"%lg\" y=\"%lg\" font-size=\"%lg\" \n",
                            n_instance, cx, cy + (width * (45.0/150.0)), width * (15.0/150.0));
  fprintf(d.svg_fp, "       alignment-baseline=\"middle\" text-anchor=\"middle\">cm</text>\n");

  if ((strlen(the_tag) > 0) && (0 != strcasecmp(the_tag, "null")))
  {
    char js_object_name[30];
    snprintf(js_object_name, sizeof(js_object_name), "gauge_obj_%03d", n_instance);

    fprintf(d.js_fp, "var %s = new gauge_t(\"gauge_needle_%03d\", \"gauge_pv_%03d\","
              " \"gauge_eu_%03d\", %lg, %lg, %lg);\n", 
              js_object_name, n_instance, n_instance, n_instance, x, y, width); 
    //add_svg_library("filter1.svg");
    add_js_library("scales.js");
    add_js_library("gauge.js");
    add_animation_object(the_tag, js_object_name);
  }
  fprintf(d.svg_fp, "<!--  END insert for gauge (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  END insert for gauge (%03d)\n", n_instance);

  n_instance++;
}

