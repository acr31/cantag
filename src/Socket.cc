/**
 * $Header$
 */

#include <tripover/Socket.hh>
#include <cerrno>

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>  
#include <arpa/inet.h>
}

Socket::Socket() : m_byte_count(0), m_host(NULL), m_port(0), m_soft_connect(false) {
  m_socket = ::socket(PF_INET,SOCK_STREAM,0);
  
  if (m_socket == -1) {
    perror(NULL);
    throw "Failed to create socket!";
  }
}

Socket::Socket(int handle) : m_socket(handle) , m_byte_count(0), m_host(NULL), m_port(0),m_soft_connect(false) {
}

Socket::~Socket() {
  if (m_socket != -1) {
    ::close(m_socket);
  }
  if (m_host) {
    delete m_host;
  }
}

void Socket::Bind(const char* host, int port) {
  struct hostent* hostdetails;
  if ((hostdetails = gethostbyname(host)) == NULL) {
    throw "Failed to lookup address";
  }

  char* address = hostdetails->h_addr_list[0];
  unsigned int q0 = address[0];
  unsigned int q1 = address[1];
  unsigned int q2 = address[2];
  unsigned int q3 = address[3];

  // lookup address
  struct sockaddr_in s;
  memset(&s,0,sizeof(sockaddr));
  s.sin_family = AF_INET;
  s.sin_port = htons(port);
  s.sin_addr.s_addr = q0 | (q1 << 8) | (q2 << 16) | (q3 << 24);
  //  int status = inet_pton(AF_INET,host,&s.sin_addr);
  //  if (status <= 0) {
  //   perror(NULL);
  //   throw "Failed to get address for chosen machine!";
  //  }
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

void Socket::SoftConnect(const char* host, int port) {
  m_host = new char[strlen(host)];
  strcpy(m_host,host);
  m_port = port;
  m_soft_connect = true;
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
  if (m_soft_connect) {
    Connect(m_host,m_port);
    m_soft_connect = false;
  }
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
