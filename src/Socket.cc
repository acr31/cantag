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

Socket::Socket() : m_byte_count(0) {
  m_socket = ::socket(PF_INET,SOCK_STREAM,0);
  
  if (m_socket == -1) {
    perror(NULL);
    throw "Failed to create socket!";
  }
}

Socket::Socket(int handle) : m_socket(handle) , m_byte_count(0) {
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
    throw "Failed to get address for chosen machine!";
  }
  // connect to remote machine
  if (::bind(m_socket,(struct sockaddr*)&s,sizeof(struct sockaddr)) != 0) {
    perror(NULL);
    throw "Failed to bind socket!";
  } 
}

void Socket::Connect(const char* host, int port) {
   // lookup address
  struct sockaddr_in s;
  memset(&s,0,sizeof(sockaddr));
  s.sin_family = AF_INET;
  s.sin_port = htons(port);
  int status = inet_pton(AF_INET,host,&s.sin_addr);
  if (status <= 0) {
    perror(NULL);
    throw "Failed to get address for chosen machine!";
  }
  // connect to remote machine
  if (::connect(m_socket,(struct sockaddr*)&s,sizeof(struct sockaddr)) != 0) {
    perror(NULL);
    throw "Failed to connect to remote machine!";
  } 
}

void Socket::Listen() {
  // listen
  if (::listen(m_socket,0) != 0) {
    perror(NULL);
    throw "Failed to listen on socket!";
  }
}

Socket* Socket::Accept() {
  struct sockaddr_in s;
  memset(&s,0,sizeof(sockaddr));
  socklen_t len = sizeof(struct sockaddr);
  int accepted = accept(m_socket,(struct sockaddr*)&s,&len);
  if (accepted == -1) {
    perror(NULL);
    throw "Failed to accept connection";
  }
  return new Socket(accepted);
}

void Socket::Recv(unsigned char* buf, size_t len) {
  int total = 0;
  while(total < len) {
    int count = ::recv(m_socket,buf+total,len-total,0);
    if (count == -1) throw "Received error/eof from socket";
    total += count;
    m_byte_count+= count;
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

int Socket::Send(const unsigned char* buf, size_t len) {
  int total = 0;
  while(total != len) {
    int sent = ::send(m_socket,buf+total,len-total,0);
    if (sent == -1) throw "Send yields -1";
    total+=sent;
    m_byte_count += sent;
  }
  return total;
}

int Socket::Send(int message) {
  return Send((unsigned char*)&message,sizeof(int));
}

int Socket::Send(float message) {
  return Send((unsigned char*)&message,sizeof(float));
}

int Socket::Send(const std::vector<float>& vec) {
  float* points = new float[vec.size()];
  float* pointsptr = points;
  for(std::vector<float>::const_iterator i = vec.begin();i!=vec.end();++i) {
    *(pointsptr++) = *i;
  }
  int count = Send((int)vec.size());
  count += Send( (unsigned char*)points, vec.size() * sizeof(float) );
  delete[] points;
  return count;
}
