If you want to build for arm/ppc processors you must build librtcommon.so for
those processors.. 

read librtcommon/README.TXT

arm  processors build steps:

make -f makefile.arm


ppc processors build steps:

make -f makefile.ppc


To see an example, run the following:

./react control_pumps.txt

Then, open FireFox, and open trunk/react/html/pump_diagrm.html

You will see the simulated animation of a pumping station.

