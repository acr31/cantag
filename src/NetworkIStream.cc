/**
 * $Header$
 */

#include <NetworkIStream.hh>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#define BUFFER_MAX 1024

NetworkIStream::NetworkIStream(int port) :   
  m_socket(-1), m_accepted(-1), m_port(port) {
  m_buffer = new char[BUFFER_MAX];
  m_buffer_pointer = m_buffer;
  m_buffer_end = m_buffer;
}

NetworkIStream::NetworkIStream(const NetworkIStream& orig) :
  m_socket(orig.m_socket), m_accepted(orig.m_socket), m_port(orig.m_port) {
  m_buffer = new char[BUFFER_MAX];
  m_buffer_pointer = m_buffer;
  m_buffer_end = m_buffer;
}

void NetworkIStream::Open() {
  // create socket
  m_socket = ::socket(PF_INET,SOCK_STREAM,0);
  
  if (m_socket == -1) {
    perror(NULL);
    throw "Failed to create socket!";
  }

  // lookup address
  struct sockaddr_in s;
  memset(&s,0,sizeof(sockaddr));
  s.sin_family = AF_INET;
  s.sin_port = htons(m_port);
  int status = inet_pton(AF_INET,"128.232.23.61",&s.sin_addr);
  if (status <= 0) {
    perror(NULL);
    throw "Failed to get address for local machine!";
  }

  // connect to remote machine
  if (::bind(m_socket,(struct sockaddr*)&s,sizeof(struct sockaddr)) != 0) {
    perror(NULL);
    throw "Failed to bind socket!";
  }

  // listen
  if (::listen(m_socket,0) != 0) {
    perror(NULL);
    throw "Failed to listen on socket!";
  }
}

void NetworkIStream::Accept() {
  struct sockaddr_in s;
  memset(&s,0,sizeof(sockaddr));
  socklen_t len = sizeof(struct sockaddr);
  m_accepted = accept(m_socket,(struct sockaddr*)&s,&len);
  if (m_accepted == -1) {
    perror(NULL);
    throw "Failed to accept connection";
  }
}

NetworkIStream::~NetworkIStream() {
  // close socket
  if (m_socket != -1) {
    ::close(m_socket);
  }
  delete[] m_buffer;
}


std::streamsize NetworkIStream::read(char* s, std::streamsize n) {
  //  std::cout << "read " << n<<std::endl;
  if (m_socket == -1) Open();
  if (m_accepted == -1) Accept();
  int storedn = n;
  while(1) {
    int i;
    for(i=0;i<m_buffer_end - m_buffer_pointer && i < n ; ++i) {
      *(s++) = *(m_buffer_pointer++);
    }
    
    if (i == n) {
      //      std::cout << "Returning " << storedn << std::endl;
      return storedn;
    }
    
    //    std::cout << "refilling buffer "<< std::endl;
    ssize_t readcount = recv(m_accepted,m_buffer,(size_t)BUFFER_MAX,0);
    if (readcount == -1) {
      //      std::cout << "Returning -1" << std::endl;
      return -1;
    }
    m_buffer_end = m_buffer + readcount;
    m_buffer_pointer = m_buffer;
    //    std::cout << "buffer contains " << readcount << std::endl;
    
    n-=i;
  }
}


