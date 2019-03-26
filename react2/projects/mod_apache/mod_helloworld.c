
/* Simple HelloWorld module
 *
 * The book presents lots of variants of this module to demonstrate
 * different points around writing a content-generator module and
 * working with the request_rec.  This is an attempt at a 'canonical'
 * version, broadly equivalent to the familiar CGI printenv.
 */

#include <httpd.h>
#include <http_protocol.h>
#include <http_config.h>

#include <pthread.h>
#include <sys/syscall.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>

#include <math.h>

static const char *dest_ip = "127.0.0.1";
static const short dest_port = 5707;
int sock_fd = -1;
int n_requests = 0;
sem_t sock_sem;
const char *init_message = "Init not called";

static n_calls = 0;

/*****************************************************/

static void connect_socket(void)
{
  struct sockaddr_in dest_addr; 

  sock_fd = socket(AF_INET, SOCK_STREAM, 0); 

  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(dest_port);
  dest_addr.sin_addr.s_addr = inet_addr(dest_ip);
  memset(&(dest_addr.sin_zero), 8, 0); 

  connect(sock_fd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
}

/*****************************************************/

static int printitem(void* rec, const char* key, const char* value) 
{
  /* rec is a userdata pointer.  We'll pass the request_rec in it */
  request_rec* r = rec ;
  ap_rprintf(r, "<tr><th scope=\"row\">%s</th><td>%s</td></tr>\n",
	ap_escape_html(r->pool, key), ap_escape_html(r->pool, value)) ;
  /* Zero would stop iterating; any other return value continues */
  return 1 ;
}

/*****************************************************/

static void printtable(request_rec* r, apr_table_t* t,
	const char* caption, const char* keyhead, const char* valhead) 
{

  /* print a table header */
  ap_rprintf(r, "<table><caption>%s</caption><thead>"
	"<tr><th scope=\"col\">%s</th><th scope=\"col\">%s"
	"</th></tr></thead><tbody>", caption, keyhead, valhead) ;

  /* Print the data: apr_table_do iterates over entries with our callback */
  apr_table_do(printitem, r, t, NULL) ;

  /* and finish the table */
  ap_rputs("</tbody></table>\n", r) ;
}

/*****************************************************/
static double tag_val = 10.0;
static int tag_dir = 1;

static int helloworld_handler(request_rec* r) 
{
  pthread_t self;
  pid_t lwp;

  if ( (r->handler == NULL) || (0 != strcmp(r->handler, "helloworld")) ) 
  {
    return DECLINED ;   /* none of our business */
  }
  if ( r->method_number != M_GET ) 
  {
    return HTTP_METHOD_NOT_ALLOWED ;  /* Reject other methods */
  }
  ap_set_content_type(r, "text/html;charset=ascii") ;

  lwp = syscall(SYS_gettid);
  self = pthread_self();

  sem_wait(&sock_sem);
  {
    if (sock_fd == -1) 
    {
      connect_socket();
      //ap_rprintf(r, "Socket connected: %d<br>\n", sock_fd);
    }
  }
  sem_post(&sock_sem);

  n_requests++;
  if (0 == strcmp(r->path_info, "/pump"))
  {
    char line[200];
    FILE *fp = fopen("/home/carr/trunk/react/projects/atemajac_pumps2/html/pump_diagram.html", "r");
    if (fp == NULL)
    {
      ap_rputs("<body><h1>Can not open file!</h1>", r) ;
      ap_rputs("</body></html>", r) ;
    }
    while (NULL != fgets(line, sizeof(line), fp))
    {
      ap_rputs(line, r) ;
    }
    ap_rputs("WOW!!", r) ;
    return OK ;
  }
  else if (0 == strcmp(r->path_info, "/script"))
  {
    ap_rprintf(r, "<title>Script Command:  %s</title>\n", r->args);
    ap_rprintf(r, "<body><h1>Command executed: %s</h1>\n", r->args);
    ap_rprintf(r, "<br>\n");
    sem_wait(&sock_sem);
    int n = write(sock_fd, r->args, strlen(r->args) + 1);
    if (n <= 0)
    {
      ap_rprintf(r, "Error: write returned %d, pid = %d, sockfd = %d, request = %d<br>\n", n, getpid(), sock_fd, n_requests);
      close(sock_fd);
      sock_fd = -1;
      sem_post(&sock_sem);
    }
    else
    {
      char buf[100];
      n = read(sock_fd, buf, sizeof(buf));
      buf[n] = '\0';
      if (n <= 0)
      {
        ap_rprintf(r, "Error: read returned %d, sockfd = %d, request = %d<br>\n", n, sock_fd, n_requests);
        close(sock_fd);
        sock_fd = -1;
        sem_post(&sock_sem);
      }
      else
      {
        sem_post(&sock_sem);
        ap_rprintf(r, "%d bytes read, pid = %d, lwp = %d, tid = %lu, sockfd = %d, request = %d<br>\n", n, getpid(), lwp, self, sock_fd, n_requests);
        ap_rputs(buf, r) ;
        ap_rprintf(r, "<br>\n");
      }
    }
    ap_rprintf(r, "%s\n", "<br><br><center>[<a HREF=\"#\" onClick=\"opener.focus(); window.close(); return false\">Cerrar ventana</a>] </center>"); 
    ap_rputs("</body></html>", r) ;
    return OK;
  }
  else if (0 == strcmp(r->path_info, "/test"))
  {
    ap_rputs("testing 1 2 3", r) ;
    return OK;
  }
  else if ((0 == strcmp(r->path_info, "/tag")) || (0 == strcmp(r->path_info, "/config")))
  {
    /**
    char buf[50];
    if (tag_dir)
    {
      tag_val += 1.67;
      if (tag_val > 300) tag_dir = !tag_dir;
    }
    else
    {
      tag_val -= 2.32;
      if (tag_val < 50) tag_dir = !tag_dir;
    }
    snprintf(buf, sizeof(buf), "%0.3lf", tag_val);
    ap_rputs(buf, r) ;
    return OK;
     ***/
    /***/
    char buf[500];
    snprintf(buf, sizeof(buf), "%s:%s", r->path_info + 1, r->args); 
    //ap_rputs(buf, r);
    sem_wait(&sock_sem);
    int n = write(sock_fd, buf, strlen(buf));
    if (n <= 0)
    {
      ap_rputs("{\"pv\":99.99,\"eulabel\":\"err\"}", r);
      close(sock_fd);
      sock_fd = -1;
      sem_post(&sock_sem);
      return OK;
    }
    else
    {
      n = read(sock_fd, buf, sizeof(buf));
      buf[n] = '\0';
      if (n <= 0)
      {
        ap_rputs("77.77", r) ;
        close(sock_fd);
        sock_fd = -1;
        sem_post(&sock_sem);
        return OK;
      }
      else
      {
        sem_post(&sock_sem);
        ap_rputs(buf, r) ;
        return OK;
      }
    }
    /***/
  }

  ap_rputs(
	"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\">\n", r) ;
  ap_rputs(
	"<html><head><title>Apache HelloWorld Module</title></head>", r) ;
  ap_rputs("<body><h1>Hello Cruel World!</h1>", r) ;
  ap_rputs("<p>This is the Apache HelloWorld module!</p>", r) ;

  /* Print the headers and env */
  /* Note that the response headers and environment may be empty at
   * this point.  The CGI environment is an overhead we dispense
   * with in a module, unless another module that requires it
   * (e.g. mod_rewrite) has set it up.
   */
  printtable(r, r->headers_in, "Request Headers", "Header", "Value") ;
  printtable(r, r->headers_out, "Response Headers", "Header", "Value") ;
  printtable(r, r->subprocess_env, "Environment", "Variable", "Value") ;

  ap_rprintf(r, "<br>\n");
  ap_rprintf(r, "init_message: %s <br>\n", init_message);
  ap_rprintf(r, "the_request: %s <br>\n", r->the_request);
  ap_rprintf(r, "protocol: %s <br>\n", r->protocol);
  ap_rprintf(r, "hostname: %s <br>\n", r->hostname);
  ap_rprintf(r, "status_line: %s <br>\n", r->status_line);
  ap_rprintf(r, "method: %s <br>\n", r->method);
  ap_rprintf(r, "range: %s <br>\n", r->range);
  ap_rprintf(r, "content_type: %s <br>\n", r->content_type);
  ap_rprintf(r, "vlist_validator: %s <br>\n", r->vlist_validator);
  ap_rprintf(r, "user: %s <br>\n", r->user);
  ap_rprintf(r, "ap_auth_type: %s <br>\n", r->ap_auth_type);
  ap_rprintf(r, "handler: %s <br>\n", r->handler);
  ap_rprintf(r, "content_encoding: %s <br>\n", r->content_encoding);
  ap_rprintf(r, "vlist_validator: %s <br>\n", r->vlist_validator);
  ap_rprintf(r, "unparsed_uri: %s <br>\n", r->unparsed_uri);
  ap_rprintf(r, "uri: %s <br>\n", r->uri);
  ap_rprintf(r, "filename: %s <br>\n", r->filename);

  ap_rprintf(r, "canonical_filename: %s <br>\n", r->canonical_filename);
  ap_rprintf(r, "connection->remote_ip: %s <br>\n", r->connection->remote_ip);
  ap_rprintf(r, "connection->remote_host: %s <br>\n", r->connection->remote_host);
  ap_rprintf(r, "connection->remote_logname: %s <br>\n", r->connection->remote_logname);
  ap_rprintf(r, "connection->local_ip: %s <br>\n", r->connection->local_ip);
  ap_rprintf(r, "connection->local_host: %s <br>\n", r->connection->local_host);

  ap_rprintf(r, "path_info: %s <br>\n", r->path_info);
  ap_rprintf(r, "args: %s <br>\n", r->args);

  ap_rprintf(r, "server->defn_name: %s <br>\n", r->server->defn_name);
  ap_rprintf(r, "server->server_hostname: %s <br>\n", r->server->server_hostname);
  ap_rprintf(r, "server->error_fname %s <br>\n", r->server->error_fname);
  ap_rprintf(r, "server->path %s <br>\n", r->server->path);

  n_calls++;
  ap_rprintf(r, "n_calls: %d<br>\n", n_calls);
  ap_rprintf(r, "PID = %d, posix tid = %lu, lwp/spid = %d<br>\n", getpid(), self, lwp);

  ap_rprintf(r, "<h1>Testing!!!!</h1><br>\n");
  ap_rprintf(r, "<br>\n");
  

  ap_rputs("</body></html>", r) ;
  return OK ;
}

/*****************************************************/

static int init_hook_n_calls=0; 
static char init_hook_msg[100];

static void helloworld_init_handler(apr_pool_t *pchild, server_rec *s) 
{
  init_hook_n_calls++;
  sem_init(&sock_sem, 0, 1);
  snprintf(init_hook_msg, sizeof(init_hook_msg), 
                "Init called, semaphore initialized, n = %d, pid = %d", 
                 init_hook_n_calls, getpid());
  init_message = init_hook_msg; 
}

/*****************************************************/

/* Hook our handler into Apache at startup */
static void helloworld_hooks(apr_pool_t* pool) 
{
  ap_hook_handler(helloworld_handler, NULL, NULL, APR_HOOK_MIDDLE) ;
  ap_hook_child_init(helloworld_init_handler, NULL, NULL, APR_HOOK_MIDDLE);
}

/*****************************************************/

module AP_MODULE_DECLARE_DATA helloworld_module = 
{
        STANDARD20_MODULE_STUFF,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        helloworld_hooks
} ;

/*****************************************************/
