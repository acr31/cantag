/*
  Copyright (C) 2004 Andrew C. Rice

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: acr31@cam.ac.uk
*/

/**
 * $Header$
 */ 

#ifndef TRANSFORMQUADTANGLEPROJ_GUARD
#define TRANSFORMQUADTANGLEPROJ_GUARD

#include <cantag/QuadTangle.hh>
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/entities/TransformEntity.hh>
#include <cantag/Function.hh>

namespace Cantag {
  class CANTAG_EXPORT TransformQuadTangleProjective : public Function<TL1(ShapeEntity<QuadTangle>),TL1(TransformEntity)> {
  public:
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
     * If the co-ordinates relative to the matrix are (u,v).  Then the
     * co-ordinates in the camera frame of reference are (x,y,z)
     *
     * x = c0 * u + c1 * v + c2
     * y = c3 * u + c4 * v + c5
     * z = c6 * u + c7 * v + c8
     *
     * There is nothing clever about the above - they are just an
     * arbitrary linear combination of u and v
     *
     * Our screen co-ordinates X and Y are a perspective projection of (x,y,z)
     *
     * X = x/z
     * Y = y/z
     *
     * Since both of the equations are over z we can divide through by c8 to give
     *
     *  X = (a0*u + a1*v + a2)/(a6*u+a7*v+1)
     *  Y = (a3*u + a4*v + a5)/(a6*u+a7*v+1)
     *
     * Where a0 = c0/c8, a1 = c1/c8,  etc...
     *
     * Given four points on the matrix and their points in the final
     * image we can set up a set of simultaneous linear equations and
     * solve for a0 to a7
     *
     * ( X0 )   (  u0  v0  1   0   0  0  -X0*u0  -X0*v0 ) ( a0 )
     * ( X1 )   (  u1  v1  1   0   0  0  -X1*u1  -X1*v1 ) ( a1 )
     * ( X2 )   (  u2  v2  1   0   0  0  -X2*u2  -X2*v2 ) ( a2 )
     * ( X3 )   (  u3  v3  1   0   0  0  -X3*u3  -X3*v3 ) ( a3 )
     * ( Y0 ) = (   0   0  0  u0  v0  1  -Y0*u0  -Y0*v0 ) ( a4 )
     * ( Y1 )   (   0   0  0  u1  v1  1  -Y1*u1  -Y1*v1 ) ( a5 )
     * ( Y2 )   (   0   0  0  u2  v2  1  -Y2*u2  -Y2*v2 ) ( a6 )
     * ( Y3 )   (   0   0  0  u3  v3  1  -Y3*u3  -Y3*y3 ) ( a7 )
     *
     * In the above (X_n,Y_n) are pairs of screen co-ordinates that
     * correspond to (u_n,v_n) pairs of co-ordinates on the tag
     *
     * We have found four points - the four corners which we have sorted
     * so that we have
     *
     * (x3,y3) +----------+ (x0,y0)
     *         |          |
     *         |          |
     *         |          |
     *         |          |
     * (x2,y2) +----------+ (x1,y1)
     *
     * Our screen co-ordinate origin is top left so:
     *
     * x0,y0 -> 1,0 (tag co-ordinates)
     * x1,y1 -> 1,1
     * x2,y2 -> 0,1
     * x3,y3 -> 0,0
     *
     * So we can now substitute x_n,y_n (known tag co-ordinates from
     * above) and X_n,Y_n (known screen co-ordinates from quadtangle into
     * the above matrix.  Do this in such a way that the matrix will be
     * easy to invert.
     *
     * x0,y0 -> u1,v1
     * x1,y1 -> u0,v0
     * x2,y2 -> u3,v3
     * x3,y3 -> u2,v2
     *
     * This gives (remembering to swap X and Y's too)
     *
     * ( X1 )   (  x1  y1  1   0   0  0  -X1*x1  -X1*y1 ) ( a0 )
     * ( X0 )   (  x0  y0  1   0   0  0  -X0*x0  -X0*y0 ) ( a1 )
     * ( X3 )   (  x3  y3  1   0   0  0  -X3*x3  -X3*y3 ) ( a2 )
     * ( X2 )   (  x2  y2  1   0   0  0  -X2*x2  -X2*y2 ) ( a3 )
     * ( Y1 ) = (   0   0  0  x1  y1  1  -Y1*x1  -Y1*y1 ) ( a4 )
     * ( Y0 )   (   0   0  0  x0  y0  1  -Y0*x0  -Y0*y0 ) ( a5 )
     * ( Y3 )   (   0   0  0  x3  y3  1  -Y3*x3  -Y3*y3 ) ( a6 )
     * ( Y2 )   (   0   0  0  x2  y2  1  -Y2*x2  -Y2*y2 ) ( a7 )
     *
     * which simplifies (because we know x_n,y_n) to:
     *
     * ( X1 )   (  1  1  1   0   0  0  -X1  -X1 ) ( a0 )
     * ( X0 )   (  1  0  1   0   0  0  -X0    0 ) ( a1 )
     * ( X3 )   (  0  0  1   0   0  0    0    0 ) ( a2 )
     * ( X2 )   (  0  1  1   0   0  0    0  -X2 ) ( a3 )
     * ( Y1 ) = (  0  0  0   1   1  1  -Y1  -Y1 ) ( a4 )
     * ( Y0 )   (  0  0  0   1   0  1  -Y0    0 ) ( a5 )
     * ( Y3 )   (  0  0  0   0   0  1    0    0 ) ( a6 )
     * ( Y2 )   (  0  0  0   0   1  1    0  -Y2 ) ( a7 )
     *
     *
     * To build a homogenous transform matrix from these parameters we need c0..c8
     *  
     * We already know a0..a7 which correspond to c1/c8...c8/c8
     * We know r = the ratio width/height = 1
     * 
     * x = c0 * u + c1 * v + c2
     * y = c3 * u + c4 * v + c5
     * z = c6 * u + c7 * v + c8 
     *
     * So to work out c8 we set u=v=0 this gives:
     *
     * x0 = c2
     * y0 = c5
     * z0 = c8
     *
     * Then set u=0,v=1 this gives:
     *
     * x1 = c1 + c2
     * y1 = c4 + c5
     * z1 = c7 + c8
     *
     * The distance between these two positions is the height of the tag:
     *
     * (x0-x1)^2 + (y0-y1)^2 + (z0-z1)^2 = 1
     *
     *  c1^2     +  c4^2     +  c7^2     = 1
     *
     *  c1^2        c4^2        c7^2          1
     *  ----     +  ----     +  ----     =  ----
     *  c8^2        c8^2        c8^2        c8^2
     *
     *  a1^2     +  a4^2     +  a7^2     =  1/c8^2
     *
     * So:
     *
     * c0 = a0 / sqrt(a1^2+a4^2+a7^2)
     * c1 = a1 / sqrt(a1^2+a4^2+a7^2)
     * etc...
     *
     * and
     * c8 = 1/sqrt(a1^2     +  a4^2     +  a7^2)
     *
     * This then gives the transform matrix as
     *
     * (  x  )    ( c0  c1  0 c2 ) ( u )
     * (  y  ) =  ( c3  c4  0 c5 ) ( v )
     * (  z  )    ( c6  c7  0 c8 ) ( w )
     * (  w  )    (  0   0  0  1 ) ( 1 )
     *
     * The final matrix is created using the cross product
     * of the first two columns to fill in the 3rd
     */
    bool operator()(const ShapeEntity<QuadTangle>& shape, TransformEntity& transform) const;
  };
}

#endif//TRANSFORMQUADTANGLEPROJ_GUARD
