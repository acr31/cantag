/**
 * $Header$
 */

#include <NetworkOStream.hh>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cerrno>


NetworkOStream::NetworkOStream(const std::string& hostname, int port) : 
  m_socket(-1),m_hostname(hostname), m_port(port) {
}

void NetworkOStream::Open() {
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
  int status = inet_pton(AF_INET,m_hostname.c_str(),&s.sin_addr);
  if (status <= 0) {
    perror(NULL);
    throw "Failed to get address for remote machine!";
  }
  
  // connect to remote machine
  if (::connect(m_socket,(struct sockaddr*)&s,sizeof(struct sockaddr)) != 0) {
    perror(NULL);
    throw "Failed to connect to remote machine!";
  }
}

NetworkOStream::~NetworkOStream() {
  // close socket
  if (m_socket != -1) {
    ::close(m_socket);
  }
}

void NetworkOStream::write(const char* s, std::streamsize n) {
  if (m_socket == -1) Open();
  int sent = 0;
  while(sent < n) {
    int thistime = send(m_socket,s+sent,n-sent,0);
    if (thistime == -1) {
      throw "Failed to send data - socket error!";
    }
    sent += thistime;
  }
}
