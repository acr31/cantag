/**
 * $Header$
 */

#include <Socket.hh>
#include <cerrno>

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
}

Socket::Socket() {
  m_socket = ::socket(PF_INET,SOCK_STREAM,0);
  
  if (m_socket == -1) {
    perror(NULL);
    throw "Failed to create socket!";
  }
}

Socket::Socket(int handle) : m_socket(handle) {
}

Socket::~Socket() {
  if (m_socket != -1) {
    ::close(m_socket);
  }
}

void Socket::Bind(const char* host, int port) {
  // lookup address
  struct sockaddr_in s;
  memset(&s,0,sizeof(sockaddr));
  s.sin_family = AF_INET;
  s.sin_port = htons(port);
  int status = inet_pton(AF_INET,host,&s.sin_addr);
  if (status <= 0) {
    perror(NULL);
    throw "Failed to get address for local machine!";
  }
  // connect to remote machine
  if (::bind(m_socket,(struct sockaddr*)&s,sizeof(struct sockaddr)) != 0) {
    perror(NULL);
    throw "Failed to bind socket!";
  } 
}

void Socket::Listen() {
  // listen
  if (::listen(m_socket,0) != 0) {
    perror(NULL);
    throw "Failed to listen on socket!";
  }
}

Socket Socket::Accept() {
  struct sockaddr_in s;
  memset(&s,0,sizeof(sockaddr));
  socklen_t len = sizeof(struct sockaddr);
  int accepted = accept(m_socket,(struct sockaddr*)&s,&len);
  if (accepted == -1) {
    perror(NULL);
    throw "Failed to accept connection";
  }
  return Socket(accepted);
}

void Socket::Recv(unsigned char* buf, size_t len) {
  int total = 0;
  while(total < len) {
    int count = ::recv(m_socket,buf,len,0);
    if (count == -1) throw "Received error/eof from socket";
    total += count;
  }
}

int Socket::RecvInt() {
  int result;
  Recv((unsigned char*)&result,sizeof(int));
  return result;
}

float Socket::RecvFloat() {
  float result;
  Recv((unsigned char*)&result,sizeof(float));
  return result;
}

void Socket::Recv(std::vector<float>& vec) {
  int size = RecvInt();
  float* data = new float[size];
  Recv( (unsigned char*)data, size * sizeof(float));
  for(int i=0;i<size;++i) {
    vec.push_back(*(data++));
  }
}

void Socket::Send(const unsigned char* buf, size_t len) {
  int sent = ::send(m_socket,buf,len,0);
  if (sent == -1 || sent != len) throw "Send yields -1 or failed to send required number of bytes";
}

void Socket::Send(int message) {
  Send((unsigned char*)&message,sizeof(int));
}

void Socket::Send(float message) {
  Send((unsigned char*)&message,sizeof(float));
}

void Socket::Send(const std::vector<float>& vec) {
  float* points = new float[vec.size()];
  float* pointsptr = points;
  for(std::vector<float>::const_iterator i = vec.begin();i!=vec.end();++i) {
    *(pointsptr++) = *i;
  }
  Send((int)vec.size());
  Send( (unsigned char*)points, vec.size() * sizeof(float) );
  delete[] points;
}
