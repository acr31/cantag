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
 $Header$
 */

#ifndef TAGDEF_GUARD
#define TAGDEF_GUARD

#include <vector>

#include <Cantag.hh>

#include "TestCircle.hh"
#include "TestSquare.hh"

#define L(HEAD,TAIL) Cantag::TypeList<HEAD,TAIL >

typedef L(CircleInnerLSFull24,
        L(CircleInnerLSFull48,
	L(CircleInnerLSFull64,
	L(CircleInnerLSFull80,
	L(CircleInnerLSFull100,
	L(CircleInnerLSFull120,
	L(CircleInnerLSFull144,
	L(CircleInnerLSFull168,
	L(CircleInnerLSFull196,
        L(CircleOuterLSFull36,
        L(CircleOuterLSLinear36,
        L(CircleOuterSimpleFull36,
        L(CircleOuterSimpleLinear36,
        L(CircleInnerLSFull36,
	L(CircleInnerLSLinear36,
	L(CircleInnerSimpleFull36,
	L(CircleInnerSimpleLinear36,
	L(CircleSplitLSFull36,
	L(CircleSplitLSLinear36,
	L(CircleSplitSimpleFull36,
	L(CircleSplitSimpleLinear36,
	L(SquareCornerProj36,
	L(SquareCornerReduced36,
	L(SquareCornerSpaceSearch36,
        L(SquarePolygonProj36,
	L(SquarePolygonReduced36,
	L(SquarePolygonSpaceSearch36,
        L(SquareConvexHullProj36,
	L(SquareConvexHullReduced36,
	L(SquareConvexHullSpaceSearch36,
        L(SquarePolygonProj25,
        L(SquarePolygonProj49,
        L(SquarePolygonProj64,
        L(SquarePolygonProj81,
        L(SquarePolygonProj100,
        L(SquarePolygonProj121,
        L(SquarePolygonProj169,
        L(SquarePolygonProj196,	  
	  Cantag::TypeListEOL)))))))))))))))))))))))))))))))))))))) AllTags;
/*
typedef L(CircleInnerLSFull24,
        L(CircleInnerLSFull48,
	L(CircleInnerLSFull64,
	L(CircleInnerLSFull80,
	L(CircleInnerLSFull100,
	L(CircleInnerLSFull120,
	L(CircleInnerLSFull144,
	L(CircleInnerLSFull168,
	  L(CircleInnerLSFull196,Cantag::TypeListEOL))))))))) AllTags;
*/

typedef	L(SquareCornerProj36,
	L(SquareCornerReduced36,
	L(SquareCornerSpaceSearch36,
	L(SquareRegressCornerProj36,
	L(SquareRegressCornerReduced36,
	L(SquareRegressCornerSpaceSearch36,
        L(SquarePolygonProj36,
	L(SquarePolygonReduced36,
	L(SquarePolygonSpaceSearch36,
        L(SquareRegressPolygonProj36,
	L(SquareRegressPolygonReduced36,
	L(SquareRegressPolygonSpaceSearch36,
        L(SquareConvexHullProj36,
	L(SquareConvexHullReduced36,
	L(SquareConvexHullSpaceSearch36,
        L(SquareRegressConvexHullProj36,
	L(SquareRegressConvexHullReduced36,
	L(SquareRegressConvexHullSpaceSearch36,
        L(SquarePolygonProj25,
        L(SquarePolygonProj49,
        L(SquarePolygonProj64,
        L(SquarePolygonProj81,
        L(SquarePolygonProj100,
        L(SquarePolygonProj121,
        L(SquarePolygonProj169,
	L(SquarePolygonProj196,
	    Cantag::TypeListEOL)))))))))))))))))))))))))) SquareTags;

typedef	L(SquareCornerProj36,
	L(SquareCornerReduced36,
        L(SquarePolygonProj36,
	L(SquarePolygonReduced36,
        L(SquareConvexHullProj36,
	L(SquareConvexHullReduced36,
        L(SquarePolygonProj25,
        L(SquarePolygonProj49,
        L(SquarePolygonProj64,
        L(SquarePolygonProj81,
        L(SquarePolygonProj100,
        L(SquarePolygonProj121,
        L(SquarePolygonProj169,
	L(SquarePolygonProj196,
	    Cantag::TypeListEOL)))))))))))))) SquareTagsNotSpace;

//typedef L(SquareConvexHullRegressProj49,Cantag::TypeListEOL) AllTags;
#endif//TAGDEF_GUARD
