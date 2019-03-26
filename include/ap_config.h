/************************************************************************
This software is part of React, a control engine
Copyright (C) 2005,2006 Donald Wayne Carr 

This program is free software; you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation; either version 2 of the License, or 
(at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
General Public License for more details.

You should have received a copy of the GNU General Public License along 
with this program; if not, write to the Free Software Foundation, Inc., 
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
***********************************************************************/

#ifndef __CONFIG_INC__
#define __CONFIG_INC__

class ap_config_t
{
private:
  int n_keys;
  int cur_key;
  char delimiter;
  char *key[100];
  char *val[100];

public:
  const char *get_config(const char *key);
  const char *get_config(const char *key, const char *default_value);
  bool get_bool(const char *key, bool default_value);
  int get_int(const char *key);
  int get_int(const char *key, int default_value);
  double get_double(const char *key);
  double get_double(const char *key, double default_value);
  void read_file(const char *path);
  const char *first(void);
  const char *next(void);
  void insert_value(const char *key, const char *value);
  void set_delimiter(char a_delimiter){delimiter = a_delimiter;};
  ap_config_t();
  ap_config_t(char a_delimiter);
};

extern ap_config_t ap_config;

#endif
