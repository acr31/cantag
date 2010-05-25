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

#ifndef DSVLIMAGESOURCE_GUARD
#define DSVLIMAGESOURCE_GUARD

#include <cantag/Config.hh>

#ifndef WIN32
# error "DSVLImageSource requires Win32"
#endif

#include <DSVL.h>

#include <cantag/Image.hh>
#include <cantag/imagesources/ImageSource.hh>

namespace Cantag {
	class CANTAG_EXPORT DSVLImageSource : public ImageSource<Pix::Sze::Byte3,Pix::Fmt::RGB24> {
	private:
		MemoryBufferHandle m_mbHandle;
		bool m_checkedout;
		DSVL_VideoSource* m_videosource;
		long m_width;
		long m_height;
		double m_fps;
		Image<Pix::Sze::Byte3,Pix::Fmt::RGB24>* m_image;

	public:
		int GetWidth() const { return m_width; }
		int GetHeight() const { return m_height; }

		DSVLImageSource(const char* xml_config);
		~DSVLImageSource();
		Image<Pix::Sze::Byte3,Pix::Fmt::RGB24>* Next();
	};
}


#endif//DSVLIMAGESOURCE_GUARD
