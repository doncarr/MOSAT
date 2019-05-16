This will be a complete re-write of REACT.

First, this version will talk native http so that you will not need an Apache module or Node.js instance to redirect data requests to REACT. We will also support listening on a Unix socket so that for instance, NGINX could readirect data urls locally for instance. The payloads will all be JSON, but, we will consider adding other formats at a later date.

All of the point types will now become plugins so that no re-compile is needed to add a point type. We will also allow both C and C++ plugins, and, likely other languages at some point.

Each point type will thus declare attributes for configuration, and, real-time attributes that can change at runtime. 

For floating point and discrete calculations, we will eliminate the stack based expression evaluator and instead, for each calculation, generate C++ code and compile it to a shared object that can be loaded at runtime.




