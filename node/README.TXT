

Install:

sudo apt-get install nodejs nodejs-dbg nodejs-dev npm 


How the webserver works

There are three special paths that are recognized:

   /react/tag
   /react/config
   /react/output

/react/tag is to return an array of values given an array tags. 

   The format of the request is /reac/tag?tag1+tag2+tag3+ . . . +tagN-1+tagN
   The format of the reply is in JSON: [value1,value2,value3, . . . ,valueN-1,valueN]
   The values can be either floating or boolean.
   So, and example reply would be: [3.5,true,-36.8,false,true,345.1]

To get the values, the web server must connect a socket to a some backend that gives real-time values

We could support other formats, but, for now, the request-reply format to the back-end is
basically the same as the webserver, but, it is a simple socke instead of http.

   So, the request is: "tag:tag1+tag2+tag3+ . . . +tagN-1+tagN"

Since this must be TOTALLY asynchronous
