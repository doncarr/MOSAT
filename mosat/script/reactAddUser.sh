#!/bin/bash
#Adding react's user and group

USER_ENTRY="react:x:12345:12345:\"React Control Engine\":/home/react:/bin/bash"
GROUP_ENTRY="react:x:12345:react"

#Adding User:
echo $USER_ENTRY >> /etc/passwd

#Adding Group
echo $GROUP_ENTRY >> /etc/group

#Updating /etc/passwd & /etc/group info
pwconv

#Setting default passwd (user should change it later...)
echo "cubiculo3" | passwd --stdin react &> /dev/null

#Creating react's home directory
mkdir /home/react
chown react:react /home/react

