#include <algorithm>
using std::max;
using std::min;
#include <queue>
using std::queue;

#include <cantag/algorithms/DecodeAnalogue.hh>

namespace Cantag
{

    DecodeAnalogue::DecodeAnalogue(GreyImage& output, const Camera& camera)
        : m_output(output),
          m_camera(camera)
    {
    }
    
    bool DecodeAnalogue::operator()(const MonochromeImage& image, const TransformEntity& source, AnalogueEntity& dest) const
    {
        const Transform* i = source.GetPreferredTransform();
        
        std::cout << "Printing transform" << std::endl;
        i->Print();
        std::cout << "In DecodeAnalogue::operator" << std::endl;
        
        if (i)
        {
            // start at the bar minimum and maximum
            const float Y_TAG_COORD = 0.0; // the bars are horizontal, so their y-coordinates are constant
            const float DUMMY = 0.0;
            float left_edge_tag_coords[] = { 1.1, Y_TAG_COORD };
            float right_edge_tag_coords[] = { 2.9, Y_TAG_COORD };
            float left_edge_image_coords[] = { DUMMY, DUMMY };
            float right_edge_image_coords[] = { DUMMY, DUMMY };
            
            // this will store our series of binary chops
            typedef int Chop;
            Chop LEFT_CHOP = 1;
            Chop RIGHT_CHOP = 2;
            queue<Chop> chops;
            
            // this will store whether the tag is upside-down in the image
            bool UPSIDE_DOWN_TAG = false;
            
            do
            {
                // transform from tag coordinates to image coordinates
                i->Apply(left_edge_tag_coords[0], left_edge_tag_coords[1], left_edge_image_coords, left_edge_image_coords + 1);
                i->Apply(right_edge_tag_coords[0], right_edge_tag_coords[1], right_edge_image_coords, right_edge_image_coords + 1);
                m_camera.NPCFToImage(left_edge_image_coords, 1);
                m_camera.NPCFToImage(right_edge_image_coords, 1);
                
                std::cout << "Looping, leftx = " << left_edge_image_coords[0]
                          <<        ", lefty = " << left_edge_image_coords[1]
                          <<       ", rightx = " << right_edge_image_coords[0]
                          <<       ", righty = " << right_edge_image_coords[1] << std::endl;
                
                // is the tag upside-down in the image?
                //  - if so, the left edge will be to the right of the right edge in the image, which affects our while test below 
                //  - N.B. we only need to check once round this loop, hence the if-test
                if (chops.empty())
                    UPSIDE_DOWN_TAG = (left_edge_image_coords[0] > right_edge_image_coords[0]);
                
                // TODO: range-checking
                
                // sample halfway between the two edges
                // (WARNING: the calculation for the y-coord assumes the bar is straight in the image)
                const float SAMPLE_X_IMAGE_COORD = (left_edge_image_coords[0] + right_edge_image_coords[0]) / 2;
                const float SAMPLE_Y_IMAGE_COORD = (left_edge_image_coords[1] + right_edge_image_coords[1]) / 2;
                bool sample = image.GetPixel(SAMPLE_X_IMAGE_COORD, SAMPLE_Y_IMAGE_COORD);
                
                // print where we're sampling
                float sample_image_coords[] = { SAMPLE_X_IMAGE_COORD, SAMPLE_Y_IMAGE_COORD };
                m_output.DrawPixel(sample_image_coords[0], sample_image_coords[1], COLOUR_BLACK);
                
                // do the binary chop for next time, i.e. redefine one of the edges
                Chop chop = (sample ? RIGHT_CHOP : LEFT_CHOP);
                std::cout << "Going " << (sample ? "right" : "left") << std::endl;
                float middle_tag_x_coord = (left_edge_tag_coords[0] + right_edge_tag_coords[0]) / 2;
                (sample ? right_edge_tag_coords : left_edge_tag_coords)[0] = middle_tag_x_coord;
                
                // store which way we did the binary chop
                chops.push(chop);
            }
            // keep going until our left and right x-coords are the same
            while (UPSIDE_DOWN_TAG ? left_edge_image_coords[0] > right_edge_image_coords[0]
                                   : left_edge_image_coords[0] < right_edge_image_coords[0]);
            
            // read out the value, and also record the maximum possible error
            float left = 0;
            float right = 1;
            while (!chops.empty())
            {
                Chop chop = chops.front();
                (chop == LEFT_CHOP ? left : right) = (left + right) / 2;
                chops.pop();
            }
            float val = (left + right) / 2;
            float max_error = max(abs(left - val), abs(right - val));
            
            // last but not least, put these in our AnalogueEntity
            dest.SetPayload(val);
            dest.SetMaxError(max_error);
            dest.SetValid(true);
            return true;
        }
        else
        {
            return false;
        }
    }
    
}
