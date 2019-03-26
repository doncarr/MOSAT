
#include <stdio.h>
#include "gen_display.h"

void doc_html_object_t::start(const char *name, const char *short_desc)
{
  this->param_number = 0;
  this->note_number = 0;
  this->example_number = 0;

  fprintf(this->fp, "<h3>'%s' - %s</h3>\n", name, short_desc);
}

void doc_html_object_t::param(const char *short_desc)
{
  this->param_number++;
  if (this->param_number == 1)
  {
    fprintf(this->fp, "<br>&nbsp;&nbsp;<b>Parameters:</b><br><br>\n");
  }
  fprintf(this->fp, "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>%d:</b> %s<br>\n", this->param_number, short_desc);
}

void doc_html_object_t::example(const char *example)
{
  this->example_number++;
  if (this->example_number == 1)
  {
    fprintf(this->fp, "<br>&nbsp;&nbsp;<b>Example:</b><br><br>\n");
  }
  fprintf(this->fp, "<pre>\n");
  fprintf(this->fp, "      %s\n", example);
  fprintf(this->fp, "</pre>\n");
}

void doc_html_object_t::notes(const char *notes)
{
  this->note_number++;
  if (this->note_number == 1)
  {
    fprintf(this->fp, "<br>&nbsp;&nbsp;<b>Notes:</b><br><br>\n\n");
  }
  fprintf(this->fp, "<p>\n");
  fprintf(this->fp, "%s\n", notes);
  fprintf(this->fp, "</p>\n");
}

void doc_html_object_t::header(void)
{
  fprintf(this->fp, "<!DOCTYPE html>\n");
  fprintf(this->fp, "<html>\n");
  fprintf(this->fp, "<!-- Auto Generated, do not edit -->\n");
  fprintf(this->fp, "<head>\n");
  fprintf(this->fp, "<title>SVG Widget Documentation</title>\n");
  fprintf(this->fp, "</head>\n");
  fprintf(this->fp, "<body bgcolor=\"cornsilk\">\n");
  fprintf(this->fp, "<hr>\n");
  fprintf(this->fp, "<h2>Using the REACT SVG Display Generator</h2><br><br>\n");
  fprintf(this->fp, "The React display generator is run with the command 'gen_display'<br><br>\n");
  fprintf(this->fp, "A common use is:<br>\n");
  fprintf(this->fp, "<pre>\n");
  fprintf(this->fp, "./gen_display -f config-file -o output-file.svg -sim\n");
  fprintf(this->fp, "</pre>\n");
  fprintf(this->fp, "For example:<br>\n");
  fprintf(this->fp, "<pre>\n");
  fprintf(this->fp, "./gen_display -f slider.txt -o slider.svg -sim\n");
  fprintf(this->fp, "</pre>\n");
  fprintf(this->fp, "For more details run with --help:<br>\n");
  fprintf(this->fp, "<pre>\n");
  fprintf(this->fp, "./gen_display --help\n");
  fprintf(this->fp, "</pre>\n");
  fprintf(this->fp, "<p>Every line in the configuration file has first a widget name, followed by all of the parameters for the given widget.\n");
  fprintf(this->fp, "The first line should be the 'svg_header', followed by the 'background', then the rest of the widgets.</p>\n");
  fprintf(this->fp, "An example configuration file with a only a signle panel light:<br>\n");
  fprintf(this->fp, "<pre>\n");
  fprintf(this->fp, "svg_header|My Title|0|0|300|150|\n");
  fprintf(this->fp, "background|thistle|\n");
  fprintf(this->fp, "simple_panel|PUMP1_ON|yellow|gray|86|109|8|\n\n");
  fprintf(this->fp, "</pre>\n");
  fprintf(this->fp, "<hr>\n");
  fprintf(this->fp, "<h2>SVG Widget Documentation</h2>\n");
  fprintf(this->fp, "<p>\n");
  fprintf(this->fp, "Following is the documentation for each of the current REACT SVG Widgets.\n");
  fprintf(this->fp, "Each line of a display configuration file that does not start with '#'\n");
  fprintf(this->fp, "defines a widget. Each line that defines a widget must begin with\n");
  fprintf(this->fp, "the name of the widget, followed by the parameters of the widget, \n");
  fprintf(this->fp, "which are delimited by a vertical bar: '|'.\n");
  fprintf(this->fp, "The widgets that come later in the file are later on the draw list, and thus,\n");
  fprintf(this->fp, "if widgets overlap, the one later in the file will be on top.\n");
  fprintf(this->fp, "For example, if you use the 'panel' object, all widgets that will be on\n");
  fprintf(this->fp, "the panel, must come in the file AFTER the panel, otherwise, they will be \n");
  fprintf(this->fp, "hidden by the panel.\n");
  fprintf(this->fp, "</p>\n");
  fprintf(this->fp, "<p>\n");
  fprintf(this->fp, "For any objects that support animation, you can turn off animation by simply leaving\n");
  fprintf(this->fp, "the tagname blank, or putting 'null' for the tagname. Of course, for some, like analog_pv, it\n");
  fprintf(this->fp, "would not make sense to turn off animation, as it is the equivalent of putting a\n");
  fprintf(this->fp, "string up that just contains '0', and never changes. For others, like pipe2d, it makes perfect sense to \n");
  fprintf(this->fp, "have a pipe for which you do not wish to animate the color.</p>\n");
  fprintf(this->fp, "</p>\n");
  fprintf(this->fp, "<hr>\n");
}

void doc_html_object_t::footer(void)
{
  fprintf(this->fp, "</body>\n");
  fprintf(this->fp, "</html>\n");
}
void doc_html_object_t::end(void)
{
  fprintf(this->fp, "<br>\n");
  fprintf(this->fp, "<hr>\n");
}

