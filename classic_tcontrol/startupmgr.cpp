
#include <stdio.h>

#include "alg.h"

#define MAX_ZONES (20)

struct zone_t
{
  bool blocked;
  double time_free;
  int n_stations;
  int stations[10];
};

int n_zones;
int key;
int zone_mapping[MAX_SECTIONS];

zone_t zones[MAX_ZONES];
alg_notify_object_t *notify_obj;

/**********************************************************************/

void light_to_green_event(int section, double t)
{
  int zn = zone_mapping[section];  
  zone_t *this_zone = &zones[zn];
  if (this_zone->blocked)
  {
    // Error
  }
  this_zone->blocked = true;
  this_zone->time_free = t + 20; // Block other trains from starting for 30 seconds.
  for (int i=0; i < this_zone->n_stations; i++)
  {
    int s = this_zone->stations[i];
    if (s != section)
    {
      notify_obj->prohibit_section_entry(s, key);
    }
  }
}

/**********************************************************************/

void update(double t)
{
  for (int i=0; i < n_zones; i++)
  {
    if (zones[i].blocked && (t > zones[i].time_free))
    {
      zone_t *this_zone = &zones[i];
      for (int i=0; i < zones[i].n_stations; i++)
      {
        int s = zones[i].stations[i];
        notify_obj->permit_section_entry(s, key);
      }
    }
  } 
}

/**********************************************************************/

