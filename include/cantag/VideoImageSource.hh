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

#ifndef VIDEO_IMAGE_SOURCE_GUARD
#define VIDEO_IMAGE_SOURCE_GUARD

#include <cantag/Config.hh>

#ifndef HAVE_AVCODEC_H
#error This version has been configured without Movie (libavcodec) support
#endif

#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>

#include <cantag/ImageSource.hh>

namespace Cantag {

  class VideoImageSource :  public ImageSource<Pix::Sze::Byte3,Pix::Fmt::BGR24> {

  public:
    VideoImageSource(const char *file, int width, int height, bool attempt_realtime);
    virtual ~VideoImageSource();

    virtual Image<Pix::Sze::Byte3,Pix::Fmt::BGR24>* Next();

    inline int GetWidth() const { return mWidth; }
    inline int GetHeight() const {  return mHeight; }
    

  private:
    int mWidth;
    int mHeight;
    int mStream;

    AVFormatContext *mFormatCtx;
    AVFrame         *mFrame; 
    AVFrame         *mFrameRGB;
    AVCodecContext  *mCodecCtx;
    AVCodec         *mCodec;
    uint8_t         *mBuffer;

    Image<Pix::Sze::Byte3,Pix::Fmt::BGR24> *mImage;

    int mInterval;

    struct timeval mTime;
    bool           mFirst;

    bool   mRealTime;

  };

};

#endif
