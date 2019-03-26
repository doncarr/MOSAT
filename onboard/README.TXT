This is the program to run onboard of each train and enforce the speed limits on each section of the tracks. 

This is in development for pilot testing at SITEUR (Light Rail System Guadalajara).

Requirements:

gnuplot

To install under ubuntu/debian type on command line:
apt-get install gnuplot

The main program is "onboard" to run it type:

onboard <options>

Example use:
./onboard -s -cp -a 
then hit 'm' twice to select the line and station to start.

Command line options:

  1) '-s' to run the program in simulation mode.
  2) '-cp' to save the profiles to disk and display a graph with gnuplot after each seccion its completed.
  3) '-f <station_number>' to set the starting station (0 - n-1).
  4) '-l' free running mode. Used if you just want to see the graphs.
  5) '-L1' set to line 1 Guadalajara
  6) '-L2' set to line 2 Guadalaajra
  7) '-a' read all profiles (includeing low, high, very high), from
   line1_profile_all.txt. Otherwise, only desired is read from line1_profile.txt
     (note: soon we will ONLY read all profiles)

After you are running, there are options such as:

  1) 'a' to go into automatic mode, so the train departs each station automatically.
  2) 'm' to go back to manual mode, so you have to hit to depart each station.
  3) 'f' for fast mode = 10x real-time
  4) 's' for slow mode or real-time mode = 1x real-time.
  5) 'c' continue, to advance to the next section if the automatic dection of end of section does not work.
  6) 'e' toggle the automatic detection of end of section on/off.
  7) 'q' to quit.

You can resize the text window, and the diagram will resize automatically to fit.

There is now a new "drive" feature that allows you to "drive the train". We are trying to mimic the functionality of the train, where positions of the shift lever are a fixed acceleration. For now, we support four accelerations and a "coast" mode that has light deceleration due to wind resistance and friction losses. Here are the commands:

0 - coast, light deceleration.
1 - 0.5 m/s² acceleration
2 - 1.0 m/s² acceleration
9 - 0.5 m/s² deceleration
8 - 1.0 m/s² deceleration

This simulates the actual train control, where there is no "hold speed" position, and the driver must switch between light acceleration and coast to maintain for example 50 km/h.

There is also a fast moving bar that appears when you are within 100 meters of the stopping point, to simulate the driver being able to visually see the platform when approaching and ease into a stop. There is a '^' to mark the point where the nose of the train should stop.

To "drive" the train and then see how you did at the end of each section, run the following from the onboard directory:

./drive -cp -a

Notice that "drive" is escentially the same program as "onboard", but the speed is determined from the acceleration levels set and how long at each acceleration mode. Finally, the speed determined in this way, is then used to calculate the accumulated distance travelled. In this mode, the option -s (simulate) would not make any sense.

This program likely will be expanded to work with radios and do collision avoidance, tell the operator about crossing guards, operation like a poor mans CBTC.
