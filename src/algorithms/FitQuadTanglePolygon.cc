/**
 * $Header$
 */

#include <total/algorithms/FitQuadTanglePolygon.hh>

/**
 * The corner curvature we use for weeding out false vertexes
 * e.g. for max angle 0.9 => 154 deg [= 360*math.acos(-0.9)/(2*math.pi)]
 */
#define DPMAX_ANGLE 0.7 

/**
 * The threshold we use to determine if we should add an extra edge
 */
//#define DPMAX_THRESH 0.00085
#define DPMAX_THRESH 0.005

#undef POLYGON_DEBUG
  
namespace Total {

  bool FitQuadTanglePolygon::operator()(const ContourEntity& contour, ShapeEntity<QuadTangle>& shape) const {
    const std::vector<float>& points = contour.GetPoints();

    if (points.size() > 4) {
      std::list<std::pair<float,float> > fulllist;
      
      std::list<std::pair<float,float> >::iterator maxi;
      float maxd=0;

      float firstx=*points.begin();
      float firsty=*++points.begin();
    
      //must copy since we want to modify this datastructure and vector is const!
      for(std::vector<float>::const_iterator i = points.begin(); i != points.end(); ++i) {
	float x = *i;
	++i;
	float y = *i;
	fulllist.push_back(std::pair<float,float>(x,y));
	float d = (firstx-x)*(firstx-x)+(firsty-y)*(firsty-y);
	if (d > maxd) {
	  maxi = --fulllist.end();
	  maxd = d;
	}
      }
      fulllist.insert(maxi,*maxi);
      DPRecurse(fulllist,fulllist.begin(),maxi);
      DPRecurse(fulllist,maxi,fulllist.end());
      maxi = fulllist.erase(maxi);
      DPJoin(fulllist,fulllist.begin(),maxi,fulllist.end());
    
      //Since we now know that this is a closed polygon, must check
      //that both ends of the polygon are joined
      if (fulllist.size() > 2) {
	float d = DPAngle(*fulllist.rbegin(),*fulllist.begin(),*++fulllist.begin());
	if (fabs(d) > DPMAX_ANGLE) {
	  fulllist.pop_front();
	}
      }
      if (fulllist.size() > 2) {
	float d = DPAngle(*++fulllist.rbegin(),*fulllist.rbegin(),*fulllist.begin());
	if (fabs(d) > DPMAX_ANGLE) {
	  fulllist.pop_back();
	}
      }    

      if (fulllist.size() == 4) {
	std::list<std::pair<float,float> >::iterator i = fulllist.begin();
	shape.m_shapeDetails = new QuadTangle(i->first,i->second,
					      (++i)->first,i->second,
					      (++i)->first,i->second,
					      (++i)->first,i->second);
	shape.m_shapeFitted = true;
	return true;
      }
    }
    return false;
  }
  

  float FitQuadTanglePolygon::DPAngle(const std::pair<float,float>& p, 
				   const std::pair<float,float>& q,
				   const std::pair<float,float>& r) const {
    
    float ax = p.first-q.first;
    float ay = p.second-q.second;
    float bx = r.first-q.first;
    float by = r.second-q.second;
    
    float dot = ax*bx+ay*by;
    float denom = sqrt(ax*ax+ay*ay)*sqrt(bx*bx+by*by);

    float result = (denom < 1e-15) ? -1 : dot/denom;
#ifdef POLYGON_DEBUG
    PROGRESS("Angle between (" << p.first << "," << p.second << "), (" << q.first << "," << q.second << "), (" <<r.first << "," << r.second << ") is " << acos(result));
#endif
    return result;
  }
  
  void FitQuadTanglePolygon::DPJoin(std::list<std::pair<float,float> >& fulllist,
				 std::list<std::pair<float,float> >::iterator start,
				 std::list<std::pair<float,float> >::iterator mid,
				 std::list<std::pair<float,float> >::iterator end) const {
    if (mid==end) return;

#ifdef POLYGON_DEBUG
    int start_index = -1;
    int mid_index = -1;
    int end_index = -1;
    int counter = 0;
    for(std::list<std::pair<float,float> >::iterator i = fulllist.begin();i!=fulllist.end();++i) {
      if (i == start) start_index = counter;
      if (i == mid) mid_index = counter;
      if (i == end) end_index = counter;
      ++counter;
    }
    if (start_index == -1) start_index = counter;
    if (mid_index == -1) mid_index = counter;
    if (end_index == -1) end_index = counter;
    PROGRESS("DPJoin with list 1 from " << start_index << " to " << mid_index << " (exclusive) and list 2 from " << mid_index << " to " << end_index << " (exclusive)");
#endif

    // keep a copy of this iterator so we can work forwards with the second list
    std::list<std::pair<float,float> >::iterator fwd_mid = mid;

    // move back to the last element of the first list
    --mid;

    std::list<std::pair<float,float> >::iterator z = mid;
#ifdef POLYGON_DEBUG
    PROGRESS("z is (" << z->first << "," << z->second << ")");
#endif
    // a is the first element of the second list
    std::list<std::pair<float,float> >::iterator a = fwd_mid;
#ifdef POLYGON_DEBUG
    PROGRESS("a is (" << a->first << "," << a->second << ")");
#endif

    // check if we have more than one element in list 1
    if (mid != start) {
      --mid;
      std::list<std::pair<float,float> >::iterator y = mid;      
#ifdef POLYGON_DEBUG
      PROGRESS("y is (" << y->first << "," << y->second << ")");
#endif
      float yza = DPAngle(*y,*z,*a);
#ifdef POLYGON_DEBUG
      PROGRESS("Angle yza is " << yza);
#endif
      if (fabs(yza) > DPMAX_ANGLE) {
#ifdef POLYGON_DEBUG
	PROGRESS("Erasing z");
#endif
	fulllist.erase(z);
	z = y;
      }
    }
    else {
#ifdef POLYGON_DEBUG
      PROGRESS("We have only one element in list 1");
#endif
    }


    ++fwd_mid;
    // check we have more than one element in list 2
    if (fwd_mid != end) {
      std::list<std::pair<float,float> >::iterator b = fwd_mid;
#ifdef POLYGON_DEBUG
      PROGRESS("b is (" << b->first << "," << b->second << ")");
#endif
      float zab = DPAngle(*z,*a,*b);
#ifdef POLYGON_DEBUG
      PROGRESS("Angle zab is " << zab);
#endif
      if (fabs(zab) > DPMAX_ANGLE) {
#ifdef POLYGON_DEBUG
	PROGRESS("Erasing a");
#endif
	fulllist.erase(a);    
      }
    }
    else {
#ifdef POLYGON_DEBUG
      PROGRESS("We have only one element in list 2");
#endif
    }

  }

  std::list<std::pair<float,float> >::iterator
  FitQuadTanglePolygon::DPSplit(std::list<std::pair<float,float> >& fulllist,
			     std::list<std::pair<float,float> >::iterator start,
			     std::list<std::pair<float,float> >::iterator end) const {

#ifdef POLYGON_DEBUG
    int start_index = -1;
    int end_index = -1;
    int counter =0;
    for(std::list<std::pair<float,float> >::iterator i = fulllist.begin();i!=fulllist.end();++i) {
      if (i==start) start_index = counter;
      if (i==end) end_index = counter;
      ++counter;
    }
    if (start_index == -1) start_index = counter;
    if (end_index == -1) end_index = counter;
    PROGRESS("Splitting range " << start_index << " to " << end_index);
#endif
    
    assert(start != end);

    // move end back to the last element of the list
    --end;
    
    if (start == end) {
#ifdef POLYGON_DEBUG
      PROGRESS("DPSplit 1 element list - returning");
#endif
      return start;
    }

    float maxd = 0;
    std::list<std::pair<float,float> >::iterator split_iterator = start;

    float fx = start->first;
    float fy = start->second;
    float lx = end->first;
    float ly = end->second;
    
    //    std::cerr << " Size:" << a.size() << std::endl;
    //    std::cerr << " Vals:" << fx << " " << fy << " " << lx << " " << ly << std::endl;
    
    for(;start != end; ++start) {
      float px = start->first;
      float py = start->second;

      float a = (fx-px)*(fx-px)+(fy-py)*(fy-py);
      float b = (px-lx)*(px-lx)+(py-ly)*(py-ly);
      float c = (fx-lx)*(fx-lx)+(fy-ly)*(fy-ly);
      float d = 0;
      
      try {
	//do we want fabs(sqrt(...)) here?
	d = sqrt(b-(((b+c-a)*(b+c-a))/(4*c)));
      } catch (...) {
	d = 0;
      }
      
      //std::cerr << "a,b,c,d " << a << " " << b << " " << c << " " << d << std::endl;
      
      if (d > maxd) {
	split_iterator = start;
	maxd=d;
      }
    }
    
#ifdef POLYGON_DEBUG
    PROGRESS("maxd = " << maxd);
    counter = 0;
    int split_index = -1;
    for(std::list<std::pair<float,float> >::iterator i = fulllist.begin();i!=fulllist.end();++i) {
      if (i==split_iterator) split_index = counter;
      ++counter;
    }
    if (split_index == -1) split_index = counter;
    PROGRESS("Split index is " << split_index);
#endif

    //    std::cerr << " - DPSplit: maxi = (" << maxi->first << "," << maxi->second 
    //	      << "), d = " << maxd << " tmpmaxi = "<< tmpmaxi << std::endl;
    
    return split_iterator;
  }
  
  void FitQuadTanglePolygon::DPRecurse(std::list<std::pair<float,float> >& fulllist,
				    std::list<std::pair<float,float> >::iterator start,
				    std::list<std::pair<float,float> >::iterator end) const {

    if (start == end) {
#ifdef POLYGON_DEBUG
      PROGRESS("DPRecurse Empty list - returning");
#endif
      return;
    }
    
    std::list<std::pair<float,float> >::iterator split_iterator = DPSplit(fulllist,start,end);
    if (split_iterator != start) { // we split the list
#ifdef POLYGON_DEBUG
      PROGRESS("DPRecurse Split list");
#endif
      std::list<std::pair<float,float> >::iterator newpos = fulllist.insert(split_iterator,*split_iterator);
      DPRecurse(fulllist,start,split_iterator);
      DPRecurse(fulllist,split_iterator,end);
      fulllist.erase(newpos);
      DPJoin(fulllist,start,split_iterator,end);
    }
    else { // we didn't split the list so we remove everything except the first and last element
      ++start;
      if (start == end) { // its only a 1 element list so nothing to do
#ifdef POLYGON_DEBUG
	PROGRESS("DPRecurse 1-element list - returning");
#endif
	return;
      }
      --end;
      if (start == end) { // its only a 2 element list so nothing to do
#ifdef POLYGON_DEBUG
	PROGRESS("DPRecurse 2-element list - returning");
#endif
	return;
      }
#ifdef POLYGON_DEBUG
      PROGRESS("DPRecurse erasing middle of list");
#endif
      fulllist.erase(start,end);
    }
  }
  
}
