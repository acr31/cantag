/*
  Copyright (C) 2004 Robert K. Harle

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

#ifdef HAVE_AVCODEC

#include <cantag/VideoImageSource.hh>

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

namespace Cantag {

  VideoImageSource::VideoImageSource(const char *file, const int width, const int height, bool attempt_realtime=true)
    : mStream(-1), mFormatCtx(0), mFrame(0), mFrameRGB(0), mCodecCtx(0), mCodec(0), 
      mBuffer(0), mImage(0), mFirst(1), mRealTime(attempt_realtime)

 {

    av_register_all();

    if(av_open_input_file(&mFormatCtx, file, NULL, 0, NULL)!=0)   throw ("Unable to open specified video file");
    if(av_find_stream_info(mFormatCtx)<0)  throw("Unable to retrieve video stream info");

    for(int i=0; i<mFormatCtx->nb_streams; i++) {
      if(mFormatCtx->streams[i]->codec.codec_type==CODEC_TYPE_VIDEO)
	{
	  mStream=i;
	  break;
        }
    }
    if(mStream==-1) throw("Unable to retrieve video stream info");

    mCodecCtx=&mFormatCtx->streams[mStream]->codec;

    mCodec=avcodec_find_decoder(mCodecCtx->codec_id);
    if(mCodec==NULL) throw("Uanbel to find correct codec for video file");

    if(avcodec_open(mCodecCtx, mCodec)<0) throw ("Unable to open relevant codec");

    mFrame=avcodec_alloc_frame();
    mFrameRGB=avcodec_alloc_frame();

    int numBytes=avpicture_get_size(PIX_FMT_BGR24, mCodecCtx->width, mCodecCtx->height);
    mBuffer=new uint8_t[numBytes];

     mHeight = mCodecCtx->height;
     mWidth  = mCodecCtx->width;
    
     mInterval = (int)floor((float)mCodecCtx->frame_rate_base/(float)mCodecCtx->frame_rate*1000000.f);

     if (width!=mWidth || height!=mHeight) {
       char exception[256];
       sprintf(exception,"Sorry - video has frames of size %ix%i. You requested %ix%i. Please correct your program",
	       mWidth, mHeight, width, height);
       throw(exception);
     }

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    avpicture_fill((AVPicture *)mFrameRGB, mBuffer, PIX_FMT_RGB24,
        mCodecCtx->width, mCodecCtx->height);
  }



  VideoImageSource::~VideoImageSource() {
    if (mBuffer) delete[] mBuffer;
    if (mFrameRGB) av_free(mFrameRGB);
    if (mFrame) av_free(mFrame);
    avcodec_close(mCodecCtx);
    av_close_input_file(mFormatCtx);
      if(mImage) delete mImage;
  }



  Image<Pix::Sze::Byte3,Pix::Fmt::BGR24>* VideoImageSource::Next() {

    AVPacket packet;
    int haveFrame=0;
    int frame_skip=1;

    // First do some timing stuff
    if (mRealTime) {
      if (mFirst) mFirst=false;
      else {
	struct timeval t;
	gettimeofday(&t,NULL);
	int nusec = (t.tv_sec-mTime.tv_sec)*1000000 + t.tv_usec - mTime.tv_usec;
	if (nusec<mInterval) usleep((mInterval-nusec));
	else if (nusec>mInterval) {
	  frame_skip = (int)floor((float)nusec/(float)mInterval);
	}
      }
      gettimeofday(&mTime,NULL);
    }

    while (!haveFrame && av_read_frame(mFormatCtx, &packet)>=0 ) {
      if(packet.stream_index==mStream) {
	int a;
	avcodec_decode_video(mCodecCtx, mFrame, &a, packet.data, packet.size);
	if (a) {
	  frame_skip--;
	  if (frame_skip==0 ) {
	    haveFrame=1;

	    img_convert((AVPicture*)mFrameRGB, PIX_FMT_RGB24, 
			(AVPicture*)mFrame, mCodecCtx->pix_fmt, mCodecCtx->width, 
			mCodecCtx->height);
	    
	    if (!mImage) mImage = new Image<Pix::Sze::Byte3,Pix::Fmt::BGR24>(mCodecCtx->width, 
									     mCodecCtx->height,
									     mFrameRGB->linesize[0], 
									     mFrameRGB->data[0]);
	  }
	}
      }
      av_free_packet(&packet);
    }

    if (!haveFrame) return NULL;
    return mImage;
  }

}
#endif//HAVE_AVCODEC
