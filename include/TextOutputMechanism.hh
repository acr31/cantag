/**
 * $Header$
 */ 

#ifndef TEXT_OUTPUT_MECHANISM_GUARD
#define TEXT_OUTPUT_MECHANISM_GUARD

#include <ostream>
#include <LocatedObject.hh>

template<int PAYLOAD_SIZE> 
class TextOutputMechanism {
private:
  std::ostream& m_ostream;

  class TextOutputFun : public SceneGraphFunctional<PAYLOAD_SIZE> {
  public:
    void Eval(LocatedObject<PAYLOAD_SIZE>*  loc) {
      std::cout << "Found tag: " << *(loc->tag_code) << 
	" Location: " << loc->location[0] << " " << loc->location[1] << " " << loc->location[2] <<
	" Normal: "   << loc->normal[0] << " " << loc->normal[1] << " " << loc->normal[2] << std::endl;
    }
  };
  

public:
  
  TextOutputMechanism(std::ostream& ostream);
  
  void NewData(const Image* newdata);
  template<class C> void Output(SceneGraph<C,PAYLOAD_SIZE>& sg);

};

template<int PAYLOAD_SIZE> TextOutputMechanism<PAYLOAD_SIZE>::TextOutputMechanism(std::ostream& ostream) :
  m_ostream(ostream) {}


template<int PAYLOAD_SIZE> template<class C> void TextOutputMechanism<PAYLOAD_SIZE>::Output(SceneGraph<C,PAYLOAD_SIZE>& sg) {

  TextOutputFun j;
  sg.Map(j);
}

template<int PAYLOAD_SIZE> void TextOutputMechanism<PAYLOAD_SIZE>::NewData(const Image* newdata) {}

#endif//TEXT_OUTPUT_MECHANISM_GUARD
