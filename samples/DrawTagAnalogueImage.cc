/*
  Copyright (C) 2007 Tom Craig

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

  Email: jatc2@cam.ac.uk
*/

/**
 * $Header$
 *
 * Draw a tag encoding the data from the command line
 */


#include <cstring>
#include <iostream>

#include <Cantag.hh>
#include <cantag/Apply.hh>
#include <cantag/algorithms/DrawTagAnalogue.hh>
#include "TagDef.hh"

using namespace Cantag;

int
main(int argc, char* argv[]) 
{
    
    int NUM_ARGS = 8;
    if (argc < NUM_ARGS)
    {
        std::cerr << "Usage: " << argv[0] << " filename bar1_min bar1_max bar2_min bar2_max bar1_val bar2_val" << std::endl;
        exit(-1);
    }
    
    const char* FILENAME = argv[1];
    const int SQUARE_SIZE = 512;
    const int NUM_BARS = 2;
    
    const int bar1_min = atoi(argv[2]);
    const int bar1_max = atoi(argv[3]);
    const int bar2_min = atoi(argv[4]);
    const int bar2_max = atoi(argv[5]);
    const int bar1_val = atoi(argv[6]);
    const int bar2_val = atoi(argv[7]);
    
    GreyImage image(SQUARE_SIZE, SQUARE_SIZE);
    
    boost::array<std::pair<int, int>, NUM_BARS> bounds = { std::make_pair(bar1_min, bar1_max), std::make_pair(bar2_min, bar2_max) };
    boost::array<float, NUM_BARS> values = { bar1_val, bar2_val };
    
    TagAnalogue<NUM_BARS> tag = TagAnalogue<NUM_BARS>::CreateAnalogueTagForDebugging(bounds);
    DrawTagAnalogue<NUM_BARS> dta(tag, values);
    Apply(image, dta);
    image.Save(FILENAME);
    std::cout << "Saved to " << FILENAME << std::endl;
    
    /*
    // arguments
    float bar_value = atof(argv[1]);
    float analogue_right_of_digital = atoi(argv[2]);
    float bars_extend_from_right = atoi(argv[3]);
    
    // some constants
    const char* FILENAME = "draw.pnm";
    const int SQUARE_SIZE = 512;
    const int SEPARATOR_WIDTH = int(DecodeAnalogue::SEPARATOR_WIDTH_FRACTION * SQUARE_SIZE);
    
    // meat and bones
    GreyImage image(SQUARE_SIZE + SEPARATOR_WIDTH + SQUARE_SIZE, SQUARE_SIZE);
    DrawTagAnalogue dta(bar_value, SEPARATOR_WIDTH, analogue_right_of_digital, bars_extend_from_right);
    Apply(image, dta);
    image.Save(FILENAME);
    std::cout << "Saved to " << FILENAME << std::endl;
    */
    
}
