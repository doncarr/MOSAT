This is the library that is common for most AdaptiveRTC programs. You must first copy all of the include files in ../include to /usr/local/include if that has not already been done. To install this library, first run "make" in this directory. Then, as root, copy the file librtcommon.so to /usr/local/lib. Finally, as root, run ldconfig. You are now ready to use the library. 

Depending on the system, you may have to add the following line to /etc/ld.so.conf:

/usr/local/lib

REQUERIMENTS:

To build library for arm processors http://www.embeddedarm.com/
Download crosstools from: ftp://ftp.embeddedarm.com/ts-arm-sbc/ based on your
table and install it. under / 


arm processors build steps:

# wget ftp://ftp.embeddedarm.com/ts-arm-sbc/ts-7400-linux/cross-toolchains/crosstool-linux-gcc-4.0.1-glibc-2.3.5.tar.bz2
# cd /
# tar -xjf crosstool-linux-gcc-4.0.1-glibc-2.3.5.tar.bz2
# export PATH=/opt/crosstool/gcc-4.0.1-glibc-2.3.5/arm-unknown-linux-gnu/bin/:$PATH 
# make armcall 

ppc processors build steps:

TODO



Then run ldconfig again.


