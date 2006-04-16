/*
  Copyright (C) 2005 Andrew C. Rice

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

#include <iostream>

#include <Cantag.hh>

using namespace Cantag;

int main(int argc,char** argv) {
  try {
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> i(atoi(argv[1]),atoi(argv[2]));
    i.Load(argv[3]);
    MonochromeImage m(i.GetWidth(),i.GetHeight());
    ThresholdGlobal<Pix::Sze::Byte1,Pix::Fmt::Grey8>(128)(i,m);
    
    Tree<ComposedEntity<TL1(ContourEntity)> > tree;
    
    ContourRestrictions res(5,5,5);
    ContourFollowerTree follower(res);
    follower(m,tree);
    
    ApplyTree(tree,PrintEntityContour(std::cout));
  }
  catch (const char* e) {
    std::cout << e << std::endl;
  }
}
