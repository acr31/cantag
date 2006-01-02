/*
  Copyright (C) 2004 Andrew C. Rice

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: acr31@cam.ac.uk
*/

/**
 * $Header$
 */

#include <cantag/Socket.hh>
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

namespace Cantag {

  Socket::Socket() : m_byte_count(0), m_host(NULL), m_port(0), m_soft_connect(false) {
#ifdef SOCKET_DEBUG
    PROGRESS("Creating socket");
#endif
    m_socket = ::socket(PF_INET,SOCK_STREAM,0);
  
    if (m_socket == -1) {
      throw "Failed to create socket!";
    }
  }

  Socket::Socket(int handle) : m_socket(handle) , m_byte_count(0), m_host(NULL), m_port(0),m_soft_connect(false) {
  }

  Socket::~Socket() {
#ifdef SOCKET_DEBUG
    PROGRESS("Closing socket");
#endif
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
#ifdef SOCKET_DEBUG
    PROGRESS("Binding socket " << host << ":" << port);
#endif
    struct sockaddr_in s;
    PopulateSockAddr(host,port,&s);
    // bind socket
    if (::bind(m_socket,(struct sockaddr*)&s,sizeof(struct sockaddr)) != 0) {
      //    perror(NULL);
      throw "Failed to bind socket!";
    } 
  }

  void Socket::Connect(const char* host, int port) {
#ifdef SOCKET_DEBUG
    PROGRESS("Connecting socket " << host << ":" << port);
#endif
    // lookup address
    struct sockaddr_in s;
    PopulateSockAddr(host,port,&s);
    // connect to remote machine
    if (::connect(m_socket,(struct sockaddr*)&s,sizeof(struct sockaddr)) != 0) {
      //    perror(NULL);
      throw "Failed to connect to remote machine!";
    } 
  }

  void Socket::SoftConnect(const char* host, int port) {
#ifdef SOCKET_DEBUG
    PROGRESS("SoftConnect scheduled to " << host << ":" << port);
#endif
    m_host = new char[strlen(host)+1];
    strcpy(m_host,host);
    m_port = port;
    m_soft_connect = true;
  }

  void Socket::Listen() {
#ifdef SOCKET_DEBUG
    PROGRESS("Listen");
#endif
    // listen
    if (::listen(m_socket,0) != 0) {
      //    perror(NULL);
      throw "Failed to listen on socket!";
    }
  }

  Socket* Socket::Accept() {
#ifdef SOCKET_DEBUG
    PROGRESS("Accept");
#endif
    struct sockaddr_in s;
    memset(&s,0,sizeof(sockaddr));
    socklen_t len = sizeof(struct sockaddr);
    int accepted = accept(m_socket,(struct sockaddr*)&s,&len);
    if (accepted == -1) {
      //    perror(NULL);
      throw "Failed to accept connection";
    }
#ifdef SOCKET_DEBUG
    PROGRESS("Accepted connection");
#endif
    return new Socket(accepted);
  }

  void Socket::Recv(unsigned char* buf, size_t len) {
#ifdef SOCKET_DEBUG
    PROGRESS("Receive unsigned char* size " << len);
#endif
    unsigned int total = 0;
    while(total < len) {
      int count = ::recv(m_socket,buf+total,len-total,0);
      if (count == -1) throw "Received error/eof from socket";
      total += count;
      m_byte_count+= count;
    }
#ifdef SOCKET_DEBUG
    PROGRESS("Received " << total << " bytes");
#endif
  }

  void Socket::Recv(float* buf, size_t len) {
#ifdef SOCKET_DEBUG
    PROGRESS("Receive float* size " << len);
#endif
    Recv((unsigned char*)buf,sizeof(float)*len);
  }

  int Socket::RecvInt() {
#ifdef SOCKET_DEBUG
    PROGRESS("Receive int");
#endif
    int result;
    Recv((unsigned char*)&result,sizeof(int));
    return result;
  }

  float Socket::RecvFloat() {
#ifdef SOCKET_DEBUG
    PROGRESS("Receive float");
#endif
    float result;
    Recv((unsigned char*)&result,sizeof(float));
    return result;
  }

  void Socket::Recv(std::vector<float>& vec) {
#ifdef SOCKET_DEBUG
    PROGRESS("Receive vector<float>");
#endif
    int size = RecvInt();
    float* data = new float[size];
    Recv( (unsigned char*)data, size * sizeof(float));
    for(int i=0;i<size;++i) {
      vec.push_back(*(data++));
    }
  }

  int Socket::Send(const unsigned char* buf, size_t len) {
#ifdef SOCKET_DEBUG
    PROGRESS("Send unsigned char* size " << len);
#endif
    while (m_soft_connect) {
#ifdef SOCKET_DEBUG
      PROGRESS("Connect needed");
#endif
      try {
	Connect(m_host,m_port);
	m_soft_connect = false;
      }
      catch (const char*) {
	sleep(1);
      }
    }
    unsigned int total = 0;
    while(total != len) {
      int sent = ::send(m_socket,buf+total,len-total,0);
      if (sent == -1) throw "Send yields -1";
      total+=sent;
      m_byte_count += sent;
    }
#ifdef SOCKET_DEBUG
    PROGRESS("Sent " << total << " bytes");
#endif
    return total;
  }

  int Socket::Send(const float* buf, size_t len) {
#ifdef SOCKET_DEBUG
    PROGRESS("Send float* size " << len);
#endif
    return Send((unsigned char*)buf, sizeof(float)*len);
  }

  int Socket::Send(int message) {
#ifdef SOCKET_DEBUG
    PROGRESS("Send int");
#endif
    return Send((unsigned char*)&message,sizeof(int));
  }

  int Socket::Send(unsigned int message) {
#ifdef SOCKET_DEBUG
    PROGRESS("Send unsigned int");
#endif
    return Send((unsigned char*)&message,sizeof(unsigned int));
  }

  int Socket::Send(float message) {
#ifdef SOCKET_DEBUG
    PROGRESS("Send float");
#endif
    return Send((unsigned char*)&message,sizeof(float));
  }

  int Socket::Send(const std::vector<float>& vec) {
#ifdef SOCKET_DEBUG
    PROGRESS("Send vector<float>");
#endif
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
#ifdef SOCKET_DEBUG
    PROGRESS("SingleSocket receive");
#endif
    if (!m_accepted) {
#ifdef SOCKET_DEBUG
      PROGRESS("SingleSocket not connected so calling accept");
#endif
      m_accepted = Socket::Accept();
    }
    m_accepted->Recv(buf,len);
  }
}

/*
  int Ellipse::Save(Socket& socket) const {
    int count = socket.Send(m_a);
    count += socket.Send(m_b);
    count += socket.Send(m_c);
    count += socket.Send(m_d);
    count += socket.Send(m_e);
    count += socket.Send(m_f);
    count += socket.Send(m_x0);
    count += socket.Send(m_y0);
    count += socket.Send(m_angle_radians);
    count += socket.Send(m_width);
    count += socket.Send(m_height);
    count += socket.Send((int)m_fitted);
    return count;
  }

  Ellipse::Ellipse(Socket& socket) {
    m_a = socket.RecvFloat();
    m_b = socket.RecvFloat();
    m_c = socket.RecvFloat();
    m_d = socket.RecvFloat();
    m_e = socket.RecvFloat();
    m_f = socket.RecvFloat();
    m_x0 = socket.RecvFloat();
    m_y0 = socket.RecvFloat();
    m_angle_radians = socket.RecvFloat();
    m_width = socket.RecvFloat();
    m_height = socket.RecvFloat();    
    m_fitted = (bool)socket.RecvInt();    
  }

   Image::ImageBase(Socket& socket) {
    m_width = socket.RecvInt();
    m_height = socket.RecvInt();
    m_width_step = socket.RecvInt();
    m_binary = socket.RecvInt() == 1;
    m_contents = new unsigned char[m_width_step*m_height];
    m_free_contents = true;
    socket.Recv(m_contents,m_height*m_width_step);
  }

   int Image::Save(Socket& socket) const {
    int count = socket.Send(m_width);
    count += socket.Send(m_height);
    count += socket.Send(m_width_step);
    count += socket.Send(m_binary ? 1 : 0);
    count += socket.Send(m_contents,m_height*m_width_step);
    return count;
  }
  QuadTangle::QuadTangle(Socket& socket) {
    m_x0 = socket.RecvFloat();
    m_y0 = socket.RecvFloat();
    m_x1 = socket.RecvFloat();
    m_y1 = socket.RecvFloat();
    m_x2 = socket.RecvFloat();
    m_y2 = socket.RecvFloat();
    m_x3 = socket.RecvFloat();
    m_y3 = socket.RecvFloat();
    m_xc = socket.RecvFloat();
    m_yc = socket.RecvFloat();
    //    m_fitted = (bool)socket.RecvInt();
  }

  int QuadTangle::Save(Socket& socket) const {
    int count = socket.Send(m_x0);
    count += socket.Send(m_y0);
    count += socket.Send(m_x1);
    count += socket.Send(m_y1);
    count += socket.Send(m_x2);
    count += socket.Send(m_y2);
    count += socket.Send(m_x3);
    count += socket.Send(m_y3);
    count += socket.Send(m_xc);
    count += socket.Send(m_yc);
    //  count += socket.Send((int)m_fitted);
    return count;
  }

  template<int BIT_COUNT> int CyclicBitSet<BIT_COUNT>::Save(Socket& socket) const {
    int count = socket.Send((int)m_rotation);
    unsigned char* buffer = new unsigned char[BIT_COUNT / 8 + 1];
    for(int i=0;i<BIT_COUNT;++i) {
      if (i%8==0) { buffer[i/8] = 0;}
      if ((*this)[i]) { buffer[i/8] |= (1 << (i%8)); }
    } 
    count += socket.Send(buffer,BIT_COUNT/8+1);
    return count;
  }


*/
