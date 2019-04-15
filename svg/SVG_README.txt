This is a document on how the real-time SVG generator works to create an animated SVG file.

First, all objects that are displayed are plugin objects, there are no built-in objects. The idea is to expose all of the APIs, insert points, include libraries, etc, so that plugins do everything. 

Second, when you write a plugin, you do NOT need to worry about the data connection, you ONLY need to give a TAG (UID), and, an object with an "init" method, and an "update" method (see "add_animation_object()" below). The system will then call the "init" method at startup with an object that has all of the attributes of the object specified by the TAG, and, periodically, call "update" with the real-time value of the object specified by the TAG. The point value is either boolean or numeric depending on the object type that the tag refers to.

Third, the tag can come from the URL, which allows you to make displays for any object of a certain type, or a group of objects of certain types. For instance, if you have a number of identical pumping stations, you can create one display that works with all pumping stations, the tags are in the URL after the '?', separated by '+'. This will be also used to create a default display for any object type. So, when you specify the tag, instead of specifying an actual tag, you do it like bash parameters, and, put, $1, $2, $3, . . . $N, to reference the tags that will be in the url after the '?'.

Plugins do NOT need to do animations, they can just draw things. A plugin that does not do animation will NOT call the add_animation_object() function. Note: a plugin can call add_animation_object() multiple times if it will animated by the values from more than one TAG.

Ok, what is exposed to plugins so that they can do their work, and how do they get their configuration information?

There is a configuration file that is a delimited text file that has the name of the plugin in the first column, and, then the parameters for the plugin in the following columns. The system will parse each line and divide it into arguments delimited by '|', and, then pass these arguments to the corresponding plugin specified in the first argument.




A plugin must inherit from "gen_plugin_base_t", which is an object with two pure virtual functions that you must implement: "get_name()", and "generate()". The method "get_name()" must return the name of the plugin, and the generate function will be called for EVERY line in th delimited configuration file that has that same name in the first column.

The generate method recieves plugin_data_t structure, as well as argc/argv with all of the arguments.

void generate(plugin_data_t d, int argc, char **argv);

The plugin_data_t is as follows.

struct plugin_data_t
{
  FILE *svg_fp;
  FILE *svg_top_of_file_fp;
  FILE *js_fp;
  bool popup_on;
  const char *file_name;
  int line_number;
  bool silent;
};

1) svg_fp: This is the file where you insert the SVG objects that need to be drawn and possibly animated. You can also generate SVG objects using the Javascript APIs from the animation object constructor, or, the init method of the animation object constructor

2) svg_top_of_file_fp: This is the file where you insert things that you want to appear at the top of the SVG file. This would be for inserting the SVG header, background, etc.

3) js_fp: This is where you put any Javascript code that you need to generate, and will be typically have ONLY the call to the constructor for you animation object with the required parameters. NOTE: the library that you need to implement your object should be inserted using the "add_js_library()" function call (see below).

4) popup_on: This tells the plugin if popups have been enabled so they can decide if they should generage any code to support popups.

5) file_name: This is the file name of the configuration file that can be used when generating an error message.

6) line_number: This is the line number in the file, which can also be used when generating an error message.

7) silent: This is a flag instructing the popup that NO debug text should be generated to stdout during generation.

Besides generating (inserting) code directly into the files specified above, a plugin can call the following functions:

1) void add_js_library(const char *file_name);

This function adds a javascript library to the list of javascript library to be inserted into the final SVG file, eliminating any duplicates so that each library is only inserted one time. The inclusion of animation objects should be handled in this manner.

2) void add_svg_library(const char *file_name);

This function adds a SVG file to the list of SVG libraries to be inserted into the SVG file, eliminating any duplicats, so that each library is inserted only once. SVG libraries typically will only include definitions, such as gradient definitions that can be re-used.

3) void add_animation_object(const char *the_tag, const char *the_js_object);

This function adds an animation object to the list of animation objects, and must include a TAG to identify the source of the animation information. Your animation object MUST have init() and update() methods, and, the system will AUTOMATICALLY call these methods. The init() method will be called once at startup, and the update() method will be called periodically.


The final content of the output SVG file is created in the following order:

1) The complete contents of the file that was written to with the file pointer "svg_after_header_fp".

2) All of the SVG library files.

3) The complete contents of the file that was written to with the file pointer "svg_fp"

4) The javascript header: <script type=\"text/ecmascript\"><![CDATA[

5) All of the javascript library files.

6) The complete contents of the file that was written to with the file pointer "js_fp"

7) The AJAX code to read the values and hook it all together.

8) The end javascript: ]]></script>

9) Then end of SVG: </svg>


Tips

See this definition of a drop shadow:

http://oskamp.dyndns.org/SiemensClock/SVG/
That site is no longer active, but, you can find a modified
version of that drop shado here: svgplugins/pipeshadow.svg
Though, it is used for more than just pipe shadows . . .


