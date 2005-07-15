/**
 * $Header$
 *
 * Draw a tag encoding the data from the command line
 */


#include <cstring>
#include <iostream>


#include <Total.hh>

// this file includes the definition of the type of tag used in the
// samples.  This is then typedef'd to be of type TagType which is
// used below
#include "TagDef.hh"

using namespace Total;

/**
 * A function to build the correct payload for the entered data.  code
 * should be a null terminated char array of '1' and '0' characters
 */
template<class TAG> void GetCode(char* code, CyclicBitSet<TAG::PayloadSize>& tag_data) {
  int length = strlen(code);

  for(int i=0;i<TAG::PayloadSize;++i) {
    tag_data[i] = 0;
  }

  if (length > TAG::PayloadSize) length = TAG::PayloadSize;
  for(int i=length-1;i>=0;i--) {
    tag_data[i] = (*code == '1');
    code++;
    if (!code[0]) break;
  }
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

    DecodeEntity<TagType::PayloadSize> d;
    DecodeEntity<TagType::PayloadSize>::Data* data = d.Add();
    GetCode<TagType>(code,data->payload);

    // keep a copy of original code so we can warn if we cannot encode it
    CyclicBitSet<TagType::PayloadSize> toencode = data->payload; // copy construct
      
    if (!Encode<TagType>()(d)) {
      std::cerr << "Failed to encode this value. Aborting" << std::endl;
      exit(-1);
    }

    std::cout << "Encoded value is: " << data->payload << std::endl;

    
    // create the image that will hold the tag design
    Image i(512,512);
    if (!DrawTagCircle(t)(d,i)) {
      std::cerr << "Failed to draw tag. Aborting" << std::endl;
      exit(-1);
    }

    if (!Decode<TagType>()(d)) {
      std::cerr << "Failed to decode encoded data.  Aborting" << std::endl;
      exit(-1);
    }

    if (data->payload != toencode) {
      std::cerr << "Warning! Failed to encode requested data " << std::endl;
    }
  
    // save the image to disk
    i.Save("draw.pnm");
	
  }
  catch (const char* message) {
    std::cerr << "Caught exception (aborting): " << message << std::endl;
  }  
}
