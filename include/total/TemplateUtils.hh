/**
 * $Header$
 */

#ifndef TEMPLATE_UTILS_GUARD
#define TEMPLATE_UTILS_GUARD

namespace Total {

  /**
   * A list of types
   */
  template<class H, class T> struct TypeList {
    typedef H Head;
    typedef T Tail;
  };
  class TypeListEOL {};
  
#define TL1(x1) TypeList<x1,TypeListEOL>
#define TL2(x1,x2) TypeList<x1,TypeList<x2,TypeListEOL> >
#define TL3(x1,x2,x3) TypeList<x1,TypeList<x2,TypeList<x3,TypeListEOL> > >
#define TL4(x1,x2,x3,x4) TypeList<x1,TypeList<x2,TypeList<x3,TypeList<x4, TypeListEOL> > > >
#define TL5(x1,x2,x3,x4,x5) TypeList<x1,TypeList<x2,TypeList<x3,TypeList<x4, TypeList<x5, TypeListEOL> > > > >
  
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


  /**
   * Return the length of the list
   */
  template<class List>
  struct Length {
    enum { value = Length<typename List::Tail>::value+1 };
  };

  template<>
  struct Length<TypeListEOL> {
    enum { value = 1 };
  };

}

#endif//TEMPLATE_UTILS_GUARD