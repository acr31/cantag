/**
 * $Header$
 */
#include <tripover/FileImageSource.hh>

FileImageSource::FileImageSource(char* filename) : m_original(new Image(filename)), m_buffer(new Image(filename)) {}    

FileImageSource::~FileImageSource() { 
  if (m_buffer != NULL) {
    delete m_buffer;
  }
}

Image* FileImageSource::Next() {
  if (m_buffer != NULL) {
    delete m_buffer;
  }
  m_buffer = new Image(*m_original);
  return m_buffer;
}

