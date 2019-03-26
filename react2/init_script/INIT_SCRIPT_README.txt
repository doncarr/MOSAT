
We are currently working on an init script for REACT based on the article below:

http://www.thegeekstuff.com/2012/03/lsbinit-script/

http://refspecs.linuxbase.org/LSB_3.1.0/LSB-Core-generic/LSB-Core-generic/iniscrptfunc.html

The idea is to conform to the LSBInit Standard and use the command "update-rc.d"

This will start react and set the home directory, userid, etc.


Unresolved issues:

1) Do we need a "reload" function, or, should we only support a "restart"?
   If we do support a reload, objects must support a "reload" function, that passes the new config information.
   But, we must delete objects that have been deleted, and create objects that are new?
   Is it doable?

2) For a restart, how do we give objects a chance to save their state and then restore it?
   Do we let each object open their own file and write information?
   Or, do we ask them to create a one line text buffer with the real-time information to be saved? Binary buffer?
   How long is too long? After a certain time, the restart data will be "stale", and we just do a normal start from scratch.
   Note: right now, no data is saved, other than objects that are logging to files periodically.

3) How to handle shutdown, so that systems with ramdisk copy files ramdisk to flash on shutdown, and flash to ramdisk on boot?
  How about create a "copy files" daemon that react depends on, so, it is started up before react to copy files to ramdisk, and stopped
  after react to copy files to flash. Or, should react just launch a process to do this? A command line parameter to react to tell
  it of the need to copy files a startup and shutdown?


