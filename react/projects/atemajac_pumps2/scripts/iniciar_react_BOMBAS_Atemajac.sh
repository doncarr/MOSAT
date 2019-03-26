#!/bin/bash
echo "Iniciar react"
cd ~/react
gnome-terminal --hide-menubar --geometry 40x40+670+200 -e "/home/react/bin/alerta-bomba.sh"
gnome-terminal --hide-menubar --geometry 80x40+0+200 -e "react -x"
echo "fin"
