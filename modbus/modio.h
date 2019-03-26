#ifndef __MODIO_INC__
#define __MODIO_INC__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class mod_io_t
{
public:
  virtual int wait_message(void) = 0;
  virtual int get_buffer(uint8 *buf, int n) = 0;
  virtual int put_buffer(uint8 *buf, int max) = 0;
  virtual int send_message(void) = 0;
  virtual ~mod_io_t(void) {};
};

class rt_mod_serial_client_t : public mod_io_t
{
private:
  int serial_fd;
  int baudrate;
  float timeout;
  char device_name[128];
  uint8 recv_buffer[1024];
  uint8 send_buffer[1024];

  int send_ptr;
  int recv_ptr;
  int total_read;
  int total_written;
public:  
  rt_mod_serial_client_t(const char *dev_name, int baudrate, float timeout); 
  int wait_message(void);
  int get_buffer(uint8 *buf, int n);
  int put_buffer(uint8 *buf, int max);
  int send_message(void);
};

class mod_tcpip_client_t : public mod_io_t
{
private:
  int sock;
  struct sockaddr_in the_dest_addr;
  uint8 recv_buffer[1024];
  uint8 send_buffer[1024];
  /* MBAP header */
  uint16 transaction_id; 
  uint16 protocol_id; 
  uint16 length;
  /* end header */
  int send_ptr;
  int recv_ptr;
  int total_read;
  int total_written;
public:
  mod_tcpip_client_t(int a_socket, sockaddr_in *a_dest_addr); 
  void reconnect(void);
  int wait_message(void);
  int get_buffer(uint8 *buf, int n);
  int put_buffer(uint8 *buf, int max);
  int send_message(void);
};


class mod_tcpip_t : public mod_io_t
{
private:
  int sock;
  uint8 recv_buffer[1024];
  uint8 send_buffer[1024];
  /* MBAP header */
  uint16 transaction_id; 
  uint16 protocol_id; 
  uint16 length;
  /* end header */
  int send_ptr;
  int recv_ptr;
  int total_read;
  int total_written;
public:
  mod_tcpip_t(int a_socket);
  int wait_message(void);
  int get_buffer(uint8 *buf, int n);
  int put_buffer(uint8 *buf, int max);
  int send_message(void);
};


class mod_tcpip_server_t : public mod_io_t
{
private:
  int sock;
  uint8 recv_buffer[1024];
  uint8 send_buffer[1024];
  int send_ptr;
  int recv_ptr;
public:
  mod_tcpip_server_t(int a_socket);
  int wait_message(void);
  int get_buffer(uint8 *buf, int n);
  int put_buffer(uint8 *buf, int max);
  int send_message(void);
};

#endif

