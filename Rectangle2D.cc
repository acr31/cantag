/**
 * $Header$
 *
 * $Log$
 * Revision 1.2  2004/01/23 18:18:11  acr31
 * added Matrix Tag and a test program and the beginning of the CRC coder
 *
 * Revision 1.1  2004/01/23 15:42:29  acr31
 * Initial commit of rectangle detection code
 *
 *
 */
#include "Rectangle2D.hh"

#undef FILENAME
#define FILENAME "Rectangle2D.cc"

Rectangle2D::Rectangle2D(float x0, float y0,float x1, float y1,float x2, float y2,float x3, float y3) :
  m_x0(x0),
  m_y0(y0),
  m_x1(x1),
  m_y1(y1),
  m_x2(x2),
  m_y2(y2),
  m_x3(x3),
  m_y3(y3) {

  compute_central_point();
  compute_alpha();

};

Rectangle2D::Rectangle2D(float* coords) :
  m_x0(coords[0]),
  m_y0(coords[1]),
  m_x1(coords[2]),
  m_y1(coords[3]),
  m_x2(coords[4]),
  m_y2(coords[5]),
  m_x3(coords[6]),
  m_y3(coords[7]) {
  compute_central_point();
  compute_alpha();
};

void Rectangle2D::ProjectPoint(float x, float y, float *projX, float *projY) const {
  *projX = (m_alpha[0]*x+m_alpha[1]*y+m_alpha[2])/(m_alpha[6]*x+m_alpha[7]*y+1);
  *projY = (m_alpha[3]*x+m_alpha[4]*y+m_alpha[5])/(m_alpha[6]*x+m_alpha[7]*y+1);
  PROGRESS("Projecting point ("<<x <<","<<y<<") on to ("<< *projX<<","<<*projY<<")");
}

void Rectangle2D::compute_alpha() {
  /*
   * Taken from:
   *
   * @InProceedings{ip:apchi:rekimoto98,
   *  author        = "Jun Rekimoto",
   *  title         = "Matrix: A Realtime Object Identification and Registration Method for Augmented Reality",
   *  booktitle     = "Proceedings of Asia Pacific Computer Human Interaction",
   *  year          = "1998",
   * }
   *
   *   
   * points x,y in the matrix code have
   * corresponding points in the image X,Y given by:
   *
   *  X = (a1*x + a2*y + a3)/(a7*x+a8*y+1)
   *  Y = (a4*x + a5*y + a6)/(a7*x+a8*y+1)
   *
   * We have found found four points - the four corners so we can
   * solve this (using this matlab script
   *
   * syms X1 X2 X3 X4 Y1 Y2 Y3 Y4
   * syms x1 x2 x3 x4 y1 y2 y3 y4
   * syms a1 a2 a3 a4 a5 a6 a7 a8
   *
   * A = [ X1; X2; X3; X4; Y1; Y2; Y3; Y4]
   * B = [ x1 y1 1 0 0 0 -X1*x1 -X1*y1;
   *       x2 y2 1 0 0 0 -X2*x2 -X2*y2;
   *       x3 y3 1 0 0 0 -X3*x3 -X3*y3;
   *       x4 y4 1 0 0 0 -X4*x4 -X4*y4;
   *       0 0 0 x1 y1 1 -Y1*x1 -Y1*y1;
   *       0 0 0 x2 y2 1 -Y2*x2 -Y2*y2;
   *       0 0 0 x3 y3 1 -Y3*x3 -Y3*y3;
   *       0 0 0 x4 y4 1 -Y4*x4 -Y3*y4 ]
   *
   * C = [ a1; a2; a3; a4; a5; a6; a7; a8]
   *
   * x1 =0
   * y1 =0
   * x2 = 1
   * y2 = 0
   * x3 = 1
   * y3 = 1
   * x4 = 0
   * y4 = 1
   *
   * D = inv(eval(B))*A
   *
   * This gives us the result:
   *
   * D = [ -X1+X2+X2/(-Y3+Y2)*Y1-X2/(-Y3+Y2)*Y2+X2/(-Y3+Y2)*Y3-X2/(-Y3+Y2)*Y4 ;
   *       -X3/(-X4+X3)*X1+X4/(-X4+X3)*X2+X4*(-X3+X2)/(Y3*X4-Y3*X3-Y2*X4+Y2*X3)*Y1-X4*(-X3+X2)/(Y3*X4-Y3*X3-Y2*X4+Y2*X3)*Y2+X4*(-X3+X2)/(Y3*X4-Y3*X3-Y2*X4+Y2*X3)*Y3-X4*(-X3+X2)/(Y3*X4-Y3*X3-Y2*X4+Y2*X3)*Y4 ;
   *        X1 ;
   *        Y3/(-Y3+Y2)*Y1-Y2/(-Y3+Y2)*Y4 ;
   *        -Y3/(-X4+X3)*X1+Y3/(-X4+X3)*X2-Y3/(-X4+X3)*X3+Y3/(-X4+X3)*X4+(Y3*X2-Y3*X4+Y2*X4-Y2*X3)/(Y3*X4-Y3*X3-Y2*X4+Y2*X3)*Y1-(-X3+X2)*Y3/(Y3*X4-Y3*X3-Y2*X4+Y2*X3)*Y2+(-X3+X2)*Y3^2/(Y3*X4-Y3*X3-Y2*X4+Y2*X3)-(Y3*X2-Y3*X4+Y2*X4-Y2*X3)/(Y3*X4-Y3*X3-Y2*X4+Y2*X3)*Y4 ;
   *        Y1 ;
   *        1/(-Y3+Y2)*Y1-Y2/(-Y3+Y2)+Y3/(-Y3+Y2)-1/(-Y3+Y2)*Y4 ;
   *        -1/(-X4+X3)*X1+1/(-X4+X3)*X2-X3/(-X4+X3)+X4/(-X4+X3)+(-X3+X2)/(Y3*X4-Y3*X3-Y2*X4+Y2*X3)*Y1-(-X3+X2)/(Y3*X4-Y3*X3-Y2*X4+Y2*X3)*Y2+(-X3+X2)*Y3/(Y3*X4-Y3*X3-Y2*X4+Y2*X3)-(-X3+X2)/(Y3*X4-Y3*X3-Y2*X4+Y2*X3)*Y4 ]
   */
  
  m_alpha[0] = -m_x0+m_x1+m_x1/(-m_y2+m_y1)*m_y0-m_x1/(-m_y2+m_y1)*m_y1+m_x1/(-m_y2+m_y1)*m_y2-m_x1/(-m_y2+m_y1)*m_y3 ;
  m_alpha[1] = -m_x2/(-m_x3+m_x2)*m_x0+m_x3/(-m_x3+m_x2)*m_x1+m_x3*(-m_x2+m_x1)/(m_y2*m_x3-m_y2*m_x2-m_y1*m_x3+m_y1*m_x2)*m_y0-m_x3*(-m_x2+m_x1)/(m_y2*m_x3-m_y2*m_x2-m_y1*m_x3+m_y1*m_x2)*m_y1+m_x3*(-m_x2+m_x1)/(m_y2*m_x3-m_y2*m_x2-m_y1*m_x3+m_y1*m_x2)*m_y2-m_x3*(-m_x2+m_x1)/(m_y2*m_x3-m_y2*m_x2-m_y1*m_x3+m_y1*m_x2)*m_y3 ;
  m_alpha[2] = m_x0;
  m_alpha[3] = m_y2/(-m_y2+m_y1)*m_y0-m_y1/(-m_y2+m_y1)*m_y3;
  m_alpha[4] = -m_y2/(-m_x3+m_x2)*m_x0+m_y2/(-m_x3+m_x2)*m_x1-m_y2/(-m_x3+m_x2)*m_x2+m_y2/(-m_x3+m_x2)*m_x3+(m_y2*m_x1-m_y2*m_x3+m_y1*m_x3-m_y1*m_x2)/(m_y2*m_x3-m_y2*m_x2-m_y1*m_x3+m_y1*m_x2)*m_y0-(-m_x2+m_x1)*m_y2/(m_y2*m_x3-m_y2*m_x2-m_y1*m_x3+m_y1*m_x2)*m_y1+(-m_x2+m_x1)*m_y2*m_y2/(m_y2*m_x3-m_y2*m_x2-m_y1*m_x3+m_y1*m_x2)-(m_y2*m_x1-m_y2*m_x3+m_y1*m_x3-m_y1*m_x2)/(m_y2*m_x3-m_y2*m_x2-m_y1*m_x3+m_y1*m_x2)*m_y3 ;
  m_alpha[5] = m_y0;
  m_alpha[6] = 1/(-m_y2+m_y1)*m_y0-m_y1/(-m_y2+m_y1)+m_y2/(-m_y2+m_y1)-1/(-m_y2+m_y1)*m_y3 ;
  m_alpha[7] = -1/(-m_x3+m_x2)*m_x0+1/(-m_x3+m_x2)*m_x1-m_x2/(-m_x3+m_x2)+m_x3/(-m_x3+m_x2)+(-m_x2+m_x1)/(m_y2*m_x3-m_y2*m_x2-m_y1*m_x3+m_y1*m_x2)*m_y0-(-m_x2+m_x1)/(m_y2*m_x3-m_y2*m_x2-m_y1*m_x3+m_y1*m_x2)*m_y1+(-m_x2+m_x1)*m_y2/(m_y2*m_x3-m_y2*m_x2-m_y1*m_x3+m_y1*m_x2)-(-m_x2+m_x1)/(m_y2*m_x3-m_y2*m_x2-m_y1*m_x3+m_y1*m_x2)*m_y3; 
  
  PROGRESS("Computed alpha[0] "<<m_alpha[0]);
  PROGRESS("         alpha[1] "<<m_alpha[1]);
  PROGRESS("         alpha[2] "<<m_alpha[2]);
  PROGRESS("         alpha[3] "<<m_alpha[3]);
  PROGRESS("         alpha[4] "<<m_alpha[4]);
  PROGRESS("         alpha[5] "<<m_alpha[5]);
  PROGRESS("         alpha[6] "<<m_alpha[6]);
  PROGRESS("         alpha[7] "<<m_alpha[7]);

}


inline void Rectangle2D::compute_central_point() {
  /*
    Find the central point of this rectangle (A,B,C,D)

     A + AC*a = B + BD*b

     Ax + ACx * a = Bx + BDx * b   (1)
     Ay + ACy * a = By + BDy * b   (2)

                a = ( Bx + BDx * b - Ax ) / ACx   (from 1)
                
     Ay + ACy * (Bx + BDx * b - Ax) / ACx = By + BDy * b  (subst in 2)

     b = ( Ay * ACx + ACy * Bx + ACy * Ax - ACx * By ) / ( ACx * (BDy + BDx) )

     Central point  = B+BD*b

  */
  float Ax = m_x0;
  float Ay = m_y0;

  float Bx = m_x1;
  float By = m_y1;

  float Cx = m_x2;
  float Cy = m_y2;

  float Dx = m_x3;
  float Dy = m_y3;

  float ACx = Cx-Ax;
  float ACy = Cy-Ay;

  float BDx = Dx-Bx;
  float BDy = Dy-By;

  float b = ( Ay * ACx + ACy * Bx + ACy * Ax - ACx * By ) / ( ACx * (BDy + BDx) );
  
  m_xc = Bx+BDx*b;
  m_yc = By+BDy*b;        
}

  
Rectangle2DChain::Rectangle2DChain(Rectangle2D *cur) : current(cur), nextchain(NULL) {};

Rectangle2DChain::~Rectangle2DChain() {
  delete(current);
  if (nextchain!=NULL) { delete(nextchain); }
}

std::ostream& operator<<(std::ostream& s, const Rectangle2D& z) { 
  s << "Position (" << z.m_x0 << "," << z.m_y0 << ") " << "(" << z.m_x1 << "," << z.m_y1 << ") " << "(" << z.m_x2 << "," <<z.m_y2 <<") " << "(" << z.m_x3 << "," << z.m_y3 << ")"; 
  return s;
}