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

/*******************************************/

static int n_instance = 1;

class panel_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

/*******************************************/

extern "C" gen_plugin_base_t *get_object(void)
{
  return new panel_t;
}

/*******************************************/

const char *panel_t::get_name(void)
{
  return "panel";
}

/*******************************************/
void panel_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("panel", "A decrative raised panel to group other widgets");
  dob->param("X of upper left corner");
  dob->param("Y of upper left corner");
  dob->param("Width");
  dob->param("Height");
  dob->param("Shadow Width");
  dob->example("panel|110|65|30|75|0.3|");
  dob->notes("For now, only in grey. Lighter upper and left edges, and darker lower and right edges");
  dob->end();

}



void panel_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 6)
  {
    printf("%s, line %d: There must be 6 arguments to panel\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  double x1 = atof(argv[1]);
  double y1 = atof(argv[2]);
  double width = atof(argv[3]);
  double height = atof(argv[4]);
  double shadow_width = atof(argv[5]);
  double x2 = x1 + width;
  double y2 = y1 + height;
  double sw = shadow_width;

  int pcolor = 200; 
  int lcolor = 240; 
  int scolor = 80;

  char str_pcolor[100];  
  char str_lcolor[100];
  char str_scolor[100];
  snprintf(str_pcolor, sizeof(str_pcolor), "rgb(%d,%d,%d)", 
          pcolor, pcolor, pcolor);
  snprintf(str_lcolor, sizeof(str_lcolor), "rgb(%d,%d,%d)", 
          lcolor, lcolor, lcolor);
  snprintf(str_scolor, sizeof(str_scolor), "rgb(%d,%d,%d)", 
          scolor, scolor, scolor);

  fprintf(d.svg_fp, "<!-- START insert for panel (%03d) -->\n", n_instance);

  fprintf(d.svg_fp, "<rect fill=\"%s\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" stroke=\"none\"/>\n",
        str_pcolor, x1, y1, width, height);

  fprintf(d.svg_fp, "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg\" fill=\"%s\" stroke=\"none\"/>\n",
     x1, y1, x1+sw, y1+sw, x2-sw, y1+sw, x2, y1, str_lcolor);
  fprintf(d.svg_fp, "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg\" fill=\"%s\" stroke=\"none\"/>\n", 
     x1, y1, x1+sw, y1+sw, x1+sw, y2-sw, x1, y2, str_lcolor);
  fprintf(d.svg_fp, "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg\" fill=\"%s\" stroke=\"none\"/>\n",
     x1, y2, x1+sw, y2-sw, x2-sw, y2-sw, x2, y2, str_scolor);
  fprintf(d.svg_fp, "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg\" fill=\"%s\" stroke=\"none\"/>\n",
     x2, y2, x2-sw, y2-sw, x2-sw, y1+sw, x2, y1, str_scolor);

 

  fprintf(d.svg_fp, "<!--  END insert for panel (%03d) -->\n", n_instance);
  n_instance++;
}

/*******************************************/

