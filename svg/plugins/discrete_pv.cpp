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

class discrete_pv_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new discrete_pv_t;
}

const char *discrete_pv_t::get_name(void)
{
  return "discrete_pv";
}

void discrete_pv_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("discrete_pv", "Dislays analog PV on screen as text");
  dob->param("Discrete Tagname");
  dob->param("Text Color");
  dob->param("X (to middle of string)n");
  dob->param("Y (to baseline of string)");
  dob->param("Text Height");
  dob->notes("We could add parameters to control the allignment");
  dob->notes("Uses tag attributes: 'lo_desc', 'hi_desc'");
  dob->example("discrete_pv|PUMP1_ON|red|129|145|8|");
  dob->end();
}

void discrete_pv_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 6)
  {
    printf("%s, line %d: There must be 6 arguments to discrete_pv\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  const char *the_tag = argv[1];
  const char *color = argv[2];
  double x = atof(argv[3]);
  double y = atof(argv[4]);
  double height = atof(argv[5]);
  double font_size = height;

  
  fprintf(d.svg_fp, "<!--  START insert for discrete_pv (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  START insert for discrete_pv (%03d)\n", n_instance);
  fprintf(d.svg_fp, "<text id=\"discrete_pv_%03d\" x=\"%lg\" y=\"%lg\" font-family=\"Verdana\" font-size=\"%lg\" fill=\"%s\" text-anchor=\"middle\">0</text>\n",
             n_instance, x, y, font_size, color); 

  if ((strlen(the_tag) > 0) && (0 != strcasecmp(the_tag, "null")))
  {
    char js_object_name[30];
    snprintf(js_object_name, sizeof(js_object_name), "discrete_pv_obj_%03d", n_instance);
    add_js_library("discrete_pv.js");
    add_animation_object(the_tag, js_object_name);
    fprintf(d.js_fp, "var %s = new discrete_pv_t(\"discrete_pv_%03d\");\n", js_object_name, n_instance); 
  }

  fprintf(d.svg_fp, "<!--  END insert for discrete_pv (%03d) -->\n", n_instance);
  fprintf(d.js_fp, "// --  END insert for discrete_pv (%03d)\n", n_instance);
  n_instance++;
}

