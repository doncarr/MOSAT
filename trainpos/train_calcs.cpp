
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "trainpos.h"
#include "train_calcs.h"
#include "section_reader.h"

//  int n_trains;
//  train_data_t *td;

/********************************************************************/

int train_calculator_t::get_n_trains(void)
{
  return n_trains;
}

/********************************************************************/

int train_calculator_t::get_num(int train_index)
{
  if ((train_index < 0) || (train_index >= n_trains))
  {
    printf("%s line %d: Out of range index (%d) to access train data\n",
       __FILE__, __LINE__, train_index);
    exit(1);
  }
  return td[train_index].num;
}

/********************************************************************/

const char *train_calculator_t::get_train_id(int train_index)
{
  if ((train_index < 0) || (train_index >= n_trains))
  {
    printf("%s line %d: Out of range index (%d) to access train data\n",
       __FILE__, __LINE__, train_index);
    exit(1);
  }
  return td[train_index].train_id;
}

/********************************************************************/

time_t train_calculator_t::get_service_entry_time(int train_index)
{
  if ((train_index < 0) || (train_index >= n_trains))
  {
    printf("%s line %d: Out of range index (%d) to access train data\n",
       __FILE__, __LINE__, train_index);
    exit(1);
  }
  return td[train_index].service_entry_time;
}

/********************************************************************/

time_t train_calculator_t::get_scheduled_entry_time(int train_index)
{
  if ((train_index < 0) || (train_index >= n_trains))
  {
    printf("%s line %d: Out of range index (%d) to access train data\n",
       __FILE__, __LINE__, train_index);
    exit(1);
  }
  return td[train_index].scheduled_service_entry_time;
}

/********************************************************************/

int train_calculator_t::get_line_location(int train_index)
{
  if ((train_index < 0) || (train_index >= n_trains))
  {
    printf("%s line %d: Out of range index (%d) to access train data\n",
       __FILE__, __LINE__, train_index);
    exit(1);
  }
  return td[train_index].line_location;
}

/********************************************************************/

int train_calculator_t::get_section(int train_index)
{
  if ((train_index < 0) || (train_index >= n_trains))
  {
    printf("%s line %d: Out of range index (%d) to access train data\n",
       __FILE__, __LINE__, train_index);
    exit(1);
  }
  return td[train_index].section;
}

/********************************************************************/

int train_calculator_t::get_seconds_late(int train_index)
{
  if ((train_index < 0) || (train_index >= n_trains))
  {
    printf("%s line %d: Out of range index (%d) to access train data\n",
       __FILE__, __LINE__, train_index);
    exit(1);
  }
  return (time_now - td[train_index].service_entry_time) - td[train_index].line_location;
}

/********************************************************************/

int train_calculator_t::seconds_in_section(int train_index)
{
  if ((train_index < 0) || (train_index >= n_trains))
  {
    printf("%s line %d: Out of range index (%d) to access train data\n",
       __FILE__, __LINE__, train_index);
    exit(1);
  }
  // This does NOT include seconds lost or gained since entering the section. Is that ok?
  return td[train_index].line_location - sections.get_time_to_start(td[train_index].section);
}

/********************************************************************/

double train_calculator_t::get_fraction_of_section_traveled(int train_index)
{
  if ((train_index < 0) || (train_index >= n_trains))
  {
    printf("%s line %d: Out of range index (%d) to access train data\n",
       __FILE__, __LINE__, train_index);
    exit(1);
  }
  int loc_in_section = td[train_index].line_location - sections.get_time_to_start(td[train_index].section);
  return double(loc_in_section) / double(sections.get_section_time(td[train_index].section));
}

/********************************************************************/

int train_calculator_t::get_seconds_to_next_train(int train_index)
{
  if ((train_index < 0) || (train_index >= n_trains))
  {
    printf("%s line %d: Out of range index (%d) to access train data\n",
       __FILE__, __LINE__, train_index);
    exit(1);
  }
  if (train_index == n_trains - 1)
  {
    return 0;
  }
  int secs = td[train_index + 1].line_location - td[train_index].line_location; 
  return (secs);
}

/********************************************************************/

bool train_calculator_t::get_departed(int train_index)
{
  if ((train_index < 0) || (train_index >= n_trains))
  {
    printf("%s line %d: Out of range index (%d) to access train data\n",
       __FILE__, __LINE__, train_index);
    exit(1);
  }
  return td[train_index].num;
}

/********************************************************************/

time_t train_calculator_t::get_arival_time(int train_index)
{
  if ((train_index < 0) || (train_index >= n_trains))
  {
    printf("%s line %d: Out of range index (%d) to access train data\n",
       __FILE__, __LINE__, train_index);
    exit(1);
  }
  return td[train_index].num;
}

/********************************************************************/

time_t train_calculator_t::get_section_entry_time(int train_index)
{
  if ((train_index < 0) || (train_index >= n_trains))
  {
    printf("%s line %d: Out of range index (%d) to access train data\n",
       __FILE__, __LINE__, train_index);
    exit(1);
  }
  return td[train_index].num;
}

/********************************************************************/

bool train_calculator_t::get_unexpected(int train_index)
{
  if ((train_index < 0) || (train_index >= n_trains))
  {
    printf("%s line %d: Out of range index (%d) to access train data\n",
       __FILE__, __LINE__, train_index);
    exit(1);
  }
  return td[train_index].num;
}

/********************************************************************/

bool train_calculator_t::get_switched_direction(int train_index)
{
  if ((train_index < 0) || (train_index >= n_trains))
  {
    printf("%s line %d: Out of range index (%d) to access train data\n",
       __FILE__, __LINE__, train_index);
    exit(1);
  }
  return td[train_index].num;
}

/********************************************************************/



