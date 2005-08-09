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

#ifndef ALGORITHM_GUARD
#define ALGORITHM_GUARD

#include <cantag/Config.hh>

namespace Cantag {

  template<class Result>
  struct Function0 {
    typedef Function0<Result> FunctionType;
    typedef Result ResultType;
  };
  
  template<class Arg1, class Result>
  struct Function1 {
    typedef Function1<Arg1,Result> FunctionType;
    typedef Arg1 SourceType1;
    typedef Result ResultType;
  };
  
  template<class Arg1, class Arg2, class Result>
  struct Function2 {
    typedef Function2<Arg1,Arg2,Result> FunctionType;
    typedef Arg1 SourceType1;
    typedef Arg2 SourceType2;
    typedef Result ResultType;
  };
  
  template<class Arg1, class Arg2, class Arg3, class Result>
  struct Function3 {
    typedef Function3<Arg1,Arg2,Arg3,Result> FunctionType;
    typedef Arg1 SourceType1;
    typedef Arg2 SourceType2;
    typedef Arg3 SourceType3;
    typedef Result ResultType;
  };

  

}
#endif//ALGORITHM_GUARD
