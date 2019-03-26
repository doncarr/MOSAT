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

class link_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new link_t;
}

const char *link_t::get_name(void)
{
  return "link";
}

void link_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("link", "Multi-line link to load other web page");
  dob->param("X position of achor point");
  dob->param("Y position of first base-line");
  dob->param("Text Height");
  dob->param("Text Color");
  dob->param("Text Anchor (start, middle, end)");
  dob->param("Hyperlink - can be relative for the same site");
  dob->param("Text of Link");
  dob->param("Now you can repeat 'Hyperlink' and 'Text' of Link as many times as needed.");
  dob->example("link|100|12|6|firebrick|middle|cool.svg|Cooling Tower|feed.svg|Feed Tanks|");
  dob->notes("Additional links appear to the right of the first with a separating '|'.");
  dob->end();
}

void link_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc < 8)
  {
    printf("%s, line %d: There must be AT LEAST 8 arguments to link\n",
           d.file_name, d.line_number);
    exit(-1);
  }
  if ((argc % 2) != 0)
  {
    printf("%s, line %d: There must be a text description for every link URL.\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  double x = atof(argv[1]);
  double y = atof(argv[2]);
  double height = atof(argv[3]);
  const char *color = argv[4];
  const char *anchor = argv[5];
  const char *the_link = argv[6];
  const char *the_text = argv[7];

  fprintf(d.svg_fp, "<!--  START insert for link (%03d) -->\n", n_instance);
    fprintf(d.svg_fp, "<text x=\"%lg\" y=\"%lg\" font-family=\"Verdana\" font-size=\"%lg\"\n"
                      "  pointer-events=\"all\"\n fill=\"%s\" text-anchor=\"%s\">\n"
                      "  <tspan onclick=\"window.open('%s', '_self');\"\n"
                      "     onmouseover=\"this.style.cursor='pointer';\">%s</tspan>\n",
                     x, y, height, color, anchor, the_link, the_text); 
  for (int i=8; i < argc; i += 2)
  { 
    the_link = argv[i];
    the_text = argv[i+1];
    fprintf(d.svg_fp, "  <tspan>    |    </tspan>\n");
    fprintf(d.svg_fp, "  <tspan onclick=\"window.open('%s', '_self');\"\n"
                      "     onmouseover=\"this.style.cursor='pointer';\">%s</tspan>\n", 
             the_link, the_text);
  }
  fprintf(d.svg_fp, "</text>\n");
  fprintf(d.svg_fp, "<!--  END insert for link (%03d) -->\n", n_instance);
  n_instance++;
}

