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


static const char *pipe_color = "gray";
static bool silent = false;

/*******************************************/

void gen_elbo_ru(FILE *fp, double x, double y, double pipe_width, double elbo_width)
{
  double small_width = elbo_width - pipe_width;
  fprintf(fp, "<path fill=\"url(#%sRadial)\" d=\"M%lg %lg a%lg %lg 0 0 0 %lg %lg h%lg a%lg,%lg 0 0 1 %lg %lg z\"/>\n",
          pipe_color,
          /* M */ x, y + (pipe_width/2), 
          /* a */  elbo_width, elbo_width, elbo_width, -elbo_width,
          /* h */  -pipe_width,
          /* a */  small_width, small_width, -small_width, small_width);
}

/*******************************************/

void gen_elbo_rd(FILE *fp, double x, double y, double pipe_width, double elbo_width)
{
  double small_width = elbo_width - pipe_width;
  fprintf(fp, "<path fill=\"url(#%sRadial)\" d=\"M%lg %lg a%lg %lg 0 0 1 %lg %lg h%lg a%lg %lg 0 0 0 %lg %lg z\"/>\n",
          pipe_color,
          /* M */ x, y - (pipe_width/2), 
          /* a */  elbo_width, elbo_width, elbo_width, elbo_width,
          /* h */  -pipe_width,
          /* a */  small_width, small_width, -small_width, -small_width);
}

/*******************************************/

void gen_elbo_lu(FILE *fp, double x, double y, double pipe_width, double elbo_width)
{
  double small_width = elbo_width - pipe_width;
  fprintf(fp, "<path fill=\"url(#%sRadial)\" d=\"M%lg %lg a%lg %lg 0 0 1 %lg %lg h%lg a%lg %lg 0 0 0 %lg %lg z\"/>\n",
          pipe_color,
          /* M */ x, y + (pipe_width/2), 
          /* a */  elbo_width, elbo_width, -elbo_width, -elbo_width,
          /* h */  pipe_width,
          /* a */  small_width, small_width, small_width, small_width);
}

/*******************************************/

void gen_elbo_ld(FILE *fp, double x, double y, double pipe_width, double elbo_width)
{
  double small_width = elbo_width - pipe_width;
  fprintf(fp, "<path fill=\"url(#%sRadial)\" d=\"M%lg %lg a%lg %lg 0 0,0 %lg %lg h%lg a%lg %lg 0 0 1 %lg %lg z\"/>\n",
          pipe_color,
          /* M */ x, y - (pipe_width/2), 
          /* a */  elbo_width, elbo_width, -elbo_width, elbo_width,
          /* h */  pipe_width,
          /* a */  small_width, small_width, small_width, -small_width);
}
  
/*******************************************/
static bool last_HR = true;
static bool last_VU = true;
static bool last_V = true;
static bool first = true;

void gen_butt_end(FILE *fp, double width, double butt_width, double x2, double y2)
{
  if (!silent) printf("butt end - width: %gf, bwidth: %lg, x2: %lg, y2: %lg\n",
         width, butt_width, x2, y2);
  if (last_V && last_VU)
  {
    // draw the butt end up
    if (!silent) printf("butt up\n");
    fprintf(fp, 
    "<polygon fill=\"url(#%sLinearV)\" points=\"%lg,%lg %lg,%lg %lg,%lg\"/>\n",
          pipe_color, x2 - (width/2.0), y2, x2 + (width/2.0), y2, x2, y2 - (butt_width/2.0));
  }
  else if (last_V && !last_VU)
  {
    // draw the butt end down 
    if (!silent) printf("butt down\n");
    fprintf(fp, 
    "<polygon fill=\"url(#%sLinearV)\" points=\"%lg,%lg %lg,%lg %lg,%lg\"/>\n",
          pipe_color, x2 - (width/2.0), y2, x2 + (width/2.0), y2, x2, y2 + (butt_width/2.0));
  }
  else if (!last_V && last_HR)
  {
    // draw the butt end right 
    if (!silent) printf("butt right\n");
    fprintf(fp, 
    "<polygon fill=\"url(#%sLinearH)\" points=\"%lg,%lg %lg,%lg %lg,%lg\"/>\n",
          pipe_color, x2, y2 - (width/2.0), x2, y2 + (width/2.0), x2 + (butt_width/2.0), y2);
  }
  else if (!last_V && !last_HR)
  {
    // draw the butt end left 
    if (!silent) printf("butt left\n");
    fprintf(fp, 
    "<polygon fill=\"url(#%sLinearH)\" points=\"%lg,%lg %lg,%lg %lg,%lg\"/>\n",
          pipe_color, x2, y2 - (width/2.0), x2, y2 + (width/2.0), x2 - (butt_width/2.0), y2);
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
     "<rect fill=\"url(#%sLinearV)\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\"/>\n",
          pipe_color, x - (width/2.0), y1, width, y2-y1);
  }
  else if (last_HR && last_VU)
  {
    // case last was to the right, next is up 
    fprintf(fp, 
    "<polygon fill=\"url(#%sLinearV)\" points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\"/>\n",
          pipe_color, x, y2, x2, y2, x2, y1, x1, y1, x1, y2 - (width * 0.5));
  }
  else if (!last_HR && last_VU)
  {
    // case last was to the left, next is up 
    fprintf(fp, 
    "<polygon fill=\"url(#%sLinearV)\" points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\"/>\n",
          pipe_color, x, y2, x1, y2, x1, y1, x2, y1, x2, y2 - (width * 0.5));
  }
  else if (last_HR && !last_VU)
  {
    // case last was to the right, next is down
    fprintf(fp, 
    "<polygon fill=\"url(#%sLinearV)\" points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\"/>\n",
          pipe_color, x, y1, x2, y1, x2, y2, x1, y2, x1, y1 + (width * 0.5));
  }
  else if (!last_HR && !last_VU)
  {
    // case last was to the left, next is down
    fprintf(fp, 
    "<polygon fill=\"url(#%sLinearV)\" points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\"/>\n",
          pipe_color, x, y1, x1, y1, x1, y2, x2, y2, x2, y1 + (width * 0.5));
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
     "<rect fill=\"url(#%sLinearH)\" x=\"%lg\" y=\"%lg\" width=\"%lg\" height=\"%lg\"/>\n",
          pipe_color, x1, y - (width / 2.0), x2-x1, width);
  }
  else if (last_HR && last_VU)
  {
    // case last was up, next is right 
    fprintf(fp, 
   "<polygon fill=\"url(#%sLinearH)\" points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\"/>\n",
          pipe_color, x1, y1, x2 , y1, x2, y2, x1 + (width * 0.5), y2, x1, y);
  }
  else if (!last_HR && last_VU)
  {
    // case last was up, next is left 
    fprintf(fp, 
   "<polygon fill=\"url(#%sLinearH)\" points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\"/>\n",
          pipe_color, x2, y1, x1 , y1, x1, y2, x2 - (width * 0.5), y2, x2, y);
  }
  else if (last_HR && !last_VU)
  {
    // case last was down, next is right 
    fprintf(fp, 
    "<polygon fill=\"url(#%sLinearH)\" points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\"/>\n",
          pipe_color, x1, y2, x2 , y2, x2, y1, x1 + (width * 0.5), y1, x1, y);
  }
  else if (!last_HR && !last_VU)
  {
    // case last was down, next is left 
    fprintf(fp, 
    "<polygon fill=\"url(#%sLinearH)\" points=\"%lg,%lg %lg,%lg %lg,%lg %lg,%lg %lg,%lg\"/>\n",
          pipe_color, x2, y2, x1 , y2, x1, y1, x2 - (width * 0.5), y1, x2, y);
  }
}

/*******************************************/

static int n_instance = 1;

class simple_pipe_t : public gen_plugin_base_t
{
public:
  const char *get_name(void); 
  void generate_doc(doc_object_base_t *dob);
  void generate(plugin_data_t d, int argc, char **argv);
};

/*******************************************/

extern "C" gen_plugin_base_t *get_object(void)
{
  return new simple_pipe_t;
}

/*******************************************/

const char *simple_pipe_t::get_name(void)
{
  return "simple_pipe";
}

/*******************************************/

void simple_pipe_t::generate_doc(doc_object_base_t *dob)
{
  dob->start("simple_pipe", "Simple pipe with color gradients to give 3d look.");
  dob->param("Color of the pipe");
  dob->param("Width of the pipe");
  dob->param("X of the starting point of the pipe");
  dob->param("Y of the starting point of the pipe");
  dob->param("Direction that must be 'V' or 'H'");
  dob->param("Stopping point");
  dob->param("Now, Repeat 5 and 6 as many times as needed to complete the pipe");
  dob->example("simple_pipe|gray|3|90|126|v|112|h|75|v|13.5|h|70|");
  dob->notes("You must alternate V and H, you can not have two Vs or two Hs in a row");
  dob->notes("You can not animate the color of this pipe right now. It is more complicated "
             "because there are three colors used: radial color gradient, vertical color "
             "gradient, and horzontal color gradient, So, you would have to set up a list "
             "of objects and then walk down the list changing the colors one at a time. "
             "For now, if you need to animate the color of the pipe, use pipe1d. "
             "With pipe1d, it was trivial to animate the color, since you can put all of the "
             "segments in a group, then you just change the color of the group. "
             "Sooner or later, I am sure somebody will make it work with 3d pipes, "
             "it is just very tricky, and I am tackling other things first. "
);
  dob->end();
}

void simple_pipe_t::generate(plugin_data_t d, int argc, char **argv)
{
  if (argc < 7)
  {
    printf("%s, line %d: There must be AT LEAST 7 arguments to simple_pipe\n",
           d.file_name, d.line_number);
    exit(-1);
  }
  if ((argc % 2) != 1)
  {
    printf("%s, line %d: There must be an odd number of arguments to simple_pipe\n",
           d.file_name, d.line_number);
    exit(-1);
  }
  silent = d.silent;

  const char *the_color = argv[1];
  double width = atof(argv[2]);
  double x1 = atof(argv[3]);
  double y1 = atof(argv[4]);
  double x2;
  double y2;
  double butt_width;

  if (0 == strcasecmp(the_color, "yellow")) 
  {
    pipe_color = "yellow";
    add_svg_library("yellow_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "black")) 
  {
    pipe_color = "black";
    add_svg_library("black_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "orange")) 
  {
    pipe_color = "orange";
    add_svg_library("orange_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "brown")) 
  {
    pipe_color = "brown";
    add_svg_library("brown_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "white")) 
  {
    pipe_color = "white";
    add_svg_library("white_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "red")) 
  {
    pipe_color = "red";
    add_svg_library("red_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "blue")) 
  {
    pipe_color = "blue";
    add_svg_library("blue_gradients.svg");
  }
  else if (0 == strcasecmp(the_color, "green")) 
  {
    pipe_color = "green";
    add_svg_library("green_gradients.svg");
  }
  else // Anything else, use gray
  {
    pipe_color = "gray";
    add_svg_library("gray_gradients.svg");
  }

  if (!silent) printf("Pipe color is: %s\n", pipe_color);
  first = true;

  fprintf(d.svg_fp, "<!-- START insert for simple_pipe (%03d) -->\n", n_instance);
  char last = ' ';

  for (int i=5; i < (argc-1); i+=2)
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
         "<circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" fill=\"url(#%sRadial)\"/>\n",
              x2, y2, width / 2.0, pipe_color);
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
         "<circle cx=\"%lg\" cy=\"%lg\" r=\"%lg\" fill=\"url(#%sRadial)\"/>\n",
              x2, y2, width / 2.0, pipe_color);
        }
        gen_v_pipe(d.svg_fp, x1, y1, y2, width);
        break;
      case 'b':
      case 'B':
        if (!silent) printf("Making piece to butt against a pipe %lg, %lg\n", x2, y2);
        if (argc != (i + 2))
        {
          printf("%s, line %d: Butt end must be the last argument for simple_pipe\n",
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

  fprintf(d.svg_fp, "<!--  END insert for simple_pipe (%03d) -->\n", n_instance);
  n_instance++;
}

/*******************************************/

