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
#include <unistd.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include "displaydata.h"

int main(int argc, char *argv[])
{
  key_t mykey;
  // First open the file and read the shared memory id.
  FILE *fp = fopen("shmid.txt", "r");
  if (fp == NULL)
  {
    perror("shmid.txt");
  }
  fscanf(fp, "%d", &mykey);
  fclose(fp);

  // Now attach to shared memory.
  void *myshmp = shmat(mykey, NULL, 0);
  if (myshmp == (void *) -1)
  {
    perror("Could not attach to shared memory");
    printf("React is probably not running!!\n");
    exit(0);
  }
  printf("I attached to shared memory, addr = %p\n", myshmp);

  char *cp = (char *) myshmp;


  display_info_t dinfo;
  get_display_pointers(myshmp, &dinfo);
  printf("n analog = %d\n", dinfo.n_analog);
  for (int i=0; i < dinfo.n_analog; i ++)
  {
    printf("%s %s %f\n", dinfo.adata[i].tag,
          dinfo.adata[i].description, dinfo.adata[i].pv);
  }
  printf("n discrete = %d\n", dinfo.n_discrete);
  for (int i=0; i < dinfo.n_discrete; i ++)
  {
    printf("%s %s %s\n", dinfo.ddata[i].tag,
          dinfo.ddata[i].description, dinfo.ddata[i].pv);
  }


  printf("Hit <enter> to continue: ");
  char buf[10];
  fgets(buf, 10, stdin);

  // Detach from shared memory.
  int ret = shmdt(myshmp);
  if (ret == -1)
  {
    perror("Could not detach from shared memory");
    exit(0);
  }
  printf("Detach successful, ret = %d\n", ret);

}


