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

static double x_offset;
static double y_offset;
static double rise;
static double font_size = 15;
static double width = 100;

/*******************************************/
static bool first = true;
static double stroke_width = 1.0;
static const char *stroke_color = "black";
//static const char *shadow = " filter=\"url(#pipeshadow)\" ";
static bool silent = false;

/*******************************************/

//          gen_h_rest(d.svg_fp, x1, y1, width, up, right);
void gen_h_rest(FILE *fp, double x, double y, double z, double width, bool right, bool up) 
{
  fprintf(fp, 
     "<rect x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\"/>\n",
          x, y - (width/2.0), width, width);
  fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" />\n",
            x + (width / 2.0), y + (0.5 * width), x, y - (width/2.0));
  fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" />\n",
            x + width,y, x, y - (width/2.0));
  double zt = z;
  z = y_offset - z;
  z = z;// + width / 2.0);
  x = x_offset + x; 
  //x = x - (width/2.0);
  fprintf(fp, 
     "<rect x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" stroke=\"blue\" fill=\"none\"/>\n",
          x, z - (3 * rise),width, 3 * rise);
  fprintf(fp, "<text x=\"%lg\" y=\"%lg\" " 
                  " font-size=\"%lg\" fill=\"black\" text-anchor=\"start\">%lg</text>\n",
                     x + width, z-3*rise, font_size, zt+3*rise);
  /**
  for (int i=0; i < 3; i++)
  {
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" stroke=\"blue\"/>\n",
            x, z, x, z-rise);
    if (i < 2)
    {
      fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" stroke=\"blue\"/>\n",
            x, z-rise, x+(width/2), z-rise);
    }
    z = z - rise;
    x = x + (width/2);
  }
  **/

}

/*******************************************/

void gen_v_rest(FILE *fp, double x, double y, double z, double width, bool up, bool right) 
{
  fprintf(fp, 
     "<rect x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\"/>\n",
          x - (width/2.0), y, width, width);
  fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" />\n",
            x - (width / 2.0), y + (0.5 * width), x + (width/2.0), y);
  fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" />\n",
            x , y + (width), x + (width/2.0), y);

  double zt = z;
  z = y_offset - z;
  x = x_offset + x; 
  x = x - (width/2.0);
  fprintf(fp, 
     "<rect x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" stroke=\"blue\" fill=\"none\"/>\n",
          x, z - (3 * rise),width, 3 * rise);
  fprintf(fp, "<text x=\"%lg\" y=\"%lg\" "
                  " font-size=\"%lg\" fill=\"black\" text-anchor=\"start\">%lg</text>\n",
                     x + width, z-3*rise, font_size, zt+3*rise);
  /***
  for (int i=0; i < 3; i++)
  {
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" stroke=\"blue\"/>\n",
            x, z, x, z-rise);
    if (i < 2)
    {
      fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" stroke=\"blue\"/>\n",
            x, z-rise, x+(width/2), z-rise);
    }
    z = z - rise;
    x = x + (width/2);
  }
  ****/

}

/*******************************************/

void gen_v_step(FILE *fp, double x, double y, double z, double width, double depth, int n)
{
  if (!silent) printf("  v_step start: x: %lg, y: %lg\n", x, y);
  bool neg = false;
  if (n < 0) 
  {
    y -= depth;
    neg = true;
    n = -n;
  }
  for (int i=0; i < n; i++)
  {
    if (!silent) printf("  v_step[%d]: x: %lg, y: %lg, z: %lg\n", i, x, y, z);
    fprintf(fp, 
     "<rect x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\"/>\n",
          x - (width/2.0), y, width, depth);
    if (neg) y -= depth;
    else y += depth;
  }
  fprintf(fp, "<!-- Start of z view -->\n");
  double zt = z;
  z = y_offset - z;
  x = x_offset + x; 
  if (neg) n++;
  for (int i=0; i < n; i++)
  {
    zt = zt + rise;
    fprintf(fp, 
     "<rect x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\"/>\n",
          x - (width/2.0), z - rise, width, rise);
    fprintf(fp, "<text x=\"%lg\" y=\"%lg\" "
                  " font-size=\"%lg\" fill=\"black\" text-anchor=\"start\">%lg</text>\n",
                     x + width/2, z-rise, font_size, zt);
    z = z - rise;
  }
}

/*******************************************/

void gen_h_step(FILE *fp, double x, double y, double z, double width, double depth, int n)
{
  double tmp_x = x;
  for (int i=0; i < n; i++)
  {
    fprintf(fp, 
     "<rect x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\"/>\n",
          tmp_x, y - (width/2.0), depth, width);
    tmp_x += depth;
  }
  fprintf(fp, "<!-- Start of z view -->\n");
  double zt = z;
  z = y_offset - z;
  x = x_offset + x; 
  for (int i=0; i < n; i++)
  {
    zt = zt + rise;
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" stroke=\"magenta\"/>\n",
            x, z, x, z-rise);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" stroke=\"magenta\"/>\n",
            x, z-rise, x + depth, z-rise);
    fprintf(fp, "<text x=\"%lg\" y=\"%lg\" "
                  " font-size=\"%lg\" fill=\"black\" text-anchor=\"start\">%lg</text>\n",
                     x + depth, z-rise, font_size, zt);
    z = z - rise;
    x = x + depth;
  }
}

/*******************************************/

static int n_instance = 1;

class step_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

/*******************************************/

extern "C" gen_plugin_base_t *get_object(void)
{
  return new step_t;
}

/*******************************************/

const char *step_t::get_name(void)
{
  return "step";
}

/*******************************************/

void step_t::generate_doc(doc_object_base_t *dob)
{
  /**
  dob->start("step", " A simple step with landings");
  dob->param("Width of each the step");
  dob->param("Depth of each the step");
  dob->param("X of the starting point of the steps");
  dob->param("Y of the starting point of the steps");
  dob->param("Direction that must be 'V' or 'H', or 'R' for Rest (landing)");
  dob->param("Number of stairs, or, lu, ld, ru, rd, ur, ul, dr, dl");
  dob->param("Now, Repeat 7 and 8 as many times as needed to complete the pipe");
  dob->example("step|100|100|100|28|18|gray|600|500|v|4|r|dr|h|2|r|ru|v|-2|");
  dob->notes("You must alternate V and H, you can not have two Vs or two Hs in a row");
  dob->end();
  **/
}



void step_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc < 9)
  {
    printf("%s, line %d: There must be AT LEAST 9 arguments to step\n",
           d.file_name, d.line_number);
    exit(-1);
  }
  if ((argc % 2) != 1)
  {
    printf("%s, line %d: There must be an odd number of arguments to step\n",
           d.file_name, d.line_number);
    exit(-1);
  }
  silent = d.silent;

  // step|x1|y1|width|depth|rise|fill|
  double x1 = atof(argv[1]);
  double y1 = atof(argv[2]);
  double z1 = 0;
  width = atof(argv[3]);
  double depth = atof(argv[4]);
  rise = atof(argv[5]);
  const char *fill_color = argv[6];
  x_offset = atof(argv[7]);
  y_offset = atof(argv[8]);
  stroke_width = 0.1 * width;

  if (!silent) printf("Fill color is: %s\n", fill_color);
  if (!silent) printf("depth: %g, width: %g, rise: %g\n", depth, width, rise);
  if (!silent) printf("x1: %g, y1: %g\n", x1, y1);
  if (!silent) printf("x_offset: %g, y offset: %g\n", x_offset, y_offset);
  first = true;

  fprintf(d.svg_fp, "<!-- START insert for step (%03d) -->\n", n_instance);
/***/
  char js_group_name[30];
  snprintf(js_group_name, sizeof(js_group_name), "step_group_obj_%03d", n_instance);
  fprintf(d.svg_fp, "<g id=\"%s\" fill=\"%s\" stroke=\"%s\">\n",
       js_group_name, fill_color, stroke_color);
/***/
  char last = ' ';
  int n_steps;
  bool up;
  bool right;
  char c1, c2;
  for (int i=9; i < (argc-1); i+=2)
  {
    if (!silent) printf("last: %c\n", last);
    switch (argv[i][0])
    {
      case 'h':
      case 'H':
        last = 'h';
        n_steps = atoi(argv[i+1]);
        gen_h_step(d.svg_fp, x1, y1, z1, width, depth, n_steps);
        x1 += depth * n_steps;
        z1 += rise * abs(n_steps);
        if (!silent) printf("hstep: x1: %lg, y1: %lg, z: %lg\n", x1, y1, z1);
        break;
      case 'v':
      case 'V':
        last = 'v';
        n_steps = atoi(argv[i+1]);
        gen_v_step(d.svg_fp, x1, y1, z1, width, depth, n_steps);
        y1 += depth * n_steps;
        z1 += rise * abs(n_steps);
        if (!silent) printf("vstep: x1: %lg, y1: %lg, z: %lg\n", x1, y1, z1);
        break;
      case 'r':
      case 'R':
        last = 'r';
        c1 = argv[i+1][0]; 
        c2 = argv[i+1][1]; 
        if ((c1 == 'r') || (c1 == 'l'))
        {
          right = (c1 == 'r');
          up = (c2 == 'u');
          gen_h_rest(d.svg_fp, x1, y1, z1, width, up, right);
        }
        else
        {
          up = (c1 == 'u');
          right = (c2 == 'r');
          gen_v_rest(d.svg_fp, x1, y1, z1, width, up, right);
        }
        if (up) y1 -= (width/2.0); else y1 += (width/2.0); 
        if (right) x1 += (width/2.0); else x1 -= (width/2.0); 
        z1 += rise * 3;
        if (!silent) printf("rest: x: %lg, y: %lg, z: %lg\n", x1, y1, z1);
        break;
      default:
        printf("Error: %s, must be either 'h' or 'v' or 'r' for the direction\n", argv[i]);
        printf("%s, line %d: found %s, must be either 'h' or 'v' or 'r' for the direction\n", 
           d.file_name, d.line_number, argv[i]);
        exit(-1);
        break;
    } 
  }
  fprintf(d.svg_fp, "</g>\n");
  fprintf(d.svg_fp, "<!--  END insert for step (%03d) -->\n", n_instance);
  n_instance++;
}

/*******************************************/

