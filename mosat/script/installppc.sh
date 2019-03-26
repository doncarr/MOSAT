#!/bin/bash
echo "You need to be root to run this scritp, are you root? (y/n)"
read answer
if [ "$answer" = n ]
then
  echo "Become root and try again..."
  exit 0
fi
mkdir -p /opt/sixnet;
if [ "$?" -ne 0 ]
then
  echo "Cannot create directory, try being root next time"
  exit 1
fi
echo "done"
echo "copying files..."
cp sxiadk.tar.gz /opt/sixnet/;
if [ "$?" -ne 0 ]
then
  echo "The sxiadk.tar.gz is not in this directory"
  exit 1
fi
echo "sxiadk.tar.gz ...done"
cp sxlib.tar.gz /opt/sixnet/;
if [ "$?" -ne 0 ]
then
  echo "The sxlib.tar.gz is not in this directory"
  exit 1
fi
echo "sxlib.tar.gz ...done"
cp sxsamples.tar.gz /opt/sixnet/;
if [ "$?" -ne 0 ]
then
  echo "The sxsamples.tar.gz is not in this directory"
  exit 1
fi
echo "sxsamples.tar.gz ...done"
echo "Moving to /opt/sixnet directory"
cd /opt/sixnet/;
echo " ...done"
echo "Uncompressing files"
tar zxf sxiadk.tar.gz
tar zxf sxlib.tar.gz
tar zxf sxsamples.tar.gz
echo "Moving to /usr/bin"
cd /usr/bin
ln -s /opt/sixnet/bin/powerpc-cpp powerpc-cpp
ln -s /opt/sixnet/bin/powerpc-gcjh powerpc-gcjh
ln -s /opt/sixnet/bin/powerpc-jcf-dump powerpc-jcf-dump
ln -s /opt/sixnet/bin/powerpc-jv-scan powerpc-jv-scan
ln -s /opt/sixnet/bin/powerpc-linux-addr2line powerpc-linux-addr2line
ln -s /opt/sixnet/bin/powerpc-linux-ar powerpc-linux-ar
ln -s /opt/sixnet/bin/powerpc-linux-as powerpc-linux-as
ln -s /opt/sixnet/bin/powerpc-linux-c++ powerpc-linux-c++
ln -s /opt/sixnet/bin/powerpc-linux-c++filt powerpc-linux-c++filt
ln -s /opt/sixnet/bin/powerpc-linux-chill powerpc-linux-chill
ln -s /opt/sixnet/bin/powerpc-linux-g++ powerpc-linux-g++
ln -s /opt/sixnet/bin/powerpc-linux-g77 powerpc-linux-g77
ln -s /opt/sixnet/bin/powerpc-linux-gcc powerpc-linux-gcc
ln -s /opt/sixnet/bin/powerpc-linux-gcj powerpc-linux-gcj
ln -s /opt/sixnet/bin/powerpc-linux-ld powerpc-linux-ld
ln -s /opt/sixnet/bin/powerpc-linux-nm powerpc-linux-nm
ln -s /opt/sixnet/bin/powerpc-linux-objcopy powerpc-linux-objcopy
ln -s /opt/sixnet/bin/powerpc-linux-objdump powerpc-linux-objdump
ln -s /opt/sixnet/bin/powerpc-linux-protoize powerpc-linux-protoize
ln -s /opt/sixnet/bin/powerpc-linux-ranlib powerpc-linux-ranlib
ln -s /opt/sixnet/bin/powerpc-linux-readelf powerpc-linux-readelf
ln -s /opt/sixnet/bin/powerpc-linux-size powerpc-linux-size
ln -s /opt/sixnet/bin/powerpc-linux-strings powerpc-linux-strings
ln -s /opt/sixnet/bin/powerpc-linux-strip powerpc-linux-strip
ln -s /opt/sixnet/bin/powerpc-linux-unprotoize powerpc-linux-unprotoize
echo "...done"
