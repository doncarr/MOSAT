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
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#include <time.h>

#include "gen_display.h"
#include "react_svg.h"

static gen_plugin_base_t *get_plugin(const char *type);
static const int RT_SIM_MODE = 0;
static const int RT_REACT_MODE = 1;
static int run_mode = RT_SIM_MODE;
static bool popup_on = false;
static bool analog_popup_on = false;
//static bool analog_popup_on = true;
static bool silent = false;
static const char *sim_file="sim_pump.js";


/*********************************/

static const int MAX_JSOBJS = 200;
static const int MAX_TAGS = MAX_JSOBJS;
static const int MAX_JSLIBS = 200;
static const int MAX_SVGLIBS = 200;
static const int MAX_PLUGINS = 200;
static const int MAX_ARG_OBJS = 50;
int n_plugins = 0;
gen_plugin_base_t *the_plugins[MAX_PLUGINS];
static int n_objs = 0;
static const char *js_objs[MAX_JSOBJS];
static const char *tags[MAX_TAGS];
static int n_js_libs = 0;
static const char *js_lib_files[MAX_JSLIBS];
static int n_svg_libs = 0;
static const char *svg_lib_files[MAX_SVGLIBS];
const static int sim_interval = 100;
const static int react_interval = 200;

struct obj_list_t 
{
  const char *obj;
  obj_list_t *next;
};

static int max_argn = -1;
static obj_list_t *arg_objs[MAX_ARG_OBJS] = {
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}; 
static int arg_count[MAX_ARG_OBJS];

/*********************************/
void find_a_place_nearby(double *x,double *y, 
                         double item_x, double item_y, 
                         double item_width, double item_height)
{
  find_a_place_nearby(x, y, 
                      300, 150, 
                      item_x, item_y, 
                      item_width, item_height,
                      60, 30); 
}

void find_a_place_nearby(double *x,double *y, 
                         double screen_width, double screen_height, 
                         double item_x, double item_y, 
                         double item_width, double item_height,
                         double popup_width, double popup_height) 
{
  // First try below
  if ((item_y + item_height + popup_height) <= screen_height) 
  {
    *y = item_y + item_height; // put it just below the item
  }
  // Now try above
  else if ((item_y - item_height - popup_height) >= 0.0) 
  {
    *y = item_y - popup_height; // put it just above the item
  }
  // Ok, if above the mid point, put it as low as possible 
  else if ((item_y + item_height) < (screen_height / 2.0))
  {
    *y = screen_height - popup_height; // put it at the bottom of the screen 
  }
  // Otherwise put it all the way at the top 
  else
  {
    *y = 0.0; // put it at the top of the screen
  }
  double x1 = (item_x + (item_width / 2.0) - (popup_width / 2.0)); 
  double x2 = (item_x + (item_width / 2.0) + (popup_width / 2.0)); 
  // first try centered with the object
  if ((x1 >= 0.0) && (x2 <= screen_width))
  {
    *x = x1; // centered under the item 
    return;
  }
  // If off the screen left, just put it all the way left 
  else if (x1 < 0.0)
  {
     *x = 0.0; // as far left as possible
     return;
  }
  // Ok, it had to be off the screen right, put it all the way right 
  else
  {
    *x = screen_width - popup_width; // as far right as possible
  }
}

/***************************************************************/

void add_arg_object(int argn, const char *obj)
{
  if (argn >= MAX_ARG_OBJS)
  {
    printf("Max args exceeded (%d), exiting . . . \n", argn);
    exit(-1);
  }
  if (argn < 1)
  {
    printf("Args can NOT be less than 1, (%d), exiting . . . \n", argn);
    exit(-1);
  }
  argn--; // Java script arrays start with zero
  arg_count[argn]++;
  if (not silent)
  {
    printf("Adding, object: %s, to be updated by the tag at position %d\n", obj, argn);
  }
  obj_list_t *tmp = new obj_list_t;
  tmp->next = arg_objs[argn];
  tmp->obj = strdup(obj);
  arg_objs[argn] = tmp;
  if (argn > max_argn) max_argn = argn;
}

/*********************************/

void add_js_library(const char *file_name)
{
  for (int i=0; i < n_js_libs; i++)
  {
    if (0 == strcmp(js_lib_files[i], file_name))
    {
      return;
    }
  }
  if (n_js_libs >= MAX_JSLIBS)
  {
    printf("Max javascript libraries exceeded, exiting . . . \n");
    exit(-1);
  }
  js_lib_files[n_js_libs] = strdup(file_name);
  n_js_libs++;
}

/*********************************/

void add_svg_library(const char *file_name)
{
  for (int i=0; i < n_svg_libs; i++)
  {
    if (0 == strcmp(svg_lib_files[i], file_name))
    {
      return;
    }
  }
  if (n_svg_libs >= MAX_SVGLIBS)
  {
    printf("Max SVG libraries exceeded, exiting . . . \n");
    exit(-1);
  }
  svg_lib_files[n_svg_libs] = strdup(file_name);
  n_svg_libs++;
}


/*********************************/

void add_animation_object(const char *the_tag, const char *the_js_object)
{
  // First check to see if the tagname will come from the command line.
  if (the_tag[0] == '$')
  {
    add_arg_object(atoi(the_tag+1), the_js_object);
    return;
  }
  // Now make sure we have not exceeded the MAX.
  if (n_objs >= MAX_JSOBJS)
  {
    printf("Max objects exceeded, exiting . . . \n");
    exit(-1);
  }
  //printf("Adding, tag: %s, object: %s\n", the_tag, the_js_object);
  tags[n_objs] = strdup(the_tag);  
  js_objs[n_objs] = strdup(the_js_object);
  n_objs++;
}

/*********************************/

static void gen_simulation(FILE *js_fp)
{
  fprintf(js_fp, "// -- START insert simulation code --\n");

  if (max_argn != -1)
  {
    // Only generate the array of arrays of objects if there is at least one argument. 
    fprintf(js_fp, "var arg_objs = new Array(");
    for (int i=0; i <= max_argn; i++)
    {
      fprintf(js_fp, "%s\n    [", i==0?"":",");
      obj_list_t *tmp = arg_objs[i]; 
      for (int j=0; tmp != NULL; j++)
      {
        fprintf(js_fp, "%s%s", j==0?"":",", tmp->obj);
        tmp = tmp->next;
      }
      fprintf(js_fp, "]"); 
    }
    fprintf(js_fp, 
           ");\n"
       "var arg_tags=null;\n"
       "var n_arg_cfg = 0;\n"
      ); 
  }

/***
  fprintf(js_fp, 
       "function convert_tag(tag)\n"
       "{\n"
       "  if ('$' == tag.charAt(0))\n"
       "  {\n"
       "    if (typeof arg_tags != 'undefined') return tag;\n"
       //"    if (arg_tags == null) return tag;\n"
       "    var argn = parseInt(tag.substring(1)) - 1;\n"
       "    return arg_tags[argn];\n"
       "  }\n"
       "  return tag;\n"
       "}\n"
      ); 
***/


  fprintf(js_fp, "const update_objs = new Array(");
  for (int i=0; i < n_objs; i++)
  {
    fprintf(js_fp, "%s%s", i==0?"":",", js_objs[i]);
  }
  fprintf(js_fp, ");\n"); 

  fprintf(js_fp, "const update_tags = new Array(");
  for (int i=0; i < n_objs; i++)
  {
    fprintf(js_fp, "%s\"%s\"", i==0?"":",", tags[i]);
  }
  fprintf(js_fp, ");\n"); 

  fprintf(js_fp, 
       "function update_objects()\n"
       "{\n"
       "  for (var i=0; i < update_objs.length; i++)\n"
       "  {\n"
       "    update_objs[i].update(sim.get_pv(update_tags[i]));\n"
       "  }\n"
    );

  if (max_argn != -1)
  {
    fprintf(js_fp, 
       "  for (var i=0; (i < arg_objs.length); i++)\n"
       "  {\n"
       "    var aobjs = arg_objs[i]\n"
       "    var one_val = sim.get_pv(arg_tags[i])\n"
       "    for (var j=0; j < aobjs.length; j++)\n"
       "    {\n"
       "      aobjs[j].update(one_val);\n"
       "    }\n"
       "  }\n"
      );
  }

  fprintf(js_fp, "}\n");
  fprintf(js_fp, "\n");

  fprintf(js_fp, 
       "function send_output(tag, new_value)\n"
       "{\n"
       "  console.log(\"send_output, tag: \" + tag + \": \" + new_value);\n"
       "  tag = convert_tag(tag);\n"
       "  sim.set_pv(tag, new_value);\n"
       //"  if ('$' == tag.charAt(0))\n"
       //"  {\n"
       //"    // only convert if arg_tags have been set\n"
       //"    if (typeof arg_tags == 'undefined') return\n"
       //"    if (arg_tags == null) return;\n"
       //"    var argn = parseInt(tag.substring(1)) - 1;\n"
       //"    sim.set_pv(arg_tags[argn], new_value);\n"
       //"  }\n"
       //"  else\n"
       //"  {\n"
       //"    sim.set_pv(tag, new_value);\n"
       //"  }\n"
       "}\n"
      "\n"
    );

  fprintf(js_fp, 
       "function get_config(the_tag)\n"
       "{\n"
       "  console.log('get_config, tag: ' + the_tag);\n"
       "  if ('$' == the_tag.charAt(0))\n"
       "  {\n"
       "    var argn = parseInt(the_tag.substring(1)) - 1;\n"
       "    return sim.get_cfg(arg_tags[argn]);\n"
       "  }\n"
       "  else\n"
       "  {\n"
       "    var the_cfg = sim.get_cfg(the_tag);\n"
       "    console.log('get_config, cfg: ' + the_cfg);\n"
       "    return the_cfg;\n" 
       "  }\n"
       "}\n"
       "\n"
    );

  fprintf(js_fp, 
     "function config_objects()\n"
     "{\n"
     "  var i;\n"
     "  for (i=0; i < update_objs.length; i++)\n"
     "  {\n"
     "    console.log(\"config tag: \" + update_tags[i]);\n"
     "    update_objs[i].init(sim.get_cfg(update_tags[i]));\n"
     "  }\n"
   );

  if (max_argn != -1)
  {
    fprintf(js_fp, 
      "  for (i=0; i < arg_objs.length; i++)\n"
      "  {\n"
      "    var aobjs = arg_objs[i]\n"
      "    for (var j=0; j < aobjs.length; j++)\n"
      "    {\n"
      "      console.log(\"arg tag: \" + arg_tags[i]);\n"
      "      aobjs[j].init(sim.get_cfg(arg_tags[i]));\n"
      "    }\n"
      "  }\n"
     );
  }

  fprintf(js_fp, "}\n");
  fprintf(js_fp, "\n");

  fprintf(js_fp, 
    "var sim_now=0.0;\n"
    "function intervalHandler()\n"
    "{\n"
    "  sim_now+=0.1\n"
    "  sim.update(sim_now);\n"
    "  update_objects();\n"
    "}\n"
    "\n"
  );

  fprintf(js_fp, "function load()\n");
  fprintf(js_fp, "{\n");
  if (max_argn != -1)
  {
    fprintf(js_fp, 
      "  var argtmp = location.search.substr(1);\n"
      "  arg_tags = argtmp.split('+');\n"
      "  for (var i=0; i < arg_tags.length; i++)\n"
      "  {\n"
      "    console.log(\"arg_tags[\" + i +  \"]: \" + arg_tags[i])\n"
      "  }\n"
      "  for (var i=0; (i < arg_objs.length); i++)\n"
      "  {\n"
      "    var aobjs = arg_objs[i]\n"
      "    for (var j=0; j < aobjs.length; j++)\n"
      "    {\n"
      "      console.log(\"arg objs[\" + i +\"][\" + j + \"]: \" + aobjs[j]);\n"
      "    }\n"
      "  }\n"
    );
  }
  fprintf(js_fp, "  config_objects();\n");
  fprintf(js_fp, "  update_objects();\n");
  //fprintf(js_fp, "  pump_1_timeout();\n");
  //fprintf(js_fp, "  pump_2_timeout();\n");
  //fprintf(js_fp, "  pump_3_timeout();\n");
  //fprintf(js_fp, "  pump_1_timeout();\n");
  //fprintf(js_fp, "  pump_2_timeout();\n");
  //fprintf(js_fp, "  pump_3_timeout();\n");

  fprintf(js_fp, 
      "  var interval = setInterval(\"intervalHandler()\", %d);\n",   
      sim_interval);

  if (popup_on)
  {
    fprintf(js_fp, "  show_main();\n");
    //fprintf(js_fp, "  show_popup(50,50, 'On', 'Off');\n");
  }

  fprintf(js_fp, "};\n");
  fprintf(js_fp, "\n");
  fprintf(js_fp, "// -- END insert simulation code --\n");
}

/*********************************/

static void gen_ajax_animation(FILE *js_fp)
{
  fprintf(js_fp, "// -- START insert AJAX animation code --\n");
  if (n_objs > 0)
  {
    fprintf(js_fp, 
    //"const react_update_hrf = \"http://\" + location.hostname + \"/react/tag?");
    "const react_update_hrf = \"http://\" + location.host + \"/react/tag?");
    for (int i=0; i < n_objs; i++)
    {
      fprintf(js_fp, "%s%s", i==0?"":"+", tags[i]);
    }
    fprintf(js_fp, "\";\n"); 

    fprintf(js_fp, "const update_objs = new Array(");
    for (int i=0; i < n_objs; i++)
    {
      fprintf(js_fp, "%s%s", i==0?"":",", js_objs[i]);
    }
    fprintf(js_fp, ");\n"); 

    fprintf(js_fp, "const update_tags = new Array(");
    for (int i=0; i < n_objs; i++)
    {
      fprintf(js_fp, "%s\"%s\"", i==0?"":",", tags[i]);
    }
    fprintf(js_fp, ");\n"); 
  }

  if (max_argn != -1)
  {
    // Only generate the array of arrays of objects if there is at least one argument. 
    fprintf(js_fp, "var arg_objs = new Array(");
    for (int i=0; i <= max_argn; i++)
    {
      fprintf(js_fp, "%s\n    [", i==0?"":",");
      obj_list_t *tmp = arg_objs[i]; 
      for (int j=0; tmp != NULL; j++)
      {
        fprintf(js_fp, "%s%s", j==0?"":",", tmp->obj);
        tmp = tmp->next;
      }
      fprintf(js_fp, "]"); 
    }
    fprintf(js_fp, ");\n"); 
    fprintf(js_fp, 
      //"arg_update_hrf = \"http://\" + location.hostname + \"/react/tag\" + location.search;\n"
      "arg_update_hrf = \"http://\" + location.host + \"/react/tag\" + location.search;\n"
      //"arg_config_hrf = \"http://\" + location.hostname + \"/react/config?\";\n"
      "arg_config_hrf = \"http://\" + location.host + \"/react/config?\";\n"
      "var arg_update_xReq;\n"
      "var arg_config_xReq;\n"
      "var arg_tags=null;\n"
      "var n_arg_cfg = 0;\n"
    );
  }


  if (n_objs > 0)
  {
    fprintf(js_fp, 
     //"var react_config_hrf = \"http://\" + location.hostname + \"/react/config?\";\n");
     "var react_config_hrf = \"http://\" + location.host + \"/react/config?\";\n");
    fprintf(js_fp, "var update_xReq;\n");
    fprintf(js_fp, "var config_xReq;\n");
    fprintf(js_fp, "var n_cfg = 0;\n");

    fprintf(js_fp, 
      "function send_output(the_tag, val)\n"
      "{\n"
      "  var tag;\n"
      "  tag = convert_tag(the_tag);\n"
      //"  if ('$' == the_tag.charAt(0))\n"
      //"  {\n"
      //"    // only convert if arg_tags have been set\n"
      //"    if (arg_tags == null) return;\n"
      //"    var argn = parseInt(the_tag.substring(1)) - 1;\n"
      //"   tag = arg_tags[argn];\n"
      //"  }\n"
      //"  else\n"
      //"  {\n"
      //"    tag = the_tag;\n"
      //"  }\n"

      "  var path = \"react/output?\" + tag + \"+\" + val;\n"
      "  console.log(\"send_output(\" + tag + \", \" + val + \")\");\n"
      "  console.log(path);\n"
      "  var post_request;\n"
      "  post_request = new XMLHttpRequest();\n"
    //  "  post_request.open(\"POST\",\"react/send\",true);\n"
      "  post_request.open(\"POST\",path,true);\n"
      "  post_request.setRequestHeader(\"Content-type\",\n"
      "           \"application/x-www-form-urlencoded\");\n"
      "  post_request.send();\n"
      "}\n"
    );

    fprintf(js_fp, 
      "var config_vals = new Array();\n"
      "function save_config(the_tag, val)\n"
      "{\n"
      "  console.log('save_config, tag: ' + the_tag);\n"
      "  config_vals[the_tag.toUpperCase()] = val;\n"
      "}\n"
      "function get_config(the_tag)\n"
      "{\n"
      "  console.log('get_config, tag: ' + the_tag);\n"
      "  if ('$' == the_tag.charAt(0))\n"
      "  {\n"
      "    var argn = parseInt(the_tag.substring(1)) - 1;\n"
      "    return config_vals[arg_tags[argn].toUpperCase()];\n"
      "  }\n"
      "  else\n"
      "  {\n"
      "    return config_vals[the_tag.toUpperCase()];\n"
      "  }\n"
      "}\n"
     );

    fprintf(js_fp, 
      "function on_config_response()\n"
      "{\n"
      "  if (config_xReq.readyState != 4)  { return; }\n"
      "  console.log(\"config response: \" + config_xReq.responseText);\n"
      "  var val = JSON.parse(config_xReq.responseText);\n"
      "  update_objs[n_cfg].init(val);\n"
      "  save_config(update_tags[n_cfg], val);\n"
      "  \n"
      "  n_cfg++;\n"
      "  if (n_cfg >= update_tags.length) \n"
      "  {\n"
      "    var interval = setInterval(\"intervalHandler()\", %d);\n"
      "    config_xReq.abort();\n"
      "    return;\n"
      "  }\n"
      "  console.log(\"config tag: \" + update_tags[n_cfg])\n"
      "  config_xReq.open(\"GET\", react_config_hrf + update_tags[n_cfg], true);\n"
      "  config_xReq.send(null);\n"
      "}\n", react_interval
    );
  }

  if (max_argn != -1)
  {
    fprintf(js_fp, 
      "function on_arg_config_response()\n"
      "{\n"
      "  if (arg_config_xReq.readyState != 4)  { return; }\n"
      "  console.log(\"arg config response: \" + arg_config_xReq.responseText);\n"
      "  var val = JSON.parse(arg_config_xReq.responseText);\n"
      "    var aobjs = arg_objs[n_arg_cfg]\n"
      "    for (var j=0; j < aobjs.length; j++)\n"
      "    {\n"
      "      aobjs[j].init(val);\n"
      "    }\n"
      "    \n"
      "  n_arg_cfg++;\n"
      "  if (n_arg_cfg >= arg_tags.length) \n"
      "  {\n"
      "    arg_config_xReq.abort();\n"
      "    return;\n"
      "  }\n"
      "  arg_config_xReq.open(\"GET\", arg_config_hrf + arg_tags[n_arg_cfg], true);\n"
      "  arg_config_xReq.send(null);\n"
      "}\n"
     );

    fprintf(js_fp, 
      "function update_arg_objects(objs, vals)\n"
      "{\n"
      "  for (var i=0; (i < vals.length) && (i < objs.length); i++)\n"
      "  {\n"
      "    var aobjs = arg_objs[i]\n"
      "    for (var j=0; j < aobjs.length; j++)\n"
      "    {\n"
      "      aobjs[j].update(vals[i]);\n"
      "    }\n"
      "  }\n"
      "}\n"
      "function on_arg_update_response()\n"
      "{\n"
      "  if (arg_update_xReq.readyState != 4)  { return; }\n"
      "  var val = JSON.parse(arg_update_xReq.responseText);\n"
      "  update_arg_objects(arg_objs, val);\n"
      "  arg_update_xReq.abort(); // set the ready state back to 0\n"
      "}\n"
    );
  }
  

  if (n_objs > 0)
  {
    fprintf(js_fp, 
    //  "var sim_val = 20;\n"
      "function on_update_response()\n"
      "{\n"
      "  if (update_xReq.readyState != 4)  { return; }\n"
      "  var val = JSON.parse(update_xReq.responseText);\n"
      "  //console.log(\"response: \" + update_xReq.responseText);\n"
      "  for (var i=0; i < update_objs.length; i++)\n"
      "  {\n"
    //  "    //console.log(\"update_objs[\" + i + \"] = \" + update_objs[i] + \"sim_val: \" + sim_val);\n"
    //  "    //update_objs[i].update(sim_val + (i * 5));\n"
    //  "    //console.log(\"val: \" + val[i]);\n"
      "    update_objs[i].update(val[i]);\n"
    //  "    sim_val++;\n"
    //  "    if (sim_val==100)\n"
    //  "    {\n"
    //  "      sim_val=0\n"
    //  "    }\n"
           
      "  }\n"
      "  update_xReq.abort(); // set the ready state back to 0\n"
      "}\n"
      "\n"
    );
  }

  fprintf(js_fp, "function intervalHandler()\n");
  fprintf(js_fp, "{\n");
  if (n_objs > 0)
  {
    fprintf(js_fp, 
      "  if (update_xReq.readyState == 0)\n"
      "  {\n"
      "    update_xReq.open(\"GET\", react_update_hrf, true);\n"
      "    update_xReq.send(null);\n"
      "  }\n"
     );
  }
  if (max_argn != -1)
  {
    fprintf(js_fp, 
      "  if (arg_update_xReq.readyState == 0)\n"
      "  {\n"
      "    arg_update_xReq.open(\"GET\", arg_update_hrf, true);\n"
      "    arg_update_xReq.send(null);\n"
      "  }\n"
     );
  }
  fprintf(js_fp, "}\n");
  fprintf(js_fp, "\n");

  fprintf(js_fp, "function load()\n");
  fprintf(js_fp, "{\n");
  if (popup_on)
  {
    fprintf(js_fp, "  show_main();\n");
  }
  if (n_objs > 0)
  {
    fprintf(js_fp, 
      "  update_xReq = new XMLHttpRequest();\n"
      "  update_xReq.onreadystatechange = on_update_response;\n"
      "  if (update_tags.length > 0)\n"
      "  {\n"
      "    console.log(\"config tag: \" + update_tags[0])\n"
      "    config_xReq = new XMLHttpRequest();\n"
      "    config_xReq.onreadystatechange = on_config_response;\n"
      "    config_xReq.open(\"GET\", react_config_hrf + update_tags[0], true);\n"
      "    config_xReq.send(null);\n"
      "  }\n"
    );
  }
  if (max_argn != -1)
  {
    fprintf(js_fp, 
      "  arg_update_xReq = new XMLHttpRequest();\n"
      "  arg_update_xReq.onreadystatechange = on_arg_update_response;\n"
      "  console.log(\"arg_update_hrf: \" + arg_update_hrf)\n"
      "  var argtmp = location.search.substr(1);\n"
      "  arg_tags = argtmp.split('+');\n"
      "  for (var i=0; i < arg_tags.length; i++)\n"
      "  {\n"
      "    console.log(\"arg_tags[\" + i +  \"]: \" + arg_tags[i])\n"
      "  }\n"
      "  for (var i=0; (i < arg_objs.length); i++)\n"
      "  {\n"
      "    var aobjs = arg_objs[i]\n"
      "    for (var j=0; j < aobjs.length; j++)\n"
      "    {\n"
      "      console.log(\"arg objs[\" + i +\"][\" + j + \"]: \" + aobjs[j]);\n"
      "    }\n"
      "  }\n"
      "  if (arg_tags.length > 0)\n"
      "  {\n"
      "    arg_config_xReq = new XMLHttpRequest();\n"
      "    arg_config_xReq.onreadystatechange = on_arg_config_response;\n"
      "    arg_config_xReq.open(\"GET\", arg_config_hrf + arg_tags[0], true);\n"
      "    arg_config_xReq.send(null);\n"
      "  }\n"
     );
  }
  if (n_objs > 0)
  {
    fprintf(js_fp, "  console.log(\"react_update_hrf = \" + react_update_hrf);\n");
    fprintf(js_fp, "  console.log(\"react_config_hrf = \" + react_config_hrf);\n");
  }

  //fprintf(js_fp, "  var interval = setInterval(\"intervalHandler()\", 200);\n");
  fprintf(js_fp, "};\n");
  fprintf(js_fp, "\n");
  fprintf(js_fp, "// -- END insert AJAX animation code --\n");
}

/*********************************/

// These functions are included to limit dependencies.
/*********************************************************************/
void rtrim(char *str)
{
  /* Trim the space from the right of a string. */
  for (int i = (strlen(str) - 1);
        isspace(str[i]) && (i >= 0); str[i--] = '\0');
}
/*********************************************************************/
void ltrim(char *str)
{
  /* Trim the space from the left side of a string. */
  int i, j;

  for (i = 0; (str[i] != '\0') && isspace(str[i]); i++);
  for (j = 0; (str[i] != '\0'); str[j++] = str[i++]);
  str[j] = '\0';
}
/*********************************************************************/
int get_svg_args(char *line, char *argv[], char delimeter, int max_args)
{
  int i;
  char *p;

  if (strlen(line) == 0)
  {
    return 0; 
  }
  p = line + (strlen(line) - 1);
  // Here we trim white space off of the end of the string, but do not trim
  // delimeters that might also be white space, such as a tab. 
  // And, you need to stop at the start of the string!
  while ((isspace(*p)) && (*p != delimeter) && (p > line))
  {
    *p = '\0';
     p--;
  }
  p = line;
  int n_delim = 0;
  for (i=0; i < max_args; i++)
  {
    if (*p == '\0')
    {
      break;
    }
    argv[i] = p;
    for ( ; (*p != '\0') && (*p != delimeter); p++);
    if (*p == delimeter)
    {
      n_delim++;
      *(p++) = '\0';
    }
    else
    {
      continue;
    }
  }
  bool last_arg_ok = false;
  for (p = argv[i-1]; *p != '\0'; p++)
  {
    if (!isspace(*p))
    {
      last_arg_ok = true;
      break;
    }
  }
  // This is a little tricky. It is possible the last arg is blank, but
  // in that case, there must be a trailing delimeter. If there is just
  // white space after the last delimiter, then it is not considered
  // to be another argument.
  if (!last_arg_ok && (n_delim < i))
  {
    i--;
  }
  return i;
}

/*********************************/

static void do_gen(const char *fname)
{
  const char *svg_file = "_tmp_display.svg";
  FILE *svg_fp = fopen(svg_file, "w");
  if (svg_fp == NULL)
  {
    perror(svg_file);
    exit(-1);
  }

  const char *js_file = "_tmp_display.js";
  FILE *js_fp = fopen(js_file, "w");
  if (svg_fp == NULL)
  {
    perror(js_file);
    exit(-1);
  }

  const char *top_of_file = "_tmp_top_of_file.svg";
  FILE *svg_top_of_file_fp = fopen(top_of_file, "w");
  if (svg_top_of_file_fp == NULL)
  {
    perror(top_of_file);
    exit(-1);
  }

  plugin_data_t pdata;
  pdata.svg_fp = svg_fp; 
  pdata.js_fp = js_fp; 
  //pdata.svg_after_header_fp = 
  pdata.svg_top_of_file_fp = svg_top_of_file_fp;
  pdata.popup_on = popup_on;
  pdata.silent = silent;

  gen_plugin_base_t *obj;
  pdata.file_name = fname;
  char line[500];
  int argc;
  char *argv[50];
  int line_num;
  FILE *fp = fopen(fname, "r");
  if (fp == NULL)
  {
    perror(fname);
    printf("Can't open: %s\n", fname);
    exit(-1);
  }
  for (int i=0; NULL != fgets(line, sizeof(line), fp); i++)
  {
    line_num++;
    ltrim(line);
    rtrim(line);
    if (line[0] == '#')
    {
      continue;
    }
    if (strlen(line) == 0)
    {
      continue;
    }
    argc = get_svg_args(line, argv, '|', 50);
    if (argc == 0)
    {
      continue;
    }
    pdata.line_number = line_num;
    if (argc < 2)
    {
      printf("Wrong number of args, line %d\n", line_num);
      continue;
    }
    if (!silent) 
    {
      printf("%d: %s", line_num, argv[0]);
      for (int i=1; i < argc; i++)
      {
        printf(", %s", argv[i]);
      }
    }
    obj = get_plugin(argv[0]);    
    if (obj == NULL)
    {
      printf("No such object type: %s\n", argv[0]);
      exit(-1);
    }
    if (!silent) printf("    Generating %s . . . .\n", argv[0]);
    obj->generate(pdata, argc, argv);
  }

  if (run_mode == RT_REACT_MODE)
  {
    if (!silent) printf("Generating connection to REACT . . . \n");
    gen_ajax_animation(js_fp);
  }
  else
  {
    add_js_library(sim_file);
    if (!silent) printf("Generating simulator . . . \n");
    gen_simulation(js_fp);
  }

  fprintf(js_fp,
       "function convert_tag(tag)\n"
       "{\n"
       "  if ('$' == tag.charAt(0))\n"
       "  {\n"
       "    if (typeof arg_tags != 'undefined') return tag;\n"
       //"    if (arg_tags == null) return tag;\n"
       "    var argn = parseInt(tag.substring(1)) - 1;\n"
       "    return arg_tags[argn];\n"
       "  }\n"
       "  return tag;\n"
       "}\n"
      );



  fclose(js_fp);
  fclose(svg_fp);
  fclose(svg_top_of_file_fp);
}

/*********************************/
static double vb_x1 = 0.0, vb_y1 = 0.0, vb_x2 = 300.0, vb_y2 = 150.0;

void set_viewbox(double x1, double y1, double x2, double y2)
{
  vb_x1 = x1;
  vb_y1 = y1;
  vb_x2 = x2;
  vb_y2 = y2;
}

/*********************************/


static void gen_final_file(const char *fname)
{
  FILE *fp = fopen(fname, "w");
  if (fp == NULL)
  {
    perror(fname);
    exit(-1);
  }

  time_t now;
  struct tm now_tm;
  char buf[40];
  
  fprintf(fp, "<!--\n");
  fprintf(fp, "Auto generated file DO NOT EDIT\n");
  fprintf(fp, "Generated by: %s, at ", __FILE__);
  now = time(NULL);
  localtime_r(&now, &now_tm);
  strftime(buf, sizeof(buf), "%F %T %z", &now_tm);
  fprintf(fp, "%s\n", buf);

  fprintf(fp, "-->\n");

  include_file(fp, ".", "_tmp_top_of_file.svg", silent);

  for (int i=0; i < n_svg_libs; i++)
  {
    fprintf(fp, "<!-- START insert svg library: %s-->\n", svg_lib_files[i]);
    include_file(fp, "./svgplugins", svg_lib_files[i], silent);
    fprintf(fp, "<!-- END insert svg library: %s -->\n", svg_lib_files[i]);
  }

  include_file(fp, ".", "_tmp_display.svg", silent);

  fprintf(fp, "</g>\n");

  // The popups must go here to be OUTSIDE of the main group
  if (popup_on)
  {
    const char *dpopup_name = "discrete_popup.svg";
    fprintf(fp, "<!-- START insert %s-->\n", dpopup_name);
    include_file(fp, "./svgplugins", dpopup_name, silent);
    fprintf(fp, "<!-- END insert %s-->\n", dpopup_name);
  }
  if (analog_popup_on)
  {
    const char *apopup_name = "analog_popup.svg";
    fprintf(fp, "<!-- START insert %s-->\n", apopup_name);
    include_file(fp, "./svgplugins", apopup_name, silent);
    fprintf(fp, "<!-- END insert %s-->\n", apopup_name);
  }
  fprintf(fp, "<script type=\"text/ecmascript\"><![CDATA[\n");
     // max one output every 250 ms
  fprintf(fp, "const MAX_OUTPUT_RATE = 250;\n"); 
    // delay update of output widgets after output for 1000 ms
    // This allows time for the new output to get to REACT to
    // avoid updating with the old value
  fprintf(fp, "const UPDATE_DELAY = 1000;\n"); 
  fprintf(fp, "const svgNS = \"http://www.w3.org/2000/svg\";\n");
  fprintf(fp, "var reactmainobj=document.getElementById(\"main_group\");\n");

  if ((popup_on) || (analog_popup_on))
  {
    fprintf(fp, 
      "function show_main()\n"
      "{\n"
      "  reactmainobj.parentNode.appendChild(reactmainobj);\n"
      "}\n"
    );
  }
  if (popup_on)
  {
    fprintf(fp, 
      "var dpopupobj=document.getElementById(\"discrete_popup\");\n"
      "function popup_on_timeout()\n"
      "  {document.getElementById('popup_bg_on').setAttribute('fill','yellow');}\n" 
      "function popup_off_timeout()\n"
      "  {document.getElementById('popup_bg_off').setAttribute('fill','yellow');}\n" 
      "function show_popup(x,y,text_on, text_off, the_tag)\n"
      "{\n"
      "  console.log(\"show_popup, tag: \" + the_tag);\n"
      "  var the_cfg = get_config(the_tag);\n"
      "  console.log(\"show_popup, cfg: \" + the_cfg);\n"
      "  dpopupobj.setAttribute(\"transform\", \"translate(\" + x +\",\" + y +\")\");\n"
      "  dpopupobj.parentNode.appendChild(dpopupobj);\n"
      "  var onobj=document.getElementById(\"popup_on\");\n"
      //"  onobj.textContent=text_on;\n"
      "  onobj.textContent=the_cfg.hi_desc;\n"
      "  var offobj=document.getElementById(\"popup_off\");\n"
      //"  offobj.textContent=text_off;\n"
      "  offobj.textContent=the_cfg.lo_desc;\n"
      "  var tagobj=document.getElementById(\"popup_tag\");\n"
      "  tagobj.textContent=convert_tag(the_tag);\n"
      "}\n"
    );
  }
  if (analog_popup_on)
  {
    fprintf(fp, 
      "var apopupobj=document.getElementById(\"analog_popup\");\n"
      "function show_analog_popup(x,y,the_tag)\n"
      "{\n"
      "  console.log(\"show_popup, tag: \" + the_tag);\n"
      "  var the_cfg = get_config(the_tag);\n"
      "  console.log(\"show_popup, cfg: \" + the_cfg);\n"
      "  apopupobj.setAttribute(\"transform\", \"translate(\" + x +\",\" + y +\")\");\n"
      "  apopupobj.parentNode.appendChild(apopupobj);\n"

      //"  var onobj=document.getElementById(\"popup_on\");\n"
      //"  onobj.textContent=the_cfg.hi_desc;\n"

      //"  var offobj=document.getElementById(\"popup_off\");\n"
      //"  offobj.textContent=the_cfg.lo_desc;\n"

      "  var tagobj=document.getElementById(\"apopup_tag\");\n"
      "  tagobj.textContent=convert_tag(the_tag);\n"
      "  slider_object_popup.set_tag(the_tag);\n"
      "}\n"
    );
    const char *apopup_js = "analog_popup.js";
    fprintf(fp, "<!-- START insert %s-->\n", apopup_js);
    include_file(fp, "./jsplugins", apopup_js, silent);
    fprintf(fp, "<!-- END insert %s-->\n", apopup_js);

  }

  for (int i=0; i < n_js_libs; i++)
  {
    fprintf(fp, "// -- START insert js library: %s\n", js_lib_files[i]);
    include_file(fp, "./jsplugins", js_lib_files[i], silent);
    fprintf(fp, "// -- END insert js library: %s\n", js_lib_files[i]);
  }

  include_file(fp, ".", "_tmp_display.js", silent);

  fprintf(fp, "]]></script>\n");

  fprintf(fp, "</svg>\n"); 

  fclose(fp);
}

/********************************/


static gen_plugin_base_t *get_plugin(const char *type)
{
  for (int i=0; i < n_plugins; i++)
  {
    if (0 == strcmp(the_plugins[i]->get_name(), type)) return the_plugins[i];
  }
  return NULL;
}

/*********************************/

void print_help()
{
  printf(
    "Usage: gen_display [-f <config-file-name>] [-d <plugin-director>]\n"
    "                   [-o <output-file-name>] [-react] [-sim]\n"
    "Run the display builder (gen_display)\n\n"
    "  -d   use an alternate directory for the plugin directory.\n"
    "       The -d command must be followed by a plugin directory.\n"
    "       This will be used as the plugin directory, in place of\n"
    "       the default of \"./plugins/\".\n"
    "  -f   use an alternate configuration file for the display.\n"
    "       The -f command must be followed by a file name.\n"
    "       This will be used as the configuration file name, in place of\n"
    "       the default of \"display.txt\".\n"
    "  -o   use an alternate output file name for the final output.\n"
    "       The -o command must be followed by a file name.\n"
    "       This will be used as the output file name, in place of\n"
    "       the default of \"dtest.svg\".\n"
    "  -react   option tells gen_display to generate Ajax code to connect to\n"
    "           REACT (via Apache)  for configuration and point value information\n"
    "  -sim     option tells gen_display to generate a simulator\n"
    "           for configuration and point value information\n"
    "             default sim file is: sim_pump.js\n"
    "  -sim_file same as -sim but, with the sim file specified in \n"
    "            the next argument\n"
    "  -popup   option enables popups for valves, pumps, etc.\n"
    "  -silent  option disables all output to stdout or stderr, except for error messages.\n"
    "  -doc   generate documentation - default is html, uses the output file specified with -o\n"
    "  -text  Set documentation output to text\n"
    "  -html  Set documentation output to html\n"
    "\n"
  );
}

/*********************************/

int main(int argc, char *argv[])
{
  DIR *dir;
  struct dirent *dent;
  void *handle;
  char *error;
  crear_objeto_t cofn;
  int i;

  char sofile[100];

  const char *dir_name = "./plugins/";
  const char *config_name = "display.txt"; 
  const char *output_name = "dtest.svg"; 
  popup_on = false;
  bool doc_on = false;
  bool html_on = true;

  for (int current_arg=1; current_arg < argc; current_arg++)
  {
    if (0 == strcmp(argv[current_arg], "-react"))
    {
      run_mode = RT_REACT_MODE;
    }
    else if (0 == strcmp(argv[current_arg], "-sim"))
    {
      run_mode = RT_SIM_MODE;
    }
    else if (0 == strcmp(argv[current_arg], "-silent"))
    {
      silent = true;
    }
    else if (0 == strcmp(argv[current_arg], "-popup"))
    {
      popup_on = true;
      add_svg_library("filter1.svg");
    }
    else if (0 == strcmp(argv[current_arg], "-doc"))
    {
      doc_on = true;
    }
    else if (0 == strcmp(argv[current_arg], "-html"))
    {
      html_on = true;
    }
    else if (0 == strcmp(argv[current_arg], "-text"))
    {
      html_on = false;
    }
    else if (0 == strcmp(argv[current_arg], "-d"))
    {
      if (argc > (current_arg + 1))
      {
        current_arg++;
        dir_name = argv[current_arg];
      }
      else
      {
        print_help();
        printf("For -d option, you MUST specify the directory, %d, %d\n", argc, current_arg);
        exit(1);
      }
    }
    else if (0 == strcmp(argv[current_arg], "-sim_file"))
    {
      if (argc > (current_arg + 1))
      {
        current_arg++;
        sim_file = argv[current_arg];
        run_mode = RT_SIM_MODE;
      }
      else
      {
        print_help();
        printf("For -sim_file option, you MUST specify the javascript sim file name, %d, %d\n", argc, current_arg);
        exit(1);
      }
    }
    else if (0 == strcmp(argv[current_arg], "-f"))
    {
      if (argc > (current_arg + 1))
      {
        current_arg++;
        config_name = argv[current_arg];
      }
      else
      {
        print_help();
        printf("For -f option, you MUST specify the configuration file name, %d, %d\n", argc, current_arg);
        exit(1);
      }
    }
    else if (0 == strcmp(argv[current_arg], "-o"))
    {
      if (argc > (current_arg + 1))
      {
        current_arg++;
        output_name = argv[current_arg];
      }
      else
      {
        print_help();
        printf("For -o option, you MUST specify the output file name, %d, %d\n", argc, current_arg);
        exit(1);
      }
    }
    else if (0 == strcmp(argv[current_arg], "--help"))
    {
      print_help();
      exit(0);
    }
    else if (argv[current_arg][0] == '-')
    {
      print_help();
      exit(1);
    }
    else
    {
      break;
    }

  }


  dir = opendir(dir_name);

  if (dir == NULL)
  {
    perror(dir_name);
    exit(0);
  }
  struct dirent entry;

//dent = readdir(dir);  old call, NOT thread safe!!

  for (readdir_r(dir, &entry, &dent); dent != NULL; readdir_r(dir, &entry, &dent))
  {
    if (dent->d_type != DT_REG)
    {
      //continue;
      // Commented out because on some VMs does not ruturn DT_REG
    }

    int len = strlen(dent->d_name);
    if (len < 4)
    {
      //printf("Skipping %s, too short\n", dent->d_name);
      continue;
    }
    char *ext = dent->d_name + (len - 3);

    //printf("File name = %s\n", dent->d_name);
    if (0 != strcmp(ext, ".so"))
    {
      // Not a shared object file, IGNORE
      continue;
    }

    snprintf(sofile, sizeof(sofile), "%s/%s", dir_name, dent->d_name);
    if (!silent)
    {
      printf("Opening plugin[%d]: %s\n", n_plugins + 1,  sofile);
    }

    if (n_plugins > (MAX_PLUGINS - 1)) 
    {
      printf("Maximum number of plugins exceeded!!!, stopping\n");
      break;
    }

    handle = dlopen(sofile, RTLD_LAZY);
    if (handle == NULL)
    {
      fprintf (stderr, "%s\n", dlerror());
      continue;
    }
    cofn = (crear_objeto_t) dlsym(handle, "get_object");
    if ((error = dlerror()) != NULL)
    {
      fprintf (stderr, "%s\n", error);
      continue;
    }
    if (n_plugins < MAX_PLUGINS)
    {
      the_plugins[n_plugins] = cofn();
      n_plugins++;
    }
    else
    {
      printf("Maximum number of plugins exceeded, exiting . . . . \n");
      exit(-1);
    }

  }
  if (!silent)
  {
    printf ("found %d plugins!!!\n", n_plugins);
  }

  doc_object_base_t *doc_object;
  FILE *docfp;
  if (doc_on)
  {
    if (html_on)
    {
      if (!silent)
      {
        printf("generating html documentation\n");
      }
      doc_object = new doc_html_object_t();
    }
    else
    {
      if (!silent)
      {
        printf("generating text documentation\n");
      }
      doc_object = new doc_text_object_t();
    }
    docfp = fopen(output_name, "w");
    if (docfp == NULL)
    {
      perror(output_name);
    }
    doc_object->set_fp(docfp);
    doc_object->header();
  }

  // Ok, probamos todos de los objetos para ver que si funcionan.
  for (i=0; i < n_plugins; i++)
  {
    if (!silent)
    {
      printf("plugin[%d]-handles objects of type: %s\n", i+1, the_plugins[i]->get_name());
    }
    rtsvg_param_t *params;
    int n_params;
    params = the_plugins[i]->param_types(&n_params);
    if (!silent)
    {
      if (params != NULL)
      {
        printf("Has param types defined\n");
      }
      else
      {
        printf("param_types: NULL\n");
      }
    }
    if (doc_on)
    {
      if (!silent)
      {
        printf("generating doc for %s\n", the_plugins[i]->get_name());
      }
      the_plugins[i]->generate_doc(doc_object);
    }
  }
  if (!silent) printf("\n");

  if (doc_on)
  {
    doc_object->footer();
    fclose(docfp);
    exit(0);
  }

  if (!silent)
  {
    printf("Config file is: %s\n", config_name);
    printf("Output file is: %s\n", output_name);
  }

  do_gen(config_name);
  if (max_argn != -1)
  {
    bool args_ok = true;
    for (int i=0; i <= max_argn; i++)
    {
      if (!silent)
      {
        printf("$%d: %d object(s)", i, arg_count[i]);
      }
      if (arg_count[i] == 0)
      {
        args_ok = false;
        printf(" ERROR");
      }
      printf("\n");
    }
    if (!args_ok)
    {
      printf("There must be at least one object using each argument\n");
      exit(-1);
    }
  }

  gen_final_file(output_name);

  // Ok, borrar todos de los objetos para ver que funciona los destructores.
  for (i=0; i < n_plugins; i++)
  {
    delete(the_plugins[i]);
  }
  if (!silent)
  {
    printf("\n\n");
  }

  return 0;
}

/*********************************/

