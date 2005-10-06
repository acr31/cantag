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

#ifndef TEMPLATE_UTILS_GUARD
#define TEMPLATE_UTILS_GUARD

#include <cantag/Config.hh>
#include <cantag/EntityTree.hh>

namespace Cantag {

  /**
   * A list of types
   */
  class TypeListEOL {
    typedef TypeListEOL Head;
  };
  template<class H, class T = TypeListEOL> struct TypeList {
    typedef H Head;
    typedef T Tail;
  };

#define TL0 Cantag::TypeListEOL
#define TL1(x1) Cantag::TypeList<x1,Cantag::TypeListEOL>
#define TL2(x1,x2) Cantag::TypeList<x1,Cantag::TypeList<x2,Cantag::TypeListEOL> >
#define TL3(x1,x2,x3) Cantag::TypeList<x1,Cantag::TypeList<x2,Cantag::TypeList<x3,Cantag::TypeListEOL> > >
#define TL4(x1,x2,x3,x4) Cantag::TypeList<x1,Cantag::TypeList<x2,Cantag::TypeList<x3,Cantag::TypeList<x4, Cantag::TypeListEOL> > > >
#define TL5(x1,x2,x3,x4,x5) Cantag::TypeList<x1,Cantag::TypeList<x2,Cantag::TypeList<x3,Cantag::TypeList<x4, Cantag::TypeList<x5, Cantag::TypeListEOL> > > > >
#define TL6(x1,x2,x3,x4,x5,x6) Cantag::TypeList<x1,Cantag::TypeList<x2,Cantag::TypeList<x3,Cantag::TypeList<x4, Cantag::TypeList<x5, Cantag::TypeList<x6,Cantag::TypeListEOL> > > > > >

  /**
   * Select the last element from a list of entities
   */
  template<class List>
  struct Last {
    typedef typename Last<typename List::Tail>::Selected Selected;
  };
  template<class Item>
  struct Last<TypeList<Item,TypeListEOL> > {
    typedef Item Selected;
  };
  
  /**
   * Select a range from the list of entities from start to stop inclusive
   */
  template<class List, class Start, class Stop>
  struct SelectPipeline {
    typedef typename SelectPipeline<typename List::Tail,Start,Stop>::Selected Selected;
  };
  template<class List, class Stop>
  struct SelectPipeline<List,typename List::Head,Stop> {
    typedef TypeList<typename List::Head,typename SelectPipeline<typename List::Tail,typename List::Tail::Head,Stop>::Selected > Selected;
  };
  template<class List>
  struct SelectPipeline<List,typename List::Head, typename List::Head> {
    typedef TypeList<typename List::Head,TypeListEOL> Selected;
  };


  /**
   * Return the position of class A in the type list
   */ 
  template<class A, class List>
  struct Position {
    enum { value = Position<A,typename List::Tail>::value + 1 };
  };
  
  template<class A, class Tail>
  struct Position<A,TypeList<A,Tail> > {
    enum { value = 1 };
  };

  template<class A, class Tail>
  struct Position<TreeNode<A>,TypeList<A,Tail> > {
    enum { value = 1 };
  };


  /**
   * Return the length of the list
   */
  template<class List>
  struct Length {
    enum { value = Length<typename List::Tail>::value+1 };
  };

  template<>
  struct Length<Cantag::TypeListEOL> {
    enum { value = 0 };
  };

  /**
   * Return the nth item in the list.  The first item in the list has index 0
   */
  template<class List,int index>
  struct Nth {
    typedef typename Nth<typename List::Tail,index-1>::value value;
  };

  template<class List>
  struct Nth<List,0> {
    typedef typename List::Head value;
  };


  template<class Element, class List, class WhenTrue, class WhenFalse>
  struct IfContains {
    typedef typename IfContains<Element,typename List::Tail,WhenTrue,WhenFalse>::value value;
  };
  
  template<class Element,class WhenTrue, class WhenFalse>
  struct IfContains<Element,TypeListEOL,WhenTrue,WhenFalse> {
    typedef WhenFalse value;
  };

  template<class Element, class Tail, class WhenTrue, class WhenFalse>
  struct IfContains<Element,TypeList<Element,Tail>,WhenTrue,WhenFalse > {
    typedef WhenTrue value;
  };

  template<class List, class OrderList>
  struct Reorder {
    typedef typename IfContains<typename OrderList::Head,List,TypeList<typename OrderList::Head, typename Reorder<List,typename OrderList::Tail>::value>, typename Reorder<List,typename OrderList::Tail>::value>::value value;
  };
  
  template<class List>
  struct Reorder<List,TypeListEOL> {
    typedef TypeListEOL value;
  };
}

#endif//TEMPLATE_UTILS_GUARD
