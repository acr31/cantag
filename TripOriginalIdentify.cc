#include "TripOriginalIdentify.hh"

#include "TripOriginalParams.hh"

bool
sample(IplImage *image, CvBox2D* ellipse, double sina, double cosa, double angle, int ring,int sync) 
{
  double x = radii_centre[ring]*cos(angle)*ellipse->size.width/2;
  double y = radii_centre[ring]*sin(angle)*ellipse->size.height/2;
  double ix = x*cosa-y*sina + ellipse->center.x;
  double iy = x*sina+y*cosa + ellipse->center.y;
  
  CvPoint p1;
  CvPoint p2;

  p1.x=(int)ellipse->center.x;
  p1.y=(int)ellipse->center.y;
  p2.x=(int)ix;
  p2.y=(int)iy;

  cvLine(image,p1,p2,4,0,8);
  
  return ((uchar*)(image->imageData + image->widthStep*(int)iy))[(int)ix] > (uchar)(128);
}

bool
identifyTag(IplImage *image, CvBox2D* ellipse, std::vector<Tag> *result)
{    

  double radians = ellipse->angle / 180 * M_PI;
  double sina = sin(radians);
  double cosa = cos(radians);

  /* Step 1.  Find synchronization sector. This is two black sectors
     aligned in each code ring */
  bool search = 0;
  int sync;
  for(sync=0;sync<SYNC_COUNT && !search;sync++) {
    search = 1;
    for(int j=0;j<RING_COUNT;j++) {
      search &= sample(image,ellipse,sina,cosa,sync_angles[sync],j,1);
    }    
  }
  
  if (sync<SYNC_COUNT) {
    /* Step 2.  We found a sync sector, now sample the code */ 
    Tag t;
    t.code =0;
    t.box = *ellipse;
    for(int ring=0;ring<RING_COUNT;ring++) {
      for(int sector=1;sector<SECTOR_COUNT;sector++) {
	t.code <<= 1;
	t.code |= sample(image,ellipse,sina,cosa,sector_angles[sector]+sync_angles[sync],ring,0);
      }
    }
    result->push_back(t);
    return 1;
  }
  else {
    /* We failed to find a sync sector return without adding to the result */
    return 0;
  }

}

