
#include <stdio.h>
#include "gen_display.h"

void doc_text_object_t::start(const char *name, const char *short_desc)
{
  this->param_number = 0;
  this->note_number = 0;
  this->example_number = 0;

  fprintf(this->fp, "-------------\n\n");
  fprintf(this->fp, "'%s' - %s\n\n", name, short_desc);
}

void doc_text_object_t::param(const char *short_desc)
{
  this->param_number++;
  if (this->param_number == 1)
  {
    fprintf(this->fp, "\nParameters:\n\n");
  }
  fprintf(this->fp, "  %d: %s\n", this->param_number, short_desc);
}

void doc_text_object_t::example(const char *example)
{
  this->example_number++;
  if (this->example_number == 1)
  {
    fprintf(this->fp, "\nExample:\n\n");
  }
  fprintf(this->fp, "%s\n\n", example);
}

void doc_text_object_t::notes(const char *notes)
{
  this->note_number++;
  if (this->note_number == 1)
  {
    fprintf(this->fp, "\nNotes:\n\n");
  }
  fprintf(this->fp, "%s\n\n", notes);
}

void doc_text_object_t::header(void)
{
  fprintf(this->fp, "SVG Widget Documentation\n\n");
}

void doc_text_object_t::footer(void)
{
  fprintf(this->fp, "\n-------------\n");
}

void doc_text_object_t::end(void)
{
  fprintf(this->fp, "\n");
}

