#!/bin/sh
TODAY=$(date +%Y%m%d)
YESTERDAY=$(date --date="$(date +%Y-%m-%d) -1 day" +'%Y%m%d')
for i in $(ls /mnt/rd/tina/log/*$YESTERDAY*.txt)  
do
   mv $i /home/artc/tina/log
done
