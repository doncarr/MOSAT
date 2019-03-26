
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mtype.h"
#include "modio.h"
#include "hprtu.h"
#include "trace.h"
#include "evlog.h"
#include "ppc_logger.h"

/********************************************************************/
 
mod_tcpip_client_t::mod_tcpip_client_t(int a_socket,  
		sockaddr_in *a_dest_addr)
{
  sock = a_socket;
  the_dest_addr = *a_dest_addr;
  send_ptr = 6; /* leave room for the MBAP header */
  recv_ptr = 6; /* leave room for the MBAP header */
  total_read = 0;
  total_written = 0;
  transaction_id = 0;
  protocol_id = 0;
}

/********************************************************************/

void mod_tcpip_client_t::reconnect(void)
{

	/**
  react_trace.dprintf(0, "To reconnect hit any key: ");
  char buf[100];
  fgets(buf, sizeof(buf), stdin);
        **/

  time_t down_time = time(NULL);
  evlog_print_event(EV_NETDN, "*** Network Down", down_time);
  /* Ok, first shutdown the socket */
  react_trace.dprintf(0, "Trying to reconnect . . . .\n");
  int err;
  /***/
  err = shutdown(sock, SHUT_RDWR);
  if (err != 0)
  {
    react_trace.dperror(6, "shutdown");
  }
  if (0 != close(sock))
  {
    react_trace.dperror(6, "close");
  }
  /***/

  sock = socket(AF_INET, SOCK_STREAM, 0); 
  if (sock < 0)
  {
    react_trace.dperror(6, "Can't create socket");
    exit(0);
  }
  struct timeval tv;
  tv.tv_sec = 2;
  tv.tv_usec = 0;
  err = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  if (err != 0)
  {
    react_trace.dperror(6, "setsockopt");
  }
  err = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
  if (err != 0)
  {
    react_trace.dperror(6, "setsockopt");
  }
   

  int n_tries = 0;
  int sleep_time = 5;
  while (true)
  {
    /* Ok, now try to reconnect */
    int n = connect(sock, (struct sockaddr *)&the_dest_addr, 
		    sizeof(struct sockaddr));
    if (n != 0)
    {
      react_trace.dperror(6, "Can't connect to port");
    }
    else
    {
      react_trace.dprintf(0, "connected\n");
      time_t up_time = time(NULL);
      evlog_print_event(EV_NETUP, "^^^ Network Up", up_time);
      evlog_print_log(EV_NETFAIL, "NETFAIL", down_time, up_time, 0, 0, 0);
      break;
    }
    n_tries++;
    if (n_tries > 10)
    {
      // After 10 tries, only try every minute to reconnect
      sleep_time = 60;
    }
    sleep(sleep_time);
  }
}

/********************************************************************/
 
int mod_tcpip_client_t::wait_message(void)
{
  react_trace.dprintf(0, "Waiting for a message . . . \n");
  //recv_buffer[total_read] = '\0';
  //react_trace.dprintf("Got %d bytes: %s\n", total_read, recv_buffer);
  while (true)
  {
  int n1 = recv(sock, recv_buffer, 6,  MSG_WAITALL);
  react_trace.dprintf(0, "Total bytes in reply: %d\n", n1);
  if (n1 <= 0)
  {
    react_trace.dperror(6, "recv");
    react_trace.dprintf(0, "recv returned: %d \n", n1);
    reconnect();
    send_message();
    continue; 
  }
  else if (n1 < 6)
  {
    react_trace.dprintf(0, "Message is too short: %d\n", n1);
    reconnect();
    send_message();
    continue; 
  }
  recv_ptr = 6; /* start reading after the MBAP header */

  uint16 tmp16;

  /* Transaction ID */
  memcpy(&tmp16, recv_buffer, 2);
  swap16(&tmp16);
  react_trace.dprintf(0, "Trans id = %d, ", (int) tmp16);
  if (transaction_id != tmp16)
  {
    react_trace.dprintf(0, "Error, transaction id in received message did not match\n");
  }

  /* Potocol ID */
  memcpy(&tmp16, recv_buffer + 2, 2);
  swap16(&tmp16);
  react_trace.dprintf(0, "Protocol id = %d, ", (int) tmp16);
  if (protocol_id != tmp16)
  {
    react_trace.dprintf(0, "Error, protocol id in received message did not match\n");
  }

  /* Length */
  /* Ok, the length can be different for the reply */
  memcpy(&length, recv_buffer + 4, 2);
  swap16(&length);
  react_trace.dprintf(0, "Length = %d, ", (int) tmp16);

  int n2 = recv(sock, recv_buffer + 6, length,  MSG_WAITALL);
  if (n2 <= 0)
  {
    react_trace.dperror(0, "recv");
    reconnect();
    send_message();
    continue; 
  }
  else if (n2 < length)
  {
    react_trace.dprintf(0, "Message is too short: %d\n", n2);
    reconnect();
    send_message();
    continue; 
  }
  total_read = n1 + n2;

  /* Unit ID */
  uint8 tmp8;
  memcpy(&tmp8, recv_buffer + 6, 1);
  react_trace.dprintf(0, "unit id = %d, ", (int) tmp8);
  memcpy(&tmp8, recv_buffer + 7, 1);
  react_trace.dprintf(0, "opcode = %d\n", (int) tmp8);

  if (length != (total_read - 6))
  {
    react_trace.dprintf(5, "Did not recieve complete message %d, %d\n",
		    (int) length, total_read);
  }
  /* Ok, reset the send buffer here */
  send_ptr = 6;
  total_written = 0;
  return total_read - 6;
  }
}

/********************************************************************/
 
int mod_tcpip_client_t::get_buffer(uint8 *buf, int n)
{
  memcpy(buf, recv_buffer + recv_ptr, n);
  recv_ptr += n;
  return n;
}

/********************************************************************/
 
int mod_tcpip_client_t::put_buffer(uint8 *buf, int n)
{
  memcpy(send_buffer + send_ptr, buf, n);
  total_written += n;
  send_ptr += n;
  return n;
}

/********************************************************************/
 
int mod_tcpip_client_t::send_message(void)
{
  transaction_id++;
  react_trace.dprintf(0, "Trans id = %d, ", (int) transaction_id);
  memcpy(send_buffer, &transaction_id, 2);
  swap16((uint16 *)send_buffer);


  protocol_id = 0;
  react_trace.dprintf(0, "protocol id = %d, ", (int) protocol_id);
  memcpy(send_buffer + 2, &protocol_id, 2);
  swap16((uint16 *)(send_buffer + 2));

  react_trace.dprintf(0, "total bytes = %d, ", (int) total_written);
  uint16 tmp16 = total_written;
  react_trace.dprintf(0, "writing length in buffer: %d\n", (int) tmp16);
  swap16(&tmp16);
  memcpy(send_buffer + 4, &tmp16, 2);

  react_trace.dprintf(0, "opcode = %d\n", (int) send_buffer[7]);

  react_trace.print_buf(0, "Sending (including TCP/IP header):\n", send_buffer, total_written+6);
  int n = send(sock, send_buffer, total_written + 6, 0);
  //write(sock, "junk", 4);
  //send_ptr = 6;
  //total_written = 0;
  return n-6;
}

/********************************************************************/
 

