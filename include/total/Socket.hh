/**
 * $Header$
 */

#ifndef SOCKET_GUARD
#define SOCKET_GUARD

#include <tripover/Config.hh>
#include <vector>

class Socket {
private:
  int m_socket;
  int m_byte_count;

  char* m_host;
  int m_port;

  bool m_soft_connect;

  Socket(int handle); // private constructor for accepting connections
public:
  Socket();
  ~Socket();
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
  void Recv(unsigned char* buf, size_t len);
  void Recv(std::vector<float>& vec);
  int RecvInt();
  float RecvFloat();
  int Send(const unsigned char* buf, size_t len);
  int Send(int item);
  int Send(float item);
  int Send(const std::vector<float>& vec);

  inline int GetByteCount() { return m_byte_count; }
};

#endif//SOCKET_GUARD
