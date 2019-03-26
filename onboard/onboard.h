#ifndef __ONBOARD_INC__
#define __ONBOARD_INC__

#define RT_MAX_SECTIONS (60)

// Note, for the following structure, we have interntionally left out any details
// of deterining which end of the train is coupled, which direction the train is going.
// It is assumed that this should be determined by the module which doors are "left", 
// and which doors are "right". This changets depending on the direction of the train!.
struct spd_discrete_t
{
  bool doors_open; // Any doors are open (left, right, or both)
  bool left_open;  // Left doors are open
  bool right_open; // Right doors are open
  bool master;     // This computer is the master
  bool raw_bits[16];
  long total_count;
  long current_count;
};

void create_profile_dir(const char *base_dir, char *dirname, int dir_sz);

#endif
