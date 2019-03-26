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

class rgauge_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new rgauge_t;
}

const char *rgauge_t::get_name(void)
{
  return "rgauge";
}

void rgauge_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("rgauge", "A fancy circular gauge with large text, circle of growing rectanges that light up to indicate value\n");
  dob->param("Type (Currently not used, and to easily switch from type 'gauge')");
  dob->param("Color of large pv text, and engieering units label");
  dob->param("X of upper left corner");
  dob->param("Y of upper left corner");
  dob->param("Width");
  dob->example("rgauge|1|PRESSURE|blue|80|30|55|");
  dob->notes("Uses the following attributes of the tag: "
       "'scale_hi', 'scale_lo', 'eu', 'decimal_places', "
       "'lo_alarm', 'lo_caution', 'hi_caution', 'hi_alarm', "
       " 'lo_alarm_enable', 'lo_caution_enable', "
       "'hi_caution_enable', 'hi_alarm_enable'");
  dob->notes("Height = Width");
  dob->end();
}

void rgauge_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 7)
  {
    printf("%s, line %d: There must be 7 arguments to rgauge\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  //int gtype = atol(argv[1]);
  const char *tag = argv[2];
  const char *rcolor = argv[3];
  double x = atof(argv[4]);
  double y = atof(argv[5]);
  double width = atof(argv[6]);

  double cx = x + width / 2.0;
  double cy = y + width / 2.0;
  double r = width / 2.0;
  double scale_factor = r / 63.5;
  
  fprintf(d.svg_fp, "<!--  START insert for rgauge (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  START insert for rgauge (%03d)\n", n_instance);

  char js_object_name[30];
  snprintf(js_object_name, sizeof(js_object_name), "rgauge_obj_%03d", n_instance);
  fprintf(d.js_fp, "var %s = new rgauge_t(\"rgauge_pv_%03d\", "
         "\"rgauge_eu_%03d\", \"rgauge_hi_%03d\", \"rgauge_lo_%03d\", %lg, %lg, %lg);\n", 
         js_object_name, n_instance, n_instance, n_instance, 
           n_instance, x + (3.5 * scale_factor), y + (3.5 * scale_factor), 
           width * (60.0/63.5)); 

  fprintf(d.svg_fp, "<circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" fill=\"antiquewhite\" stroke=\"black\" stroke-width=\"%lg\" filter=\"url(#filter1)\"/>\n", 
         cx, cy, r, 2.0 * scale_factor);

  fprintf(d.svg_fp, "<text id=\"rgauge_pv_%03d\" text-anchor=\"middle\" x=\"%lg\" y=\"%lg\" font-size=\"%lg\" fill=\"%s\" filter=\"url(#filter1)\">00</text>\n",
         n_instance, cx - (5 * scale_factor), cy + (15 * scale_factor), 50.0 * scale_factor, rcolor); 

  fprintf(d.svg_fp, "<line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" stroke =\"darkgray\" stroke-width=\"%lg\"/>\n", 
         cx - (46 * scale_factor), cy - (23 * scale_factor),
         cx + (37 * scale_factor), cy - (23 * scale_factor),
         0.6 * scale_factor);

  fprintf(d.svg_fp, "<line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" stroke =\"darkgray\" stroke-width=\"%lg\"/>\n",
         cx - (46.0 * scale_factor), cy + (21.0 * scale_factor),
         cx + (37.0 * scale_factor), cy + (21.0 * scale_factor),
         0.6 * scale_factor);


  fprintf(d.svg_fp, "<text id=\"rgauge_eu_%03d\" text-anchor=\"middle\" x=\"%lg\" y=\"%lg\" font-size=\"%lg\" fill=\"%s\" filter=\"url(#filter1)\">amps</text>\n",
                n_instance, cx, cy + (33.0 * scale_factor), 15 * scale_factor, rcolor); 

  fprintf(d.svg_fp, "<text id=\"rgauge_hi_%03d\" text-anchor=\"start\" x=\"%lg\" y=\"%lg\" font-size=\"%lg\" fill=\"%s\" filter=\"url(#filter1)\">0</text>\n", 
       n_instance, cx - (23.0 * scale_factor), cy + (50.0 * scale_factor), 8 * scale_factor, rcolor); 

  fprintf(d.svg_fp, "<text id=\"rgauge_lo_%03d\" text-anchor=\"end\" x=\"%lg\" y=\"%lg\" font-size=\"%lg\" fill=\"%s\" filter=\"url(#filter1)\">30</text>\n", 
        n_instance, cx + (24.0 * scale_factor), cy + (50.0 * scale_factor), 8 * scale_factor, rcolor); 

  fprintf(d.svg_fp, "<line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" stroke =\"darkgray\" stroke-width=\"%lg\"/>\n", 
        cx - (23.0 * scale_factor), cy + (52.0 * scale_factor), 
        cx + (25.0 * scale_factor), cy + (52.0 * scale_factor), 
        0.6 * scale_factor); 

  fprintf(d.svg_fp, "<!--  END insert for rgauge (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  END insert for rgauge (%03d)\n", n_instance);
  add_svg_library("filter1.svg");
  //add_js_library("scales.js");
  add_js_library("rgauge.js");
  add_animation_object(tag, js_object_name);

  n_instance++;
}

