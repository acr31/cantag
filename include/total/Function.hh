/**
 * $Header$
 */

#ifndef ALGORITHM_GUARD
#define ALGORITHM_GUARD

namespace Total {
  
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
