
#include <stdio.h>
#include <stdlib.h>

void include_file(FILE *fp, const char *dirname, const char *fname, bool silent)
{
  // This inserts another file into a file that is already open for writing
  char *thepath;
  if (-1 == asprintf(&thepath, "%s/%s", dirname, fname))
  {
    perror(fname);
    fprintf(stderr, "Call to asprintf failed: %s, %s\n", dirname, fname);
    return;
  }
  if (!silent) printf("//Including file: %s\n", thepath);
  FILE *fp_include = fopen(thepath, "r");
  if (fp_include == NULL)
  {
    perror(thepath);
    fprintf(stderr, "Can not open file for include: %s\n", thepath);
    free(thepath);
    return;
  }
  char line[200];
  while (NULL != fgets(line, sizeof(line), fp_include))
  {
    fprintf(fp, "%s", line);
  }
  fclose(fp_include);
  free(thepath);
}
