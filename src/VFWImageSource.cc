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


#include <cantag/VFWImageSource.hh>

namespace Cantag {

LRESULT CALLBACK FrameCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr) { 
	std::cout << "Got callback" << std::endl;
	assert(VFWImageSource::m_self->m_bitcount == 16);
	VFWImageSource::m_self->m_running = false;
	capCaptureStop(hWnd);
	LPBYTE buffer = lpVHdr->lpData;
	for(int y=0;y<VFWImageSource::m_self->m_height;++y) {
		PixRow<Pix::Fmt::Grey8> row = VFWImageSource::m_self->m_image->GetRow(y);
		for(PixRow<Pix::Fmt::Grey8>::iterator i = row.begin(); i!=row.end();++i) {
			int val = *buffer;
			++buffer;
			val <<= 8;
			val |= *buffer;
			++buffer;

			int blue = val & 0x1F;
			val >>=5;
			int green = val & 0x1F;
			val>>=5;
			int red = val & 0x1F;

			int intensity = (blue + green + red)/3;
			i.v(intensity);
			++i;
		}
	}
	return (LRESULT) TRUE ; 
} 

VFWImageSource::VFWImageSource() {
		m_captureWindow = capCreateCaptureWindow(NULL,WS_POPUP,0,0, 100, 100,0,0);
		m_self = this;
		if (!m_captureWindow) throw "Failed to create capture window!";

		ShowWindow(m_captureWindow, SW_HIDE);
		UpdateWindow(m_captureWindow);
		
		if (!capSetCallbackOnFrame(m_captureWindow, &FrameCallbackProc))
			throw "Failed to set VideoStream callback";

		if (!capDriverConnect(m_captureWindow, 0)) 
			throw "Failed to connect default video driver to capture window";

		CAPTUREPARMS capParms = {0};
		if (!capCaptureGetSetup(m_captureWindow,&capParms,sizeof(capParms))) 
			throw "Failed to get setup information from capture device";
		capParms.fAbortLeftMouse = FALSE;
	    capParms.fAbortRightMouse = FALSE;
	    capParms.fYield = TRUE;
		capParms.fCaptureAudio = FALSE;
		capParms.wPercentDropForError = 100;
		if (!capCaptureSetSetup(m_captureWindow,&capParms,sizeof(capParms))) 
			throw "Failed to set setup information to capture device";

		BITMAPINFO info = {0};
		if (!capGetVideoFormat(m_captureWindow,&info,sizeof(info))) 
			throw "Failed to get video format information from capture device";
		m_width = info.bmiHeader.biWidth;
		m_height = info.bmiHeader.biHeight;
		m_bitcount = info.bmiHeader.biBitCount;
		//info.bmiHeader.biCompression = BI_RGB;
		//if (!capSetVideoFormat(m_captureWindow,&info,sizeof(info)))
		//	throw "Failed to set video format on capture device";

		m_image = new Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>(m_width,m_height);
}	

VFWImageSource::~VFWImageSource() {
		capSetCallbackOnVideoStream(m_captureWindow,NULL);
		capCaptureAbort(m_captureWindow);
		capDriverDisconnect(m_captureWindow);
		DestroyWindow(m_captureWindow);
}

Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* VFWImageSource::Next() {
	std::cout << "Next called" << std::endl;
		m_running = true;
		
		capGrabFrame(m_captureWindow);		
		//while (m_running) ::Sleep(1);
		MSG msg;
		while(m_running && GetMessage(&msg,NULL,0,0)) {
			TranslateMessage (&msg) ;
            DispatchMessage (&msg) ;
		}
		std::cout << "next done" << std::endl;
		return m_image;	
}

VFWImageSource* VFWImageSource::m_self = NULL;
}

#endif//WIN32