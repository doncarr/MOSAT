This will be a complete re-write of REACT.

First, this version will talk native http so that you will not need an Apache module or Node.js instance to redirect data requests to REACT. We will also support listening on a Unix socket so that for instance, NGINX could readirect data urls locally. The payloads will all be JSON, but, we will consider adding other formats at a later date.

All of the point types will now become plugins so that no re-compile is needed to add a point type. We will also allow both C and C++ plugins, and, likely other languages at some point.

Each point type will thus declare attributes for configuration, and, real-time attributes that can change at runtime. 

For floating point and discrete calculations, we will eliminate the stack based expression evaluator and instead, for each calculation, generate C++ code and compile it to a shared object that can be loaded at runtime.

We need to consider if we should continue to support the main program directly supporting real-time displays. As an example, for a testing system, you could use the terminal (ncurses) to directly display real-time values during the test.

And, this gets us into the how much we should support "light-weight" applications vs "heavy back-end" applications.

------------------
scripting

How should we support a scripting language? Should we support remote scripts that send one line at a time to system?

Each point type should expose functions that can be executed as part of a script.

We could create object types to facilitate complex control sequences rather than trying to do it all in a script. In this case, a script could kick send a message to an object to start the process.

Could we even create an object on the fly to do an operation?

Scripts for bach processing or testing applications?
------------------

We will require testing for all point types and in general all changes to REACT.

-------------------
Modes of operation
We also need to decide if we still support two different modes that we have supported before: 

1) Back-end continuous running applications.
2) Time limited applications such as a test or batches.

Though, there might be a system that runs 24 hours a day that needs to kick off a bach in part of the sysystem, so, there are mixes of things.
---------------------

--------------
History
How should we do user security. Should we have compartments, or, only levels? Compartments based on AOR?

In any case, users will be authenticated and we will be able to determine the currently logged in user from the http headers, so this to a certain extent implies an external system to authenticate users.
--------------

Another issue is how much we support run-time re-configuration. Before we did NOT support adding a new point type at run time, but, each point type could support changing configuration at runtime through a script. For instance, a PID control point could support changing a setpoint at runtime. If we DO support this, should we support letting a point type decide to save his state.

How do we support storing the configuration of a system. Before we only supported flat delimited text files. The initial plan is to switch to sqlite and support automatic translation between a database row and a C structure. But, this raises the question of supporting arbitrary data types like with JSON with arrays of structures and structures that contain structures.  

What support should there be for history? How should individual point types be required to support history? My initial thought is to have plugable history so that others can support different history, but, the history interface to individual database points should be defined. 

--------------
History
What about real-time history where you have a small window of history into the past that can be immmediately accessed when an event is detected. The even could be a sudden pressure change in a pipeline, or, a transmission shifting. When an even occurs, you might want to go into the past and the future. So, keep say 15 seconds into the past, and 15 seconds into the future. For a transmission shift, this might be more like 500 miliseconds into the past and 500 miliseconds into the future.

Typical history types:

1) instantaneous: An array of recent values over the last hour or minutes. The time interval between these scans depends on the scan rate. On a "fast" system, you would have less time to avoid using too much memory. On a "slow" system, you might have enough memory for more time. A slower running SCADA system might keep a running window of the last hour of readings for each point.
2) Hour averages / totals
3) Day averages / totals
4) Week averages / totals
5) Month averages / totals
6) Yearly averages / totals

History storage.
We typically get a scan of values multiple inputs that we store together, but, to retrieve, we want to read history by columns, in other words, an array of values for a single input so that we can graph it. So, how to we both make storage and retrieval of history efficieant?

Errors in totalization and averageing of history. For some inputs like gas flow, we want to know how much total gas flows past a certain point, thus you multiply flow rate by elapsed time to get total cubic feet for a period of time. The problem is the limited sampling rate from the server will result in some error in performing this integration. To solve this, the local device can sample at a much higher rate to get a much better total, then, transmit the total at the end of the hour for instance. The trick is to allow an interface to correct the total at the end of each hour. You can also return the hour total so far on each read so that you have a more accurate up to the minute total, and, so that the total does not jump a little bit at the end of the hour.
-------------------
Alarms

How should we support an alarm manager? Previously, there was no alarm manager, it was up to individual database points to have fields to indicate alarms, and then a display could show alarms. Should we support the concept of "active", "acknowledged", severity ("alarm", "caution", "normal"). Should it be 100% the responsibility of individual point types to determine alarm states, or, should we allow the configuration in an alarm manager? I am leaning to a combination, where we can configure a new alarm on an attribure of an existing point type, but, also let point types detect their own alarms. 

Alarm manager design issues: Under high alarm load, how do we limit the number of messages to clients? There has to be a tradeoff between immediately dispaching alarms and the total number of messages sent. One algorithm would be to send out an alarm immediately if there has been no message sent in the last 5 seconds, otherwise, queue the current alarm for up to 5 seconds total from the last time an alarm was sent waiting for more alarms to send along at the same time. Thus, there would be a maximum of one message every 5 seconds to each client, keeping the system and network from overloading when something happens to trigger a lot of alarms. Also the alarm manager should incorporate AOR (area of responsibility), to filter the alarms sent to each client. Could we use Redis to cache alarms?

How should the alarms be cached for the current users? There could be hundreds of alarms that are active (more than can be displayed to the user), such that he must scroll through them. So, should they be cached locally, or, on the server. I would go for on the server, as, the local copy could become stale, even after a minute or so, and when you page down, you should ask the server for the next group to display. and, this is standard web services practice to limit the number of items requested in a single call, to that you can then do a "next" with the last one returned in the previous call. Obviously, if the user were to change the sort order, it would require another request to the server.

Alarm display Types
1) Active
2) Unacknowledged
3) Acknowledged

---------------

This brings up the need to define standard attribute names and whether or not they should be case sensitive. 

How should we allow inheritance? Should we have an interface concept like Java? Should we have both inheritance and interfaces? Should we allow multiple inheritance?

Attributes for ALL points
  tag (tag) // tag is a string that identifies a point
  description (text)
  aor_read (bits 64) // this is area of responsibility, to support compartmentalized security
  aor_write (bits 64) // for write operations 
  location_tag (tag)

Attributes for Analog
  pv (float)
  pv_string (text)
  high_alarm (float)
  high_alarm_enable
  high_caution (float)
  high_caution_enable
  low_caution (float)
  low_caution_enable
  low_alarm (float)
  low_alarm_enable
  alarm_deadband (float)
  zero_cutoff (float)
  decimal_points (int)
  min_scale (float)
  max_scale (float)
  --
  set_pv(float pv)

Discrete
  pv (boolean)
  pv_string (text)
  low_description (text)
  high_description (text)
  caution_state (enum - high,low,none) If there is an alarm_state, then this should be "none"
  alarm_state (enum - high,low,none) If there is a caution_state, then this should be "none"
  --
  set_pv(bool pv)

Int
  pv (int)
  pv_string (text)
  --
  set_pv(int pv)

Enum
  pv (int) 
  pv_string (text)
  pv_values (array of {int, text}) We need to specify all possible states and the value for each.
  caution_state (text)  blank for none.
  alarm_state (text) blank for none. 
  --
  --
  set_pv(int pv) // must be one of the integer values or will fail
  set_pv_string(text pv) // must be one of the strings or will fail

Analog Input (extra attributes)
  driver (text) // tag of the driver
  channel (text) // channel specification is unique to each driver.
                 // how to control editing and verifying the channel?
  raw_low (float)
  raw_high (float)
  eu_lo (float)
  eu_hi (float)
  //Should we support piecewize linear?
  //Should we support pluggable linearization? 
  //How should we support calibration?

Discrete Input (extra attributes)
  driver (tag) // tag of the driver
  channel (text) // channel specification is unique to each driver.
  invert_input (boolean)

Analog Control
  ai_point (tag)
  ao_point (tag)
  setpoint_limit_hi (float)
  setpoint_limit_lo (float)
  setpoint (float)
  deviation_alarm (float)
  deviation_alarm_enable (bool)
  deviation_caution (float)
  deviation_caution_enable (bool)
  scale_low (float)
  scale_high (float)
  --
  change_setpoint(float new_setpoint, float ramp_time)
  start_control()
  stop_control()

PID Control
  p_gain (float)
  i_time (float)
  d_time (float)

Location
  name (text) // short name that can be displayed, can contain spaces, puncutation.
  lattitude (float)
  longitude (float)
  long_description (text) (multi-line very long, describing hazards, how to enter, etc)
  // Should we support any real-time attributes here such as "door-open", "intrusion-detected"?
  // We could reference other discrete points for the attributes.

Redis? Can we leverage Redis to support possibly hundreds of clients reading real-time data? Thus, react would feed Redis with real-time values, and then, a cluster of Redis servers could support clients. How would we batch updates in real-time to a remote server.

Driver
  name (text) // short name that can be displayed, can contain spaces, puncutation.
  // should we support the card/channel concept? 

How should we handle concurrency, and, where does the responsibility lie? Should the system only allow one request at a time for each point type, or should each point type be required to administer concurrency? Should we even allow general concurrency?

------------
Blockchain interface.

So, the "control system" should have a signing key such that, when certain operations are performed, the system will log that operation onto a blockchain, with the user ID of the person that triggered it. You could possibly use a "smart bage" system so that you would have to hold your bage close to a scanner to authorize the operation. Thus, the item put on the block chain would be signed by the control system, AND the employee.

This system would NOT be or include a blockchain, rather, it would interface to blockchains.

Alternatively, for less critical things, it could be set up to just log to a file all operations and the employeee that initiated them.
--------------
How to reference values, and write values.

There are a number of different ways for a point type to reference other point types or values that a driver reads from the real world.

Publish/Subscribe
Values are pushed
Values are pulled

We could let individual point types ask for a reference and maintain those references privately, use them to read values on demand, but, then, we do not know what each point is depending on.   


There is also a desire to be able to draw a dependency graph.

There is also the issue of overloading the system with updates. A point that references multiple values should be updated ONLY once when that group of values updates, NOT once for every referenced value that changes. I was personally involved with a system where the user that had a widget that would turn red when there are any alarms, so you could click on it, and jump to a map with regions, and each region would be either red or green depending on if there was an alarm in that region. Finally, you could click on a region and see the sub-regions, and so on. So, we really ONLY want to update after each scan, and, then ONLY if something changed. On a "fast" system, you could run these kinds of updates once after ALL points have been scanned. On a "slow" system, you would want to run after each group was scanned. But, then, you only want to propegate calculations if something changed. 
-----------------
Types of I/O

The typical way to read values from the real-world would be to periodically ask remove devices to "give me your values", and then receive those values and update the points that depend on those values.

Another way is to let the devices "call in" with new values every once it a while, and, possibly just send a "no values changed" or "I'm OK"  message if nothing changed. With this method, the system will need to trigger a "device down" if messages do not arrive as expected. 
---------------
Remote processing vs local processing.

-----------------
Default Panels for each point type.

We should build a default panel for each point type, so, you can enter a tagname, and, pop a panel to see the real-time values and configuration for a point. The panel can also have a way to edit those configurations that it makes sense to edited at run-time.

If the creator does not have the time to create a default panel then we should create a basic one automatically. Thus we will require the creator of a point type to AT LEAST list parameters that are for configuration, and those that are run-time, and finally, those that it would make sense to let a user edit at runtime. 

Thus, for different UIs / Widget sets, we could at least auto-create a default panel.

-----------------
