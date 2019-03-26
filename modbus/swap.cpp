
#include <netinet/in.h>
#include "mtype.h"

/**********************************************************************/

void swap16(uint16 *word)
{
  *word = htons(*word); // Well, both htons and ntohs swap the bytes 
                        //   (if needed for the architecture)
  /******
  uint16 w1, w2;
  w1 = *word << 8;
  w2 = *word >> 8;
  *word = w1 | w2;
  ******/
}
