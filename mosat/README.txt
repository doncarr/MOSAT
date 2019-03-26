This is a Software for monitoring the departures of a Light Rail Train.

To have this software running you need to have some specific hardware such as:
At least 1 RTU (Remote Terminal Unit)
...
<List of hardware>
...

Also you need some extra software you might not have:
gcc
make
powerpc-linux-gcc
apache web server
gnuplot
...
<list of software>
...

When you have this requirements the you are ready to start.

The first step is to copy the include files in /usr/local/include or anywhere your compiler can find the libraries, then you're ready to run make on the lib directory, this is in order to obtain the library librtcommon.so. When this library is generated you need to copy it into /usr/local/lib, then append this line to the file /etc/ld.so.conf:
/usr/local/lib and run ldconfig. Ever command but the make are meant to be runned as root.

When this steps are done you're ready to compile.

Type make on the trunk directory.

When compilation is finished you need to do an ftp to the rtu's and copy ppc_recordi, dump_events and mosat_sync files on the /bin directory and copy the script on siteur.sh from the script directory to <place directory here> directory in the rtu.

You also need to make some configuration files no the rtu to make work. This
files are:
rtu_channel_config.txt
recordlog.txt
<here will be a description of every file, its contents and its location>

Before you can run this application you need to write (or get) the files with the departures for every kind of workday (laboral.txt, sabado.txt, domingo.txt), and place them in their respective directories inside the apache home directory.
<here will be a description of every file, its contents and its location>

<some steps skiped>

to start generating the reports you need to run ./runit ./ntt and ./autoreport and then open your web browser and look for the html report on <your ip><directori>
