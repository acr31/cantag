#include <Config.hh>
#include <fitquadtangle.hh>

/**
 * Fits a quadtangle to a contour
 *
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/02/11 08:23:49  acr31
 * *** empty log message ***
 *
 */

/**
 * Fit a quadtangle to an array of x,y pairs. 
 *
 * The array points contains 2*numpoints elements in x,y pairs.  This
 * function splits the points into smallish chunks, computes a linear
 * regression of each set of three chunks advancing one chunk each
 * time i.e. 1,2,3 then 2,3,4 then 3,4,5 etc.  It then looks for dips
 * in the correlation value - these correspond to corners on our
 * shape.  To work out where the corners are, it discards the chunks
 * the happened to lie over each corner and then computes another
 * regression over the four whole straight section of chunks.  Then it
 * works out where they intersect.  Most elements of the regressions
 * are only calculated once due to some cunning tricks with the
 * calculations.
 */
QuadTangle2D fitquadtangle(unsigned int* points, int numpoints) {
  for(int i=0;i<numpoints*2;i+=2) {
    std::cout << points[i] << " "<<points[i+1]<<std::endl;
  }

  PROGRESS("Fitting "<<numpoints);
  // slice our points up into smallish chunks
  int numchunks = 80;
  int chunksize = numpoints/numchunks;

  // precalculate various parts of the regression calculations
  unsigned int sumx[numchunks];
  unsigned int sumy[numchunks];
  unsigned int sumxsq[numchunks];
  unsigned int sumysq[numchunks];
  unsigned int sumxy[numchunks];

  int pointer = 0;
  for(int chunk=0;chunk<numchunks;chunk++) {
    sumx[chunk] = 0;
    sumy[chunk] = 0;
    sumxsq[chunk] = 0;
    sumysq[chunk] = 0;
    sumxy[chunk] = 0;
    for(int i=0;i<chunksize;i++) {     
      unsigned int x = points[pointer];
      unsigned int y = points[pointer+1];
      sumx[chunk] += x;
      sumy[chunk] += y;
      sumxsq[chunk] += x*x;
      sumysq[chunk] += y*y;
      sumxy[chunk] += x*y;
      pointer+=2;
    }

    if (chunk>2) {
      // compute a linear regression of three chunks at a time advancing by one chunk each time.
      unsigned long long tsumx = sumx[chunk-2]+sumx[chunk-1]+sumx[chunk];
      unsigned long long tsumy = sumy[chunk-2]+sumy[chunk-1]+sumy[chunk];
      unsigned long long tsumxsq = sumxsq[chunk-2]+sumxsq[chunk-1]+sumxsq[chunk];
      unsigned long long tsumysq = sumysq[chunk-2]+sumysq[chunk-1]+sumysq[chunk];
      unsigned long long tsumxy = sumxy[chunk-2]+sumxy[chunk-1]+sumxy[chunk];
      unsigned int n = chunksize*3;
      unsigned long long numerator = n*tsumxy - tsumx*tsumy;
      numerator *= numerator;

      unsigned long long denominator = (n*tsumxsq-tsumx*tsumx)*(n*tsumysq-tsumy*tsumy);
      //      PROGRESS("Chunks: "<<(chunk-2)<<","<<(chunk-1)<<","<<chunk<<" "<<numerator<<" " << denominator);

      double coeff;
      if (denominator == 0) {
	// we know that either the x or y co-ordinates are all the
	// same - therefore this is pretty good match for a line
	coeff = 1;
      }
      else {
	coeff = (double)numerator/(double)denominator;
      }

      std::cout << coeff << std::endl;
    }
  }

}
