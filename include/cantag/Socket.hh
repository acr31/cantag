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

#ifndef SOCKET_GUARD
#define SOCKET_GUARD

#include <cantag/Config.hh>
#include <vector>
#include <bitset>

extern "C" {
#include <netinet/in.h>
}

namespace Cantag {

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
    int Send(unsigned int item);
    int Send(float item);
    int Send(const float* buf, size_t len);
    int Send(const std::vector<float>& vec);
    virtual inline int GetByteCount() { return m_byte_count; }
    inline const char* GetHostName() const { return m_host; } 
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
}
#endif//SOCKET_GUARD

