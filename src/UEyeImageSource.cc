/*
  Copyright (C) 2006 Andrew Rice

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

#include <cantag/Config.hh>


#ifdef HAVE_UEYE_H

#include <cantag/UEyeImageSource.hh>

namespace Cantag {


  UEyeImageSource::UEyeImageSource() : m_image(NULL),m_hCamera(NULL) {
    
    if ( is_InitCamera( &m_hCamera, 0 ) != IS_SUCCESS ) 
      throw "Failed to open camera";

    if ( is_GetCameraInfo( m_hCamera, &m_CamInfo ) != IS_SUCCESS )
      throw "Failed to get camera info";

    if ( is_GetSensorInfo( m_hCamera, &m_SensorInfo ) != IS_SUCCESS ) 
      throw "Failed to initialise sensor array";

    m_width  = m_SensorInfo.nMaxWidth;
    m_height = m_SensorInfo.nMaxHeight;
    
    is_SetColorMode( m_hCamera, IS_SET_CM_Y8);  // set the colour mode to 8bit greyscale
    is_SetEdgeEnhancement( m_hCamera, IS_EDGE_EN_STRONG);
    is_SetPixelClock( m_hCamera, 30);
    double newFps;
    is_SetFrameRate( m_hCamera, 20, &newFps);
    for(int i=0;i<BUFFER_SIZE;++i) {
      if (is_AllocImageMem(m_hCamera, m_width, m_height, 8 , &m_ring_buffer[i].m_buf, &m_ring_buffer[i].m_imageID ) != IS_SUCCESS )
	throw "Failed to allocate memory buffer";

      if (is_AddToSequence( m_hCamera, m_ring_buffer[i].m_buf, m_ring_buffer[i].m_imageID ) != IS_SUCCESS )
	throw "Failed to add memory to ring buffer";
    }

    is_EnableEvent( m_hCamera, IS_SET_EVENT_FRAME );
    is_CaptureVideo( m_hCamera, IS_DONT_WAIT );
  }

  
  UEyeImageSource::~UEyeImageSource() {
    is_ExitCamera( m_hCamera );
    if (m_image) delete m_image;
  }


  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* UEyeImageSource::Next() {
    if (m_image) delete m_image;
    if (is_WaitEvent( m_hCamera, IS_SET_EVENT_FRAME, 500 ) != IS_SUCCESS )
          throw "Failed to get new frame event before timeout";
    
    INT num;
    char* current_mem;
    char* last_mem;
    is_GetActSeqBuf(m_hCamera, &num, &current_mem, &last_mem );
    assert(num != 0);
    return new Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>(m_width,m_height,m_width,(unsigned char*)last_mem);
  }

}


#endif//HAVE_UEYE_H
