/**
 * $Header$
 */

#ifndef RING_TAG_GUARD
#define RING_TAG_GUARD

#include <Config.hh>
#include <Tag.hh>
#include <Coder.hh>
#include <Camera.hh>
#include <ShapeChain.hh>
#include <Ellipse.hh>
#include <findtransform.hh>

#ifdef TEXT_DEBUG
# define RING_TAG_DEBUG
#endif

#ifdef IMAGE_DEBUG
# define  RING_TAG_IMAGE_DEBUG
# undef   DRAW_FIELD_DEBUG
#else 
# undef RING_TAG_IMAGE_DEUG
# undef DRAW_FIELD_DEBUG
#endif

int debug_image_counter = 0;

#undef LINEAR_ELLIPSE

/**
 * The number of readings to make from a tag.  We then look for pairs
 * of reading that are the same in order to guess the correct angle of
 * the tag.
 *
 * \todo regression test with gl harness
 */
#define READING_COUNT 10
#define MAX_CHILD_DISTANCE 1
template<int RING_COUNT, int SECTOR_COUNT>
class RingTag : public virtual Tag< ShapeChain<Ellipse>, RING_COUNT*SECTOR_COUNT >, 
		protected virtual Coder<RING_COUNT*SECTOR_COUNT> {
private:
  float m_bullseye_inner_radius;
  float m_bullseye_outer_radius;
  float m_data_inner_radius;
  float m_data_outer_radius;
  float *m_data_ring_outer_radii;
  float *m_data_ring_centre_radii;
  float *m_sector_angles;
  float *m_read_angles;

  float *m_sin_read_angles;
  float *m_cos_read_angles;

public:
  RingTag(float bullseye_inner_radius,
	  float bullseye_outer_radius,
	  float data_inner_radius,
	  float data_outer_radius);
  virtual ~RingTag();
  virtual void Draw2D(Image& image, CyclicBitSet<RING_COUNT*SECTOR_COUNT>& tag_data) const;
  virtual bool DecodeNode(SceneGraphNode<  ShapeChain<Ellipse>, RING_COUNT*SECTOR_COUNT >* node, const Camera& camera, const Image& image) const;
 
private:
  void draw_circle(Image& debug0, const Camera& camera, float l[16], double radius) const;
  void draw_read(const Image& image, const Camera& camera, float l[16], int i) const;

};

template<int RING_COUNT,int SECTOR_COUNT> RingTag<RING_COUNT,SECTOR_COUNT>::RingTag(float bullseye_inner_radius,
										    float bullseye_outer_radius,
										    float data_inner_radius,
										    float data_outer_radius) : 
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
    
  m_data_ring_centre_radii = new float[RING_COUNT];
  m_data_ring_outer_radii = new float[RING_COUNT];
    
  float ring_width = (m_data_outer_radius-m_data_inner_radius)/RING_COUNT;
    
  for(int i=0;i<RING_COUNT;i++) {
    m_data_ring_outer_radii[i] = m_data_inner_radius + ring_width*(i+1);
    m_data_ring_centre_radii[i] = m_data_ring_outer_radii[i]-ring_width/2;    
  }
    
  // now the sector angles - go one more than necessary it makes drawing easier ;-)
    
  m_sector_angles = new float[SECTOR_COUNT+1];
  for(int i=0;i<SECTOR_COUNT+1;i++) {
    m_sector_angles[i] = 2*PI/SECTOR_COUNT *i;
  }
    
  // when we read the tag we read a total of five times and then
  // look for three codes which are the same
  m_read_angles = new float[SECTOR_COUNT*READING_COUNT];
  m_sin_read_angles = new float[SECTOR_COUNT*READING_COUNT];
  m_cos_read_angles = new float[SECTOR_COUNT*READING_COUNT];
  for(int i=0;i<SECTOR_COUNT*READING_COUNT;i++) {
    m_read_angles[i] = 2*PI/SECTOR_COUNT/READING_COUNT * i;
    m_sin_read_angles[i] = sin(m_read_angles[i]);
    m_cos_read_angles[i] = cos(m_read_angles[i]);
  }
}


template<int RING_COUNT,int SECTOR_COUNT> RingTag<RING_COUNT,SECTOR_COUNT>::~RingTag() {
  delete[] m_data_ring_outer_radii;
  delete[] m_data_ring_centre_radii;
  delete[] m_sector_angles;
  delete[] m_read_angles;
  delete[] m_sin_read_angles;
  delete[] m_cos_read_angles;
}

template<int RING_COUNT,int SECTOR_COUNT> void RingTag<RING_COUNT,SECTOR_COUNT>::Draw2D(Image& image, CyclicBitSet<RING_COUNT*SECTOR_COUNT>& tag_data) const {

  EncodePayload(tag_data);
      
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

  if (!setscale) {
    scalefactor = (float)size/m_data_ring_outer_radii[RING_COUNT-1];   
#ifdef RING_TAG_DEBUG
    PROGRESS("Set scale factor to: " <<scalefactor);
#endif
  }
  
  for(int i=RING_COUNT-1;i>=0;i--) {
    for(int j=0;j<SECTOR_COUNT;j++) {	
      // pick the colour based on the value we encode - sensible
      int colour = tag_data[j*RING_COUNT+i] ? COLOUR_BLACK : COLOUR_WHITE;
      // or pick the colour based on which sector we are encoding - useful for debugging
      //int colour = (int)((float)(i+j*RING_COUNT) / (float)(RING_COUNT*SECTOR_COUNT) * 128)+128;
      // or pick the colour as if we were reading the tag in the wrong order - useful for pictures
      //int colour = (int)((float)(i*SECTOR_COUNT+j) / (float)(RING_COUNT*SECTOR_COUNT) * 128)+128;
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

template<int RING_COUNT,int SECTOR_COUNT> bool RingTag<RING_COUNT,SECTOR_COUNT>::DecodeNode(SceneGraphNode<  ShapeChain<Ellipse>, RING_COUNT*SECTOR_COUNT >* node, const Camera& camera, const Image& image) const {
#ifdef RING_TAG_DEBUG
  PROGRESS("DecodeNode called");
#endif

  if (node->IsInspected()) {
#ifdef RING_TAG_DEBUG
    PROGRESS("This node has already been inspected. Skipping it");
#endif
    return false;
  }

  if (!node->HasChildren()) {
#ifdef RING_TAG_DEBUG
    PROGRESS("This node has no children.  Skipping it");
#endif
    node->ClearLocatedObject();
    return false;
  }

  // get the ellipse this node encompasses
  ShapeChain<Ellipse> sce = node->GetShapes();
  Ellipse el = sce.GetShape();
  //  Ellipse el = node->GetShapes().GetShape();

  if (!el.IsFitted()) {
#ifdef RING_TAG_DEBUG
    PROGRESS("Discarding unfitted ellipse.");
#endif
    node->ClearLocatedObject();

    return false;
  }

  bool found = false;
  for(typename std::vector< SceneGraphNode< ShapeChain<Ellipse>,  RING_COUNT*SECTOR_COUNT >* >::iterator i = node->GetChildren().begin(); i!=node->GetChildren().end();i++) {
    Ellipse c1 = (*i)->GetShapes().GetShape();
    if (c1.IsFitted()) {
      float dist = (c1.GetX0()-el.GetX0())*(c1.GetX0()-el.GetX0()) + (c1.GetY0()-el.GetY0())*(c1.GetY0()-el.GetY0());
#ifdef RING_TAG_DEBUG
      PROGRESS("Distance is " << dist);
#endif
      if (dist < MAX_CHILD_DISTANCE) {
	found = true;
	break;
      }
    }
  }

  if (!found) {
#ifdef RING_TAG_DEBUG
    PROGRESS("No concentric children");
#endif
    return false;
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

#ifdef LINEAR_ELLIPSE
  el.GetTransformLinear(transform1,transform2);
#else
  el.GetTransform(transform1,transform2);
#endif
  
  // project some points for the inner circle using both interpretations and check which one fits  
  int count = 200;
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
  float* correctnormal = NULL;
  float normal1[3];
  GetNormalVector(transform1,camera,normal1);
  float normal2[3];
  GetNormalVector(transform2,camera,normal2);
#ifdef RING_TAG_DEBUG
  PROGRESS("Normal vector for transform1 " << normal1[0] << " " << normal1[1] << " " << normal1[2]);
  PROGRESS("Normal vector for transform2 " << normal2[0] << " " << normal2[1] << " " << normal2[2]);
#endif
  
  if ((normal1[2] < 0) && (normal2[2] > 0)) {
#ifdef RING_TAG_DEBUG
    PROGRESS("Chose orientation 1 because it points towards the camera and orientation 2 doesnt");
#endif
    correcttrans = transform1;
    correctnormal = normal1;
  }
  else if ((normal1[2] > 0) && (normal2[2] < 0)) {
#ifdef RING_TAG_DEBUG
    PROGRESS("Chose orientation 2 because it points towards the camera and orientation 1 doesnt");
#endif
    correcttrans = transform2;
    correctnormal = normal2;
  }
  else {
    // find out where the centre of each of the projected ellipses would be
    float points1[2];
    float points2[2];
    ApplyTransform(transform1,0,0,points1,points1+1);
    ApplyTransform(transform2,0,0,points2,points2+1);

    // now work out the direction of the vector that would run from
    // the centre of the main ellipse to the centre of each projeted
    // ellipse.
    //    float direction1 = atan((el.GetY0()-points1[1])/(el.GetX0()-points1[0]));
    //    float direction2 = atan((el.GetY0()-points2[1])/(el.GetX0()-points2[0]));

    for(typename std::vector< SceneGraphNode< ShapeChain<Ellipse>,  RING_COUNT*SECTOR_COUNT >* >::iterator i = node->GetChildren().begin(); i!=node->GetChildren().end();i++) {
      
      Ellipse child = (*i)->GetShapes().GetShape();


      //      float test_dir
      /*
      float error1 = (le.GetX0()-points1[0])*(le.GetX0()-points1[0]) + (le.GetY0()-points1[1])*(le.GetY0()-points1[1]);
	float error2 = (le.GetX0()-points2[0])*(le.GetX0()-points2[0]) + (le.GetY0()-points2[1])*(le.GetY0()-points2[1]);
      */

      float error1 = (*i)->GetShapes().GetShape().GetError(projected1,count);
      float error2 = (*i)->GetShapes().GetShape().GetError(projected2,count);
#ifdef RING_TAG_DEBUG
      PROGRESS("Error 1 " << error1);
      PROGRESS("Error 2 " << error2);   
#endif
      if ((error1 < error2) && (error1 < 0.01)) {
#ifdef RING_TAG_DEBUG
	PROGRESS("Chose orientation 1 with error "<<error1<<" instead of orientation 2 with error "<<error2);
#endif
	correcttrans = transform1;
	correctnormal = normal1;
	break;
      }
      else if ((error2 <= error1) && (error2 < 0.01)) {
#ifdef RING_TAG_DEBUG
	PROGRESS("Chose orientation 2 with error "<<error2<<" instead of orientation 1 with error "<<error1);
#endif
	correcttrans = transform2;
	correctnormal = normal2;
	break;
      }
    }
  }

  if (correcttrans == NULL) {
#ifdef RING_TAG_DEBUG
    PROGRESS("Failed to find a valid transform - just selecting one arbitrarily!");
#endif
    //    node->ClearLocatedObject();
    //    return false;
    correcttrans = transform1;
    correctnormal = normal1;
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
    CyclicBitSet<RING_COUNT*SECTOR_COUNT>*  read_code[READING_COUNT];
    for(int b=0;b<READING_COUNT;b++) { 
      read_code[b] = new CyclicBitSet<RING_COUNT*SECTOR_COUNT>();
    };

    for(int j=0;j<SECTOR_COUNT*READING_COUNT;j++) {
      // read a chunk by sampling each ring and shifting and adding
      int currentcode = j%READING_COUNT;      
      for(int k=RING_COUNT-1;k>=0;k--) {
	float tpt[]=  {  m_cos_read_angles[j] * m_data_ring_centre_radii[k]/m_bullseye_outer_radius,
			 m_sin_read_angles[j] * m_data_ring_centre_radii[k]/m_bullseye_outer_radius };
	ApplyTransform(correcttrans,tpt[0],tpt[1],tpt,tpt+1);
	camera.NPCFToImage(tpt,1);
	bool sample = image.Sample(tpt[0],tpt[1]);
	(*read_code[currentcode])[j/READING_COUNT * RING_COUNT + k] = sample;
      }
    }
    
#ifdef RING_TAG_DEBUG
    for(int i=0;i<READING_COUNT;i++) {
      PROGRESS("Code candidate " << i << " is " << *read_code[i]);
    }
#endif

    // scan round decoding candidates until we find two successful reads that are the same
    CyclicBitSet<RING_COUNT*SECTOR_COUNT>* last_read = NULL;
    int orientation;
    int code_ptr;
    for(code_ptr=0;code_ptr<READING_COUNT;code_ptr++) {
      if ((orientation = DecodePayload(*read_code[code_ptr])) >= 0) {
	if ((last_read != NULL) && (*last_read == *read_code[code_ptr])) {
	  break;
	}
	last_read = &(*read_code[code_ptr]);
      }
    }


    if (code_ptr < READING_COUNT) {
      // we now know that we had to rotate the code by "orientation"
      // bits in order to align it.  This corresponds to a rotation of
      // orientation/ring_count sectors.  Which corresponds to a
      // rotation of orientation/ring_count*sector_count/360 degrees.
      // we also know that this reading was made starting from m_read_angles[code_ptr]
      // this gives the final rotation as 
      // m_read_angles[code_ptr] + orientation/ring_count*sector_count/2/M_PI
      
      float angle = m_read_angles[code_ptr-1] + orientation/RING_COUNT*2*M_PI/SECTOR_COUNT;

#ifdef RING_TAG_IMAGE_DEBUG
      draw_read(image,camera,correcttrans,code_ptr);
#endif
#ifdef RING_TAG_DEBUG
      PROGRESS("Found code " << *read_code[code_ptr]);
#endif	
#ifdef TEXT_DEBUG
      projected1[0] = 0;
      projected1[1] = 0;
      ApplyTransform(correcttrans,projected1[0],projected1[1],projected1,projected1+1);
      camera.NPCFToImage(projected1,1);
      PROGRESS("Found code " << *read_code[code_ptr] << " at " << projected1[0] << "," << projected1[1]);
#endif
#ifdef RING_TAG_DEBUG

      PROGRESS("Ellipse position is "<<projected1[0]<<","<<projected1[1]);
#endif
      LocatedObject<RING_COUNT*SECTOR_COUNT>* lobj = node->GetLocatedObject();
      lobj->LoadTransform(correcttrans,1,angle,camera);
      lobj->tag_code = read_code[code_ptr];	   
      for(int i=0;i<READING_COUNT;++i) {
	if (i != code_ptr) {
	  delete read_code[i];
	}
      }
      return true;
    }
    else {
      for(int i=0;i<READING_COUNT;++i) {
	delete read_code[i];
      } 
#ifdef RING_TAG_DEBUG
      PROGRESS("Failed to decode two identical readings");
#endif
#ifdef RING_TAG_IMAGE_DEBUG
      draw_read(image,camera,correcttrans,0);
#endif
    }
  }
  else {   
#ifdef RING_TAG_DEBUG
    PROGRESS("Failed to find a valid transformation");
#endif
  }  
  node->ClearLocatedObject();
  return false;
};  

template<int RING_COUNT,int SECTOR_COUNT> void RingTag<RING_COUNT,SECTOR_COUNT>::draw_circle(Image& debug0, const Camera& camera, float l[16], double radius) const {
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

template<int RING_COUNT,int SECTOR_COUNT>  void RingTag<RING_COUNT,SECTOR_COUNT>::draw_read(const Image& image, const Camera& camera, float l[16], int i) const {
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
    
  for(int r=0;r<RING_COUNT;r++) {
    draw_circle(debug0,camera,l,m_data_ring_outer_radii[r] / m_bullseye_outer_radius);
  }
  
  int counter=0;
  for(int k=0;k<SECTOR_COUNT;k++) {
    for(int r=RING_COUNT-1;r>=0;r--) {
      float pts[2];
      pts[0] = cos( m_read_angles[READING_COUNT*k+((i+1)%READING_COUNT)] ) * m_data_ring_centre_radii[r]/m_bullseye_outer_radius;
      pts[1] = sin( m_read_angles[READING_COUNT*k+((i+1)%READING_COUNT)] ) * m_data_ring_centre_radii[r]/m_bullseye_outer_radius;
      ApplyTransform(l,pts,1);
      camera.NPCFToImage(pts,1);
      // pick the colour to be the opposite of the sampled point so we can see the dot
      int colour = image.Sample(pts[0],pts[1]) ? COLOUR_BLACK:COLOUR_WHITE; // our debug image is inverted 255 : 0;
      // or pick the colour to be on a gradient so we see the order it samples in
      //int colour = (int)((double)(k*RING_COUNT+(RING_COUNT-1-r))/(double)(SECTOR_COUNT*RING_COUNT)*255);
      debug0.DrawPoint(pts[0],pts[1],colour,3);
    }
    counter++;
  }
#endif
  char filename[256];
  snprintf(filename,255,"debug-decode-%0.5d.bmp",debug_image_counter++);
  filename[255]=0;
  debug0.Save(filename);
}

#endif//RING_TAG_GUARD


