/**
 * $Header$
 */

#ifndef NETWORK_I_STREAM_GUARD
#define NETWORK_I_STREAM_GUARD

#include <boost/iostreams/concepts.hpp>
#include <Config.hh>

class NetworkIStream : public boost::io::source {
private:
  int m_socket;
  int m_port;
  int m_accepted;
  char* m_buffer;
  char* m_buffer_pointer;
  char* m_buffer_end;

  void Open();
  void Accept();
public:
  NetworkIStream(int port);
  NetworkIStream(const NetworkIStream& nistream);
  ~NetworkIStream();
  std::streamsize read(char* s, std::streamsize n);
};

#endif//NETWORK_I_STREAM_GUARD
