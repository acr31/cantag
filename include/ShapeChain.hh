/**
 * $Header$
 */

#ifndef SHAPE_CHAIN_GUARD
#define SHAPE_CHAIN_GUARD


#ifdef HAVE_BOOST_ARCHIVE
#include <boost/serialization/access.hpp>
#endif

/**
 * Placeholder for end of shape chain list
 */
class ShapeChainEOL {
public:
  ShapeChainEOL() {};
  ShapeChainEOL(const std::vector<float>& points) {};
  ShapeChainEOL(const std::vector<float>& points,bool fitted) {};
  inline bool IsChainFitted() const { return false; }
  inline void DrawChain(Image& image) const {};

private:
#ifdef HAVE_BOOST_ARCHIVE
  friend class boost::serialization::access;
  template<class Archive> void serialize(Archive & ar, const unsigned int version) {}
#endif
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
  ShapeChain();
  ShapeChain(const std::vector<float>& points);
  ShapeChain(const std::vector<float>& points, bool prev_fitted);
  ~ShapeChain();
  inline bool IsFitted() const;

  inline bool IsChainFitted() const;

  inline const S& Next() const;

  inline const M& GetShape() const;

  inline bool Compare(ShapeChain<M,S> o) const;

  void DrawChain(Image& image) const;
  
private:
#ifdef HAVE_BOOST_ARCHIVE
  friend class boost::serialization::access;
  template<class Archive> void serialize(Archive & ar, const unsigned int version);
#endif
};

#ifdef HAVE_BOOST_ARCHIVE
//BOOST_CLASS_TRACKING(ShapeChain, boost::serialization::track_never);
namespace boost { 
  namespace serialization {
    template<class M,class S>
    struct tracking_level<ShapeChain<M,S> >
    {
      typedef mpl::integral_c_tag tag;
      typedef mpl::int_<track_never> type;
      BOOST_STATIC_CONSTANT(
			    enum tracking_type, 
			    value = static_cast<enum tracking_type>(type::value)
			    );
    };
  } // serialization
} // boost

//BOOST_CLASS_IMPLEMENTATION(ShapeChain, boost::serialization::object_serializable);
namespace boost { 
  namespace serialization {
    template<class M,class S>
    struct implementation_level<ShapeChain<M,S> >
    {
      typedef mpl::integral_c_tag tag;
      typedef mpl::int_<object_serializable> type;
      BOOST_STATIC_CONSTANT(
			    enum level_type,
			    value = static_cast<enum level_type>(type::value)
			    );
    };
  } // serialization
} // boost

template<class M, class S> template<class Archive> void ShapeChain<M,S>::serialize(Archive & ar, const unsigned int version) {
  ar & m_shape;
  ar & m_next;
}
#endif

template<class M, class S> ShapeChain<M,S>::ShapeChain() : m_shape(), m_next() {};
template<class M, class S> ShapeChain<M,S>::~ShapeChain() {};

template<class M, class S> ShapeChain<M,S>::ShapeChain(const std::vector<float>& points) : 
    m_shape(points,false),
    m_next(points,m_shape.IsFitted()) {};

template<class M, class S> ShapeChain<M,S>::ShapeChain(const std::vector<float>& points, bool prev_fitted) :
    m_shape(points,prev_fitted),
    m_next(points,prev_fitted) {};


template<class M, class S> bool ShapeChain<M,S>::IsFitted() const {
    return m_shape.IsFitted();
  }

template<class M, class S> bool ShapeChain<M,S>::IsChainFitted() const {
    return m_shape.IsFitted() ||
      m_next.IsChainFitted();
  }

template<class M, class S> const S& ShapeChain<M,S>::Next() const {
    return m_next;
  }

template<class M, class S> const M& ShapeChain<M,S>::GetShape() const {
  return m_shape;
}

template<class M, class S> bool ShapeChain<M,S>::Compare(ShapeChain<M,S> o) const {
    return 
      m_shape.Compare(o.m_shape) &&
      m_next.Compare(o.m_next);      
  }

template<class M, class S> void ShapeChain<M,S>::DrawChain(Image& image) const {
  m_shape.Draw(image);
  m_next.DrawChain(image);
}


#endif//SHAPE_CHAIN_GUARD
