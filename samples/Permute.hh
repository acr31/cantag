/* an algorithm to permute the sampled data as if it were read in the
   order of the circle tag encoding used in the SN08 survey */

#include <Cantag.hh>

template<int PAYLOAD_SIZE>
struct Permute : public Cantag::Function<TL0,TL1(Cantag::DecodeEntity<PAYLOAD_SIZE>)> {
  
  bool operator()(Cantag::DecodeEntity<PAYLOAD_SIZE>& d) const {
    for(typename std::vector<typename Cantag::DecodeEntity<PAYLOAD_SIZE>::Data*>::iterator i = d.GetPayloads().begin(); i != d.GetPayloads().end(); ++i ) {
      Cantag::CyclicBitSet<PAYLOAD_SIZE>& p = (*i)->payload;

      Cantag::CyclicBitSet<PAYLOAD_SIZE> clone(p);

      for(int j=0;j<PAYLOAD_SIZE;j+=2) {
	// swap alternate bits
	p.Set(j,clone[j+1]);
	p.Set(j+1,clone[j]);
      }
    }
    return true;
  } 
};


template<int PAYLOAD_SIZE>
struct Permute2 : public Cantag::Function<TL0,TL1(Cantag::DecodeEntity<PAYLOAD_SIZE>)> {
  
  bool operator()(Cantag::DecodeEntity<PAYLOAD_SIZE>& d) const {
    for(typename std::vector<typename Cantag::DecodeEntity<PAYLOAD_SIZE>::Data*>::iterator i = d.GetPayloads().begin(); i != d.GetPayloads().end(); ++i ) {
      Cantag::CyclicBitSet<PAYLOAD_SIZE>& p = (*i)->payload;

      Cantag::CyclicBitSet<PAYLOAD_SIZE> clone(p);

      for(int j=0;j<PAYLOAD_SIZE;j++) {
	p.Set(j,clone[PAYLOAD_SIZE-1-j]);
      }
    }
    return true;
  } 
};
