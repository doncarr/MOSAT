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

/**********************************************************/

void gen_imasa_logo(FILE *fp, double x, double y, double height, double width)
{
  double halfy = height / 2.0;
  double halfx = width / 2.0;
  const double ftopa = 0.53; 
  const double ftopb = 0.59; 
  const double fbota = 0.38; 
  const double fbotb = 0.46; 
  const double ftop2a = 0.47; 
  const double ftop2b = 0.54; 
  const double fbot2a = 0.35;
  const double fbot2b = 0.44;
  const double fmxa = 0.51;
  const double fmya = 0.67;

  const double fmxb = 0.52;
  const double fmyb = 0.79;

  const double fmxl = 0.30;
  const double fmyl = 0.36;

  const double fmxr = 0.72;
  const double fmyr = 0.36;
  
  fprintf(fp, "\n");
  fprintf(fp, "<!-- START of IMASA logo -->\n");
      // First, we have to define the gradients to use
  fprintf(fp, "  <defs>\n");
  fprintf(fp, "    <linearGradient id=\"imasaLinearBlue\" x1=\"0%%\" y1=\"0%%\" x2=\"0%%\" y2=\"100%%\" spreadMethod=\"pad\" gradientUnits=\"objectBoundingBox\">\n");
  fprintf(fp, "      <stop offset=\"0%%\" stop-color=\"mediumblue\" stop-opacity=\"1\"/>\n");
  fprintf(fp, "      <stop offset=\"100%%\" stop-color=\"lightblue\" stop-opacity=\"1\"/>\n");
  fprintf(fp, "    </linearGradient>\n");
  fprintf(fp, "    <linearGradient id=\"imasaLinearGrey\" x1=\"0%%\" y1=\"0%%\" x2=\"0%%\" y2=\"100%%\" spreadMethod=\"pad\" gradientUnits=\"objectBoundingBox\">\n");
  fprintf(fp, "      <stop offset=\"0%%\" stop-color=\"lightgrey\" stop-opacity=\"1\"/>\n");
  fprintf(fp, "      <stop offset=\"100%%\" stop-color=\"grey\" stop-opacity=\"1\"/>\n");
  fprintf(fp, "    </linearGradient>\n");
  fprintf(fp, "    <linearGradient id=\"imasaLinearWhite\" x1=\"0%%\" y1=\"0%%\" x2=\"0%%\" y2=\"100%%\" spreadMethod=\"pad\" gradientUnits=\"objectBoundingBox\">\n");
  fprintf(fp, "      <stop offset=\"0%%\" stop-color=\"lightgrey\" stop-opacity=\"1\"/>\n");
  fprintf(fp, "      <stop offset=\"50%%\" stop-color=\"white\" stop-opacity=\"1\"/>\n");
  fprintf(fp, "      <stop offset=\"100%%\" stop-color=\"lightgrey\" stop-opacity=\"1\"/>\n");
  fprintf(fp, "    </linearGradient>\n");
  fprintf(fp, "  </defs>\n");

      // Now, draw the upper polygon in gradient of blue 
  fprintf(fp, "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg\"\n",
       x + 0, y + halfy, x + halfx, y + 0, 
       x + width, y + halfy, x + halfx, y + height);
  fprintf(fp, "  style=\"fill:url(#imasaLinearBlue);stroke:none;stroke-width:0\"/>\n");

      // Now, draw the lower polygon in gradient of gray 
  fprintf(fp, "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\"\n",
           x + (halfx * fbotb), 
           y + halfy + (halfy * fbotb),

           x + (width * fmxl), 
           y + (height * fmyl), 

           x + (width * fmxb), 
           y + (height * fmyb),

           x + (width * fmxr), 
           y + (height * fmyr),

           x + width - (halfx * fbot2b), 
           y + halfy + (halfy * fbot2b),

           x + halfx, 
           y + height);
  fprintf(fp, "  style=\"fill:url(#imasaLinearGrey);stroke:none;stroke-width:0\"/>\n");

      // Now, draw the IMASA 'M' in gradient of white 
  fprintf(fp, "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\"\n",
           x + (halfx * fbotb), 
           y + halfy + (halfy * fbotb),

           x + (width * fmxl), 
           y + (height * fmyl), 

           x + (width * fmxb), 
           y + (height * fmyb),

           x + (width * fmxr), 
           y + (height * fmyr),

           x + width - (halfx * fbot2b), 
           y + halfy + (halfy * fbot2b),

           x + width - (halfx * fbot2a), 
           y + halfy + (halfy * fbot2a),

           x + width - (halfx * ftop2a), 
           y + (halfy * (1.0-ftop2a)), 

           x + width - (halfx * ftop2b), 
           y + (halfy * (1.0-ftop2b)), 

           x + (width * fmxa), 
           y + (height * fmya),

           x + (halfx * ftopb), 
           y + (halfy * (1.0-ftopb)), 

           x + (halfx * ftopa), 
           y + (halfy * (1.0-ftopa)), 

           x + (halfx * fbota), 
           y + halfy + (halfy * fbota) );
  fprintf(fp, "  style=\"fill:url(#imasaLinearWhite);stroke:none;stroke-width:0\"/>\n");
  fprintf(fp, "<!-- END of IMASA logo -->\n");
  fprintf(fp, "\n");
}
 
/**********************************************************/

#include "gen_display.h"

static int n_instance = 1;

class imasa_logo_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

extern "C" gen_plugin_base_t *get_object(void)
{
  return new imasa_logo_t;
}

const char *imasa_logo_t::get_name(void)
{
  return "imasa_logo";
}

void imasa_logo_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("imasa_logo", "Logo created for IMASA Guadalajara");
  dob->param("X of upper left corner");
  dob->param("Y of upper left corner");
  dob->param("Height");
  dob->notes("Width = Height");
  dob->end();
}

void imasa_logo_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc != 4)
  {
    printf("%s, line %d: There must be 4 arguments to imasa_logo\n",
           d.file_name, d.line_number);
    exit(-1);
  }

  double x = atof(argv[1]);
  double y = atof(argv[2]);
  double height = atof(argv[3]);
  double width = height;
  
  //fprintf(d.svg_fp, "<!--  START insert for imasa_logo (%03d) -->\n", n_instance);
  gen_imasa_logo(d.svg_fp, x, y, height, width);
  //fprintf(d.svg_fp, "<!--  END insert for imasa_logo (%03d) -->\n", n_instance);
  n_instance++;
}

