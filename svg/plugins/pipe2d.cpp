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
static bool last_HR = true;
static bool last_VU = true;
static bool last_V = true;
static bool first = true;
static double stroke_width = 1.0;
static const char *stroke_color = "black";
static const char *shadow = " filter=\"url(#pipeshadow)\" ";
static bool silent = false;

void gen_butt_end(FILE *fp, double width, double butt_width, double x2, double y2)
{
  if (!silent) printf("butt end - width: %lg, bwidth: %lg, x2: %lg, y2: %lg\n",
         width, butt_width, x2, y2);
  if (last_V && last_VU)
  {
    // draw the butt end up
    if (!silent) printf("butt up\n");
    fprintf(fp, 
    "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg\"/>\n",
          x2 - (width/2.0), y2, x2 + (width/2.0), y2, x2, y2 - (butt_width/2.0));
  }
  else if (last_V && !last_VU)
  {
    // draw the butt end down 
    if (!silent) printf("butt down\n");
    fprintf(fp, 
    "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg\"/>\n",
          x2 - (width/2.0), y2, x2 + (width/2.0), y2, x2, y2 + (butt_width/2.0));
  }
  else if (!last_V && last_HR)
  {
    // draw the butt end right 
    if (!silent) printf("butt right\n");
    fprintf(fp, 
    "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg\"/>\n",
          x2, y2 - (width/2.0), x2, y2 + (width/2.0), x2 + (butt_width/2.0), y2);
  }
  else if (!last_V && !last_HR)
  {
    // draw the butt end left 
    if (!silent) printf("butt left\n");
    fprintf(fp, 
    "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg\"/>\n",
          x2, y2 - (width/2.0), x2, y2 + (width/2.0), x2 - (butt_width/2.0), y2);
  }
  
}

/*******************************************/

void gen_v_pipe(FILE *fp, double x, double y1, double y2, double width) 
{
  double x1 = x - (width / 2.0);
  double x2 = x + (width / 2.0);

  
  last_V = true;
  if(y1 > y2)
  {
    double tmp;
    tmp = y1;
    y1 = y2;
    y2 = tmp; 
    last_VU = true;
  } 
  else
  {
    last_VU = false;
  }

  if (!silent) printf("V - x1: %lg, y1: %lg, x2: %lg, y2: %lg\n", x1, y1, x2, y2);

  if (first)
  {
    first = false;
    fprintf(fp, 
     "<rect x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" %s/>\n",
          x - (width/2.0), y1, width, y2-y1, shadow);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x1, y1, x1, y2); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x2, y1, x2, y2); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
  }
  else if (last_HR && last_VU)
  {
    // case last was to the right, next is up 
    fprintf(fp, 
    "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\" %s/>\n",
          x, y2, x2, y2, x2, y1, x1, y1, x1, y2 - (width * 0.5), shadow);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x1, y1, x1, y2 - (width * 0.5)); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x2, y1, x2, y2); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
  }
  else if (!last_HR && last_VU)
  {
    // case last was to the left, next is up 
    fprintf(fp, 
    "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\" %s/>\n",
          x, y2, x1, y2, x1, y1, x2, y1, x2, y2 - (width * 0.5), shadow);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x1, y1, x1, y2); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x2, y1, x2, y2 - (width * 0.5)); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
    
  }
  else if (last_HR && !last_VU)
  {
    // case last was to the right, next is down
    fprintf(fp, 
    "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\" %s/>\n",
          x, y1, x2, y1, x2, y2, x1, y2, x1, y1 + (width * 0.5), shadow);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x1, y1 + (width * 0.5), x1, y2); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x2, y1, x2, y2); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
  }
  else if (!last_HR && !last_VU)
  {
    // case last was to the left, next is down
    fprintf(fp, 
    "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\" %s/>\n",
          x, y1, x1, y1, x1, y2, x2, y2, x2, y1 + (width * 0.5), shadow);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x1, y1, x1, y2); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x2, y1 + (width * 0.5), x2, y2); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
  }
}

/*******************************************/

void gen_h_pipe(FILE *fp, double x1, double y, double x2, double width)
{
  double y1 = y - (width / 2.0);
  double y2 = y + (width / 2.0);

  last_V = false;
  if(x1 > x2)
  {
    double tmp;
    tmp = x1;
    x1 = x2;
    x2 = tmp; 
    last_HR = false; 
  } 
  else
  {
    last_HR = true; 
  }
  if (!silent) printf("H - x1: %lg, y1: %lg, x2: %lg, y2: %lg\n", x1, y1, x2, y2);

  if (first)
  {
    first = false;
    fprintf(fp, 
     "<rect x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\" %s/>\n",
          x1, y - (width / 2.0), x2-x1, width, shadow);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x1, y1, x2, y1); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x1, y2, x2, y2); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
  }
  else if (last_HR && last_VU)
  {
    // case last was up, next is right 
    fprintf(fp, 
   "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\" %s/>\n",
          x1, y1, x2 , y1, x2, y2, x1 + (width * 0.5), y2, x1, y, shadow);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x1, y1, x2, y1); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x1 + (width * 0.5), y2, x2, y2); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
  }
  else if (!last_HR && last_VU)
  {
    // case last was up, next is left 
    fprintf(fp, 
   "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\" %s/>\n",
          x2, y1, x1 , y1, x1, y2, x2 - (width * 0.5), y2, x2, y, shadow);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x1, y1, x2, y1); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x1, y2, x2 - (width * 0.5), y2); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
  }
  else if (last_HR && !last_VU)
  {
    // case last was down, next is right 
    fprintf(fp, 
    "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\" %s/>\n",
          x1, y2, x2 , y2, x2, y1, x1 + (width * 0.5), y1, x1, y, shadow);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x1 + (width * 0.5), y1, x2, y1); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x1, y2, x2, y2); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
  }
  else if (!last_HR && !last_VU)
  {
    // case last was down, next is left 
    fprintf(fp, 
    "<polygon points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\" %s/>\n",
          x2, y2, x1 , y2, x1, y1, x2 - (width * 0.5), y1, x2, y, shadow);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x1, y1, x2 - (width * 0.5), y1); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
    fprintf(fp, "<line x1=\"%lg\" y1=\"%lg\"  x2=\"%lg\" y2=\"%lg\" ",
            x1, y2, x2, y2); 
    fprintf(fp, "stroke-width=\"%lg\" stroke=\"%s\"/>\n",
         stroke_width, stroke_color);
  }
}

/*******************************************/

static int n_instance = 1;

class pipe2d_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

/*******************************************/

extern "C" gen_plugin_base_t *get_object(void)
{
  return new pipe2d_t;
}

/*******************************************/

const char *pipe2d_t::get_name(void)
{
  return "pipe2d";
}

/*******************************************/

void pipe2d_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("pipe2d", " A simple pipe with no color gradient for 3d look, but, you can animate the color");
  dob->param("Discrete Tag (Can be null or blank if you do not want animation of color)");
  dob->param("On Color");
  dob->param("Off Color");
  dob->param("Width of the pipe");
  dob->param("X of the starting point of the pipe");
  dob->param("Y of the starting point of the pipe");
  dob->param("Direction that must be 'V' or 'H'");
  dob->param("Stopping point");
  dob->param("Now, Repeat 7 and 8 as many times as needed to complete the pipe");
  dob->example("pipe2d|PUMP2_ON|orange|grey|2|134|88|h|150|v|109|");
  dob->notes("You must alternate V and H, you can not have two Vs or two Hs in a row");
  dob->notes("Has a slight shadow below and to the right for a subtle effect");
  dob->end();
}



void pipe2d_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc < 9)
  {
    printf("%s, line %d: There must be AT LEAST 9 arguments to pipe2d\n",
           d.file_name, d.line_number);
    exit(-1);
  }
  if ((argc % 2) != 1)
  {
    printf("%s, line %d: There must be an odd number of arguments to pipe2d\n",
           d.file_name, d.line_number);
    exit(-1);
  }
  silent = d.silent;

  const char *the_tag = argv[1];
  const char *on_color = argv[2];
  const char *off_color = argv[3];
  double width = atof(argv[4]);
  double x1 = atof(argv[5]);
  double y1 = atof(argv[6]);
  double x2;
  double y2;
  double butt_width;
  stroke_width = 0.1 * width;

  if (!silent) printf("Pipe color is: %s\n", on_color);
  if (!silent) printf("Tag is: %s\n", the_tag);
  first = true;

  fprintf(d.svg_fp, "<!-- START insert for pipe2d (%03d) -->\n", n_instance);
/***/
  char js_object_name[30];
  char js_group_name[30];
  snprintf(js_object_name, sizeof(js_object_name), "pipe2d_obj_%03d", n_instance);
  snprintf(js_group_name, sizeof(js_group_name), "pipe2d_group_obj_%03d", n_instance);
  fprintf(d.svg_fp, "<g id=\"%s\" fill=\"%s\" stroke=\"none\">\n",
       js_group_name, on_color);
/***/
  char last = ' ';
  for (int i=7; i < (argc-1); i+=2)
  {
    switch (argv[i][0])
    {
      case 'h':
      case 'H':
        if ((!first) && (last != 'V'))
        {
          printf("%s, line %d: you must alternate horizontal and vertical\n",
             d.file_name, d.line_number);
          exit(-1);
        }
        last = 'H';
        x2 = atof(argv[i+1]);
        if (!silent) printf("Making Horizontal section to %lg\n", x2);
        y2 = y1;
        if (((i+2) < (argc-1)) && (argv[i+2][0] != 'b') && (argv[i+2][0] != 'B'))
        {
          fprintf(d.svg_fp,  
         "<circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" \n",
              x2, y2, width / 2.0);
          fprintf(d.svg_fp, "stroke-width=\"%lg\" stroke=\"%s\" %s/>\n",
             stroke_width, stroke_color, shadow);
        }
        gen_h_pipe(d.svg_fp, x1, y1,  x2, width);
        break;
      case 'v':
      case 'V':
        if ((!first) && (last != 'H'))
        {
          printf("%s, line %d: you must alternate horizontal and vertical\n",
             d.file_name, d.line_number);
          exit(-1);
        }
        last = 'V';
        y2 = atof(argv[i+1]);
        if (!silent) printf("Making Vertical section to %lg\n", y2);
        x2 = x1;
        if (((i+2) < (argc-1)) && (argv[i+2][0] != 'b') && (argv[i+2][0] != 'B'))
        {
          fprintf(d.svg_fp,  
         "<circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" \n",
              x2, y2, width / 2.0);
          fprintf(d.svg_fp, "stroke-width=\"%lg\" stroke=\"%s\" %s/>\n",
               stroke_width, stroke_color, shadow);
        }
        gen_v_pipe(d.svg_fp, x1, y1, y2, width);
        break;
      case 'b':
      case 'B':
        if (!silent) printf("Making piece to butt against a pipe %lg, %lg\n", x2, y2);
        if (argc != (i + 2))
        {
          printf("%s, line %d: Butt end must be the last argument for pipe2d\n",
             d.file_name, d.line_number);
          exit(-1);
        }
        butt_width = atof(argv[i+1]);
        gen_butt_end(d.svg_fp, width, butt_width, x2, y2);
        break;
      default:
        printf("Error: %s, must be either 'h' or 'v' or 'b' for the direction\n", argv[i]);
        printf("%s, line %d: found %s, must be either 'h' or 'v' or 'b' for the direction\n", 
           d.file_name, d.line_number, argv[i]);
        exit(-1);
        break;
    } 
    x1 = x2;
    y1 = y2;
  }
  fprintf(d.svg_fp, "</g>\n");
  fprintf(d.js_fp, "var %s = new pump_t(\"%s\", \"%s\", \"%s\");\n",
                       js_object_name, js_group_name, on_color, off_color);


  fprintf(d.svg_fp, "<!--  END insert for pipe2d (%03d) -->\n", n_instance);
  if ((strlen(the_tag) > 0) && (0 != strcasecmp(the_tag, "null")))
  {
    add_js_library("pump.js");
    add_animation_object(the_tag, js_object_name);
  }
  add_svg_library("pipeshadow.svg");
  n_instance++;
}

/*******************************************/

