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

class only_text_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new only_text_t;
}

const char *only_text_t::get_name(void)
{
  return "only_text";
}

void only_text_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("only_text", "Inserts just text with no animation");
  dob->param("The text to insert");
  dob->param("The color of the text");
  dob->param("X to the anchor point given in parameter 6");
  dob->param("Y to the base-line of the text");
  dob->param("Font Height of the text");
  dob->param("Anchor point in X (start, middle, or end)");
  dob->notes("deprecated");
  dob->end();
}

void only_text_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 7)
  {
    printf("%s, line %d: There must be 7 arguments to only_text\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  const char *the_text = argv[1];
  const char *color = argv[2];
  double x = atof(argv[3]);
  double y = atof(argv[4]);
  double height = atof(argv[5]);
  const char *anchor = argv[6];
  
  fprintf(d.svg_fp, "<!--  START insert for only_text (%03d) -->\n", n_instance);
  fprintf(d.svg_fp, "<text x=\"%lg\" y=\"%lg\" font-family=\"Verdana\" font-size=\"%lg\" fill=\"%s\" text-anchor=\"%s\">%s</text>\n",
                     x, y, height, color, anchor, the_text); 
  fprintf(d.svg_fp, "<!--  END insert for only_text (%03d) -->\n", n_instance);
  n_instance++;
}

