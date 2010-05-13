#ifndef VIDEO_OUTPUT_MECHANISM_HH
#define  VIDEO_OUTPUT_MECHANISM_HH

#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>
#include <cstdio>
#include <iostream>
#include <fstream>

#ifndef HAVE_AVCODEC_H
#error This version has been configured without Movie (libavcodec) support
#endif

namespace Cantag {

  
  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout> class VideoOutputMechanism {
  public:
    VideoOutputMechanism(const char *file,
			 const int frame_rate=25,
			 const int frame_rate_base=1);
    virtual ~VideoOutputMechanism();

    /*
     * Add a frame to the video. This will write
     * out that frame. The first time this is called,
     * it derives the setup (resolution etc) from
     * the supplied Image. Thereafter they must match
     */
    void AddFrame(Image<size,layout> &image);

    /*
     * Close the video and add the trailer
     */
    void CloseVideo();

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    
  private:

    bool                m_closed;

    AVCodecContext     *m_c;
    AVCodec            *m_codec;

    AVFrame            *m_frame;
    AVFrame            *m_frame_converted;

    unsigned char      *m_frame_buf;
    unsigned char      *m_encode_buffer;

    int                 m_width;
    int                 m_height;

    std::ofstream       m_output;
    
  };


  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout> VideoOutputMechanism<size, layout>::VideoOutputMechanism(
														 const char *file,
														 const int frame_rate,
														 const int frame_rate_base
														 ) 
    : m_closed(false), m_c(NULL), m_codec(NULL), m_frame(NULL), m_frame_converted(NULL), m_width(-1), m_height(-1)
  {

    av_register_all();
    m_codec = avcodec_find_encoder_by_name("mpeg2video");//(CODEC_ID_MPEG2VIDEO);
    if (!m_codec) throw("Could not find correct codec");
  
    m_c= avcodec_alloc_context();
    m_frame= avcodec_alloc_frame();
    m_frame_converted=avcodec_alloc_frame();
    
    /* put sample parameters */
    m_c->bit_rate = 6000000;
    m_c->frame_rate = frame_rate;  
    m_c->frame_rate_base= frame_rate_base;
    m_c->gop_size = 10; /* emit one intra frame every ten frames */
    m_c->max_b_frames=1;

    m_output.open(file,std::ofstream::binary);
    if (!m_output) throw ("Unable to open video file for output");
    
    m_encode_buffer = new unsigned char[500000];
    
  }

  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout> void VideoOutputMechanism<size, layout>::AddFrame(Image<size,layout> &image) {
    // Check the frame has valid properties
    // or setup if it's the first we've seen
    if (m_width<0 || m_height<0) {
      m_width=image.GetWidth();
      m_height=image.GetHeight();

      if (m_width%2!=0 || m_height%2!=0)
	throw ("Image dimensions must be a multiple of 2 for video creation");

      m_c->width = m_width;  
      m_c->height = m_height;

      avpicture_alloc((AVPicture *)m_frame_converted,m_c->pix_fmt,m_width, m_height);

      if (avcodec_open(m_c, m_codec) < 0) throw ("Failed to open video codec");
    }
    else {
      if (m_width!=image.GetWidth() || m_height!=image.GetHeight()) 
	throw ("Attempt to add invalidle-sized frame to video");
    }

    // Now we actually deal with the frame
    if (!m_closed) {
      int input_format = -1;
      if (layout==Pix::Fmt::YUV420) input_format=PIX_FMT_YUV420P;
      if (layout==Pix::Fmt::RGB24) input_format=PIX_FMT_RGB24;
      if (layout==Pix::Fmt::BGR24) input_format=PIX_FMT_BGR24;
      if (layout==Pix::Fmt::Grey8) input_format=PIX_FMT_GRAY8;
      if (input_format<0) throw ("Unable to interpret layout format for video output");
      
      avpicture_fill((AVPicture *)m_frame,  image.GetContents(), input_format,m_c->width,m_c->height);
      img_convert((AVPicture *)m_frame_converted, m_c->pix_fmt, (AVPicture *)m_frame,input_format, GetWidth(), GetHeight());
      int s = avcodec_encode_video(m_c, m_encode_buffer, 500000, m_frame_converted);
      m_output.write((char *)m_encode_buffer, s);
    }
  }


  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout> void VideoOutputMechanism<size, layout>::CloseVideo() {
    m_encode_buffer[0] = 0x00;
    m_encode_buffer[1] = 0x00;
    m_encode_buffer[2] = 0x01;
    m_encode_buffer[3] = 0xb7;
    m_output.write((char *)m_encode_buffer, 4);
    m_output.close();

    delete[] m_encode_buffer;
    
    if (m_c) {
      avcodec_close(m_c);
      free(m_c);
    }
    if (m_frame_converted) {
      avpicture_free((AVPicture *)m_frame_converted);
      free(m_frame_converted);
    }

    m_closed=true;
  }

  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout>  VideoOutputMechanism<size, layout>::~VideoOutputMechanism() {
    if (!m_closed) CloseVideo();
  }


}



#endif
