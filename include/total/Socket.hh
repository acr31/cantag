/**
 * $Header$
 */

#ifndef SOCKET_GUARD
#define SOCKET_GUARD

#include <tripover/Config.hh>
#include <vector>
#include <bitset>

extern "C" {
#include <netinet/in.h>
}

class Socket {
private:
  int m_socket;
  int m_byte_count;

  char* m_host;
  int m_port;

  bool m_soft_connect;

  Socket(int handle); // private constructor for accepting connections

  void PopulateSockAddr(const char* host, int port, sockaddr_in* s);
public:
  Socket();
  virtual ~Socket();
  void Bind(const char* host, int port);
  void Listen();
  void Connect(const char* host, int port);

  /**
   * Schedule this socket to connect to the destination given.  No
   * connect will be attempted until the first time someone tries to
   * write to this socket.
   */ 
  void SoftConnect(const char* host, int port);
  Socket* Accept();
  virtual void Recv(unsigned char* buf, size_t len);
  void Recv(float* buf, size_t len);
  void Recv(std::vector<float>& vec);
  int RecvInt();
  float RecvFloat();
  int Send(const unsigned char* buf, size_t len);
  int Send(int item);
  int Send(float item);
  int Send(const float* buf, size_t len);
  int Send(const std::vector<float>& vec);
  virtual inline int GetByteCount() { return m_byte_count; }
};

/**
 * This is a wrapper around the socket class to provide a server
 * socket (that can recieve) that accepts the first waiting connection
 * and reads from that until it is closed
 */
class SingleSocket : public Socket {
private:
  Socket* m_accepted;
public:
  SingleSocket();
  virtual ~SingleSocket();
  virtual void Recv(unsigned char* buf, size_t len);
  virtual inline int GetByteCount() { return m_accepted ? m_accepted->GetByteCount() : 0; }
};

#endif//SOCKET_GUARD

