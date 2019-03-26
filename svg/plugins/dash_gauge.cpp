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
#include <math.h>

#include "gen_display.h"

static int n_instance = 1;

class dash_gauge_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new dash_gauge_t;
}

const char *dash_gauge_t::get_name(void)
{
  return "dash_gauge";
}

void dash_gauge_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("dash_gauge", "A circular gage with needle, automatic scale, pv text, engineering units\n");
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

void dash_gauge_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 7)
  {
    printf("%s, line %d: There must be 7 arguments to dash_gauge\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  //int gtype = atol(argv[1]);
  const char *the_tag = argv[2];
  printf("the tag = %s\n", the_tag);
  //const char *the_color = argv[3];
  double x = atof(argv[4]);
  double y = atof(argv[5]);
  double width = atof(argv[6]);
  double cx = x + width / 2.0;
  double cy = y + width / 2.0;

  fprintf(d.svg_fp, "<!--  START insert for dash_gauge (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  START insert for dash_gauge (%03d)\n", n_instance);

  char the_d[300];
  double the_r = width * (140.0/150.0);
  double dx = 0;
  double dy = the_r * 0.4;
  double first_r = the_r;
  const char * very_dark_gray = "rgb(20,20,20)";
  
  dx = sqrt((the_r * the_r) - (dy * dy));
  printf("dx = %lg\ndy = %lg\n", dx, dy);
  snprintf(the_d, sizeof(the_d), 
             "d=\"M%lg %lg A%lg %lg 0 1 1 %lg %lg Z\"", 
             cx - dx, cy + dy, the_r, the_r * 1.08, 
             cx + dx, cy + dy);
  fprintf(d.svg_fp, "  <path fill=\"%s\"\n", 
              "black");
  fprintf(d.svg_fp, "      %s />\n", the_d);

  the_r *= 0.99;
  dx = sqrt((the_r * the_r) - (dy * dy));
  printf("dx = %lg\ndy = %lg\n", dx, dy);
  snprintf(the_d, sizeof(the_d), 
             "d=\"M%lg %lg A%lg %lg 0 1 1 %lg %lg Z\"", 
             cx - dx, cy + dy, the_r, the_r * 1.07, 
             cx + dx, cy + dy);
  fprintf(d.svg_fp, "  <path fill=\"%s\"\n", 
              //"darkslateblue");
              "midnightblue");
  fprintf(d.svg_fp, "      %s />\n", the_d);

  the_r *= 0.99;
  dx = sqrt((the_r * the_r) - (dy * dy));
  printf("dx = %lg\ndy = %lg\n", dx, dy);
  snprintf(the_d, sizeof(the_d), 
             "d=\"M%lg %lg A%lg %lg 0 1 1 %lg %lg Z\"", 
             cx - dx, cy + dy, the_r, the_r, 
             cx + dx, cy + dy);
  fprintf(d.svg_fp, "  <path fill=\"%s\"\n", 
              very_dark_gray);
  fprintf(d.svg_fp, "      %s />\n", the_d);

  double tick_r2 = the_r;

  snprintf(the_d, sizeof(the_d), 
             "d=\"M%lg %lg A%lg %lg 0 1 1 %lg %lg\"", 
             cx - dx, cy + dy, the_r, the_r, 
             cx + dx, cy + dy);
  fprintf(d.svg_fp, "  <path stroke=\"white\" fill=\"%s\"\n", 
              "none");
  fprintf(d.svg_fp, "      %s />\n", the_d);

  the_r *= 0.85;
  printf("dx = %lg\ndy = %lg\n", dx, dy);
  dx = sqrt((the_r * the_r) - (dy * dy));
  printf("dx = %lg\ndy = %lg\n", dx, dy);
  snprintf(the_d, sizeof(the_d), 
             "d=\"M%lg %lg A%lg %lg 0 1 1 %lg %lg Z\"", 
             cx - dx, cy + dy, the_r, the_r, 
             cx + dx, cy + dy);
  fprintf(d.svg_fp, "  <path fill=\"%s\"\n", 
              //"dodgerblue");
              "rgb(16, 78, 139)");
  fprintf(d.svg_fp, "      %s />\n", the_d);

  double tick_r1 = the_r;

  snprintf(the_d, sizeof(the_d),
             "d=\"M%lg %lg A%lg %lg 0 1 1 %lg %lg\"",
             cx - dx, cy + dy, the_r, the_r,
             cx + dx, cy + dy);
  fprintf(d.svg_fp, "  <path stroke=\"rgb(210,210,210)\" fill=\"%s\"\n",
              "none");
  fprintf(d.svg_fp, "      %s />\n", the_d);


  the_r *= 0.85;
  dx = sqrt((the_r * the_r) - (dy * dy));
  printf("dx = %lg\ndy = %lg\n", dx, dy);
  snprintf(the_d, sizeof(the_d), 
             "d=\"M%lg %lg A%lg %lg 0 1 1 %lg %lg Z\"", 
             cx - dx, cy + dy, the_r, the_r, 
             cx + dx, cy + dy);
  fprintf(d.svg_fp, "  <path fill=\"%s\"\n", 
              "black");
  fprintf(d.svg_fp, "      %s />\n", the_d);

  the_r = first_r;
  dx = sqrt((the_r * the_r) - (dy * dy));
  printf("dx = %lg\ndy = %lg\n", dx, dy);
  snprintf(the_d, sizeof(the_d), 
             "d=\"M%lg %lg A%lg %lg 0 0 1 %lg %lg Z\"", 
             cx - dx, cy + dy, 2.5 * the_r, 2.0 * the_r, 
             cx + dx, cy + dy);
  fprintf(d.svg_fp, "  <path fill=\"%s\"\n", 
              very_dark_gray);
  fprintf(d.svg_fp, "      %s />\n", the_d);
  /*** START code for needle ****/
  //fprintf(fp, "<circle cx=\"%lf\" cy=\"%lf\" r=\"%lf\" stroke=\"black\" stroke-width=\"1\" fill=\"url(#%s)\"/>",
   //                         x, y, diameter / 2.0, radial);
  fprintf(d.svg_fp, "<circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" fill=\"grey\"/>\n",
             cx, cy, first_r * 0.1);

  the_r = first_r * 0.01;
  //dx = sqrt((the_r * the_r) - (dy * dy));
  dx = first_r * 0.015;
  dy = first_r * 0.045; 
  printf("dx = %lg\ndy = %lg\n", dx, dy);
  snprintf(the_d, sizeof(the_d), 
             "d=\"M %lg %lg A %lg %lg 0 0 1 %lg %lg L %lg %lgL %lg %lg L %lg %lg L %lg %lg L %lg %lg L %lg %lg Z\"", 
             cx - dx, cy + dy, 2.5 * the_r, 2.0 * the_r, 
             cx + dx, cy + dy,
             cx + (dx * 1.2), cy + dy + (first_r * 0.075),
             cx + (dx * 1.2), cy + dy + (first_r * 0.75),

             cx + (dx * 0.2), cy + dy + (first_r * 0.85),
             cx - (dx * 0.2), cy + dy + (first_r * 0.85),

             cx - (dx * 1.2), cy + dy + (first_r * 0.75),
             cx - (dx * 1.2), cy + dy + (first_r * 0.075)
           );
  char tstr[100];  
  snprintf(tstr, sizeof(tstr), "transform=\"rotate(%lg %lg,%lg)\"",
              100.0, cx, cy);
  fprintf(d.svg_fp, "<g id=\"rotate_name_%03d\" %s> \n", n_instance, tstr);
  fprintf(d.svg_fp, "  <path fill=\"%s\" stroke=\"black\" stroke-width=\"%lg\"\n", 
    //          "orangered");
              "rgb(255, 83, 51)",
                first_r * .007);
  fprintf(d.svg_fp, "      %s />\n", the_d);
  fprintf(d.svg_fp, 
     "<rect x=\"%lg\"  y=\"%lg\" width=\"%lg\" height=\"%lg\" fill=\"orange\"/>\n", 
                   cx - (dx * 0.2), cy + dy, dx * 0.4, first_r * 0.85);
  fprintf(d.svg_fp, "</g>\n");

  /*** END code for needle ****/
  /*** START code for tick marks and numbers ****/
  double angle1 = 70;
  double angle2 = 290;
  int max = 80;
  double text_r = 0.85 * tick_r1;
  double text_h = tick_r1 * 0.18;

  for (int i=0; i <= max; i++)
  {
    double angle =  (angle1) + ((double(i)/double(max)) * (angle2 - angle1));
    snprintf(tstr, sizeof(tstr), "transform=\"rotate(%lg %lg,%lg)\"", 
              angle, cx, cy);
    printf("Angle = %lg, ", angle);
    if ((i % 10) == 0)
    {
      double rads = (angle + 90.0) * ((2.0 * M_PI) / 360.0);
      double tcos = cos(rads); 
      double tsin = sin(rads); 
      double tx = cx + (text_r * tcos);
      double ty = cy + (text_r * tsin);
      printf(" %d, cos = %lg, sin = %lg, tx = %lg ty = %lg\n", i, tcos, tsin, tx, ty);
      fprintf(d.svg_fp, 
            "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" \n",
                   cx, cy + (tick_r1 * 1.02), cx, cy + (tick_r2 * 0.9));
      fprintf(d.svg_fp, "     stroke =\"white\" stroke-width=\"%lg\" %s/>\n",
               tick_r1 * 0.02, tstr);
      fprintf(d.svg_fp, 
        "  <text x=\"%lg\" y=\"%lg\" font-size=\"%lg\" \n"
              "fill=\"%s\" stroke=\"%s\" stroke-width=\"0\"\n",
          tx, ty, text_h, "white", "none");
      fprintf(d.svg_fp, 
        " alignment-baseline=\"middle\" text-anchor=\"middle\">%d</text>\n", 
            i);

    }
    else if ((i % 5) == 0)
    {
      printf("5\n");
      fprintf(d.svg_fp, 
            "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" \n",
                   cx, cy + (tick_r1 * 1.02), cx, cy + (tick_r2 * 0.89));
      fprintf(d.svg_fp, "     stroke =\"white\" stroke-width=\"%lg\" %s/>\n",
           tick_r1 * 0.014, tstr);
    }
    else
    {
      printf("1\n");
      fprintf(d.svg_fp, 
           "  <line x1=\"%lg\" y1=\"%lg\" x2=\"%lg\" y2=\"%lg\" \n",
               cx, cy + (tick_r1 * 1.02), cx, cy + (tick_r2 * 0.88));
      fprintf(d.svg_fp, "     stroke =\"white\" stroke-width=\"%lg\" %s/>\n",
           tick_r1 * 0.008, tstr);
    }
  }
  /*** END code for tick marks and numbers ****/


  fprintf(d.svg_fp, "<!--  END insert for dash_gauge (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  END insert for dash_gauge (%03d)\n", n_instance);

  if ((strlen(the_tag) > 0) && (0 != strcasecmp(the_tag, "null")))
  {
    char js_object_name[30];
    snprintf(js_object_name, sizeof(js_object_name), "rotate_obj_%03d", n_instance);

    fprintf(d.js_fp, "var %s = new rotate_t(\"rotate_name_%03d\", " 
              "%lg, %lg, %lg, %lg);\n",
              js_object_name, n_instance, cx, cy, angle1, angle2);
    add_js_library("rotate.js");
    add_animation_object(the_tag, js_object_name);
  }
  fprintf(d.svg_fp, "<!--  END insert for gauge (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  END insert for gauge (%03d)\n", n_instance);



  n_instance++;
}

