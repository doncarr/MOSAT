#!/bin/sh
echo "Starting siteur record deamon ..."
/usr/local/bin/ppc_record > /dev/null 2>/dev/null &
echo "Siteur record deamon started"
echo "Starting siteur sync deamon ..."
/usr/local/bin/mosatsync > /dev/null 2>/dev/null &
echo "Siteur sync deamon started"
