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

#ifndef ROI_GUARD
#define ROI_GUARD

#include <cantag/SpeedMath.hh>

namespace Cantag {

  struct CANTAG_EXPORT ROI {
    float minx;
    float maxx;
    float miny;
    float maxy;

    ROI(float pminx, float pmaxx, float pminy, float pmaxy) : minx(pminx), maxx(pmaxx), miny(pminy), maxy(pmaxy) {};
    int ScaleX(int x, int imageWidth) const { return (x - Round(minx)) * imageWidth / Round(maxx-minx);}
    int ScaleX(float x, int imageWidth) const { return Round((x - (float)minx) * (float)imageWidth / (float)(maxx-minx)); }
    int ScaleY(int y, int imageHeight) const { return (y - Round(miny)) * imageHeight / Round(maxy-miny);}
    int ScaleY(float y, int imageHeight) const { return Round((y - (float)miny) * (float)imageHeight / (float)(maxy-miny)); }
  };

}

#endif
