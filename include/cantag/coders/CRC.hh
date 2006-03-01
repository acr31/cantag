#ifndef CRC_HH
#define CRC_HH

#include <bitset>
#include <iostream>


template <int CRC_SIZE, int PAYLOAD_SIZE>
struct CRCPolynomial {
  enum {
    Value = CRCPolynomial<CRC_SIZE,PAYLOAD_SIZE-1>::Value
  };
};



/**
 * A SIMPLE implementation of a CRC
 * No doubt it could be improved, but it works!
 */
template <int DATA_BITS, int CRC_BITS> class CRC {
public:

  std::bitset<CRC_BITS> GetCRC(std::bitset<DATA_BITS> &payload) {
    // First of all get the polynomial for this setup
    std::bitset<CRC_BITS+1> gen_poly = std::bitset<CRC_BITS+1>( CRCPolynomial<CRC_BITS,DATA_BITS>::Value );

    std::bitset<CRC_BITS+1> numer(0);
    std::bitset<CRC_BITS> result(0);
	int extent = CRC_BITS + 1;
	if (extent > DATA_BITS) extent = DATA_BITS;
    for (int i=0; i<extent; i++) {
      if (payload[DATA_BITS-1-i]) numer.set(CRC_BITS-i,1);
    }
    
    for (int it=0; it<DATA_BITS; it++) {
      if (numer[CRC_BITS]) numer = numer ^ gen_poly;  
      numer <<= 1;
      int idx = DATA_BITS-1-(CRC_BITS+1)-it;
      if (idx>=0 && payload[idx]) numer.set(0,1);
    }
    numer >>= 1;
    
    for (int i=0; i<CRC_BITS; i++) {
      if (numer[i]) result.set(i,1);
    }
    
    return result;
  }
}; 



template<> struct CRCPolynomial<2,2> {
  enum {
    Value = 3
  };
};

template<> struct CRCPolynomial<3,0> {
	enum {
		Value = 5
	};
};
template<> struct CRCPolynomial<4,0> {
	enum {
		Value = 9
	};
};
template<> struct CRCPolynomial<4,12> {
	enum {
		Value = 9
	};
};
template<> struct CRCPolynomial<5,0> {
	enum {
		Value = 21
	};
};
template<> struct CRCPolynomial<5,11> {
	enum {
		Value = 18
	};
};
template<> struct CRCPolynomial<5,27> {
	enum {
		Value = 18
	};
};
template<> struct CRCPolynomial<6,0> {
	enum {
		Value = 44
	};
};
template<> struct CRCPolynomial<6,26> {
	enum {
		Value = 18
	};
};
template<> struct CRCPolynomial<6,58> {
	enum {
		Value = 33
	};
};
template<> struct CRCPolynomial<7,0> {
	enum {
		Value = 91
	};
};
template<> struct CRCPolynomial<7,57> {
	enum {
		Value = 72
	};
};
template<> struct CRCPolynomial<7,121> {
	enum {
		Value = 72
	};
};
template<> struct CRCPolynomial<8,0> {
	enum {
		Value = 156
	};
};
template<> struct CRCPolynomial<8,10> {
	enum {
		Value = 151
	};
};
template<> struct CRCPolynomial<8,120> {
	enum {
		Value = 166
	};
};
template<> struct CRCPolynomial<8,248> {
	enum {
		Value = 166
	};
};
template<> struct CRCPolynomial<9,0> {
	enum {
		Value = 316
	};
};
template<> struct CRCPolynomial<9,9> {
	enum {
		Value = 389
	};
};
template<> struct CRCPolynomial<9,14> {
	enum {
		Value = 331
	};
};
template<> struct CRCPolynomial<9,247> {
	enum {
		Value = 359
	};
};
template<> struct CRCPolynomial<9,503> {
	enum {
		Value = 359
	};
};
template<> struct CRCPolynomial<10,0> {
	enum {
		Value = 654
	};
};
template<> struct CRCPolynomial<10,13> {
	enum {
		Value = 697
	};
};
template<> struct CRCPolynomial<10,22> {
	enum {
		Value = 793
	};
};
template<> struct CRCPolynomial<10,502> {
	enum {
		Value = 807
	};
};
template<> struct CRCPolynomial<10,1014> {
	enum {
		Value = 807
	};
};
template<> struct CRCPolynomial<11,0> {
	enum {
		Value = 1393
	};
};
template<> struct CRCPolynomial<11,13> {
	enum {
		Value = 1330
	};
};
template<> struct CRCPolynomial<11,23> {
	enum {
		Value = 1495
	};
};
template<> struct CRCPolynomial<11,26> {
	enum {
		Value = 1411
	};
};
template<> struct CRCPolynomial<11,1013> {
	enum {
		Value = 1613
	};
};
template<> struct CRCPolynomial<11,2037> {
	enum {
		Value = 1613
	};
};
template<> struct CRCPolynomial<12,0> {
	enum {
		Value = 2639
	};
};
template<> struct CRCPolynomial<12,12> {
	enum {
		Value = 2881
	};
};
template<> struct CRCPolynomial<12,28> {
	enum {
		Value = 2296
	};
};
template<> struct CRCPolynomial<12,54> {
	enum {
		Value = 3079
	};
};
template<> struct CRCPolynomial<12,2036> {
	enum {
		Value = 2933
	};
};
template<> struct CRCPolynomial<13,0> {
	enum {
		Value = 4279
	};
};
template<> struct CRCPolynomial<13,12> {
	enum {
		Value = 4773
	};
};
template<> struct CRCPolynomial<13,13> {
	enum {
		Value = 6409
	};
};
template<> struct CRCPolynomial<13,53> {
	enum {
		Value = 4138
	};
};
template<> struct CRCPolynomial<14,0> {
	enum {
		Value = 9073
	};
};
template<> struct CRCPolynomial<14,12> {
	enum {
		Value = 10409
	};
};
template<> struct CRCPolynomial<14,14> {
	enum {
		Value = 14123
	};
};
template<> struct CRCPolynomial<14,58> {
	enum {
		Value = 8493
	};
};
template<> struct CRCPolynomial<14,113> {
	enum {
		Value = 8680
	};
};
template<> struct CRCPolynomial<15,0> {
	enum {
		Value = 25355
	};
};
template<> struct CRCPolynomial<15,13> {
	enum {
		Value = 23509
	};
};
template<> struct CRCPolynomial<15,17> {
	enum {
		Value = 22330
	};
};
template<> struct CRCPolynomial<15,115> {
	enum {
		Value = 27277
	};
};
template<> struct CRCPolynomial<15,137> {
	enum {
		Value = 18806
	};
};
template<> struct CRCPolynomial<16,0> {
	enum {
		Value = 36827
	};
};
template<> struct CRCPolynomial<16,16> {
	enum {
		Value = 38539
	};
};
template<> struct CRCPolynomial<16,20> {
	enum {
		Value = 51308
	};
};
template<> struct CRCPolynomial<16,136> {
	enum {
		Value = 44186
	};
};
template<> struct CRCPolynomial<16,242> {
	enum {
		Value = 47789
	};
};

#endif
