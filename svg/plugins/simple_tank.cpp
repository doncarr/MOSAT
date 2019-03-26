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

static const char *tank_color = "gray";
static bool silent = false;

/******************************************/

void gen_cone_tank(FILE *fp, double x, double y, double width, double height, double cap_height, 
              double cone_height, double cone_width)
{
  fprintf(fp, "  <g stroke=\"#000000\" stroke-width=\"0\">\n");
  fprintf(fp, "    <rect fill=\"url(#%sLinearV)\"\n", tank_color);
  fprintf(fp, "      x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\"/>\n", 
                          x, y + cap_height, width, height - cone_height - cap_height);
  fprintf(fp, "    <path fill=\"url(#%sRadial)\"\n", tank_color);
  fprintf(fp, "      d=\"M%lg %lg A%lg %lg 0 1 1 %lg %lg z M%lg %lg L%lg %lg\"/>\n",
                     //d="M0,50 A125,50 0 1,1 250,50 M250,100 L250,100"/>
                          x, y + cap_height, 
                          width / 2.0, cap_height, 
                          x + width, y + cap_height,
                          x, y + (cap_height * 2.0),
                          x, y + (cap_height * 2.0)); 
  fprintf(fp, "    <path fill=\"url(#%sRadial)\"\n", tank_color);
  fprintf(fp, "d=\"M%lg %lg h%lg l%lg %lg h%lg z m 0 %lg h 0\"/>\n",
                     //d="M0,400 h200 l-50,100 h-100 z "/>
                          x, y + height - cone_height,
                          width, 
                          (width - cone_width)/-2.0, cone_height, 
                          cone_width * -1.0,
                          cone_height * -1.0);
  fprintf(fp, "  </g>\n");
}

/******************************************/

void gen_pressure_tank(FILE *fp, double x, double y, double width, double height, double top_height)
{
  fprintf(fp, "  <g stroke=\"#000000\" stroke-width=\"0\">\n");
  fprintf(fp, "    <rect fill=\"url(#%sLinearV)\"\n", tank_color);
  fprintf(fp, "      x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\"/>\n", 
                          x, y + top_height, width, height - (2 * top_height));
  fprintf(fp, "    <path fill=\"url(#%sRadial)\"\n", tank_color);
  fprintf(fp, "      d=\"M%lg %lg A%lg %lg 0 1 1 %lg %lg z M%lg %lg L%lg %lg\"/>\n",
                     //d="M0,50 A125,50 0 1,1 250,50 M250,100 L250,100"/>
                          x, y + top_height, 
                          width / 2.0, top_height, 
                          x + width, y + top_height,
                          x, y + (top_height * 2.0),
                          x, y + (top_height * 2.0)); 
  fprintf(fp, "    <path fill=\"url(#%sRadial)\"\n", tank_color);
  fprintf(fp, "      d=\"M%lg %lg A%lg %lg 0 1,0 %lg %lg z M%lg %lg L%lg %lg\"/>\n",
                      //d="M0,450 A125,50 0 1,0 250,450 M250,400 L250,400"/>
                          x, y + height - (top_height), 
                          (width / 2.0), (top_height), 
                          x + width, y + height - (top_height),
                          x, y + height - (top_height * 2.0),
                          x, y + height - (top_height * 2.0)); 
  fprintf(fp, "\n  </g>\n");
}

/******************************************/

static int n_instance = 1;

class simple_tank_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

/**********************************************/

extern "C" gen_plugin_base_t *get_object(void)
{
  return new simple_tank_t;
}

/**********************************************/

const char *simple_tank_t::get_name(void)
{
  return "simple_tank";
}

/**********************************************/

void simple_tank_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("simple_tank", "Either a standard pressure tank, or a cone tank");
  dob->param("Type (c = cone, p = pressure)");
  dob->param("tank color (both types)");
  dob->param("X of upper left (both types)");
  dob->param("Y of upper left (both types)");
  dob->param("Width (both types)");
  dob->param("Total Height (both types)");
  dob->param("Cap Height (both types)");
  dob->param("Cone Height (cone only)");
  dob->param("Cone width at bottom (cone only)");
  dob->example("simple_tank|c|black|30|10|40|110|1.5|25|3|");
  dob->example("simple_tank|p|gray|60|20|35|100|8|");
  dob->notes("No animation");
  dob->end();
}

void simple_tank_t::generate(plugin_data_t d, int argc, char **argv)
{
  if ((argc != 8) && (argc != 10))
  {
    printf("%s, line %d: There must be 8 or 10 arguments to simple_tank\n",
           d.file_name, d.line_number);
    exit(-1);
  }
  silent = d.silent;

  double x = 50;
  double y = 50;
  double width = 800;
  double height = 600; 
  double cap_height = 50;
  double cone_height = 200;
  double cone_width = 30;

  const char *the_color = argv[2];

  if (0 == strcasecmp(the_color, "yellow")) 
  {
    tank_color = "yellow";
    add_svg_library("yellow_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "black")) 
  {
    tank_color = "black";
    add_svg_library("black_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "orange")) 
  {
    tank_color = "orange";
    add_svg_library("orange_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "brown")) 
  {
    tank_color = "brown";
    add_svg_library("brown_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "white")) 
  {
    tank_color = "white";
    add_svg_library("white_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "red")) 
  {
    tank_color = "red";
    add_svg_library("red_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "blue")) 
  {
    tank_color = "blue";
    add_svg_library("blue_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "green")) 
  {
    tank_color = "green";
    add_svg_library("green_gradients.svg");
  }
  else // Anything else, use gray
  {
    tank_color = "gray";
    add_svg_library("gray_gradients.svg");
  }



  fprintf(d.svg_fp, "<!--  START insert for simple_tank (%03d) -->\n", n_instance);
  if ((argc == 10) && (argv[1][0] == 'c'))
  {
    // /gen_tank c 0 0 300 400 20 50 20
    x = atof(argv[3]);
    y = atof(argv[4]);
    width = atof(argv[5]);
    height = atof(argv[6]);
    cap_height = atof(argv[7]);
    cone_height = atof(argv[8]);
    cone_width = atof(argv[9]);
    if (!silent) 
    {
      printf("cone tank:\n");
      printf("x = %lg\n", x);
      printf("y = %lg\n", y);
      printf("width = %lg\n", width);
      printf("height = %lg\n", height);
      printf("cap_height = %lg\n", cap_height);
      printf("cone_height = %lg\n", cone_height);
      printf("cone_width = %lg\n", cone_width);
    }
    gen_cone_tank(d.svg_fp, x, y, width, height, cap_height, cone_height, cone_width);
  }
  else if ((argc == 8) && (argv[1][0] == 'p'))
  {
    // /gen_tank p 0 0 300 400 20 
    x = atof(argv[3]);
    y = atof(argv[4]);
    width = atof(argv[5]);
    height = atof(argv[6]);
    cap_height = atof(argv[7]);
    if (!silent) 
    {
      printf("pressure tank:\n");
      printf("x = %lg\n", x);
      printf("y = %lg\n", y);
      printf("width = %lg\n", width);
      printf("height = %lg\n", height);
      printf("cap_height = %lg\n", cap_height);
    }
    gen_pressure_tank(d.svg_fp, x, y, width, height, cap_height);
  }
  else
  {
    printf("%s, line %d: Incorrect arguments for tank, examples:\n",
           d.file_name, d.line_number);
    printf("    |simple_tank|c|0|0|300|400|20|50|20|\n");
    printf("    |simple_tank|p|0|0|300|400|20|\n");
    exit(-1);
  }
  fprintf(d.svg_fp, "<!--  END insert for simple_tank (%03d) -->\n", n_instance);
  //add_svg_library("gray_gradients.svg");
  n_instance++;
}

