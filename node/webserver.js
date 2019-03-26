var http = require("http");
var url = require("url");
var path = require("path");
var fs = require("fs");
var net = require("net");

var react_port = 5707;
var react_host = "localhost";
var html_root = __dirname + "/../svg";

var port = process.argv[2] || 8888;
process.chdir(html_root);

var resp = new Array();

console.log("Connecting to " + react_host + ", port " + react_port);

function connected()
{
  console.log('connected to REACT at ' + react_host + ', port ' + react_port);
};

var client = net.createConnection(react_port, react_host, connected); 

client.on('error', function(e) {
  console.log("Socket Error!!!");
  console.log(e.toString());
});

var np = 0;
client.on('data', function(data) {
  console.log("sock data: " + data);
  var bstart = 0;
  var i;
  for (i=0; i < data.length; i++)
  {
    if (data[i] == 0)
    {
      resp[0].write(data.slice(bstart, i));
      resp[0].end();
      resp.shift();
      bstart = i + 1;
      console.log("'\\0' found: np = " + np + " bstart = " + 
        bstart + " len = " + data.length + " backlog = " + resp.length);
    }
  }
  if (bstart < (data.length - 1))
  {
    // Ok, there was a partial message in the buffer,
    //   we write the partial and wait for more.
    np++;
    resp[0].write(data.slice(bstart, data.length - 1));
    console.log("partial message left: bstart = " + 
      bstart + " len = " + data.length + " backlog = " + resp.length);
  }
});


client.on('end', function() {
  console.log('client disconnected');
});

http.createServer(function(request, response) {
  var purl = url.parse(request.url);
  var uri = purl.pathname; 
  if (uri == "/react/tag")
  {
    console.log("REACT tag API");
    var rq = 'tag:' + purl.query + '\0';
    console.log("data: " + rq);
    try 
    {
      client.write(rq);
    }
    catch (err) 
    {
      console.log("Caught socket write error");
      console.log(err);
      response.writeHead(200,{ "Content-Type": "text/plain"});
      response.write("[]");
      response.end();
      return;
    }
    resp.push(response);
    return;
  }
  else if (uri == "/react/config")
  {
    console.log("REACT config API");
    var rq = 'config:' + purl.query + '\0';
    console.log("data: " + rq);
    try 
    {
      client.write(rq);
    }
    catch (err) 
    {
      console.log("Caught socket write error");
      console.log(err);
      response.writeHead(200,{ "Content-Type": "text/plain"});
      response.write("{}");
      response.end();
      return;
    }
    resp.push(response);
    return;
  }
  else if (uri == "/react/output")
  {
    console.log("REACT output API");
    var rq = 'output:' + purl.query + '\0';
    console.log("data: " + rq);
    try
    {
      client.write(rq);
    }
    catch (err) 
    {
      console.log("Caught socket write error");
      console.log(err);
      response.writeHead(200,{ "Content-Type": "text/plain"});
      response.write("Error");
      response.end();
      return;
    }
    resp.push(response);
    return;
  }
  var filename = path.join(process.cwd(), uri);
  path.exists(filename, function(exists) {
    if(!exists) {
      response.writeHead(404, {"Content-Type": "text/plain"});
      response.write("404 Not Found\n");
      response.end();
      return;
    }
 
	if (fs.statSync(filename).isDirectory()) filename += '/index.html';
 
    fs.readFile(filename, "binary", function(err, file) {
      if(err) {        
        response.writeHead(500, {"Content-Type": "text/plain"});
        response.write(err + "\n");
        response.end();
        return;
      }
 
      response.writeHead(200);
      response.write(file, "binary");
      response.end();
    });
  });
}).listen(parseInt(port, 10));
 
console.log("Static file server running at\n  => http://localhost:" + port + "/\nCTRL + C to shutdown");

console.log("arg[0] = " + process.argv[0]);
console.log("arg[1] = " + process.argv[1]);
console.log("arg[2] = " + process.argv[2]);
