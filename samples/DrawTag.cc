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
 *
 * Draw a tag encoding the data from the command line
 */


#include <cstring>
#include <iostream>


#include <Cantag.hh>

// this file includes the definition of the type of tag used in the
// samples.  This is then typedef'd to be of type TagType which is
// used below
#include "TagDef.hh"

using namespace Cantag;


typedef TestSquare TagType;

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
    data->payload.SetCode(code);
    
    // keep a copy of original code so we can warn if we cannot encode it
    CyclicBitSet<TagType::PayloadSize> toencode = data->payload; // copy construct
      
    if (!Encode<TagType>()(d)) {
      std::cerr << "Failed to encode this value. Aborting" << std::endl;
      exit(-1);
    }

    std::cout << "Encoded value is: " << data->payload << std::endl;

    
    // create the image that will hold the tag design
    Image<Colour::Grey> i(512,512);
    if (!DrawTag(t)(d,i)) {
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
