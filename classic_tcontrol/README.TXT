

This is our classic train control project.

To run, you need to type "./sim"

The keys you can use are:

1) 'f', to run at about 10x real-time

2) 's' to run "slow", or real-time, 1 second = 1 second (very boring)

3) 'l' or "libre" in Spanish. Free Running in English, it will run as FAST as it can.

4) 'i' to insert another train. Be carefull, if you instert the train at the wrong time, it will give an error and exit.

Command line options:

1) -nc : No control. With this, there is no control of distance between trains, only secutity.
	If you run it long enough with no control, all the trains will be lined up behind the slowest one.

3) -jg : Use the control algorithm for Jorge Gutierez as one of the safety algrorithms. He implemented it 
	slightly differently (on purpose) to only give a green light if there was a train in the previous section.
	Other algorithms always give a green light when it is safe to enter a section, regardless if there is a train
 	that could enter.


Notice: When I say "green" light, train systems never use red and green lights so that threre is no
	possibility they will be confused with stoplights for car traffic. "green" just means whatever
	light is used to indicate that it is possible to enter.

