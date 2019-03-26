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
/*******************************************************************/

int main(int argc, char *argv[])
{
  printf("%s %d\n", __FILE__, __LINE__);
  logfile_t *lg = new logfile_t();
  lg->vprint("It worked, %s, %d\n", "Yes", 99);
  lg->print("testing\n");
  FILE *fp = lg->open_file_in_log_dir("dataout.txt");
  fprintf(fp, "This is the data file!!!\n");
  fclose(fp);
  lg->close();
}

/*******************************************************************/
