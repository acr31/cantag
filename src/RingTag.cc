/**
 * $Header$
 *
 * $Log$
 * Revision 1.3  2004/02/03 16:25:11  acr31
 * more work on template tags and some function signature changes
 *
 * Revision 1.2  2004/02/03 07:48:31  acr31
 * added template tag
 *
 * Revision 1.1  2004/02/01 14:26:48  acr31
 * Implementations for Matrixtag and ringtag
 *
 * Revision 1.3  2004/01/30 16:54:17  acr31
 * changed the Coder api -reimplemented various bits
 *
 * Revision 1.2  2004/01/27 18:06:58  acr31
 * changed inheriting classes to inherit publicly from their parents
 *
 * Revision 1.1  2004/01/25 14:54:37  acr31
 * moved over to automake/autoconf build system
 *
 * Revision 1.9  2004/01/24 19:29:24  acr31
 * removed ellipsetoxy and put the project method in Ellipse2D objects
 *
 * Revision 1.8  2004/01/24 10:33:35  acr31
 * changed unsigned long to unsigned long long
 *
 * Revision 1.7  2004/01/23 16:08:55  acr31
 * remove spurious #include
 *
 * Revision 1.6  2004/01/23 16:03:51  acr31
 * moved CircularTag back to Tag - but its now templated on the type of tag - ellipse or rectangle
 *
 * Revision 1.5  2004/01/23 12:05:48  acr31
 * moved Tag to CircularTag in preparation for Squaretag
 *
 * Revision 1.4  2004/01/23 11:57:08  acr31
 * moved Location2D to Ellipse2D in preparation for Square Tags
 *
 * Revision 1.3  2004/01/21 13:41:36  acr31
 * added pose from circle to triptest - (pose from circle is unstable at the moment)
 *
 * Revision 1.2  2004/01/21 11:52:29  acr31
 * added keywords
 *
 */

#include <Config.hh>
#include <Drawing.hh>
#include <Coder.hh>
#include <Ellipse2D.hh>
#include <RingTag.hh>

#undef FILENAME
#define FILENAME "RingTag.cc"

RingTag::RingTag(int ring_count,
		 int sector_count,
		 float bullseye_inner_radius,
		 float bullseye_outer_radius,
		 float data_inner_radius,
		 float data_outer_radius) :
  m_ring_count(ring_count),
  m_sector_count(sector_count),
  m_bullseye_inner_radius(bullseye_inner_radius),
  m_bullseye_outer_radius(bullseye_outer_radius),
  m_data_inner_radius(data_inner_radius),
  m_data_outer_radius(data_outer_radius) {
  
  assert(bullseye_inner_radius < bullseye_outer_radius);
  assert(data_inner_radius < data_outer_radius);
  
  // bullseye_inner_radius < bullseye_outer_radius < data_inner_radius < data_outer_radius
  // data_inner_radius < data_outer_radius < bullseye_inner_radius < bullseye_outer_radius
  // bullseye_inner_radius < data_inner_radius < data_outer_radius < bullseye_outer_radius
  
  // Populate the radii cache
  // Slice up the range data_inner_radius to data_outer_radius evenly.
  
  // there are two ways of doing this.
  
  // 1) give each one an equal amount
  // 2) give them different amounts so that the area of each sector is the same
  
  // Lets give each one an equal amount - first pass
  
  m_data_ring_centre_radii = new float[m_ring_count];
  m_data_ring_outer_radii = new float[m_ring_count];
  
  float ring_width = (m_data_outer_radius-m_data_inner_radius)/m_ring_count;
  
  for(int i=0;i<m_ring_count;i++) {
    m_data_ring_outer_radii[i] = m_data_inner_radius + ring_width*(i+1);
    m_data_ring_centre_radii[i] = m_data_ring_outer_radii[i]-ring_width/2;    
  }
  
  // now the sector angles - go one more than necessary it makes drawing easier ;-)
  
  m_sector_angles = new float[m_sector_count+1];
  for(int i=0;i<m_sector_count+1;i++) {
    m_sector_angles[i] = 2*PI/m_sector_count *i;
  }
  
  // when we read the tag we read a total of five times and then
  // look for three codes which are the same
  m_read_angles = new float[m_sector_count*5];
  for(int i=0;i<m_sector_count*5;i++) {
    m_read_angles[i] = 2*PI/m_sector_count/5 * i;
  }
}

RingTag::~RingTag() {
  delete[] m_data_ring_outer_radii;
  delete[] m_data_ring_centre_radii;
  delete[] m_sector_angles;
  delete[] m_read_angles;
}

void RingTag::Draw2D(Image* image,unsigned long long code, int black, int white) {
  Ellipse2D l(image->width/2, image->height/2,image->width,image->height,0);
  PROGRESS("Draw2D called for ellipse centre (" << l.m_x << "," << l.m_y<< "), size ("<<l.m_width<<","<<l.m_height<<"), code "<<code);
  
  // Work from the outside inwards
  
  if (m_bullseye_outer_radius > m_data_outer_radius) {
    PROGRESS("Drawing outer bullseye edge");
    DrawFilledEllipse(image, 
		      l.m_x, 
		      l.m_y,
		      l.m_width*m_bullseye_outer_radius, 
		      l.m_height*m_bullseye_outer_radius,
		      l.m_angle_radians, 
		      black);
  }
  
  if (m_bullseye_inner_radius > m_data_outer_radius) {
    PROGRESS("Drawing inner bullseye edge");
    DrawFilledEllipse(image, 
		      l.m_x, 
		      l.m_y,
		      l.m_width*m_bullseye_inner_radius, 
		      l.m_height*m_bullseye_inner_radius,
		      l.m_angle_radians, 
		      white);
  }
  
  PROGRESS("Drawing data rings");
  unsigned long long encoded = EncodeTag(code);
  
  for(int i=m_ring_count-1;i>=0;i--) {
    unsigned long long working = encoded;
    for(int j=m_sector_count-1;j>=0;j--) {	
      int colour = ((working & (1<<i)) == (1<<i)) ? black : white;
      working >>= m_ring_count;
      DrawFilledEllipse(image,
			l.m_x,
			l.m_y,
			l.m_width*m_data_ring_outer_radii[i],
			l.m_height*m_data_ring_outer_radii[i],
			l.m_angle_radians,
			m_sector_angles[j],
			m_sector_angles[j+1],
			colour);	
    }
  }
  
  PROGRESS("Blanking out inside of data rings");
  if (m_data_inner_radius != 0) {
    DrawFilledEllipse(image,
		      l.m_x,
		      l.m_y,
		      l.m_width*m_data_inner_radius,
		      l.m_height*m_data_inner_radius,
		      l.m_angle_radians,
		      (m_bullseye_inner_radius < m_data_inner_radius &&
		       m_bullseye_outer_radius > m_data_inner_radius) ? black : white);
  }
  
  
  if (m_bullseye_outer_radius < m_data_inner_radius) {
    PROGRESS("Drawing outer bullseye edge");
    DrawFilledEllipse(image, 
		      l.m_x, 
		      l.m_y,
		      l.m_width*m_bullseye_outer_radius, 
		      l.m_height*m_bullseye_outer_radius,
		      l.m_angle_radians, 
		      black);
  }
  
  if (m_bullseye_inner_radius < m_data_inner_radius) {
    PROGRESS("Drawing inner bullseye edge");	   
    DrawFilledEllipse(image, 
		      l.m_x, 
		      l.m_y,
		      l.m_width*m_bullseye_inner_radius, 
		      l.m_height*m_bullseye_inner_radius,
		      l.m_angle_radians, 
		      white);
  }
}

unsigned long long RingTag::Decode(Image *image, const Ellipse2D *l) {
  // the location we have here locates the outer ring of the
  // bullseye.  Therefore we will need to scale it by the actual
  // size of the bullseye to hit the data sectors properly.
  
  // loop round reading chunks and passing them to the decoder
  
  // if it returns false on any insert then we might be misaligned
  // so rotate a little bit round and try again
  
  // if it throws an InvalidSymbol exception then we stop trying
  
  // if we read a full 360 degrees then we stop and ask it for the
  // code
  
  unsigned long long read_code[5];
  read_code[0] = 0;
  read_code[1] = 0;
  read_code[2] = 0;
  read_code[3] = 0;
  read_code[4] = 0;
  for(int j=m_sector_count*5 - 1;j>=0;j--) {
    // read a chunk by sampling each ring and shifting and adding
    int currentcode = j%5;      
    for(int k=m_ring_count-1;k>=0;k--) {
      float x;
      float y;
      l->ProjectPoint(m_read_angles[j],
		      m_data_ring_centre_radii[k]/m_bullseye_outer_radius,
		      &x,
		      &y);
      bool sample = SampleImage(image,x,y) > 128;
      read_code[currentcode] <<=1;
      read_code[currentcode] |= (sample ? 1:0);
    }
  }
  
#ifdef TEXT_DEBUG
  for(int i=0;i<5;i++) {
    PROGRESS("Code candidate " << i << " is " << read_code[i]);
  }
#endif

  // we now have 5 readings each a fifth of a sector apart
  // search for three in a row that read the same
  // i.e.   
  // read_code[0] == read_code[1] == read_code[2]
  // read_code[1] == read_code[2] == read_code[3]
  // read_code[2] == read_code[3] == read_code[4]
  // read_code[3] == read_code[4] == read_code[0]
  // read_code[4] == read_code[0] == read_code[1]
  for(int i=0;i<5;i++) {
    if ((read_code[i] == read_code[(i+1) % 5])) { 
#ifdef IMAGE_DEBUG
      Image* debug0 = cvCloneImage(image);
      cvConvertScale(debug0,debug0,0.5,128); 
      
      DrawEllipse(debug0,
		  l->m_x,
		  l->m_y,
		  l->m_width*m_data_inner_radius/m_bullseye_outer_radius,
		  l->m_height*m_data_inner_radius/m_bullseye_outer_radius,
		  l->m_angle_radians,
		  0,
		  1);
      
      for(int r=0;r<m_ring_count;r++) {
	DrawEllipse(debug0,
		    l->m_x,
		    l->m_y,
		    l->m_width*m_data_ring_outer_radii[r]/m_bullseye_outer_radius,
		    l->m_height*m_data_ring_outer_radii[r]/m_bullseye_outer_radius,
		    l->m_angle_radians,
		    0,
		    1);	
      }
      
      for(int k=0;k<m_sector_count;k++) {
	for(int r=0;r<m_ring_count;r++) {
	  float x;
	  float y;
	  l->ProjectPoint(m_read_angles[5*k+((i+3)%5)],
			  m_data_ring_outer_radii[r]/m_bullseye_outer_radius,
			  &x,
			  &y);
	  cvLine(debug0,
		 cvPoint(cvRound(l->m_x),cvRound(l->m_y)),
		 cvPoint(cvRound(x),cvRound(y)),
		 0,1);
	  
	  l->ProjectPoint(m_read_angles[5*k+((i+1)%5)],
			  m_data_ring_centre_radii[r]/m_bullseye_outer_radius,
			  &x,
			  &y);
	  cvLine(debug0,cvPoint(cvRound(x),cvRound(y)),cvPoint(cvRound(x),cvRound(y)), SampleImage(image,x,y) < 128 ? 255 : 0,3);
	  
	  
	}
      }
      cvSaveImage("debug-decode.jpg",debug0);	  	
      cvReleaseImage(&debug0);
#endif
      return DecodeTag(read_code[i]);	
    }
  }

#ifdef IMAGE_DEBUG
  Image* debug0 = cvCloneImage(image);
  cvConvertScale(debug0,debug0,0.5,128); 
      
  DrawEllipse(debug0,
	      l->m_x,
	      l->m_y,
	      l->m_width*m_data_inner_radius/m_bullseye_outer_radius,
	      l->m_height*m_data_inner_radius/m_bullseye_outer_radius,
	      l->m_angle_radians,
	      0,
	      1);
      
  for(int r=0;r<m_ring_count;r++) {
    DrawEllipse(debug0,
		l->m_x,
		l->m_y,
		l->m_width*m_data_ring_outer_radii[r]/m_bullseye_outer_radius,
		l->m_height*m_data_ring_outer_radii[r]/m_bullseye_outer_radius,
		l->m_angle_radians,
		0,
		1);	
  }
      
  for(int k=0;k<m_sector_count;k++) {
    for(int r=0;r<m_ring_count;r++) {
      float x;
      float y;
      l->ProjectPoint(m_read_angles[5*k],
		      m_data_ring_outer_radii[r]/m_bullseye_outer_radius,
		      &x,
		      &y);
      cvLine(debug0,
	     cvPoint(cvRound(l->m_x),cvRound(l->m_y)),
	     cvPoint(cvRound(x),cvRound(y)),
	     0,1);
	  
      l->ProjectPoint(m_read_angles[5*k],
		      m_data_ring_centre_radii[r]/m_bullseye_outer_radius,
		      &x,
		      &y);
      cvLine(debug0,cvPoint(cvRound(x),cvRound(y)),cvPoint(cvRound(x),cvRound(y)), SampleImage(image,x,y) < 128 ? 255 : 0,3);
	  
	  
    }
  }
  cvSaveImage("debug-decode.jpg",debug0);	  	
  cvReleaseImage(&debug0);
#endif
  throw Coder::InvalidCode();
};

