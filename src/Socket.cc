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
#include <unistd.h>
}

#define SOCKET_DEBUG

Socket::Socket() : m_byte_count(0), m_host(NULL), m_port(0), m_soft_connect(false) {
  PROGRES("Creating socket");
  m_socket = ::socket(PF_INET,SOCK_STREAM,0);
  
  if (m_socket == -1) {
    throw "Failed to create socket!";
  }
}

Socket::Socket(int handle) : m_socket(handle) , m_byte_count(0), m_host(NULL), m_port(0),m_soft_connect(false) {
}

Socket::~Socket() {
  PROGRESS("Closing socket");
  if (m_socket != -1) {
    ::close(m_socket);
  }
  if (m_host) {
    delete m_host;
  }
}

void Socket::PopulateSockAddr(const char* host, int port, sockaddr_in* s) {
  struct hostent* hostdetails;
  if ((hostdetails = gethostbyname(host)) == NULL) {
    throw "Failed to lookup address";
  }
  unsigned char* address = (unsigned char*)hostdetails->h_addr_list[0];
  unsigned int q0 = address[0];
  unsigned int q1 = address[1];
  unsigned int q2 = address[2];
  unsigned int q3 = address[3];
  char addressstring[16];
  snprintf(addressstring,16,"%d.%d.%d.%d",q0,q1,q2,q3);
  // lookup address

  memset(s,0,sizeof(sockaddr_in));
  s->sin_family = AF_INET;
  s->sin_port = htons(port);
  int result = inet_aton(addressstring,&s->sin_addr);
  if (result == 0) {
    throw "Invalid IP address!";
  }
}

void Socket::Bind(const char* host, int port) {
  PROGRESS("Binding socket " << host << ":" << port);
  struct sockaddr_in s;
  PopulateSockAddr(host,port,&s);
  // bind socket
  if (::bind(m_socket,(struct sockaddr*)&s,sizeof(struct sockaddr)) != 0) {
    perror(NULL);
    throw "Failed to bind socket!";
  } 
}

void Socket::Connect(const char* host, int port) {
  PROGRESS("Connecting socket " << host << ":" << port);
   // lookup address
  struct sockaddr_in s;
  PopulateSockAddr(host,port,&s);
  // connect to remote machine
  if (::connect(m_socket,(struct sockaddr*)&s,sizeof(struct sockaddr)) != 0) {
    perror(NULL);
    throw "Failed to connect to remote machine!";
  } 
}

void Socket::SoftConnect(const char* host, int port) {
  PROGRESS("SoftConnect scheduled to " << host << ":" << port);
  m_host = new char[strlen(host)+1];
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
  PROGRESS("Accept");
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
  PROGRESS("Receive unsigned char* size " << len);
  int total = 0;
  while(total < len) {
    int count = ::recv(m_socket,buf+total,len-total,0);
    if (count == -1) throw "Received error/eof from socket";
    total += count;
    m_byte_count+= count;
  }
  PROGRESS("Received " << total << " bytes");
}

void Socket::Recv(float* buf, size_t len) {
  PROGRESS("Receive float* size " << len);
  Recv((unsigned char*)buf,sizeof(float)*len);
}

int Socket::RecvInt() {
  PROGRESS("Receive int");
  int result;
  Recv((unsigned char*)&result,sizeof(int));
  return result;
}

float Socket::RecvFloat() {
  PROGRESS("Receive float");
  float result;
  Recv((unsigned char*)&result,sizeof(float));
  return result;
}

void Socket::Recv(std::vector<float>& vec) {
  PROGRESS("Receive vector<float>");
  int size = RecvInt();
  float* data = new float[size];
  Recv( (unsigned char*)data, size * sizeof(float));
  for(int i=0;i<size;++i) {
    vec.push_back(*(data++));
  }
}

int Socket::Send(const unsigned char* buf, size_t len) {
  PROGRESS("Send unsigned char* size " << len);
  while (m_soft_connect) {
    try {
      Connect(m_host,m_port);
      m_soft_connect = false;
    }
    catch (const char*) {
      std::cout << "retry"<<std::endl;
      sleep(1);
    }
  }
  int total = 0;
  while(total != len) {
    int sent = ::send(m_socket,buf+total,len-total,0);
    if (sent == -1) throw "Send yields -1";
    total+=sent;
    m_byte_count += sent;
  }
  PROGRESS("Sent " << total << " bytes");
  return total;
}

int Socket::Send(const float* buf, size_t len) {
  PROGRESS("Send float* size " << len);
  return Send((unsigned char*)buf, sizeof(float)*len);
}

int Socket::Send(int message) {
  PROGRES("Send int");
  return Send((unsigned char*)&message,sizeof(int));
}

int Socket::Send(float message) {
  PROGRESS("Send float");
  return Send((unsigned char*)&message,sizeof(float));
}

int Socket::Send(const std::vector<float>& vec) {
  PROGRESS("Send vector<float>");
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

SingleSocket::SingleSocket() : Socket(), m_accepted(NULL) {}
SingleSocket::~SingleSocket() {
  if (m_accepted) delete m_accepted;
};

void SingleSocket::Recv(unsigned char* buf, size_t len) {
  if (!m_accepted) {
    m_accepted = Socket::Accept();
  }
  m_accepted->Recv(buf,len);
}
