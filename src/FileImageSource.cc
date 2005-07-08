/**
 * $Header$
 */
#include <total/FileImageSource.hh>

namespace Total {
    FileImageSource::FileImageSource(char* filename) : m_original(new Image(filename)), m_buffer(new Image(filename)) {}    
    
    int FileImageSource::GetWidth() const {
	return m_original->GetWidth();
    }

    int FileImageSource::GetHeight() const {
	return m_original->GetHeight();
    }

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
    
}
