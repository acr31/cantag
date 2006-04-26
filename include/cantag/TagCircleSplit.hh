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

#ifndef TAG_CIRCLE_SPLIT_GUARD
#define TAG_CIRCLE_SPLIT_GUARD


#include <cantag/Config.hh>
#include <cantag/TagCircle.hh>
#include <cantag/Ellipse.hh>

namespace Cantag {

  template<int PARAM_RING_COUNT,int PARAM_SECTOR_COUNT,int PARAM_READ_COUNT=50>
  class TagCircleSplit : public TagCircle<PARAM_RING_COUNT,PARAM_SECTOR_COUNT,PARAM_READ_COUNT> {
  public:
    enum { RING_COUNT = PARAM_RING_COUNT };
    enum { SECTOR_COUNT = PARAM_SECTOR_COUNT };
    enum { READ_COUNT = PARAM_READ_COUNT };
    typedef Ellipse Shape;

    TagCircleSplit();
  };

  template<int PARAM_RING_COUNT,int PARAM_SECTOR_COUNT,int PARAM_READ_COUNT> TagCircleSplit<PARAM_RING_COUNT,PARAM_SECTOR_COUNT,PARAM_READ_COUNT>::TagCircleSplit() :
    TagCircle<PARAM_RING_COUNT,PARAM_SECTOR_COUNT,PARAM_READ_COUNT>
  (TagCircle<PARAM_RING_COUNT,PARAM_SECTOR_COUNT,PARAM_READ_COUNT>::ComputeDelta() * (1.f - (1.f / ( TagCircle<PARAM_RING_COUNT,PARAM_SECTOR_COUNT,PARAM_READ_COUNT>::ComputeAlpha() + 1.f))) - 1.f / ( TagCircle<PARAM_RING_COUNT,PARAM_SECTOR_COUNT,PARAM_READ_COUNT>::ComputeAlpha() + 1.f),
   1.f,
   TagCircle<PARAM_RING_COUNT,PARAM_SECTOR_COUNT,PARAM_READ_COUNT>::ComputeDelta() * (1.f - (1.f / ( TagCircle<PARAM_RING_COUNT,PARAM_SECTOR_COUNT,PARAM_READ_COUNT>::ComputeAlpha() + 1.f))),
   1.f - (1.f / ( TagCircle<PARAM_RING_COUNT,PARAM_SECTOR_COUNT,PARAM_READ_COUNT>::ComputeAlpha() + 1.f)))
  {
    if (TagCircle<PARAM_RING_COUNT,PARAM_SECTOR_COUNT,PARAM_READ_COUNT>::GetBullseyeInnerEdge() < 0.5f / ( TagCircle<PARAM_RING_COUNT,PARAM_SECTOR_COUNT,PARAM_READ_COUNT>::ComputeAlpha() + 1.f)) {
      throw "Cannot generate an optimal CircleSplit tag design for these parameters";
    }
  }
}
#endif//TAG_CIRCLE_SPLIT_GUARD
