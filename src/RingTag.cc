/**
 * $Header$
 */

#include <Config.hh>
#include <Drawing.hh>
#include <Coder.hh>
#include <Ellipse2D.hh>
#include <RingTag.hh>
#include <Camera.hh>
#include <circletransform.hh>
#include <vector>
#define RING_TAG_DEBUG
#define RING_TAG_IMAGE_DEBUG

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
      // pick the colour based on the value we encode - sensible
      int colour = ((working & (1<<i)) == (1<<i)) ? black : white;

      // or pick the colour based on which sector we are encoding - useful for debuggin
      //      int colour = (int)((double)(i*m_sector_count+j) / (double)(m_ring_count*m_sector_count) * 255);

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

void RingTag::DecodeNode(SceneGraphNode< ShapeChain<Ellipse> >* node, const Camera& camera, const Image& image) {
#ifdef RING_TAG_DEBUG
  PROGRESS("DecodeNode called");
#endif

  // get the ellipse this node encompasses
  const Ellipse el = node->GetShapes().GetShape();
#ifdef RING_TAG_DEBUG
  PROGRESS("Ellipse is a=["<<
	   el.GetA()<<","<<
	   el.GetB()<<","<<
	   el.GetC()<<","<<
	   el.GetD()<<","<<
	   el.GetE()<<","<<
	   el.GetF()<<"];");
#endif
  
  // extract its pose
  float transform1[16];
  float transform2[16];
  GetTransform(el,transform1,transform2);
  
  // project some points for the inner circle using both interpretations and check which one fits  
  int count = 10;
  float projected1[count*2];
  float projected2[count*2];
  for(int i=0;i<count*2;i+=2) {
    float x = cos( (float)i*PI/(float)count ) * m_bullseye_inner_radius / m_bullseye_outer_radius;
    float y = sin( (float)i*PI/(float)count ) * m_bullseye_inner_radius / m_bullseye_outer_radius;
    ApplyTransform(transform1,x,y,projected1+i,projected1+i+1);
    ApplyTransform(transform2,x,y,projected2+i,projected2+i+1);
  }     

  Image* t = cvCloneImage(&image);
  for(int i=0;i<count*2;i++) {
    cvLine(t,
	   cvPoint((int)projected1[i],(int)projected1[i+1]),
	   cvPoint((int)projected1[i],(int)projected1[i+1]),
	   128,5);    
  }
  cvSaveImage("test.jpg",t);


  // get the children of this node and check for a good match with either interpretation
  float* correcttrans = NULL;
  for(std::vector< SceneGraphNode< ShapeChain<Ellipse> >* >::iterator i = node->GetChildren().begin(); i!=node->GetChildren().end();i++) {
    if ( (*i)->GetShapes().GetShape().CheckError(projected1,count,0.1) ) {
      correcttrans = transform1;
      break;
    }
#ifdef RING_TAG_DEBUG
    PROGRESS("Child does not match in orientation 1");
#endif
    
    if ( (*i)->GetShapes().GetShape().CheckError(projected2,count,0.1) ) {
      correcttrans = transform2;
      break;
    }
#ifdef RING_TAG_DEBUG
    PROGRESS("Child does not match in orientation 2");
#endif
  }

  if (correcttrans != NULL) {
#ifdef RING_TAG_DEBUG
    PROGRESS("Found a concentric inner circle with matching contour");
#endif
    // we've found a good transform

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
	float pt[2] = { cos(m_read_angles[j]) * m_data_ring_centre_radii[k]/m_bullseye_outer_radius,
			sin(m_read_angles[j]) * m_data_ring_centre_radii[k]/m_bullseye_outer_radius };
	ApplyTransform(correcttrans,pt,1);
	camera.NPCFToImage(pt,1);
	bool sample = SampleImage(image,pt[0],pt[1]) > 128;
	read_code[currentcode] <<=1;
	read_code[currentcode] |= (sample ? 1:0);
      }
    }
    
#ifdef RING_TAG_DEBUG
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
#ifdef RING_TAG_IMAGE_DEBUG
	draw_read(image,camera,correcttrans,i);
#endif
	unsigned long long code = DecodeTag(read_code[i]);

#ifdef RING_TAG_DEBUG
	PROGRESS("Found code " << code);
#endif	
	LocatedObject* lobj = node->GetLocatedObject();
	for(int i=0;i<16;i++) {
	  lobj->transform[i] = correcttrans[i];
	}		
	node->SetInspected();
	return;
      }
    }
    
#ifdef RING_TAG_DEBUG
    PROGRESS("Failed to read code");    
#endif
#ifdef RING_TAG_IMAGE_DEBUG
    draw_read(image,camera,correcttrans,0);
#endif
    node->SetInspected();
    return;
  }
};  

void RingTag::draw_circle(Image* debug0, const Camera& camera, float l[16], double radius) {
  float oldpts[2] = { radius ,
		      0 };
  float pts[2];
  ApplyTransform(l,oldpts,1);
  camera.NPCFToImage(oldpts,1);
  int count = 360;
  for(int step=1;step<=count;step++) {
    pts[0] = cos( (float)step*2*PI/(float)count ) * radius;
    pts[1] = sin( (float)step*2*PI/(float)count ) * radius;
    camera.NPCFToImage(pts,1);
    cvLine(debug0,cvPoint(cvRound(oldpts[0]),cvRound(oldpts[1])),cvPoint(cvRound(pts[0]),cvRound(pts[1])), 0,1);	
    oldpts[0] = pts[0];
    oldpts[1] = pts[1];
  }  
}

void RingTag::draw_read(const Image& image, const Camera& camera, float l[16], int i) {
  Image* debug0 = cvCloneImage(&image);
  cvConvertScale(debug0,debug0,-1,255); 
  cvConvertScale(debug0,debug0,0.5,128); 
  
  draw_circle(debug0,camera,l,m_bullseye_inner_radius / m_bullseye_outer_radius);
  draw_circle(debug0,camera,l,1);
  draw_circle(debug0,camera,l,m_data_inner_radius / m_bullseye_outer_radius);
    
  for(int r=0;r<m_ring_count;r++) {
    draw_circle(debug0,camera,l,m_data_ring_outer_radii[r] / m_bullseye_outer_radius);
  }
  
  for(int k=0;k<m_sector_count;k++) {
    for(int r=0;r<m_ring_count;r++) {
      float pts[2];
      pts[0] = cos( m_read_angles[5*k+((i+1)%5)] ) * m_data_ring_centre_radii[r]/m_bullseye_outer_radius;
      pts[1] = sin( m_read_angles[5*k+((i+1)%5)] ) * m_data_ring_centre_radii[r]/m_bullseye_outer_radius;
      
      camera.NPCFToImage(pts,1);
      // pick the colour to be the opposite of the sampled point so we can see the dot
      int colour = SampleImage(image,pts[0],pts[1]) < 128 ? 0: 255; // our debug image is inverted 255 : 0;
      // or pick the colour to be on a gradient so we see the order it samples in
      //int colour = (int)((double)k/(double)m_sector_count*255);
      cvLine(debug0,cvPoint(cvRound(pts[0]),cvRound(pts[1])),cvPoint(cvRound(pts[0]),cvRound(pts[1])), colour,4);
      
    }
  }
  cvSaveImage("debug-decode.jpg",debug0);	  	
  cvReleaseImage(&debug0);
}
