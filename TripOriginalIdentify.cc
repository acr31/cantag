#include "TripOriginalIdentify.hh"
#include <iostream>
#include "TripOriginalParams.hh"

bool
sample(IplImage *image, CvBox2D* ellipse, double sina, double cosa, double angle, int ring,int draw,int sector) 
{
  double x = radii_centre[ring]*cos(angle)*ellipse->size.width/2;
  double y = radii_centre[ring]*sin(angle)*ellipse->size.height/2;
  double ix = x*cosa-y*sina + ellipse->center.x;
  double iy = x*sina+y*cosa + ellipse->center.y;
  
  bool result = ((uchar*)(image->imageData + image->widthStep*(int)iy))[(int)ix] < (uchar)(128);

  if (draw) {
    CvPoint p1;
    CvPoint p2;
    
    p1.x=(int)ix;
    p1.y=(int)iy;
    p2.x=(int)ix;
    p2.y=(int)iy;
    
    cvLine(image,p1,p2, sector*10,5,8);
    cvLine(image,p1,p2, 255-sector*10,2,8);
  }
  return result;
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
  for(sync=0;sync<SYNC_COUNT;sync++) {
    search = 1;
    for(int j=0;j<RING_COUNT;j++) {
      bool result = sample(image,ellipse,sina,cosa,sync_angles[sync],j,0,9);
      search &= result;
    }    
    if (search) break;
  }

  if (sync<SYNC_COUNT) {
    /* Step 2.  We found a sync sector, now sample the code */ 
    Tag t;
    t.code =0;
    t.box = *ellipse;
    for(int ring=RING_COUNT-1;ring>=0;ring--) {
      for(int sector=0;sector<SECTOR_COUNT;sector++) {
	bool result = sample(image,ellipse,sina,cosa,sector_angles[sector]+sync_angles[sync],ring,1,sector);
	std::cout << ring << " " << sector_angles[sector]+sync_angles[sync] << " " << result << std::endl;
	t.code |= (result ? 1<<sector+(RING_COUNT-ring-1)*SECTOR_COUNT : 0);
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

