/**
 * $Header$
 */

#ifndef SOCKET_GUARD
#define SOCKET_GUARD

#include <Config.hh>
#include <vector>

class Socket {
private:
  int m_socket;
  Socket(int handle); // private constructor for accepting connections
public:
  Socket();
  ~Socket();
  void Bind(const char* host, int port);
  void Listen();
  Socket Accept();
  void Recv(unsigned char* buf, size_t len);
  void Recv(std::vector<float>& vec);
  int RecvInt();
  float RecvFloat();
  void Send(const unsigned char* buf, size_t len);
  void Send(int item);
  void Send(float item);
  void Send(const std::vector<float>& vec);
};

#endif//SOCKET_GUARD
