/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/02/20 22:25:55  acr31
 * major reworking of matching algorithms and architecture
 *
 */

#ifndef SHAPE_CHAIN_GUARD
#define SHAPE_CHAIN_GUARD

/**
 * Placeholder for end of shape chain list
 */
class ShapeChainEOL {
public:
  ShapeChainEOL() {};
  ShapeChainEOL(float* points, int numpoints) {};
  ShapeChainEOL(float* points, int numpoints,bool fitted) {};
  inline bool IsChainFitted() const { return false; }
};


/**
 * Store a chain of objects capable of matching a contour.  Every time
 * we see a contour we pass it off to the shape chain.  Each matcher
 * in turn gets a go at matching the shape.  If it fails then it
 * passes the request on to the next one.  The first templated
 * parameter M is the shape that this stage matches and the second
 * parameters is another shape chain with another shape to match on
 */
template<class M, class S = ShapeChainEOL>
class ShapeChain {
private:
  M m_shape;
  S m_next;

public:
  ShapeChain() : m_shape(NULL,0,true), m_next() {};
  ShapeChain(float* points, int numpoints) : 
    m_shape(points,numpoints,false),
    m_next(points,numpoints,m_shape.IsFitted()) {};
  ShapeChain(float* points, int numpoints, bool prev_fitted) :
    m_shape(points,numpoints,prev_fitted),
    m_next(points,numpoints,prev_fitted) {};

  inline bool IsFitted() const {
    return m_shape.IsFitted();
  }

  inline bool IsChainFitted() const {
    return m_shape.IsFitted() ||
      m_next.IsChainFitted();
  }

  inline const S Next() const {
    return m_next;
  }

  inline const M GetShape() const {
    return m_shape;
  }

  inline bool Compare(ShapeChain<M,S> o) const {
    return 
      m_shape.Compare(o.m_shape) &&
      m_next.Compare(o.m_next);      
  }

};



#endif//SHAPE_CHAIN_GUARD
