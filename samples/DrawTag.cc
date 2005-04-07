/**
 * $Header$
 *
 * Draw a tag encoding the data from the command line
 */

#include <Total.hh>

// this file includes the definition of the type of tag used in the
// samples.  This is then typedef'd to be of type TagType which is
// used below
#include "TagDef.hh"

#include <cstring>
#include <iostream>

using namespace Total;

/**
 * A function to build the correct payload for the entered data.  code
 * should be a null terminated char array of '1' and '0' characters
 */
template<class TAG> CyclicBitSet<TAG::TagPayloadSize> GetCode(char* code) {
  CyclicBitSet<TAG::TagPayloadSize> tag_data;

  int length = strlen(code);

  for(int i=0;i<TAG::TagPayloadSize;++i) {
    tag_data[i] = 0;
  }

  if (length > TAG::TagPayloadSize) length = TAG::TagPayloadSize;
  for(int i=length-1;i>=0;i--) {
    tag_data[i] = (*code == '1');
    code++;
    if (!code[0]) break;
  }
  
  return tag_data;
}

int
main(int argc, char* argv[]) 
{

  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " code" << std::endl;
    std::cerr << "where code is either a string of 1 and 0 e.g 11101010101 or a text characters e.g. Total" << std::cerr;
    exit(-1);
  }
  try {
    // create the image that will hold the tag design
    Image i(512,512);

    // create the tag
    TagType t;

    // build the code
    char* code = argv[1];
    if (code[0] == '0' || code[0] == '1') {}
    else {
      char* newcode = new char[strlen(code)*8];
      char* cptr = newcode;
      while(*code) {
	char letter = *code;
	for(int i=0;i<8;++i) {
	  *cptr = (letter & 0x1) ? '1' : '0';
	  ++cptr;
	  letter >>=1;
	}
	++code;
      }
      code = newcode;
    }

    CyclicBitSet<TagType::TagPayloadSize> cbs=GetCode<TagType>(code);

    // keep a copy of original code so we can warn if we cannot encode it
    CyclicBitSet<TagType::TagPayloadSize> toencode = cbs; // copy construct
    if (!t.EncodedValue(toencode)) {
      std::cerr << "Failed to encode/decode this value. Aborting" << std::endl;
      exit(-1);
    }

    if (cbs != toencode) {
      std::cerr << "Warning! Failed to encode requested data " << std::endl;
    }
  
    // draw the code onto the image
    t.Draw2D(i,cbs);

    // save the image to disk
    i.Save("draw.pnm");
	
    std::cout << "Encoded value is: " << toencode << std::endl;
  }
  catch (const char* message) {
    std::cerr << "Caught exception (aborting): " << message << std::endl;
  }  
}
