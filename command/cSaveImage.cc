/*
  Copyright (C) 2006 Andrew C. Rice

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

#include <Cantag.hh>

using namespace Cantag;

int main(int argc,char* argv[]) {
  const char* filename  = argc == 2 ? argv[1] : NULL;
  char fbuf[255];
  try {
    std::cin.exceptions(std::ios_base::eofbit|std::ios_base::badbit|std::ios_base::failbit);
    if (!filename) throw "Please specify a filename (use %d for image number)";
    int counter = 0;
    while(true) {
      ++counter;
      char buffer[50];
      buffer[0] = 0;
      while(!buffer[0]) {
	std::cin.getline(buffer,50);
      }
      if (strncmp(buffer,"P2",2) != 0) {
	throw "Can only load greyscale PNM images (no P2 tag)!";
      }
      unsigned int width;
      std::cin >> width;
      unsigned int height;
      std::cin >> height;
      int colourdepth;
      std::cin >> colourdepth;

      if (colourdepth != 255) {
	throw "Can only load greyscale PNM images (wrong colour depth)!";
      }

      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> image(width,height);
      for(unsigned int i=0;i<height;++i) {
	for(unsigned int j=0;j<width;++j) {
	  int val;
	  std::cin >> val;	  
	  image.DrawPixelNoCheck(j,i,val);
	}
      }

      if (index(filename,'%')) {
	sprintf(fbuf,filename,counter);
	image.Save(fbuf);
      }
      else {
	image.Save(filename);
      }
      
    }
  }
  catch (const char* exception) {
    std::cerr << "Caught exception: " << exception<< std::endl;
  }
  catch (const std::exception& e) {
    std::cerr << "Caught exception: " << e.what() << std::endl;
  }
}

