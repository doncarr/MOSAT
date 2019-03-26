
#include <stdio.h>
#include <stdlib.h>

#include "rtcommon.h"

#include "section_reader.h"

#include "ap_config.h"


ap_config_t ap_config(',');

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("You must specify the config file, exiting\n");
    return -1;
  }

  ap_config.read_file(argv[1]);

  const char *sections_file = ap_config.get_config("SECTIONS_FILE");
  if (sections_file == NULL)
  {
    sections_file = "sections.txt";
  }

  section_reader_t sections;
  sections.read_section_file();

}


