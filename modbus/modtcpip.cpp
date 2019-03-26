
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "mtype.h"
#include "modio.h"
#include "hprtu.h"
#include "trace.h"




/********************************************************************/
 
mod_tcpip_t::mod_tcpip_t(int a_socket)
{
  sock = a_socket;
  send_ptr = 6; /* leave room for the MBAP header */
  recv_ptr = 6; /* leave room for the MBAP header */
  total_read = 0;
  total_written = 0;
  transaction_id = 0;
  protocol_id = 0;

}

/********************************************************************/
 
int mod_tcpip_t::wait_message(void)
{
  //recv_buffer[total_read] = '\0';
  //printf("Got %d bytes: %s\n", total_read, recv_buffer);
  int n1 = recv(sock, recv_buffer, 6, MSG_WAITALL);
  if (n1 <= 0)
  {
    perror("read");
    return -1; 
  }
  else if (n1 < 6)
  {
    react_trace.dprintf(5, "Message is too short: %d\n", n1);
    return -1; 
  }

  recv_ptr = 6; /* start reading after the MBAP header */
  /* Transaction ID */
  memcpy(&transaction_id, recv_buffer, 2);
  swap16(&transaction_id);

  /* Potocol ID */
  memcpy(&protocol_id, recv_buffer + 2, 2);
  swap16(&protocol_id);

  /* Length */
  memcpy(&length, recv_buffer + 4, 2);
  swap16(&length);

  int n2 = recv(sock, recv_buffer + 6, length, MSG_WAITALL);
  react_trace.dprintf(0, "bytes in message: %d\n", n2);
  if (n2 <= 0)
  {
    perror("read");
    return -1; 
  }
  else if (n2 < length)
  {
    react_trace.dprintf(5, "Message is too short: %d\n", n2);
    return -1; 
  }
  total_read = n1 + n2;
  
  if (length != (total_read - 6))
  {
    react_trace.dprintf(5, "Did not recieve complete message %d, %d\n",
		    (int) length, total_read);
  }
  return total_read - 6;
}

/********************************************************************/
 
int mod_tcpip_t::get_buffer(uint8 *buf, int n)
{
  memcpy(buf, recv_buffer + recv_ptr, n);
  recv_ptr += n;
  return n;
}

/********************************************************************/
 
int mod_tcpip_t::put_buffer(uint8 *buf, int n)
{
  memcpy(send_buffer + send_ptr, buf, n);
  total_written += n;
  send_ptr += n;
  return n;
}

/********************************************************************/
 
int mod_tcpip_t::send_message(void)
{
  memcpy(send_buffer, &transaction_id, 2);
  swap16((uint16 *)send_buffer);


  protocol_id = 0;
  memcpy(send_buffer + 2, &protocol_id, 2);
  swap16((uint16 *)(send_buffer + 2));

  uint16 tmp16 = total_written;
  swap16(&tmp16);
  memcpy(send_buffer + 4, &tmp16, 2);

  react_trace.print_buf(0, "Sending:\n", send_buffer, total_written+6);
  int n = send(sock, send_buffer, total_written + 6, 0);
  //write(sock, "junk", 4);
  send_ptr = 6;
  total_written = 0;
  return n-6;
}

/********************************************************************/
 

