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

#include <iostream>
#include <map>

#include <Cantag.hh>


#include <gsl/gsl_multimin.h>

// this file includes the definition of the type of tag used in the
// samples.  This is then typedef'd to be of type TagType which is
// used below
#include "TagDef.hh"

using namespace Cantag;


int main(int argc,char* argv[]) {
  
 //  float ff[] = {
//     -1, -0.0001035357127, -7.820783312e-19, -0.007195744663, 
//     -8.815282813e-18, -1.000103593, 0.08871846646, -0.007195744663, 
//     0, 0.08871846646, 1.000103593, 6.165944099, 
//     0, 0, 0, 1 
//   };

//   Transform f(ff,1.0);;
//   f.Print();

//   float a,b,c;

//   f.GetAngleRepresentation(&a,&b,&c);
//   std::cout << a << " " << b << " " << c << std::endl;
//   exit(0);


  float p[]= {
    -0.1843879819, 0.1493776143,
    -0.1517116278, -0.1237033308,
    0.1493776143, -0.1237033308,
    0.1820539683, 0.1493776143
  };
  ShapeEntity<QuadTangle> s;
  s.SetShape(new QuadTangle(
			    p[0], p[1],
			    p[2], p[3],
			    p[4], p[5],
			    p[6], p[7],
			    0,0,0,0));

  std::cout << p[0] << " " << p[1] << std::endl;
    std::cout << p[2] << " " << p[3] << std::endl;
    std::cout << p[4] << " " << p[5] << std::endl;
    std::cout << p[6] << " " << p[7] << std::endl;
    std::cout <<  std::endl;

   TransformEntity t;

  if (!TransformQuadTangleProjective()(s,t)) {
    std::cerr << "Transform failed" << std::endl;
    exit(-1);
  }

  const Transform *tt = t.GetPreferredTransform();
  // tt->Print();

  float X0,Y0;
  tt->Apply(-1.0, -1.0, 0.0, &X0, &Y0);

  float X1,Y1;
  tt->Apply(1.0, -1.0, 0.0, &X1, &Y1);
  
  float X2,Y2;
  tt->Apply(1.0, 1.0, 0.0, &X2, &Y2);
  
  float X3,Y3;
  tt->Apply(-1.0, 1.0, 0.0, &X3, &Y3);

  tt->Print();


  std::cout << X0 << " " << Y0 << std::endl;
  std::cout << X1 << " " << Y1 << std::endl;
  std::cout << X2 << " " << Y2 << std::endl;
  std::cout << X3 << " " << Y3 << std::endl;
  std::cout << std::endl;


  if (!TransformQuadTangleSpaceSearch()(s,t)) {
    std::cerr << "Transform failed" << std::endl;
    exit(-1);
  }

  tt = t.GetPreferredTransform();
  // tt->Print();


  tt->Apply(-1.0, -1.0, 0.0, &X0, &Y0);
  
  tt->Apply(1.0, -1.0, 0.0, &X1, &Y1);
  
  tt->Apply(1.0, 1.0, 0.0, &X2, &Y2);
  
  tt->Apply(-1.0, 1.0, 0.0, &X3, &Y3);
  
  std::cout << std::endl;
  std::cout << X0 << " " << Y0 << std::endl;
  std::cout << X1 << " " << Y1 << std::endl;
  std::cout << X2 << " " << Y2 << std::endl;
  std::cout << X3 << " " << Y3 << std::endl;

  
  exit(0);


}
