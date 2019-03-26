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

class bar_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new bar_t;
}

const char *bar_t::get_name(void)
{
  return "bar";
}

void bar_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("bar", "A fancy vertical bar to indicate analog value with scale, alarm limits, etc");
  dob->param("Analog Tagname");
  dob->param("Bar Color (blue, red, black, grey, yellow, orange, green)");
  dob->param("X of top left corner of bar");
  dob->param("Y of top left corner of bar");
  dob->param("Height of bar");
  dob->notes("Uses the following attributes of the tag: "
       "'scale_hi', 'scale_lo', 'eu', 'decimal_places', "
       "'lo_alarm', 'lo_caution', 'hi_caution', 'hi_alarm', "
       " 'lo_alarm_enable', 'lo_caution_enable', "
       "'hi_caution_enable', 'hi_alarm_enable'");
  dob->notes("Automatically calculates width");
  dob->example("bar|PRESSURE|orange|110|45|70|");
  dob->end();
}


void bar_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 6)
  {
    printf("%s, line %d: There must be 6 arguments to bar\n",
           d.file_name, d.line_number);
    exit(-1);
  }
  const char *bar_color = "blue";
  const char *the_tag = argv[1];
  const char *the_color = argv[2];
  double x = atof(argv[3]);
  double y = atof(argv[4]);
  double height = atof(argv[5]);
  double width = height * 0.12;
  double font_size = height * 0.1;
  double cx = x + (width/2.0);
  double cy = y + (height/2.0);
  double stroke_width = height / 100.0;

  if (0 == strcasecmp(the_color, "yellow")) 
  {
    bar_color = "yellow";
    add_svg_library("yellow_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "black")) 
  {
    bar_color = "black";
    add_svg_library("black_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "orange")) 
  {
    bar_color = "orange";
    add_svg_library("orange_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "brown")) 
  {
    bar_color = "brown";
    add_svg_library("brown_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "white")) 
  {
    bar_color = "gray"; // Can NOT be white, no contrast with above bar
    add_svg_library("gray_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "red")) 
  {
    bar_color = "red";
    add_svg_library("red_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "blue")) 
  {
    bar_color = "blue";
    add_svg_library("blue_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "green")) 
  {
    bar_color = "green";
    add_svg_library("green_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "gray")) 
  {
    bar_color = "gray";
    add_svg_library("gray_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "grey")) 
  {
    bar_color = "gray";
    add_svg_library("gray_gradients.svg");
  }
  else // Anything else, use blue 
  {
    bar_color = "blue";
    add_svg_library("blue_gradients.svg");
  }
  add_svg_library("white_gradients.svg");

  
  fprintf(d.svg_fp, "<!--  START insert for bar (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  START insert for bar (%03d)\n", n_instance);

//----------
  fprintf(d.svg_fp, "  <rect fill=\"url(#whiteLinearV)\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" stroke=\"none\" stroke-width=\"0\"/>\n",
                         x, y, width, height);
  fprintf(d.svg_fp, "  <rect fill=\"url(#%sLinearV)\" id=\"bar_%03d\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" stroke=\"none\" stroke-width=\"0\" transform=\"rotate(180 %lg %lg)\"/>\n",
                         bar_color,
                         n_instance, x, y, width, height, cx, cy);
  fprintf(d.svg_fp, "  <rect x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" fill=\"none\" stroke=\"#000000\" stroke-width=\"%lg\" />\n",
                         x, y, width, height, stroke_width);

//----------
//  fprintf(d.svg_fp, "<rect  fill=\"cornsilk\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" stroke=\"black\" stroke-width=\"%lg\"/>\n",
 //                    x, y, width, height, stroke_width);
  //fprintf(d.svg_fp, "<rect  id=\"bar_%03d\" fill=\"%s\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" stroke=\"none\" stroke-width=\"0\" transform=\"rotate(180 %lg %lg)\"/>\n",
//                     n_instance, color, (x + stroke_width / 2.0), 
 //                    y, width-stroke_width, height, cx, cy);
  fprintf(d.svg_fp, "<text id=\"bar_pv_%03d\" x=\"%lg\" y=\"%lg\" font-family=\"Verdana\" font-size=\"%lg\" fill=\"black\" text-anchor=\"middle\">0</text>\n",
                    n_instance, cx, y + height + (font_size * 1.1), font_size); 
//---------------

  if ((strlen(the_tag) > 0) && (0 != strcasecmp(the_tag, "null")))
  {
    char js_object_name[30];
    snprintf(js_object_name, sizeof(js_object_name), "bar_obj_%03d", n_instance);
    fprintf(d.js_fp, "var %s = new bar_t(\"bar_%03d\", \"bar_pv_%03d\", %lg, %lg, %lg, %lg, %lg);\n", 
               js_object_name, n_instance, n_instance, x, y, x+width, y+height, stroke_width); 
    add_js_library("scales.js");
    add_js_library("bar.js");
    //add_svg_library("bar_lib.svg");
    add_animation_object(the_tag, js_object_name);
  }


  fprintf(d.svg_fp, "<!--  END insert for bar (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  END insert for bar (%03d)\n", n_instance);

  n_instance++;
}

/******
void gen_analog_bar(FILE *fp, const char *id, double x, double y, double width, double height)
{
  fprintf(fp, "  <rect fill=\"url(#grBarGrey)\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" stroke=\"none\" stroke-width=\"0\"/>\n",
                         x, y, width, height);
  fprintf(fp, "  <rect fill=\"url(#grBarBlue)\" id=\"%s\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" stroke=\"none\" stroke-width=\"0\" transform=\"rotate(180 %lg %lg)\"/>\n",
                         id, x, y, width, height, x + (width * 0.5), y + (height * 0.5));
  fprintf(fp, "  <rect x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" fill=\"none\" stroke=\"#000000\" stroke-width=\"1\" />\n",
                         x, y, width, height);
}
*********/
