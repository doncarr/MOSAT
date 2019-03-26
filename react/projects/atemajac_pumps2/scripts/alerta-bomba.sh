#!/bin/bash
cont=0
while true; 
do 
    alert=$(cat /home/react/public_html/log/atemajac/pump_diagram.html  | grep embed | grep redalert.wav)
    if [ ! -z "$alert" ]
    then 
        echo "Alarma!!!! :-("
	# Debug
	# echo "Alerta: -- $alert"
	# echo "Contador de sonido: -- $cont"
	if [ $cont -lt 30 ]
	then
        	play /home/react/public_html/log/atemajac/redalert.wav
		cont=$(echo $cont + 1  | bc)
	fi
    else
	cont=0
	echo "No Alarma.... :-)"
    fi  
    sleep 2 
done
