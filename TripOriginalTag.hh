#ifndef TRIP_ORIGINAL_TAG_GUARD
#define TRIP_ORIGINAL_TAG_GUARD

#include <vector>
#include <cv.h>


#include "Tag.hh"
#include "concentricellipse.hh"
#include "posefromcircle.hh"

#define IMAGEDEBUG
#define TEXTDEBUG

#ifdef TEXTDEBUG
#include <iostream>
#define PROGRESS(x) std::cout << x << std::endl
#else
#define PROGRESS(x) 
#endif

#ifdef IMAGEDEBUG
#include <highgui.h>
#endif

/**
 * Encapsulates all the properties of a Trip original style tag
 *
 * The detection method is:
 *
 * Convert image to grayscale
 * Adaptive threshold
 * Contour following
 * Identifying concentric ellipses
 * Decoding tag name
 *
 * The coding system is:
 *
 * One sector all black for synchronization
 * Then some number of checksum sectors
 * Then remaining data sectors
 * Each sector encodes 2^(RING_COUNT-1) values - all set is not allowed
 *
 *
 * RING_COUNT = number of data rings
 * SECTOR_COUNT = total number of sectors 
 * SYNC_COUNT = total number of positions to try when looking for a sync sector
 * CHECKSUM_COUNT = number of checksum sectors
 * TAG_SIZE_SCALE = the outer ring of the tag is this many times the radius of the outer edge of the bulleseye
 * DATA_RING_OFFSET = the width of the bullseye is ths many times the gap between the bullseye and the first data ring
 */

template<int RING_COUNT=2, 
	 int SECTOR_COUNT=16, 
	 int SYNC_COUNT=24, 
	 int CHECKSUM_COUNT=2, 
	 int TAG_SIZE_SCALE=2,
	 int DATA_RING_OFFSET=10>
class TripOriginalTag : virtual Tag {
public:
  static const int ring_count = RING_COUNT;
  static const int sync_angles = SYNC_COUNT;
  static const int checksum_count = CHECKSUM_COUNT;
  static const int sector_count = SECTOR_COUNT;  
  static const int tag_size_scale = TAG_SIZE_SCALE;
  static const int data_ring_offset = DATA_RING_OFFSET;

  static const double sector_angle = 2*CV_PI / (double)SECTOR_COUNT;
  static const double sync_angle = 2*CV_PI / (double)SYNC_COUNT;

  TripOriginalTag(CvBox2D ellipse, long code, CvPoint3D32f normal,CvPoint3D32f centre ) : m_ellipse(ellipse), m_code(code), m_normal(normal), m_centre(centre) {};

  TripOriginalTag(CvBox2D ellipse, long code) : m_ellipse(ellipse), m_code(code) {};

  std::ostream& Print(std::ostream& s) const
  {
    s << "Trip Tag (original)" << std::endl << 
      "Code: " << m_code << std::endl <<
      "Normal: [" << m_normal.x << " " << m_normal.y << " " << m_normal.z << "]"<<std::endl <<
      "Centre: [" << m_centre.x << " " << m_centre.y << " " << m_centre.z << "]"<<std::endl;
    return s;
  }

  void Show(IplImage *image) {
    /*
      cvEllipseBox(image,m_ellipse,CV_RGB(255,255,0),2);
      for(int i=0;i<RING_COUNT;i++) {
      cvEllipse(image,
      cvPointFrom32f(m_ellipse.center),
      cvSize(cvRound(m_ellipse.size.height*radii_outer[i]/2),
      cvRound(m_ellipse.size.width*radii_outer[i]/2)),
      m_ellipse.angle*180/M_PI,
      0,
      360,
      CV_RGB(255,0,0),
      2);
      }
    */
    CvFont font;
    cvInitFont(&font,CV_FONT_VECTOR0,1,1,0,2);
  
    char label[255];
    snprintf(label,255,"%u",m_code);
    cvPutText(image,label,cvPointFrom32f(m_ellipse.center),&font,CV_RGB(0,255,0));  
    for(double theta = 0;theta<2*M_PI;theta+=M_PI/2) {
      double x = Radius(RING_COUNT-1)*cos(theta)*m_ellipse.size.width/2;
      double y = Radius(RING_COUNT-1)*sin(theta)*m_ellipse.size.height/2;
      double ix = x*cos(m_ellipse.angle*180/M_PI)-y*sin(m_ellipse.angle*180/M_PI) + m_ellipse.center.x;
      double iy = x*sin(m_ellipse.angle*180/M_PI)+y*cos(m_ellipse.angle*180/M_PI) + m_ellipse.center.y;    
      CvPoint j;
      j.x = (int)ix;
      j.y = (int)iy;
      cvLine(image,cvPointFrom32f(m_ellipse.center),j,CV_RGB(255,0,255),2);
    }
  }

  void Synthesize(IplImage *image, int white, int black) {
    for(int i=RING_COUNT-1;i>=0;i--) {
      // Draw a white filled circle to cover this ring
      cvEllipse(image,
		cvPointFrom32f(m_ellipse.center),
		cvSize(cvRound(m_ellipse.size.height*Radius(i+1)/2),
		       cvRound(m_ellipse.size.width*Radius(i+1)/2)),
		m_ellipse.angle*180/M_PI,
		0,
		360,
		white,
		-1);
    
      unsigned long code = m_code;
      int base = (1<<RING_COUNT)-1;

      unsigned long checksum = 0;

      // For each data sector
      for(int j=SECTOR_COUNT-1;j>CHECKSUM_COUNT;j--) {

	// Work out the current digit for this sector (need to offset
	// the sector number by the checksum and sync sectors
	double pwr = pow(base,j-CHECKSUM_COUNT-1);
	int value = (int)(trunc(code/pwr));
      
	int colour = (value & (1<<i)) ? black : white;

	code = code % (int)(trunc(pwr));
	checksum += value;

	PlotSegment(image,j,i,colour);
      }
    
      checksum = checksum % (int)pow(base,CHECKSUM_COUNT);

      // Now draw the checksum       
      for(int j=CHECKSUM_COUNT;j>0;j--) {
	// offset for the sync sector
	double pwr = pow(base,j-1);
	int value = (int)(trunc(checksum/pwr));
	int colour = (value & (1<<i)) ? black : white;
	checksum = checksum % (int)trunc(pwr);
	PlotSegment(image,j,i,colour);
      }

      PlotSegment(image,0,i,black);

      // White out all of the tag inside this ring ready for drawing again
      cvEllipse(image,
		cvPointFrom32f(m_ellipse.center),
		cvSize(cvRound( m_ellipse.size.height*Radius(i)/2),
		       cvRound( m_ellipse.size.width*Radius(i)/2)),
		m_ellipse.angle*180/M_PI,
		0,
		360,
		white,
		-1);
    }

    // Now draw the central bullseye
    cvEllipse(image,
	      cvPointFrom32f(m_ellipse.center),
	      cvSize(cvRound( m_ellipse.size.height/2),
		     cvRound( m_ellipse.size.width/2)),
	      m_ellipse.angle*180/M_PI,
	      0,
	      360,
	      black,
	      -1);

    cvEllipse(image,
	      cvPointFrom32f(m_ellipse.center),
	      cvSize(cvRound( m_ellipse.size.height*0.6/2),
		     cvRound( m_ellipse.size.width*0.6/2)),
	      m_ellipse.angle*180/M_PI,
	      0,
	      360,
	      white,
	      -1);

  }


  static void Process(IplImage *image,std::vector<Tag*> *result) {
    IplImage *gray = GrayScaleImage(image);
    AdaptiveThreshold(gray);

    CvMemStorage *store = cvCreateMemStorage(0);
    CvSeq *seq = FollowContours(gray,store,image);

    std::vector<CvBox2D> ellipses;
    ConcentricEllipses(gray,seq,&ellipses,image);

    cvReleaseMemStorage(&store);

    for(std::vector<CvBox2D>::const_iterator i = ellipses.begin(); i != ellipses.end(); i++) 
      {
	Tag* tag = DecodeTag(gray, *i,image );
	if (tag != NULL) {
	  result->push_back(tag);
	}
      }
    cvReleaseImage(&gray);
    
  }

  static inline IplImage* GrayScaleImage(IplImage *image) {
    /* ------------ */
    /* 1 - Convert image to grayscale */
    PROGRESS("Converting image to greyscale");
    IplImage *gray;
    if (image->nChannels==3) {
      gray = cvCreateImage(cvSize(image->width,image->height),IPL_DEPTH_8U,1);
      cvCvtColor(image,gray,CV_RGB2GRAY);
    }
    else {
      gray = cvCloneImage(image);
    }
#ifdef IMAGEDEBUG
    cvSaveImage("debug-grayscale.jpg",gray);
#endif
    return gray;
  }    

  static inline void AdaptiveThreshold(IplImage* gray) {
    /* ------------ */
    /* 2 - Adaptive Threshold */
    PROGRESS("Adaptive thresholding");
    int window_size = gray->width/8;
    window_size+= 1-(window_size %2); /* window size must be an odd number */
    cvAdaptiveThreshold(gray,gray,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,window_size,40);
#ifdef IMAGEDEBUG
    cvSaveImage("debug-adaptive.jpg",gray);
#endif
  }    

  static inline CvSeq* FollowContours(IplImage* gray, CvMemStorage* store, IplImage* image) {
    /* ------------ */
    /* 3 - Contour Following */
    PROGRESS("Contour following");
    IplImage *copy = cvCloneImage(gray); // the find contours process changes the image ;-(
    CvSeq* seq = NULL;
    int num = cvFindContours(copy,store,&seq,sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);
    cvReleaseImage(&copy);
#ifdef IMAGEDEBUG
    IplImage *debug1Clone = cvCloneImage(image);
    cvDrawContours(debug1Clone,seq,CV_RGB(255,255,0),CV_RGB(255,255,0),100,2,8);
    cvSaveImage("debug-contours.jpg",debug1Clone);
    cvReleaseImage(&debug1Clone);
#endif
    PROGRESS("Found " << num << " contours");
    return seq;
  }
    
  static inline void ConcentricEllipses(IplImage* gray, CvSeq *seq, std::vector<CvBox2D> *ellipses, IplImage *image) {
    /* ------------ */
    /* 4 - Identifying concentric ellipses */
    PROGRESS("Concentric ellipses");
    int numEllipse = FindConcentricEllipses(seq,ellipses);
#ifdef IMAGEDEBUG
    IplImage *debug2Clone = cvCloneImage(image);
    for(std::vector<CvBox2D>::const_iterator step = ellipses->begin();step != ellipses->end(); step++) 
      {	
	cvEllipseBox(debug2Clone,*step,CV_RGB(255,255,0),2);
      }
    cvSaveImage("debug-ellipse.jpg",debug2Clone);
    cvReleaseImage(&debug2Clone);
#endif
    PROGRESS("Found " << numEllipse << " concentric pairs");
  }
    
  static inline TripOriginalTag<RING_COUNT,SECTOR_COUNT,SYNC_COUNT,CHECKSUM_COUNT,TAG_SIZE_SCALE>* DecodeTag(IplImage *gray, const CvBox2D &ellipse, IplImage *image) {
    /* ------------ */
    /* 5 - Decoding tag name */
    PROGRESS("Decoding tag");
    double radians = ellipse.angle / 180 * M_PI;
    double sint = sin(radians);
    double cost = cos(radians);
    double a = ellipse.size.width/2;
    double b = ellipse.size.height/2;
    
    /* ------------ */
    /* 5.1 - Find synchronization sector. This is two black sectors
       aligned in each code ring */
    PROGRESS("Searching for synchronization sector");
    bool search = 0;
    bool secondtry =0;
    int sync_try;
#ifdef IMAGEDEBUG
    IplImage* debug3Clone = cvCloneImage(image);
#endif
    int sync_ring = 0;
    for(sync_try=0;sync_try<SYNC_COUNT;sync_try++) {
      search = 1;
      for(sync_ring=0;sync_ring<RING_COUNT;sync_ring++) {
#ifdef IMAGEDEBUG
	bool result = Sample(gray,sync_ring,sync_try,cost,sint,a,b,ellipse.center.x,ellipse.center.y,sync_angle,0,debug3Clone);
#else
	bool result = Sample(gray,sync_ring,sync_try,cost,sint,a,b,ellipse.center.x,ellipse.center.y,sync_angle,0);
#endif
	if (!result) { break; }
      }    

      if (secondtry) { 
	// we stop after the extra pass through.  If the search was
	// successful we leave secondtry to be 1 to signal that we
	// found the sync sector twice.
	if (sync_ring != RING_COUNT) secondtry=0;
	sync_ring = RING_COUNT;
	break; 
      }
      if (sync_ring == RING_COUNT) {
	// if we find a sync sector then loop round once more to see
	// if we find another one.  If we do find another then we
	// assume they are the same sector and we have fallen on an
	// edge and so we'll take the average of the two angles
	secondtry = 1;
      } 
    }
#ifdef IMAGEDEBUG
    cvSaveImage("debug-scan.jpg",debug3Clone);
    cvReleaseImage(&debug3Clone);
#endif      	
    /* ------------ */
    /* 5.2 - If we found a sync sector then read tag */
    int success = 0;
    unsigned long code = 0;
    unsigned long checksum = 0;
    unsigned long target_checksum = 0;
    if (sync_ring == RING_COUNT) {
      PROGRESS ("Sync Found.  Reading Code");
      double angle_offset;
      /* If we found the sync sector on two adjacent samples then
	 take the mid value for the start point */
      if (secondtry) {
	PROGRESS ("Adjusted sync position for sector boundary");
	angle_offset = sync_angle*(sync_try-0.5);
      }
      else {
	angle_offset = sync_angle*(sync_try-1);
      }

#ifdef IMAGEDEBUG
      IplImage* debug4Clone = cvCloneImage(image);
#endif
      success = 1;
      for(int sector=1;sector<SECTOR_COUNT;sector++) {
	unsigned int sector_value = 0;
	for(int ring=RING_COUNT-1;ring>=0;ring--) {
	  sector_value <<=1;
#ifdef IMAGEDEBUG
	  sector_value |= (Sample(gray,ring,sector,cost,sint,a,b,ellipse.center.x,ellipse.center.y,sector_angle,angle_offset,debug4Clone) ? 1 : 0);
#else
	  sector_value |= (Sample(gray,ring,sector,cost,sint,a,b,ellipse.center.x,ellipse.center.y,sector_angle,angle_offset) ? 1 : 0);
#endif
	}
	PROGRESS("Read sector " << sector << ": " << sector_value);
	if (sector_value == (1<<RING_COUNT)-1) {
	  /* We read a sector of all 1's - which we shouldn't have done */
	  PROGRESS("Read bogus sector - aborting this tag");
	  success = 0;
	  break;
	}
	
	if (sector < CHECKSUM_COUNT + 1) {
	  target_checksum += sector_value * (unsigned long)pow((1<<RING_COUNT)-1,sector-1);
	}
	else {
	  code += sector_value * (unsigned long)pow((1<<RING_COUNT)-1,sector-CHECKSUM_COUNT-1);
	  checksum += sector_value;
	}
      }
#ifdef IMAGEDEBUG
      cvSaveImage("debug-read.jpg",debug4Clone);
      cvReleaseImage(&debug4Clone);
#endif	
    }
    else {
      PROGRESS("Failed to find synchronization sector - aborting this tag");
    }
    /* ------------ */
    
    
    if (success) {
      checksum = checksum % (int)pow((1<<RING_COUNT)-1,CHECKSUM_COUNT);
      
      PROGRESS("Code is " << code);
      PROGRESS("Target checksum is " << target_checksum);
      PROGRESS("Checksum is " << checksum);
      
      if (checksum == target_checksum) {
	PROGRESS("Accepting checksum");

	CvPoint3D32f unit_normal;
	CvPoint3D32f centre;
	PoseFromCircle(ellipse,1.0,&unit_normal,&centre);

	return new TripOriginalTag<RING_COUNT,SECTOR_COUNT,SYNC_COUNT,CHECKSUM_COUNT,TAG_SIZE_SCALE>(ellipse,code,unit_normal,centre);	
      }
      else {
	PROGRESS("Checksum did not match - refusing tag");
	return NULL;
      }
    }
    else {
      return NULL;
    }
  }

  static double TagRadius() {
    return Radius(RING_COUNT);
  }
  
private:
  CvBox2D m_ellipse;
  CvPoint3D32f m_normal;
  CvPoint3D32f m_centre;

  void PlotSegment(IplImage *image,
		   int sector, int ring,
		   int value) {
    // convert angles to degrees
    double a1 = sector_angle * ((sector+1) % SECTOR_COUNT)/M_PI*180;
    double a2 = sector_angle * sector /M_PI*180;
    
    a1=360-a1;
    a2=360-a2;
  
    // if a2 < a1 then we have wrapped around so add 360 to a2
    if (a2<a1) a2+=360;
  
    cvEllipse(image,
	      cvPointFrom32f(m_ellipse.center),
	      cvSize(cvRound( m_ellipse.size.height*Radius(ring+1)/2),
		     cvRound( m_ellipse.size.width*Radius(ring+1)/2)),
	      m_ellipse.angle*180/M_PI,
	      a1,
	      a2,
	      value,
	      -1);
  }

  static inline double Radius(double ring) {
    /* The width of each data ring (w) is:
     
    (TAG_SIZE_SCALE - 1/DATA_RING_OFFSET - 1)/RING_COUNT
  
    So the position to poll is the distance to the edge of the bullseye + the white ring around the bullseye + ring*width 
    */
    return (ring * ( TAG_SIZE_SCALE - 1/DATA_RING_OFFSET -1) / RING_COUNT)+1+DATA_RING_OFFSET;
  }
  
  static inline double CentreRadius(double ring) {
    return Radius(ring) + (TAG_SIZE_SCALE-1/DATA_RING_OFFSET-1)/RING_COUNT/2;
  }

  static inline bool Sample(IplImage* image, 
			    int ring, 
			    int sector,
			    double cost, 
			    double sint,
			    double ellipse_a, 
			    double ellipse_b,
			    double ellipse_centre_x, 
			    double ellipse_centre_y,
			    double angle_size,
			    double angle_offset
#ifdef IMAGEDEBUG
			    ,IplImage* debug
#endif
			    ) 
  {
    /* The width of each data ring (w) is:
       
    (TAG_SIZE_SCALE - 1/DATA_RING_OFFSET)/RING_COUNT
  
    So the position to poll is the distance to the edge of the bullseye + the white ring around the bullseye + ring*width + width/2 
  
    The width/2 term is needed to get us into the middle of the ring
    */
    double radius = CentreRadius(ring);
    double x = radius*cos(angle_size*sector+angle_offset)*ellipse_a;
    double y = radius*sin(angle_size*sector+angle_offset)*ellipse_b;
    double ix = x*cost-y*sint + ellipse_centre_x;
    double iy = x*sint+y*cost + ellipse_centre_y;
#ifdef IMAGEDEBUG
    CvPoint p;
    p.x=(int)ix;
    p.y=(int)iy;
    cvLine(debug,p,p,CV_RGB(255,255,0),1,8);
#endif
    return ((uchar*)(image->imageData + image->widthStep*(int)iy))[(int)ix] < (uchar)(128);	  
  }

};


#endif//TRIP_ORIGINAL_TAG_GUARD
