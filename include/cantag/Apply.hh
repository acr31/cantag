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

#ifndef APPLY_GUARD
#define APPLY_GUARD

#include <vector>

#include <cantag/ComposeEntity.hh>
#include <cantag/EntityTree.hh>

//#define PRINTFUNCTION() std::cout << __PRETTY_FUNCTION__ << std::endl
#define PRINTFUNCTION()
namespace Cantag {

  namespace Internal {
    template<class List, class Algorithm> bool _Apply_ComposedEntity(ComposedEntity<List>& entity, Algorithm& algorithm);

    template<class List, class Algorithm> bool _Apply_ComposedEntityTree(Tree<ComposedEntity<List> >& entity, Algorithm& algorithm);
    template<class Tree, class Arg,class Alg> struct ApplyAlternation {
      static inline bool Choose(Tree& arg,Alg& alg) {
	PRINTFUNCTION();
	return _Apply_ComposedEntity(*(arg.GetNode()),alg);
      }
    };

    template<class Tree, class Arg,class Alg> struct ApplyAlternation<Tree,TreeNode<Arg>,Alg > {
      static inline bool Choose(Tree& arg, Alg& alg) {
	PRINTFUNCTION();
	return _Apply_ComposedEntityTree(arg,alg);
      }
    };

    /**
     * Apply the algorithm to this tree node and then recursively to the
     * children. Returns true if any of the function applications return
     * true
     */
    template<class Tree, class Algorithm> bool _Apply_Tree(Tree& tree, Algorithm& algorithm) {
      PRINTFUNCTION();
      bool result = ApplyAlternation<Tree,typename Nth<typename Algorithm::Results,0>::value,Algorithm>::Choose(tree,algorithm);
      for(typename std::vector<Tree*>::iterator i = tree.GetChildren().begin(); i != tree.GetChildren().end(); ++i) {
	result |= _Apply_Tree<Tree,Algorithm>(*(*i),algorithm);
      }    
      return result;
    };

    /**
     * Apply this 0-ary function to the entity.  0-ary functions are
     * deemed to be in-place mutators and so the result must be valid in
     * order for the algorithm to be run.  The validity of the result is
     * then set according to the result of the algorithm
     */
    template<class Algorithm> bool _Apply_Fn0(typename Nth<typename Algorithm::Results,0>::value& result, Algorithm& algorithm) {
      PRINTFUNCTION();
      bool r = false;
      bool valid = result.IsValid();
      if (valid) {
	r = algorithm(result);
	result.SetValid(r);
      }
      return r;
    }

    /**
     * Apply this 1-ary function to arg1 (if it is valid) and store the
     * result in result (setting its validity as returned by the function
     */
    template<class Algorithm> inline bool _Apply_Fn1(const typename Nth<typename Algorithm::Arguments,0>::value& arg1, typename Nth<typename Algorithm::Results,0>::value& result, Algorithm& algorithm) {
      PRINTFUNCTION();
      bool r = false;
      bool valid = arg1.IsValid();
      if (valid) {
	r = algorithm(arg1,result);
	result.SetValid(r);
      }
      return r;
    }
    
    /**
     * Apply this 2-ary function to the arguments (if both are valid)
     * and store the result in result and set its validity
     */
    template<class Algorithm> bool _Apply_Fn2(const typename Nth<typename Algorithm::Arguments,0>::value& arg1, const typename Nth<typename Algorithm::Arguments,1>::value& arg2, typename Nth<typename Algorithm::Results,0>::value& result, Algorithm& algorithm) {
      PRINTFUNCTION();
      bool r = false;
      bool valid = arg1.IsValid() && arg2.IsValid();
      if (valid) {
	r = algorithm(arg1,arg2,result);
	result.SetValid(r);
      }
      return r;
    }

    template<class Algorithm> bool _Apply_Fn2_0(const typename Nth<typename Algorithm::Arguments,0>::value& arg1, const typename Nth<typename Algorithm::Arguments,1>::value& arg2, Algorithm& algorithm) {
      PRINTFUNCTION();
      bool r = false;
      bool valid = arg1.IsValid() && arg2.IsValid();
      if (valid) {
	r = algorithm(arg1,arg2);
      }
      return r;
    }
    
    /**
     * Apply this 3-ary function to the arguments 
     */
    template<class Algorithm> bool _Apply_Fn3(const typename Nth<typename Algorithm::Arguments,0>::value& arg1, const typename Nth<typename Algorithm::Arguments,1>::value& arg2, const typename Nth<typename Algorithm::Arguments,2>::value& arg3, typename Nth<typename Algorithm::Results,0>::value& result, Algorithm& algorithm) {
      PRINTFUNCTION();
      bool r = false;
      bool valid = arg1.IsValid() && arg2.IsValid() && arg3.IsValid();
      if (valid) {
	r = algorithm(arg1,arg2,arg3,result);
	result.SetValid(r);
      }
      return r;
    }    

    /**
     * Apply this 4-ary function to the arguments 
     */
    template<class Algorithm> bool _Apply_Fn4(const typename Nth<typename Algorithm::Arguments,0>::value& arg1, const typename Nth<typename Algorithm::Arguments,1>::value& arg2, const typename Nth<typename Algorithm::Arguments,2>::value& arg3, const typename Nth<typename Algorithm::Arguments,3>::value& arg4, typename Nth<typename Algorithm::Results,0>::value& result, Algorithm& algorithm) {
      PRINTFUNCTION();
      bool r = false;
      bool valid = arg1.IsValid() && arg2.IsValid() && arg3.IsValid() && arg4.IsValid();
      if (valid) {
	r = algorithm(arg1,arg2,arg3,arg4,result);
	result.SetValid(r);
      }
      return r;
    }    
    

    template<class ComposedEntity> 
    struct ApplyHelperOuter {
      template<class Algorithm, class Fn> class ApplyHelper {};
      
      template<class Algorithm, class ReturnType>
      class ApplyHelper<Algorithm,Function<TL0,TL1(ReturnType)> > {
      public:
	inline bool operator()(Algorithm& algorithm, const ComposedEntity& me, ReturnType& result) const {
	  PRINTFUNCTION();
	  (void) me;
	  return _Apply_Fn0<Algorithm>(result,algorithm);
	}

	inline bool operator()(Algorithm& algorithm, const Tree<ComposedEntity>& me, ReturnType& result) const {
	  PRINTFUNCTION();
	  (void) me;
	  return _Apply_Fn0<Algorithm>(result,algorithm);
	}

      };
      
      template<class Algorithm, class SourceType1, class ReturnType> 
      class ApplyHelper<Algorithm,Function<TL1(SourceType1),TL1(ReturnType)> > {
      public:
	inline bool operator()(Algorithm& algorithm, const ComposedEntity& me, ReturnType& result) const {
	  PRINTFUNCTION();
	  return _Apply_Fn1<Algorithm>(me,result,algorithm);
	}

	inline bool operator()(Algorithm& algorithm, const Tree<ComposedEntity>& me, ReturnType& result) const {
	  PRINTFUNCTION();
	  return _Apply_Fn1<Algorithm>(me,result,algorithm);
	}
      };
      
      template<class Algorithm, class SourceType1, class SourceType2, class ReturnType> 
      class ApplyHelper<Algorithm,Function<TL2(SourceType1,SourceType2),TL1(ReturnType)> > {
      public:
	inline bool operator()(Algorithm& algorithm, const ComposedEntity& me, ReturnType& result) const {
	  PRINTFUNCTION();
	  return _Apply_Fn2<Algorithm>(me,me,result,algorithm);
	}
      };

      template<class Algorithm, class SourceType1, class SourceType2> 
      class ApplyHelper<Algorithm,Function<TL2(SourceType1,SourceType2),TL0> > {
      public:
	inline bool operator()(Algorithm& algorithm, const ComposedEntity& me, SourceType2& result) const {
	  PRINTFUNCTION();
	  return _Apply_Fn2_0<Algorithm>(me,me,algorithm);
	}
      };
      
      template<class Algorithm, class SourceType1, class SourceType2, class SourceType3, class ReturnType> 
      class ApplyHelper<Algorithm,Function<TL3(SourceType1,SourceType2,SourceType3), TL1(ReturnType)> > {
      public:
	inline bool operator()(Algorithm& algorithm, const ComposedEntity& me, ReturnType& result) const {
	  PRINTFUNCTION();
	  return _Apply_Fn3<Algorithm>(me,me,me,result,algorithm);
	}
      };

      template<class Algorithm, class SourceType1, class SourceType2, class SourceType3, class SourceType4, class ReturnType> 
      class ApplyHelper<Algorithm,Function<TL4(SourceType1,SourceType2,SourceType3,SourceType4), TL1(ReturnType)> > {
      public:
	inline bool operator()(Algorithm& algorithm, const ComposedEntity& me, ReturnType& result) const {
	  PRINTFUNCTION();
	  return _Apply_Fn4<Algorithm>(me,me,me,me,result,algorithm);
	}
      };
    };

    /**
     * Use this algorithm to transform the data.  The algorithm will
     * only be run if the pipeline is valid for this item - i.e the
     * highest completed stage of the pipeline has valid data
     *
     * We then call the general apply function which will only call the
     * function if all the source types have valid data
     * 
     * If the algorithm returns true then the output stage data will
     * have been marked as valid and the highest stage index increased
     * to the index of the output stage (if it is not bigger than it
     * already)
     */
    template<class Algorithm, class List, int N> struct Alternate {
      static void exec(ComposedEntity<List>& entity, bool result) {
	if (Position<typename Nth<typename Algorithm::Results,0>::value,List>::value > entity.GetProgress()) {
	  entity.SetProgress(Position<typename Nth<typename Algorithm::Results,0>::value,List>::value);
	}
	entity.SetValid(result);
      }
    };
    template<class Algorithm, class List> struct Alternate<Algorithm, List,0> {
      static void exec(ComposedEntity<List>& entity, bool result);
    };

    template<class List, class Algorithm> bool _Apply_ComposedEntity(ComposedEntity<List>& entity, Algorithm& algorithm) {    
      PRINTFUNCTION();
      if (entity.IsPipelineValid()) {
	typename Internal::ApplyHelperOuter<ComposedEntity<List> >::template ApplyHelper<Algorithm,typename Algorithm::FunctionType> a;
	bool result = a(algorithm,entity,entity);	
	Alternate<Algorithm,List,Length<typename Algorithm::Results>::value>::exec(entity,result);
	return result;
      }
      return false;
    }

    template<class List, class Algorithm> bool _Apply_ComposedEntityTree(Tree<ComposedEntity<List> >& entity, Algorithm& algorithm) {
      PRINTFUNCTION();
      if (entity.IsPipelineValid()) {
	typename Internal::ApplyHelperOuter<ComposedEntity<List> >::template ApplyHelper<Algorithm,typename Algorithm::FunctionType> a;
	if (Position<typename Nth<typename Algorithm::Results,0>::value,List>::value > entity.GetProgress()) {
	  entity.SetProgress(Position<typename Nth<typename Algorithm::Results,0>::value,List>::value);
	}
	bool result = a(algorithm,entity,entity);
	return result;
      }
      return false;
    }
  } // end namespace Internal


  template<class C, class Algorithm> inline bool ApplyTree(Tree<C>& tree, Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Tree<Tree<C>,Algorithm>(tree,algorithm);
  }
  template<class C, class Algorithm> inline bool ApplyTree(const Tree<C>& tree, Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Tree<const Tree<C>,Algorithm>(tree,algorithm);
  }
  template<class C, class Algorithm> inline bool ApplyTree(Tree<C>& tree, const Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Tree<Tree<C>,const Algorithm>(tree,algorithm);
  }
  template<class C, class Algorithm> inline bool ApplyTree(const Tree<C>& tree, const Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Tree<const Tree<C>,const Algorithm>(tree,algorithm);
  }

#if 0 // TOM: never seems to be used
  template<class List, class Algorithm> inline bool Apply(ComposedEntity<List>& entity, Algorithm& algorithm) {    
    PRINTFUNCTION();
    return Internal::_Apply_ComposedEntity(entity,algorithm);
  }
#endif

  
  template<class Algorithm> inline bool Apply(typename Nth<typename Algorithm::Results,0>::value& result, const Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn0<const Algorithm>(result,algorithm);
  }
  template<class Algorithm> inline bool Apply(typename Nth<typename Algorithm::Results,0>::value& result, Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn0<Algorithm>(result,algorithm);
  }

  template<class Algorithm> inline bool Apply(const typename Nth<typename Algorithm::Arguments,0>::value& arg1, 
					      typename Nth<typename Algorithm::Results,0>::value& result, const Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn1<const Algorithm>(arg1,result,algorithm);
  }

  template<class Algorithm> inline bool Apply(TreeNode <typename Nth<typename Algorithm::Arguments,0>::value>& tree, typename Nth<typename Algorithm::Results,0>::value& dest, Algorithm& algorithm) {
    PRINTFUNCTION();
    bool result = Apply(*(tree.GetNode()),dest,algorithm);
    for(typename std::vector<TreeNode <typename Algorithm::FunctionType::SourceType1>*>::iterator i = tree.GetChildren().begin(); i != tree.GetChildren().end(); ++i) {
      result |= Apply<Algorithm>(*(*i),dest,algorithm);
    }    
    return result;
  }

  template<class Algorithm> inline bool Apply(const typename Nth<typename Algorithm::Arguments,0>::value& arg1, 
					      typename Nth<typename Algorithm::Results,0>::value& result, Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn1<Algorithm>(arg1,result,algorithm);    
  }
  
  template<class Algorithm> inline bool Apply(const typename Nth<typename Algorithm::Arguments,0>::value& arg1,
					      const typename Nth<typename Algorithm::Arguments,1>::value& arg2, 
					      typename Nth<typename Algorithm::Results,0>::value& result, Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn2<Algorithm>(arg1,arg2,result,algorithm);
  }

  template<class Algorithm> inline bool Apply(const typename Nth<typename Algorithm::Arguments,0>::value& arg1,
					      const typename Nth<typename Algorithm::Arguments,1>::value& arg2, 
					      typename Nth<typename Algorithm::Results,0>::value& result, const Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn2<const Algorithm>(arg1,arg2,result,algorithm);
  }

  template<class Algorithm> inline bool Apply(const typename Nth<typename Algorithm::Arguments,0>::value& arg1,
					      const typename Nth<typename Algorithm::Arguments,1>::value& arg2,
					      const typename Nth<typename Algorithm::Arguments,2>::value& arg3,
					      typename Nth<typename Algorithm::Results,0>::value& result, Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn3<Algorithm>(arg1,arg2,arg3,result,algorithm);
  }

  template<class Algorithm> inline bool Apply(const typename Nth<typename Algorithm::Arguments,0>::value& arg1,
					      const typename Nth<typename Algorithm::Arguments,1>::value& arg2,
					      const typename Nth<typename Algorithm::Arguments,2>::value& arg3,
					      typename Nth<typename Algorithm::Results,0>::value& result, const Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn3<const Algorithm>(arg1,arg2,arg3,result,algorithm);
  }

  template<class Algorithm> inline bool Apply(const typename Nth<typename Algorithm::Arguments,0>::value& arg1,
					      const typename Nth<typename Algorithm::Arguments,1>::value& arg2,
					      const typename Nth<typename Algorithm::Arguments,2>::value& arg3,
					      const typename Nth<typename Algorithm::Arguments,3>::value& arg4,
					      typename Nth<typename Algorithm::Results,0>::value& result, Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn4<Algorithm>(arg1,arg2,arg3,arg4,result,algorithm);
  }

  template<class Algorithm> inline bool Apply(const typename Nth<typename Algorithm::Arguments,0>::value& arg1,
					      const typename Nth<typename Algorithm::Arguments,1>::value& arg2,
					      const typename Nth<typename Algorithm::Arguments,2>::value& arg3,
					      const typename Nth<typename Algorithm::Arguments,3>::value& arg4,
					      typename Nth<typename Algorithm::Results,0>::value& result, const Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn4<const Algorithm>(arg1,arg2,arg3,arg4,result,algorithm);
  }
};

#endif//APPLY_GUARD
