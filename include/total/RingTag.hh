/**
 * $Header$
 */

#ifndef RING_TAG_GUARD
#define RING_TAG_GUARD

#include <tripover/Config.hh>
#include <tripover/Tag.hh>
#include <tripover/Coder.hh>
#include <tripover/Camera.hh>
#include <tripover/ShapeChain.hh>
#include <tripover/EllipseTransform.hh>
#include <tripover/findtransform.hh>
#include <tripover/Ellipse.hh>
#include <tripover/Coder.hh>
#include <iostream>
#include <cassert>

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

static int debug_image_counter = 0;


/**
 * \todo regression test with gl harness
 */
#define READING_COUNT 200
#define MAX_CHILD_DISTANCE 0.1
template<int RING_COUNT, int SECTOR_COUNT, class C>
class RingTag : public virtual Tag< ShapeChain<C>, RING_COUNT*SECTOR_COUNT >, 
		public virtual Coder<RING_COUNT*SECTOR_COUNT>,
                protected virtual EllipseTransform {
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
  virtual LocatedObject<RING_COUNT*SECTOR_COUNT>* GetTransform(typename ShapeTree<ShapeChain<C> >::Node* node, 
							       const Camera& camera, const Image& image) const;
  virtual bool DecodeNode(LocatedObject<RING_COUNT*SECTOR_COUNT>* lobj,
			  const Camera& camera, const Image& image) const;

  virtual bool CheckTransform(const LocatedObject<RING_COUNT*SECTOR_COUNT>* lobj, typename ShapeTree<ShapeChain<C> >::Node* node) const;
  virtual bool CheckDecode(const LocatedObject<RING_COUNT*SECTOR_COUNT>* lobj, const Camera& camera, const Image& image) const;
 
private:
  void draw_circle(Image& debug0, const Camera& camera, float l[16], double radius) const;
  void draw_read(const Image& image, const Camera& camera, float l[16]) const;

};

template<int RING_COUNT,int SECTOR_COUNT,class C> RingTag<RING_COUNT,SECTOR_COUNT,C>::RingTag(float bullseye_inner_radius,
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


template<int RING_COUNT,int SECTOR_COUNT,class C> RingTag<RING_COUNT,SECTOR_COUNT,C>::~RingTag() {
  delete[] m_data_ring_outer_radii;
  delete[] m_data_ring_centre_radii;
  delete[] m_sector_angles;
  delete[] m_read_angles;
  delete[] m_sin_read_angles;
  delete[] m_cos_read_angles;
}

template<int RING_COUNT,int SECTOR_COUNT,class C> void RingTag<RING_COUNT,SECTOR_COUNT,C>::Draw2D(Image& image, CyclicBitSet<RING_COUNT*SECTOR_COUNT>& tag_data) const {

  EncodePayload(tag_data);
      
  // Work from the outside inwards
    
  int x0 = image.GetWidth()/2;
  int y0 = image.GetHeight()/2;
  int size = (image.GetWidth() < image.GetHeight()) ? image.GetWidth()/2 : image.GetHeight()/2;
    
  bool setscale = false;
  float scalefactor = 0;

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
  
  int pointer = 0;
  for(int i=0;i<SECTOR_COUNT;++i) {
    for(int j=0;j<RING_COUNT;++j) {
      int colour = tag_data[pointer] ? COLOUR_BLACK : COLOUR_WHITE;
      // or pick the colour based on which sector we are encoding - useful for debugging
      //int colour = (int)((float)(pointer) / (float)(RING_COUNT*SECTOR_COUNT) * 128)+128;
      image.DrawSector(x0,y0,scalefactor*m_data_ring_outer_radii[RING_COUNT-j-1],m_sector_angles[i],m_sector_angles[i+1],colour);
      ++pointer;
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

template<int RING_COUNT,int SECTOR_COUNT,class C> LocatedObject<RING_COUNT*SECTOR_COUNT>* RingTag<RING_COUNT,SECTOR_COUNT,C>::GetTransform(typename ShapeTree<ShapeChain<C> >::Node* node, 
															       const Camera& camera, const Image& image) const {
#ifdef RING_TAG_DEBUG
  PROGRESS("DecodeNode called");
#endif

  /**
   * \todo this check will not work for the SplitRing tags and it didn;t work for ring tags either
   * push the outerborder field for contours onto the shapes
   */
  if (node->children.size() == 0 && false) {
#ifdef RING_TAG_DEBUG
    PROGRESS("This node has " << node->children.size() << " children.  Skipping it");
#endif
    return NULL;
  }

  // get the ellipse this node encompasses
  ShapeChain<C> sce = node->matched;
  C el = sce.GetShape();

  if (!el.IsFitted()) { return false; }

  bool found = false;
  for(typename std::vector< typename ShapeTree<ShapeChain<C> >::Node* >::iterator i = node->children.begin(); 
      i!=node->children.end();
      ++i) {
    C c1 = (*i)->matched.GetShape();
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
    return NULL;
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

  TransformEllipse(el,transform1,transform2);
  
  // project some points for the inner circle using both interpretations and check which one fits  
  int count = 200;
  std::vector<float> projected1;
  std::vector<float> projected2;
  std::vector<float> projected1b; // this one is a circle between the outer and the inner - should be all 0-pixels
  std::vector<float> projected2b; // this one is a circle between the outer and the inner - should be all 0-pixels
  for(int i=0;i<count*2;i+=2) {
    float x = cos( (float)i*PI/(float)count );
    float innerx = x * m_bullseye_inner_radius / m_bullseye_outer_radius;
    float midx = x * (1-m_bullseye_inner_radius / m_bullseye_outer_radius)/2;
    float y = sin( (float)i*PI/(float)count );
    float innery = y * m_bullseye_inner_radius / m_bullseye_outer_radius;
    float midy = y * (1-m_bullseye_inner_radius / m_bullseye_outer_radius)/2;
    ApplyTransform(transform1,innerx,innery,projected1);
    ApplyTransform(transform2,innerx,innery,projected2);

    ApplyTransform(transform1,midx,midy,projected1b);
    ApplyTransform(transform2,midx,midy,projected2b);
  }     
  camera.NPCFToImage(projected1b);
  camera.NPCFToImage(projected2b);
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
  
  // first check that the points on the mid transforms are all 0-pixels
  bool transform1mid_ok = true;
  /*
  for(std::vector<float>::const_iterator i = projected1b.begin();
      i!=projected1b.end();
      ++i) {
    float x = *i;
    ++i;
    float y = *i;
    if (image.Sample(x,y)) { transform1mid_ok = false; break; }
  }
  */
#ifdef RING_TAG_DEBUG
  PROGRESS("Transform1mid_ok " << transform1mid_ok);
#endif

  // first check that the points on the mid transforms are all 0-pixels
  bool transform2mid_ok = true;
  /*
  for(std::vector<float>::const_iterator i = projected2b.begin();
      i!=projected2b.end();
      ++i) {
    float x = *i;
    ++i;
    float y = *i;
    if (image.Sample(x,y)) { transform2mid_ok = false; break; }
  }
  */
#ifdef RING_TAG_DEBUG
  PROGRESS("Transform2mid_ok " << transform2mid_ok);
#endif

  if (!transform1mid_ok && !transform2mid_ok) {
#ifdef RING_TAG_DEBUG
    PROGRESS("Neither transform1mid or transform2mid are ok - aborting");
#endif
    return NULL;
  }

  if ((normal1[2] < 0) && (normal2[2] > 0)) {
    if (transform1mid_ok) {
#ifdef RING_TAG_DEBUG
      PROGRESS("Chose orientation 1 because it points towards the camera and orientation 2 doesnt");
#endif
      correcttrans = transform1;
      correctnormal = normal1;
    }
    else {
#ifdef RING_TAG_DEBUG
      PROGRESS("Orientation 1 points towards camera, and orientation 2 doesnt but Orientation 1 mid is false - aborting");
#endif      
      return NULL;
    }
  }
  else if ((normal1[2] > 0) && (normal2[2] < 0)) {
    if (transform2mid_ok) {
#ifdef RING_TAG_DEBUG
      PROGRESS("Chose orientation 2 because it points towards the camera and orientation 1 doesnt");
#endif
      correcttrans = transform2;
      correctnormal = normal2;
    }
    else {
#ifdef RING_TAG_DEBUG
      PROGRESS("Orientation 2 points towards camera, and orientation 1 doesnt but Orientation 2 mid is false - aborting");
#endif      
      return NULL;
    }
  }
  else {
    if (transform1mid_ok && !transform2mid_ok) {
#ifdef RING_TAG_DEBUG
      PROGRESS("Orientation 1 mid is true and Orientation 2 mid is false - choosing 1");
#endif      
      correcttrans = transform1;
      correctnormal = normal1;
    }
    else if (transform2mid_ok && !transform1mid_ok) {
#ifdef RING_TAG_DEBUG
      PROGRESS("Orientation 2 mid is true and Orientation 1 mid is false - choosing 2");
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
      
      for(typename std::vector< typename ShapeTree<ShapeChain<C> >::Node* >::iterator i = node->children.begin();
	  i!=node->children.end();
	  ++i) {      
	C child = (*i)->matched.GetShape();
	
	
	//      float test_dir
	/*
	  float error1 = (le.GetX0()-points1[0])*(le.GetX0()-points1[0]) + (le.GetY0()-points1[1])*(le.GetY0()-points1[1]);
	  float error2 = (le.GetX0()-points2[0])*(le.GetX0()-points2[0]) + (le.GetY0()-points2[1])*(le.GetY0()-points2[1]);
	*/
	
	float error1 = (*i)->matched.GetShape().GetError(projected1);
	float error2 = (*i)->matched.GetShape().GetError(projected2);
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
    
    LocatedObject<RING_COUNT*SECTOR_COUNT>* lobj = new LocatedObject<RING_COUNT*SECTOR_COUNT>();
    lobj->LoadTransform(correcttrans,1, camera);
    return lobj;
  }
  return NULL;
}

template<int RING_COUNT,int SECTOR_COUNT,class C> bool RingTag<RING_COUNT,SECTOR_COUNT,C>::DecodeNode(LocatedObject<RING_COUNT*SECTOR_COUNT>* lobj,const Camera& camera, const Image& image) const {

  float* correcttrans = lobj->transform;
  
  bool left = true;
  bool right = true;
  int k=RING_COUNT-1;
  int j;
  // scan round reading a sector width apart until we read two adjacent cells of different values
  for(j=0;j<SECTOR_COUNT*READING_COUNT;j+=READING_COUNT) {
    float tpt[]=  {  m_cos_read_angles[j] * m_data_ring_centre_radii[k]/m_bullseye_outer_radius,
		     m_sin_read_angles[j] * m_data_ring_centre_radii[k]/m_bullseye_outer_radius };
    ApplyTransform(correcttrans,tpt[0],tpt[1],tpt,tpt+1);
    camera.NPCFToImage(tpt,1);
    right = image.Sample(tpt[0],tpt[1]) != 0;
    if ((j>0) && (left != right)) break;
    left = right;
  }
  
  if (left == right) {
#ifdef RING_TAG_DEBUG
    PROGRESS("Failed to find a sector edge!");
#endif
    return false;
  }
  
  int leftindex = j-READING_COUNT;
  int rightindex = j;
  while(rightindex - leftindex > 1) {
    int centre = (leftindex + rightindex) / 2;
    float tpt[]=  {  m_cos_read_angles[centre] * m_data_ring_centre_radii[k]/m_bullseye_outer_radius,
		     m_sin_read_angles[centre] * m_data_ring_centre_radii[k]/m_bullseye_outer_radius };
    ApplyTransform(correcttrans,tpt[0],tpt[1],tpt,tpt+1);
    camera.NPCFToImage(tpt,1);
    bool sample = image.Sample(tpt[0],tpt[1]) != 0;
    if (sample) {
      if (left && !right) {
	leftindex = centre;
      }
      else if (!left && right) {
	rightindex = centre;
      }
      else {
	assert(false);
      }
    }
    else {
      if (left && !right) {
	rightindex = centre;
      }
      else if (!left && right) {
	leftindex = centre;
      }
      else {
	assert(false);
      }
    }
  }

  CyclicBitSet<RING_COUNT*SECTOR_COUNT>* read_code = new CyclicBitSet<RING_COUNT*SECTOR_COUNT>();  
  int index = 0;
  int readindex = (leftindex + READING_COUNT/2) % (SECTOR_COUNT * READING_COUNT);
  for(int j=0;j<SECTOR_COUNT;++j) {
    // read a chunk by sampling each ring and shifting and adding
    for(int k=0;k<RING_COUNT;++k) {
      float tpt[]=  {  m_cos_read_angles[readindex] * m_data_ring_centre_radii[RING_COUNT-1-k]/m_bullseye_outer_radius,
		       m_sin_read_angles[readindex] * m_data_ring_centre_radii[RING_COUNT-1-k]/m_bullseye_outer_radius };
      ApplyTransform(correcttrans,tpt[0],tpt[1],tpt,tpt+1);
      camera.NPCFToImage(tpt,1);
      bool sample = image.Sample(tpt[0],tpt[1]) != 0;
      (*read_code)[index++] = sample;      
    }
    readindex+=READING_COUNT;
    readindex %= SECTOR_COUNT * READING_COUNT;
  }

#ifdef RING_TAG_DEBUG
  PROGRESS("Code candidate is " << *read_code);
#endif
  
  int orientation = DecodePayload(*read_code);
  
  if (orientation >= 0) {
    // we now know that we had to rotate the code by "orientation"
    // bits in order to align it.  This corresponds to a rotation of
    // orientation/ring_count sectors. 
    
    int index = (leftindex + (orientation/RING_COUNT) * READING_COUNT) % (SECTOR_COUNT * READING_COUNT);
    
    float rotation[16] = { m_cos_read_angles[index], -m_sin_read_angles[index],0,0,
			   m_sin_read_angles[index], m_cos_read_angles[index],0,0,
			   0,0,1,0,
			   0,0,0,1};
    
    
    float finaltrans[16] = {0};
    // premultiply rotations by transforms
    for(int row=0;row<4;row++) {
      for(int col=0;col<4;col++) {
	for(int k=0;k<4;k++) {
	  finaltrans[row*4+col] += correcttrans[row*4+k] * rotation[k*4+col];
	}
      }
    }

#ifdef RING_TAG_IMAGE_DEBUG
    draw_read(image,camera,finaltrans);
#endif
#ifdef RING_TAG_DEBUG
    PROGRESS("Found code " << *read_code);
#endif	
#ifdef RING_TAG_DEBUG
#ifdef TEXT_DEBUG
    float temp[2] = {0,0};
    ApplyTransform(correcttrans,temp[0],temp[1],temp,temp+1);
    camera.NPCFToImage(temp,1);
    PROGRESS("Found code " << *read_code << " at " << temp[0] << "," << temp[1]);
#endif
#endif
    lobj->LoadTransform(finaltrans,1,camera);
    lobj->tag_codes.push_back(read_code);
    return true;
  }
  else {
    delete read_code;
#ifdef RING_TAG_DEBUG
    PROGRESS("Failed to decode code");
#endif
#ifdef RING_TAG_IMAGE_DEBUG
    int index = (leftindex + (orientation/RING_COUNT) * READING_COUNT) % (SECTOR_COUNT * READING_COUNT);
    
    float rotation[16] = { m_cos_read_angles[index], -m_sin_read_angles[index],0,0,
			   m_sin_read_angles[index], m_cos_read_angles[index],0,0,
			   0,0,1,0,
			   0,0,0,1};
    
    
    float finaltrans[16] = {0};
    // premultiply rotations by transforms
    for(int row=0;row<4;row++) {
      for(int col=0;col<4;col++) {
	for(int k=0;k<4;k++) {
	  finaltrans[row*4+col] += correcttrans[row*4+k] * rotation[k*4+col];
	}
      }
    }

    draw_read(image,camera,finaltrans);
#endif
    return false;
  }
};  

template<int RING_COUNT,int SECTOR_COUNT,class C> void RingTag<RING_COUNT,SECTOR_COUNT,C>::draw_circle(Image& debug0, const Camera& camera, float l[16], double radius) const {
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
    debug0.DrawLine(oldpts[0],oldpts[1],pts[0],pts[1],colour,2);
    oldpts[0] = pts[0];
    oldpts[1] = pts[1];
  } 

  oldpts[0] = 0; oldpts[1] = 0;
  ApplyTransform(l,oldpts,1);
  camera.NPCFToImage(oldpts,1);

  for(int i=0;i<SECTOR_COUNT*READING_COUNT;i+=READING_COUNT) {
    pts[0] = m_cos_read_angles[i]*radius;
    pts[1] = m_sin_read_angles[i]*radius;
    ApplyTransform(l,pts,1);
    camera.NPCFToImage(pts,1);
    debug0.DrawLine(oldpts[0],oldpts[1],pts[0],pts[1],COLOUR_BLACK,2);
  }
}

template<int RING_COUNT,int SECTOR_COUNT,class C>  void RingTag<RING_COUNT,SECTOR_COUNT,C>::draw_read(const Image& image, const Camera& camera, float l[16]) const {
  int i =0;
  Image debug0(image);
  debug0.ConvertScale(50,0);
  debug0.ConvertScale(-1,255);
  
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
    debug0.DrawLine(pts[0],pts[1],pts[2],pts[3],COLOUR_BLACK,2);
    debug0.DrawLine(pts[4],pts[5],pts[6],pts[7],COLOUR_BLACK,2);
  }
#else
  draw_circle(debug0,camera,l,m_bullseye_inner_radius / m_bullseye_outer_radius);
  draw_circle(debug0,camera,l,1);
  draw_circle(debug0,camera,l,m_data_inner_radius / m_bullseye_outer_radius);
    
  for(int r=0;r<RING_COUNT;r++) {
    draw_circle(debug0,camera,l,m_data_ring_outer_radii[r] / m_bullseye_outer_radius);
  }
  
  int counter=0;
  for(int k=0;k<SECTOR_COUNT*READING_COUNT;k+=READING_COUNT) {
    for(int r=0;r<RING_COUNT;++r) {
      float pts[2];
      pts[0] = cos( m_read_angles[k + READING_COUNT/2] ) * m_data_ring_centre_radii[RING_COUNT-1-r]/m_bullseye_outer_radius;
      pts[1] = sin( m_read_angles[k + READING_COUNT/2] ) * m_data_ring_centre_radii[RING_COUNT-1-r]/m_bullseye_outer_radius;
      ApplyTransform(l,pts,1);
      camera.NPCFToImage(pts,1);
      // pick the colour to be the opposite of the sampled point so we can see the dot
      int colour = image.Sample(pts[0],pts[1]) ? COLOUR_BLACK:COLOUR_WHITE; // our debug image is inverted 255 : 0;
      // or pick the colour to be on a gradient so we see the order it samples in
      //int colour = (int)((double)counter/(double)(SECTOR_COUNT*RING_COUNT)*255);
      debug0.DrawPoint(pts[0],pts[1],colour,4);
      counter++;
    }
  }
#endif
  char filename[256];
  snprintf(filename,255,"debug-decode-%0.5d.pnm",debug_image_counter++);
  filename[255]=0;
  debug0.Save(filename);
}

template<int RING_COUNT,int SECTOR_COUNT,class C> bool RingTag<RING_COUNT,SECTOR_COUNT,C>::CheckTransform(const LocatedObject<RING_COUNT*SECTOR_COUNT>* lobj, typename ShapeTree<ShapeChain<C> >::Node* node) const {
  // project some points for the inner circle using both interpretations and check which one fits  
  int count = 200;
  std::vector<float> projected;
  for(int i=0;i<count*2;i+=2) {
    float x = cos( (float)i*PI/(float)count );
    float y = sin( (float)i*PI/(float)count );
    ApplyTransform(lobj->transform,x,y,projected);
  }       
  return node->matched.GetShape().Check(projected);
}

/**
 * \todo implement this properly
 */
template<int RING_COUNT,int SECTOR_COUNT,class C> bool RingTag<RING_COUNT,SECTOR_COUNT,C>::CheckDecode(const LocatedObject<RING_COUNT*SECTOR_COUNT>* lobj, const Camera& camera, const Image& image) const {
  LocatedObject<RING_COUNT*SECTOR_COUNT> copy(*lobj);
  copy.tag_codes.clear();
  DecodeNode(&copy,camera,image);
  return copy.tag_codes.size() == lobj->tag_codes.size();
}


#endif//RING_TAG_GUARD


