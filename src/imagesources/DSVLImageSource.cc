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

#include <cantag/Config.hh>

#ifdef WIN32

#include <cantag/imagesources/DSVLImageSource.hh>

namespace Cantag {

	DSVLImageSource::DSVLImageSource(const char* xml_config) : m_checkedout(false), m_image(NULL) {
	CoInitialize(NULL);
	m_videosource = new DSVL_VideoSource();
	if(FAILED(m_videosource->BuildGraphFromXMLFile((char*)xml_config)))
		throw "Failed to build graph from XML file";
	
	if(FAILED(m_videosource->GetCurrentMediaFormat(&m_width,&m_height,&m_fps,NULL))) 
		throw "Failed to get current format from video driver";

	if(FAILED(m_videosource->EnableMemoryBuffer(1)))
		throw "Failed to enable memory buffer";
		
	if(FAILED(m_videosource->Run())) 
		throw "Failed to start video grabber";
	
}	

DSVLImageSource::~DSVLImageSource() {
		if (m_checkedout) m_videosource->CheckinMemoryBuffer(m_mbHandle);
		if (m_image) delete m_image;
		delete m_videosource;
}

Image<Pix::Sze::Byte3,Pix::Fmt::RGB24>* DSVLImageSource::Next() {
	if (m_checkedout) { m_videosource->CheckinMemoryBuffer(m_mbHandle); m_checkedout = false; }
    DWORD wait_result = m_videosource->WaitForNextSample(1000);
	if (wait_result != WAIT_TIMEOUT) {
		if (m_image) delete m_image;
		unsigned char* buffer_pointer = NULL;
		if(SUCCEEDED(m_videosource->CheckoutMemoryBuffer(&m_mbHandle, &buffer_pointer))) {
			m_image = new Image<Pix::Sze::Byte3,Pix::Fmt::RGB24>(m_width,m_height,m_width*3,buffer_pointer);
			m_checkedout = true;
			return m_image;
		}
		else {
			throw "Failed to check out memory buffer";
		}
	}
	else {
		throw "Wait timeout exceeded when grabbing frame!";
	}

}
}

#endif//WIN32
