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
#include <ctype.h>

#include "gen_display.h"

static int n_instance = 1;

class rect_light_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new rect_light_t;
}

const char *rect_light_t::get_name(void)
{
  return "rect_light";
}

void rect_light_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("rect_light", "Simple rectangular panel light");
  dob->param("Discrete Tag");
  dob->param("On Color");
  dob->param("Off Color");
  dob->param("X of upper left corner");
  dob->param("Y of upper left corner");
  dob->param("Width");
  dob->param("Height");
  dob->param("Text Location (B = below, A = Above, L = left, R = right, N = none)");
  dob->param("Generate popup 1 = yes, 0 = no");
  dob->example("rect_light|hydraulic_1a|lime|gray|120|68|8|8|N|");
  dob->notes("Uses tag attributes: 'lo_desc', 'hi_desc'");
  dob->end();
}

void rect_light_t::generate(plugin_data_t d, int argc, char **argv)
{

  if ((argc != 10) && (argc != 9))
  {
    printf("%s, line %d: There must be 9 or 10 arguments to rect_light\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  const char *the_tag = argv[1];
  const char *on_color = argv[2];
  const char *off_color = argv[3];
  double x = atof(argv[4]);
  double y = atof(argv[5]);
  double width = atof(argv[6]);
  double height = atof(argv[7]);
  char text_location = toupper(argv[8][0]);
  double stroke_width = ((width + height) / 2) * 0.1;
  double font_size = ((width + height) / 2 )  * 0.6;
  double text_x, text_y;
  const char *text_align;
  bool has_text=true;
  bool gen_popup = false;
  if (d.popup_on && (argc > 9))
  {
    gen_popup = (argv[9][0] == '1');
  }

  
  switch (text_location)
   {
	case 'B': text_x =  x + (width/2);
                  text_y =  y+height + (font_size * 1.1);
                  text_align = "middle";
                  break;
 	case 'A': text_x =  x + (width/2);
                  text_y =  y - (font_size  / 3 );
                  text_align = "middle";
                  break;
 	case 'L': text_x =  x - (font_size * 0.2); 
                  text_y =  y + (font_size  * 0.7);
                  text_align = "end";
                  break;
 	case 'R': text_x =  x + width + (font_size * 0.2);
                  text_y =  y + (font_size * 0.7);
                  text_align = "start";
                  break;
  	default:  text_x =  0;
                  text_y =  0;
                  text_align = NULL;
                  has_text = false;
                  break;
 }
                

  fprintf(d.svg_fp, "<!--  START insert for rect_light (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  START insert for rect_light (%03d)\n", n_instance);

  fprintf(d.svg_fp, "<rect id=\"rect_light_%03d\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" fill=\"%s\" stroke=\"black\" stroke-width=\"%lg\"/>\n",
            n_instance, x, y, width, height, on_color, stroke_width);
  if (has_text)
  	fprintf(d.svg_fp, "<text id=\"rect_light_pv_%03d\" x=\"%lg\" y=\"%lg\" font-family=\"Verdana\" font-size=\"%lg\" fill=\"black\" text-anchor=\"%s\">0</text>\n",
                     n_instance, text_x, text_y, font_size, text_align);


  char js_object_name[30];
  char js_pv_name[30];
  snprintf(js_object_name, sizeof(js_object_name), "rect_light_%03d", n_instance);
  if (has_text)
     snprintf(js_pv_name, sizeof(js_pv_name), "\"rect_light_pv_%03d\"", n_instance);
  else 
     snprintf(js_pv_name, sizeof(js_pv_name), "null");

  fprintf(d.js_fp, "var %s = new rect_light_t(\"rect_light_%03d\", %s, \"%s\", \"%s\");\n", js_object_name, n_instance, js_pv_name, on_color, off_color); 

  if (gen_popup)
  {
    double px, py;
    find_a_place_nearby(&px, &py, x, y, width, height);
    fprintf(d.svg_fp, "<rect x=\"%lg\"  y=\"%lg\" width=\"%lg\" height=\"%lg\"\n"
       "  onclick=\"show_popup(%lg,%lg,'ON', 'Off', '%s')\" visibility=\"hidden\"\n"
       "  pointer-events=\"all\" onmouseover=\"this.style.cursor='pointer';\"/>\n",
         x, y, width, height, px, py, the_tag);
  }

  fprintf(d.svg_fp, "<!--  END insert for rect_light (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  END insert for rect_light (%03d)\n", n_instance);

  if ((strlen(the_tag) > 0) && (0 != strcasecmp(the_tag, "null")))
  {
    add_js_library("rect_light.js");
    add_animation_object(the_tag, js_object_name);
  }

  n_instance++;
}

