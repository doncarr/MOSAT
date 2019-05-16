This will be a complete re-write of REACT.


First, this version will talk native http so that you will not need an Apache module or Node.js instance to redirect data requests to REACT. We will also support listening on a Unix socket so that for instance, NGINX could readirect data urls locally for instance. The payloads will all be JSON, but, we will consider adding other formats at a later date.

All of the point types will now become plugins so that no re-compile is needed to add a point type. We will also allow both C and C++ plugins, and, likely other languages at some point.

Each point type will thus declare attributes for configuration, and, real-time attributes that can change at runtime. 

For floating point and discrete calculations, we will eliminate the stack based expression evaluator and instead, for each calculation, generate C++ code and compile it to a shared object that can be loaded at runtime.

We need to consider if we should continue to support the main program directly supporting real-time displays. As an example, for a testing system, you could use the terminal (ncurses) to directly display real-time values during the test.

And, this gets us into the how much we should support "light-weight" applications vs "heavy back-end" applications.

We also need to decide if we still support two different modes that we have supported before: 

1) Back-end continuous running applications.
2) Time limited applications such as a test or single batches.

How should we do user security. Should we have compartments, or, only levels?

Another issue is how much we support run-time re-configuration. Before we did NOT support adding a new point type at run time, but, each point type could support changing configuration at runtime through a script. For instance, a PID control point could support changing a setpoint at runtime. If we DO support this, should we support letting a point type decide to save his state.

How do we support storing the configuration of a system. Before we only supported flat delimited text files. The initial plan is to switch to sqlite and support automatic translation between a database row and a C structure. But, this raises the question of supporting arbitrary data types like with JSON with arrays of structures and structures that contain structures.  

What support should there be for history? How should individual point types be required to support history? My initial thought is to have plugable history so that others can support different history, but, the history interface to individual database points should be defined.

How should we support an alarm manager? Previously, there was no alarm manager, it was up to individual database points to have fields to indicate alarms, and then a display could show alarms. Should we support the concept of "active", "acknowledged", severity ("alarm", "caution", "normal"). Should it be 100% the responsibility of individual point types to determine alarm states, or, should we allow the configuration in an alarm manager? I am leaning to a combination, where we can configure a new alarm on an attribure of an existing point type. 

This brings up the need to define standard attribute names and whether or not they should be case sensitive. 

All
  tag (text)
  description (text)
  aor_read (bits 64) // this is area of responsibility, to support compartmentalized security
  aor_write (bits 64) // for write operations 
  location_tag (text)  


Analog
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

Discrete
  pv (boolean)
  pv_string (text)
  low_description (text)
  high_description (text)
  alarm_state (enum - high,low,none)

Analog Input (extra attributes)
  raw_low (float)
  raw_high (float)
  eu_lo (float)
  eu_hi (float)
  //Should we support piecewize linear?
  //Should we support pluggable linearization? 
  //How should we support calibration?

Discrete Input (extra attributes)
  invert_input (boolean)

Location
  name (text) // short name that can be displayed, can contain spaces, puncutation.
  lattitude (float)
  longitude (float)
  long_description (text) (multi-line very long, describing hazards, how to enter, etc)
  // Should we support any real-time attributes here such as "door-open", "intrusion-detected"?
  // We could reference other discrete points for the attributes.

Driver
  name (text) // short name that can be displayed, can contain spaces, puncutation.
  // should we support the card/channel concept? 

How should we handle concurrency, and, where does the responsibility lie? Should the system only allow one request at a time for each point type, or should each point type be required to administer concurrency? Should we even allow general concurrency?

