#include "TripOriginalTag.hh"
#include "concentricellipse.hh"

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

TripOriginalTag::TripOriginalTag(CvBox2D ellipse, long code) : m_ellipse(ellipse), m_code(code) {};

const double TripOriginalTag::sector_angles[] = {0,
						 0.392699081698724,
						 0.785398163397448,
						 1.17809724509617,
						 1.5707963267949,
						 1.96349540849362,
						 2.35619449019234,
						 2.74889357189107,
						 3.14159265358979,
						 -2.74889357189107,
						 -2.35619449019234,
						 -1.96349540849362,
						 -1.5707963267949,
						 -1.17809724509617,
						 -0.785398163397448,
						 -0.392699081698725};
  
const double TripOriginalTag::sync_angles[] = {0,
					       0.261799387799149,
					       0.523598775598299,
					       0.785398163397448,
					       1.0471975511966,
					       1.30899693899575,
					       1.5707963267949,
					       1.83259571459405,
					       2.0943951023932,
					       2.35619449019234,
					       2.61799387799149,
					       2.87979326579064,
					       3.14159265358979,
					       -2.87979326579064,
					       -2.61799387799149,
					       -2.35619449019234,
					       -2.0943951023932,
					       -1.83259571459405,
					       -1.5707963267949,
					       -1.30899693899575,
					       -1.0471975511966,
					       -0.785398163397448,
					       -0.523598775598299,
					       -0.261799387799149};

const double TripOriginalTag::radii_outer[] = {1.5,1.9};
const double TripOriginalTag::radii_centre[] = {1.3,1.7};
const double TripOriginalTag::radii_inner[] = {1.1,1.5};

void TripOriginalTag::Process(IplImage *image, 
			      std::vector<Tag*> *result) 
{

  /* ------------ */
  /* 1 - Convert image to grayscale */
  PROGRESS("Converting image to greyscale");
  IplImage *gray = cvCreateImage(cvSize(image->width,image->height),IPL_DEPTH_8U,1);
  cvCvtColor(image,gray,CV_RGB2GRAY);
#ifdef IMAGEDEBUG
  cvSaveImage("debug-grayscale.jpg",gray);
#endif
  PROGRESS("Done");
  /* ------------ */

  /* ------------ */
  /* 2 - Adaptive Threshold */
  PROGRESS("Adaptive thresholding");
  int window_size = gray->width/8;
  window_size+= 1-(window_size %2); /* window size must be an odd number */
  cvAdaptiveThreshold(gray,gray,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,window_size,40);
#ifdef IMAGEDEBUG
  cvSaveImage("debug-adaptive.jpg",gray);
#endif
  PROGRESS("Done");
  /* ------------ */
  
  /* ------------ */
  /* 3 - Contour Following */
  PROGRESS("Contour following");
  CvMemStorage *store = cvCreateMemStorage(0);
  CvSeq *seq = NULL;
  IplImage *copy = cvCloneImage(gray); // the find contours process changes the image ;-(
  int num = cvFindContours(copy,store,&seq,sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_TC89_KCOS);
  cvReleaseImage(&copy);
#ifdef IMAGEDEBUG
  IplImage *debug1Clone = cvCloneImage(image);
  cvDrawContours(debug1Clone,seq,CV_RGB(255,255,0),CV_RGB(255,255,0),100,2,8);
  cvSaveImage("debug-contours.jpg",debug1Clone);
  cvReleaseImage(&debug1Clone);
#endif
  PROGRESS("Done - Found " << num << " contours");
  /* ------------ */


  /* ------------ */
  /* 4 - Identifying concentric ellipses */
  PROGRESS("Concentric ellipses");
  std::vector<CvBox2D> ellipses;
  int numEllipse = FindConcentricEllipses(seq,&ellipses);
#ifdef IMAGEDEBUG
  IplImage *debug2Clone = cvCloneImage(image);
  for(std::vector<CvBox2D>::const_iterator i = ellipses.begin();i != ellipses.end(); i++) 
    {
      cvEllipseBox(debug2Clone,*i,CV_RGB(255,255,0),2);
    }
  cvSaveImage("debug-ellipse.jpg",debug2Clone);
  cvReleaseImage(&debug2Clone);
#endif
  PROGRESS("Done - Found " << numEllipse << " concentric pairs");
  /* ------------ */
  

  /* ------------ */
  /* 5 - Decoding tag name */
  PROGRESS("Decoding tags");
  for(std::vector<CvBox2D>::const_iterator i = ellipses.begin(); i != ellipses.end(); i++) 
    {
      double radians = i->angle / 180 * M_PI;
      double sint = sin(radians);
      double cost = cos(radians);
      double a = i->size.width/2;
      double b = i->size.height/2;

      /* ------------ */
      /* 5.1 - Find synchronization sector. This is two black sectors
	 aligned in each code ring */
      PROGRESS("Searching for synchronization sector");
      bool search = 0;
      bool secondtry =0;
      int sync;
#ifdef IMAGEDEBUG
      IplImage* debug3Clone = cvCloneImage(image);
#endif
      for(sync=0;sync<SYNC_COUNT;sync++) {
	search = 1;
	for(int ring=0;ring<RING_COUNT;ring++) {
	  double x = radii_centre[ring]*cos(sync_angles[sync])*a;
	  double y = radii_centre[ring]*sin(sync_angles[sync])*b;
	  double ix = x*cost-y*sint + i->center.x;
	  double iy = x*sint+y*cost + i->center.y;
	  bool result = ((uchar*)(gray->imageData + gray->widthStep*(int)iy))[(int)ix] < (uchar)(128);	  
#ifdef IMAGEDEBUG
	  CvPoint p;
	  p.x=(int)ix;
	  p.y=(int)iy;
	  cvLine(debug3Clone,p,p,CV_RGB(255,255,0),5,8);
#endif
	  search &= result;
	}    
	if (secondtry) { 
	  if (!search) secondtry=0;
	  break; 
	}
	if (search) {
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
      if (sync<SYNC_COUNT+secondtry) {
	PROGRESS ("Sync Found.  Reading Code");
	double syncangle;
	/* If we found the sync sector on two adjacent samples then
	   take the mid value for the start point */
	if (secondtry) {
	  PROGRESS ("Adjusted sync position for sector boundary");
	  syncangle = (sync_angles[sync-1] + sync_angles[sync])/2;
	}
	else {
	  syncangle = sync_angles[sync-1];
	}

#ifdef IMAGEDEBUG
	IplImage* debug4Clone = cvCloneImage(image);
#endif
	success = 1;
	for(int sector=1;sector<SECTOR_COUNT;sector++) {
	  unsigned int sector_value = 0;
	  for(int ring=RING_COUNT-1;ring>=0;ring--) {
	    double x = radii_centre[ring]*cos(sector_angles[sector]+syncangle)*a;
	    double y = radii_centre[ring]*sin(sector_angles[sector]+syncangle)*b;
	    double ix = x*cost-y*sint + i->center.x;
	    double iy = x*sint+y*cost + i->center.y;
	    sector_value <<=1;
	    sector_value |= (((uchar*)(gray->imageData + gray->widthStep*(int)iy))[(int)ix] < (uchar)(128) ? 1:0);
#ifdef IMAGEDEBUG
	    CvPoint p;
	    p.x=(int)ix;
	    p.y=(int)iy;
	    cvLine(debug4Clone,p,p,CV_RGB(255,255,0),5,8);
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
	  result->push_back(new TripOriginalTag(*i,code));
	}
	else {
	  PROGRESS("Checksum did not match - refusing tag");
	}
      }
    }
  cvReleaseImage(&gray);
}
   
std::ostream& TripOriginalTag::Print(std::ostream& s) const
{
  s << m_code;
  return s;
}

void TripOriginalTag::Show(IplImage *image) {
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
    double x = radii_outer[RING_COUNT-1]*cos(theta)*m_ellipse.size.width/2;
    double y = radii_outer[RING_COUNT-1]*sin(theta)*m_ellipse.size.height/2;
    double ix = x*cos(m_ellipse.angle*180/M_PI)-y*sin(m_ellipse.angle*180/M_PI) + m_ellipse.center.x;
    double iy = x*sin(m_ellipse.angle*180/M_PI)+y*cos(m_ellipse.angle*180/M_PI) + m_ellipse.center.y;    
    CvPoint j;
    j.x = (int)ix;
    j.y = (int)iy;
    cvLine(image,cvPointFrom32f(m_ellipse.center),j,CV_RGB(255,0,255),2);
  }

  
}

void TripOriginalTag::PlotSegment(IplImage *image,
				  int sector, int ring,
				  int value) {
  // convert angles to degrees
  double a1 = sector_angles[(sector+1) % SECTOR_COUNT]/M_PI*180;
  double a2 = sector_angles[sector]/M_PI*180;
  
  // remove negatives
  if (a1<0) a1+=360;
  if (a2<0) a2+=360;
  
  a1=360-a1;
  a2=360-a2;
  
  // if a2 < a1 then we have wrapped around so add 360 to a2
  if (a2<a1) a2+=360;
  
  cvEllipse(image,
	    cvPointFrom32f(m_ellipse.center),
	    cvSize(cvRound( m_ellipse.size.height*radii_outer[ring]/2),
		   cvRound( m_ellipse.size.width*radii_outer[ring]/2)),
	    m_ellipse.angle*180/M_PI,
	    a1,
	    a2,
	    value,
	    -1);
}

void TripOriginalTag::Synthesize(IplImage *image, int white, int black) {
  
  for(int i=RING_COUNT-1;i>=0;i--) {
    // Draw a white filled circle to cover this ring
    cvEllipse(image,
	      cvPointFrom32f(m_ellipse.center),
	      cvSize(cvRound(m_ellipse.size.height*radii_outer[i]/2),
		     cvRound(m_ellipse.size.width*radii_outer[i]/2)),
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
	      cvSize(cvRound( m_ellipse.size.height*radii_inner[i]/2),
		     cvRound( m_ellipse.size.width*radii_inner[i]/2)),
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

