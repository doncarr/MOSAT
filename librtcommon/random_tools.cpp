
#include <stdlib.h>

/**********************************************************************/

double add_rand_noise(double val, double full_scale, double fraction_noise)
{
  double rand_mult = (((double) random() / (double) RAND_MAX)
        * 2.0 * fraction_noise) - fraction_noise;
  return val + (full_scale * rand_mult);
}

/**********************************************************************/
