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
      bool result = ApplyAlternation<Tree,typename Algorithm::ResultType,Algorithm>::Choose(tree,algorithm);
      for(typename std::vector<Tree*>::iterator i = tree.GetChildren().begin(); i != tree.GetChildren().end(); ++i) {
	result |= _Apply_Tree<Tree,Algorithm>(*(*i),algorithm);
      }    
      return result;
    };

    /**
     * Apply the algorithm to source storing the result in dest_current.
     * If it succeeds then create a new child of current and recurse
     * with current as the parent node on the first child.  If this
     * recursion returns true then create another child and repeat on
     * the second child.  If it fails, then pass the first new child
     * once more.
     * 
     * If it fails then recurse using dest_current as the child and
     * creating new nodes in dest_parent.
     *
     * Return true if we successfully applied or if one of our children
     * did
     */
    template<class Algorithm, class TreeType> bool _Apply_Tree_Tree(const Tree<TreeType>& source, 
								    TreeNode<typename Algorithm::FunctionType::ResultType>& dest_parent, 
								    TreeNode<typename Algorithm::FunctionType::ResultType>& dest_current, 
								    const Algorithm& algorithm) {
      PRINTFUNCTION();
      bool result = Apply(*(source.GetNode()), *(dest_current.GetNode()),algorithm);
      TreeNode<typename Algorithm::FunctionType::ResultType>* parent;
      TreeNode<typename Algorithm::FunctionType::ResultType>* child;
      if (result) {
	parent = &dest_current;
	child = parent->AddChild();
      }
      else {
	parent = &dest_parent;
	child = &dest_current;
      }
      bool child_result = false;
      for(typename std::vector<Tree<TreeType>*>::const_iterator i = source.GetChildren().begin(); 
	  i != source.GetChildren().end(); 
	  ++i) {      
	if (child_result) child = parent->AddChild();
	child_result = _Apply_Tree_Tree(*(*i),*parent,*child,algorithm);
	result |= child_result;
      }
      return result;
    }

    
    /**
     * Apply this 0-ary function to the entity.  0-ary functions are
     * deemed to be in-place mutators and so the result must be valid in
     * order for the algorithm to be run.  The validity of the result is
     * then set according to the result of the algorithm
     */
    template<class Algorithm> bool _Apply_Fn0(typename Algorithm::FunctionType::ResultType& result, Algorithm& algorithm) {
      PRINTFUNCTION();
      bool r = false;
      if (result.IsValid()) {
	r = algorithm(result);
	result.SetValid(r);
      }
      return r;
    }

    /**
     * Apply this 1-ary function to arg1 (if it is valid) and store the
     * result in result (setting its validity as returned by the function
     */
    template<class Algorithm> inline bool _Apply_Fn1(const typename Algorithm::FunctionType::SourceType1& arg1, typename Algorithm::FunctionType::ResultType& result, Algorithm& algorithm) {
      PRINTFUNCTION();
      bool r = false;
      if (arg1.IsValid()) {
	r = algorithm(arg1,result);
	result.SetValid(r);
      }
      return r;
    }
    
    /**
     * Apply this 2-ary function to the arguments (if both are valid)
     * and store the result in result and set its validity
     */
    template<class Algorithm> bool _Apply_Fn2(const typename Algorithm::FunctionType::SourceType1& arg1, const typename Algorithm::FunctionType::SourceType2& arg2, typename Algorithm::FunctionType::ResultType& result, const Algorithm& algorithm) {
      PRINTFUNCTION();
      bool r = false;
      if (arg1.IsValid() && arg2.IsValid()) {
	r = algorithm(arg1,arg2,result);
	result.SetValid(r);
      }
      return r;
    }
    
    /**
     * Apply this 3-ary function to the arguments 
     */
    template<class Algorithm> bool _Apply_Fn3(const typename Algorithm::FunctionType::SourceType1& arg1, const typename Algorithm::FunctionType::SourceType2& arg2, const typename Algorithm::FunctionType::SourceType3& arg3, typename Algorithm::FunctionType::ResultType& result, Algorithm& algorithm) {
      PRINTFUNCTION();
      bool r = false;
      if (arg1.IsValid() && arg2.IsValid() && arg3.IsValid()) {
	r = algorithm(arg1,arg2,arg3,result);
	result.SetValid(r);
      }
      return r;
    }    
    

    template<class ComposedEntity> 
    struct ApplyHelperOuter {
      template<class Algorithm, class Fn> class ApplyHelper {};
      
      template<class Algorithm, class ReturnType>
      class ApplyHelper<Algorithm,Function0<ReturnType> > {
      public:
	inline bool operator()(Algorithm& algorithm, const ComposedEntity& me, ReturnType& result) const {
	  PRINTFUNCTION();
	  return _Apply_Fn0<Algorithm>(result,algorithm);
	}

	inline bool operator()(Algorithm& algorithm, const Tree<ComposedEntity>& me, ReturnType& result) const {
	  PRINTFUNCTION();
	  return _Apply_Fn0<Algorithm>(result,algorithm);
	}
      };
      
      template<class Algorithm, class SourceType1, class ReturnType> 
      class ApplyHelper<Algorithm,Function1<SourceType1,ReturnType> > {
      public:
	inline bool operator()(Algorithm& algorithm, const ComposedEntity& me, ReturnType& result) const {
	  PRINTFUNCTION();
	  return _Apply_Fn1<Algorithm>(me,result,algorithm);
	}
      };
      
      template<class Algorithm, class SourceType1, class SourceType2, class ReturnType> 
      class ApplyHelper<Algorithm,Function2<SourceType1,SourceType2,ReturnType> > {
      public:
	inline bool operator()(Algorithm& algorithm, const ComposedEntity& me, ReturnType& result) const {
	  PRINTFUNCTION();
	  return _Apply_Fn2<Algorithm>(me,me,result,algorithm);
	}
      };
      
    template<class Algorithm, class SourceType1, class SourceType2, class SourceType3, class ReturnType> 
      class ApplyHelper<Algorithm,Function3<SourceType1,SourceType2,SourceType3, ReturnType> > {
      public:
	inline bool operator()(Algorithm& algorithm, const ComposedEntity& me, ReturnType& result) const {
	  PRINTFUNCTION();
	  return _Apply_Fn3<Algorithm>(me,me,me,result,algorithm);
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
    template<class List, class Algorithm> bool _Apply_ComposedEntity(ComposedEntity<List>& entity, Algorithm& algorithm) {    
      PRINTFUNCTION();
      if (entity.IsPipelineValid()) {
	typename Internal::ApplyHelperOuter<ComposedEntity<List> >::template ApplyHelper<Algorithm,typename Algorithm::FunctionType> a;
	if (Position<typename Algorithm::ResultType,List>::value > entity.GetProgress()) {
	  entity.SetProgress(Position<typename Algorithm::ResultType,List>::value);
	}
	bool result = a(algorithm,entity,entity);
	entity.SetValid(result);
	return result;
      }
      return false;
    }

    template<class List, class Algorithm> bool _Apply_ComposedEntityTree(Tree<ComposedEntity<List> >& entity, Algorithm& algorithm) {
      PRINTFUNCTION();
      if (entity.IsPipelineValid()) {
	typename Internal::ApplyHelperOuter<ComposedEntity<List> >::template ApplyHelper<Algorithm,typename Algorithm::FunctionType> a;
	if (Position<typename Algorithm::ResultType,List>::value > entity.GetProgress()) {
	  entity.SetProgress(Position<typename Algorithm::ResultType,List>::value);
	}
	bool result = a(algorithm,entity,entity);
	return result;
      }
      return false;
    }
  }


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

  template<class Algorithm> inline bool ApplyTree(const Tree<typename Algorithm::FunctionType::SourceType1>& source, 
					   TreeNode<typename Algorithm::FunctionType::ResultType>& dest, 
					   const Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Tree_Tree(source, dest,dest,algorithm);
  }

  template<class List, class Algorithm> inline bool Apply(ComposedEntity<List>& entity, Algorithm& algorithm) {    
    PRINTFUNCTION();
    return Internal::_Apply_ComposedEntity(entity,algorithm);
  }

  
  template<class Algorithm> inline bool Apply(typename Algorithm::FunctionType::ResultType& result, const Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn0<const Algorithm>(result,algorithm);
  }
  template<class Algorithm> inline bool Apply(typename Algorithm::FunctionType::ResultType& result, Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn0<Algorithm>(result,algorithm);
  }

  template<class Algorithm> inline bool Apply(const typename Algorithm::FunctionType::SourceType1& arg1, typename Algorithm::FunctionType::ResultType& result, const Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn1<const Algorithm>(arg1,result,algorithm);
  }

  template<class Algorithm> inline bool Apply(TreeNode <typename Algorithm::FunctionType::SourceType1>& tree, typename Algorithm::FunctionType::ResultType& dest, Algorithm& algorithm) {
    PRINTFUNCTION();
    bool result = Apply(*(tree.GetNode()),dest,algorithm);
    for(typename std::vector<TreeNode <typename Algorithm::FunctionType::SourceType1>*>::iterator i = tree.GetChildren().begin(); i != tree.GetChildren().end(); ++i) {
      result |= Apply<Algorithm>(*(*i),dest,algorithm);
    }    
    return result;
  }

  template<class Algorithm> inline bool Apply(const typename Algorithm::FunctionType::SourceType1& arg1, typename Algorithm::FunctionType::ResultType& result, Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn1<Algorithm>(arg1,result,algorithm);    
  }
  
  template<class Algorithm> inline bool Apply(const typename Algorithm::FunctionType::SourceType1& arg1, const typename Algorithm::FunctionType::SourceType2& arg2, typename Algorithm::FunctionType::ResultType& result, Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn2<Algorithm>(arg1,arg2,result,algorithm);
  }

  template<class Algorithm> inline bool Apply(const typename Algorithm::FunctionType::SourceType1& arg1, const typename Algorithm::FunctionType::SourceType2& arg2, typename Algorithm::FunctionType::ResultType& result, const Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn2<const Algorithm>(arg1,arg2,result,algorithm);
  }

  template<class Algorithm> inline bool Apply(const typename Algorithm::FunctionType::SourceType1& arg1, const typename Algorithm::FunctionType::SourceType2& arg2, const typename Algorithm::FunctionType::SourceType3& arg3, typename Algorithm::FunctionType::ResultType& result, Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn3<Algorithm>(arg1,arg2,arg3,result,algorithm);
  }

  template<class Algorithm> inline bool Apply(const typename Algorithm::FunctionType::SourceType1& arg1, const typename Algorithm::FunctionType::SourceType2& arg2, const typename Algorithm::FunctionType::SourceType3& arg3, typename Algorithm::FunctionType::ResultType& result, const Algorithm& algorithm) {
    PRINTFUNCTION();
    return Internal::_Apply_Fn3<const Algorithm>(arg1,arg2,arg3,result,algorithm);
  }
};

#endif//APPLY_GUARD
