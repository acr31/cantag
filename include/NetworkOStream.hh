/**
 * $Header$
 */

#ifndef NETWORK_O_STREAM_GUARD
#define NETWORK_O_STREAM_GUARD

#include <boost/iostreams/concepts.hpp>
#include <Config.hh>
#include <string>

class NetworkOStream : public boost::io::sink {
private:
  int m_socket;
  const std::string m_hostname;
  int m_port;

  void Open();
public:
  NetworkOStream(const std::string& hostname, int port);
  ~NetworkOStream();
  void write(const char* s, std::streamsize n);
};

#endif//NETWORK_O_STREAM_GUARD
