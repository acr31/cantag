/**
 * $Header$
 */

#include <RingTag.hh>
#include <findtransform.hh>
#include <vector>

#define RING_TAG_DEBUG
#define RING_TAG_IMAGE_DEBUG
#undef DRAW_FIELD_DEBUG
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

void RingTag::Draw2D(Image& image,unsigned long long code) {
  // Work from the outside inwards
  
  int x0 = image.GetWidth()/2;
  int y0 = image.GetHeight()/2;
  int size = (image.GetWidth() < image.GetHeight()) ? image.GetWidth()/2 : image.GetHeight()/2;

  bool setscale = false;
  float scalefactor;

  if (m_bullseye_outer_radius > m_data_outer_radius) {
    scalefactor = (float)size/m_bullseye_outer_radius;
    setscale = true;
#ifdef RING_TAG_DEBUG
    PROGRESS("Drawing outer bullseye edge");
    PROGRESS("Set scale factor to : "<<scalefactor);
#endif
    image.DrawFilledCircle(x0,y0,scalefactor*m_bullseye_outer_radius,COLOUR_BLACK);
  }
  
  if (m_bullseye_inner_radius > m_data_outer_radius) {
#ifdef RING_TAG_DEBUG
    PROGRESS("Drawing inner bullseye edge");
#endif
    if (!setscale) {
      scalefactor = (float)size/m_bullseye_inner_radius;
      setscale=true;
#ifdef RING_TAG_DEBUG
      PROGRESS("Set scale factor to: " <<scalefactor);
#endif
    }
    image.DrawFilledCircle(x0,y0,scalefactor*m_bullseye_inner_radius,COLOUR_WHITE);
  }

#ifdef RING_TAG_DEBUG
  PROGRESS("Drawing data rings");
#endif
  unsigned long long encoded = EncodeTag(code);

  if (!setscale) {
    scalefactor = (float)size/m_data_ring_outer_radii[m_ring_count-1];   
#ifdef RING_TAG_DEBUG
      PROGRESS("Set scale factor to: " <<scalefactor);
#endif
  }
  
  for(int i=m_ring_count-1;i>=0;i--) {
    unsigned long long working = encoded;
    for(int j=0;j<m_sector_count;j++) {	
      // pick the colour based on the value we encode - sensible
      int colour = ((working & (1<<i)) == (1<<i)) ? COLOUR_BLACK : COLOUR_WHITE;
      // or pick the colour based on which sector we are encoding - useful for debugging
      //      int colour = (int)((float)(i+j*m_ring_count) / (float)(m_ring_count*m_sector_count) * 255);
      working >>= m_ring_count;
      image.DrawSector(x0,y0,scalefactor*m_data_ring_outer_radii[i],m_sector_angles[j],m_sector_angles[j+1],colour);
    }
  }

#ifdef RING_TAG_DEBUG  
  PROGRESS("Blanking out inside of data rings");
#endif
  if (m_data_inner_radius != 0) {
        image.DrawFilledCircle(x0,y0,m_data_inner_radius*scalefactor,
			       (m_bullseye_inner_radius < m_data_inner_radius &&
				m_bullseye_outer_radius > m_data_inner_radius) ? COLOUR_BLACK : COLOUR_WHITE);
  }
  
  if (m_bullseye_outer_radius < m_data_inner_radius) {
#ifdef RING_TAG_DEBUG
    PROGRESS("Drawing outer bullseye edge");
#endif
    image.DrawFilledCircle(x0,y0,m_bullseye_outer_radius*scalefactor, COLOUR_BLACK);
  }
  
  if (m_bullseye_inner_radius < m_data_inner_radius) {
#ifdef RING_TAG_DEBUG
    PROGRESS("Drawing inner bullseye edge");	   
#endif
    image.DrawFilledCircle(x0,y0,m_bullseye_inner_radius*scalefactor,COLOUR_WHITE);
  }
}

void RingTag::DecodeNode(SceneGraphNode< ShapeChain<Ellipse> >* node, const Camera& camera, const Image& image) {
#ifdef RING_TAG_DEBUG
  PROGRESS("DecodeNode called");
#endif

  // get the ellipse this node encompasses
  const Ellipse el = node->GetShapes().GetShape();

  if (!el.IsFitted()) {
#ifdef RING_TAG_DEBUG
    PROGRESS("Discarding unfitted ellipse.");
#endif
    return;
  }

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
  el.GetTransform(transform1,transform2);
  
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

  // get the children of this node and check for a good match with either interpretation
  float* correcttrans = NULL;
  for(std::vector< SceneGraphNode< ShapeChain<Ellipse> >* >::iterator i = node->GetChildren().begin(); i!=node->GetChildren().end();i++) {
    float error1 = (*i)->GetShapes().GetShape().GetError(projected1,count);
    float error2 = (*i)->GetShapes().GetShape().GetError(projected2,count);

#ifdef RING_TAG_DEBUG
    PROGRESS("Error 1 " << error1);
    PROGRESS("Error 2 " << error2);    
#endif

    if ((error1 < error2) && (error1 < 0.001)) {
#ifdef RING_TAG_DEBUG
      PROGRESS("Chose orientation 1 with error "<<error1<<" instead of orientation 2 with error "<<error2);
#endif
      correcttrans = transform1;
      break;
    }

    if ((error2 <= error1) && (error2 < 0.001)) {
#ifdef RING_TAG_DEBUG
      PROGRESS("Chose orientation 2 with error "<<error2<<" instead of orientation 1 with error "<<error1);
#endif
      correcttrans = transform2;
      break;
    }
  }


  if (correcttrans == NULL) {
    PROGRESS("Failed to find a valid transform - just selecting one arbitrarily!");
    correcttrans = transform1;
  }

  if (correcttrans != NULL) {
    float normal[3];
    GetNormalVector(correcttrans,normal);


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

    unsigned long long read_code[5] = {0};
    for(int j=0;j<m_sector_count*5;j++) {
      // read a chunk by sampling each ring and shifting and adding
      int currentcode = j%5;      
      for(int k=m_ring_count-1;k>=0;k--) {
	float tpt[]=  {  cos(m_read_angles[j]) * m_data_ring_centre_radii[k]/m_bullseye_outer_radius,
			 sin(m_read_angles[j]) * m_data_ring_centre_radii[k]/m_bullseye_outer_radius };
	ApplyTransform(correcttrans,tpt[0],tpt[1],tpt,tpt+1);
	camera.NPCFToImage(tpt,1);
	bool sample = image.Sample(tpt[0],tpt[1]) > 128;
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
	draw_read(image,camera,correcttrans,(i+1)%5);
#endif
	unsigned long long code = DecodeTag(read_code[i]);

#ifdef RING_TAG_DEBUG
	PROGRESS("Found code " << code);
#endif	
	projected1[0] = 0;
	projected1[1] = 0;
	ApplyTransform(correcttrans,projected1[0],projected1[1],projected1,projected1+1);
	camera.NPCFToImage(projected1,1);
	PROGRESS("Ellipse position is "<<projected1[0]<<","<<projected1[1]);
	
	LocatedObject* lobj = node->GetLocatedObject();
	for(int i=0;i<16;i++) {
	  lobj->transform[i] = correcttrans[i];
	}		
	node->SetInspected();
	exit(0);
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
    exit(0);
    return;
  }
  else {
#ifdef RING_TAG_DEBUG
    PROGRESS("Failed to find a valid transformation");
#endif
  }
};  

void RingTag::draw_circle(Image& debug0, const Camera& camera, float l[16], double radius) {
  float oldpts[2] = { radius ,
		      0 };
  float pts[2];
  ApplyTransform(l,oldpts,1);
  camera.NPCFToImage(oldpts,1);
  int count = 360;
  for(int step=1;step<=count;step++) {
    pts[0] = cos( (float)step*2*PI/(float)count ) * radius;
    pts[1] = sin( (float)step*2*PI/(float)count ) * radius;
    ApplyTransform(l,pts,1);
    camera.NPCFToImage(pts,1);
    int colour = COLOUR_BLACK;
    //int colour = (int)((float)step/(float)count * 255);
    debug0.DrawLine(oldpts[0],oldpts[1],pts[0],pts[1],colour,1);
    oldpts[0] = pts[0];
    oldpts[1] = pts[1];
  }  
}

void RingTag::draw_read(const Image& image, const Camera& camera, float l[16], int i) {
  Image debug0(image);
  debug0.ConvertScale(-1,255);
  debug0.ConvertScale(0.5,128);
  
#ifdef DRAW_FIELD_DEBUG
  float step = 0.2;
  float max = (m_bullseye_outer_radius < m_data_outer_radius ? m_data_outer_radius : m_bullseye_outer_radius) / m_bullseye_inner_radius / 2;

  for(float k=-1;k<=1;k+=step) {
      float pts[] = { -max,k*max,
		      max,k*max,
		      k*max,-max,
		      k*max,max };
      ApplyTransform(l,pts,4);
      camera.NPCFToImage(pts,4);
      debug0.DrawLine(pts[0],pts[1],pts[2],pts[3],COLOUR_BLACK,1);
      debug0.DrawLine(pts[4],pts[5],pts[6],pts[7],COLOUR_BLACK,1);
  }
#else
  draw_circle(debug0,camera,l,m_bullseye_inner_radius / m_bullseye_outer_radius);
  draw_circle(debug0,camera,l,1);
  draw_circle(debug0,camera,l,m_data_inner_radius / m_bullseye_outer_radius);
    
  for(int r=0;r<m_ring_count;r++) {
    draw_circle(debug0,camera,l,m_data_ring_outer_radii[r] / m_bullseye_outer_radius);
  }
  
  int counter=0;
  for(int k=0;k<m_sector_count;k++) {
    for(int r=m_ring_count-1;r>=0;r--) {
      float pts[2];
      pts[0] = cos( m_read_angles[5*k+((i+1)%5)] ) * m_data_ring_centre_radii[r]/m_bullseye_outer_radius;
      pts[1] = sin( m_read_angles[5*k+((i+1)%5)] ) * m_data_ring_centre_radii[r]/m_bullseye_outer_radius;
      ApplyTransform(l,pts,1);
      camera.NPCFToImage(pts,1);
      // pick the colour to be the opposite of the sampled point so we can see the dot
      int colour = image.Sample(pts[0],pts[1]) < 128 ? COLOUR_BLACK:COLOUR_WHITE; // our debug image is inverted 255 : 0;
      // or pick the colour to be on a gradient so we see the order it samples in
      //int colour = (int)((double)counter/(double)m_sector_count*255);
      debug0.DrawPoint(pts[0],pts[1],colour,4);
    }
    counter++;
  }
#endif
  debug0.Save("debug-decode.bmp");
}
