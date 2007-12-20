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
 */

#include <algorithm>
#include <queue>

#include <cantag/TagAnalogue.hh>
#include <cantag/algorithms/SampleTagAnalogue.hh>

namespace Cantag
{
	
	template<>
	bool SampleTagAnalogueUsingLineTrace<2>::operator()(const MonochromeImage& image, const TransformEntity& source, SampleAnalogueEntity<2>& dest) const
	{
		const int NUM_BARS = 2;
		
		if (source.GetPreferredTransform() != NULL)
		{
			const Transform& tag_to_npcf_transform = *source.GetPreferredTransform();
			
			for (int i = 0; i < NUM_BARS; i++)
			{
				const AnalogueBarSpec& bar_spec = m_tag.GetBarSpecs()[i];
				const Transform& bar_to_tag_transform = bar_spec.GetTransform();
				
				// FIXME: we're using ints here; this may not be ideal
				
				std::cout << "bar_spec min val is " << bar_spec.GetMinVal() << std::endl;
				std::cout << "bar_spec max val is " << bar_spec.GetMaxVal() << std::endl;

				// declare our many variables! (initialise only the bar coordinates)				
				FPoint left_extremum_bar_coords(bar_spec.GetMinVal(), (bar_spec.GetMinY() + bar_spec.GetMaxY()) / 2);
				FPoint left_extremum_tag_coords, left_extremum_npcf_coords, left_extremum_image_coords, left_extremum_rotated_image_coords;
				FPoint right_extremum_bar_coords(bar_spec.GetMaxVal(), (bar_spec.GetMinY() + bar_spec.GetMaxY()) / 2);
				FPoint right_extremum_tag_coords, right_extremum_npcf_coords, right_extremum_image_coords, right_extremum_rotated_image_coords;
				
				// transform from bar to image coordinates
				bar_to_tag_transform.Apply(left_extremum_bar_coords, left_extremum_tag_coords);
				bar_to_tag_transform.Apply(right_extremum_bar_coords, right_extremum_tag_coords);
				tag_to_npcf_transform.Apply(left_extremum_tag_coords, left_extremum_npcf_coords);
				tag_to_npcf_transform.Apply(right_extremum_tag_coords, right_extremum_npcf_coords);
				left_extremum_image_coords = left_extremum_npcf_coords;
				m_camera.NPCFToImage(left_extremum_image_coords);
				right_extremum_image_coords = right_extremum_npcf_coords;
				m_camera.NPCFToImage(right_extremum_image_coords);
				
				// work out the transform required to rotate the image such that:
				//  - the bar's x-axis is parallel to the image's x-axis
				//  - the left extremum's x-coordinate is less than the right extremum's (i.e. left really is left!)
				typedef FPoint Vector;
				Vector extremum_difference_image_coords;
				extremum_difference_image_coords.x(right_extremum_image_coords.x() - left_extremum_image_coords.x());
				extremum_difference_image_coords.y(right_extremum_image_coords.y() - left_extremum_image_coords.y());
				float angle_of_left_extremum_to_right_extremum_vector = atan2(extremum_difference_image_coords.y(), extremum_difference_image_coords.x());
				float rotation_angle = FLT_PI - angle_of_left_extremum_to_right_extremum_vector;
				Transform image_to_rotated_image_transform = Transform::MakeXYRotationTransform(rotation_angle);
//				image_to_rotated_image_transform[11] = 1.0; // FIXME
				
				// perform this transform
				image_to_rotated_image_transform.Apply(left_extremum_image_coords, left_extremum_rotated_image_coords);
				image_to_rotated_image_transform.Apply(right_extremum_image_coords, right_extremum_rotated_image_coords);
				
				// work out the inverse transform, which we'll use later
//				image_to_rotated_image_transform[11] = 0.0; // FIXME
				Transform rotated_image_to_image_transform = image_to_rotated_image_transform;
				rotated_image_to_image_transform.Invert();
//				image_to_rotated_image_transform[11] = 1.0; // FIXME
//				rotated_image_to_image_transform[11] = 1.0; // FIXME
				
				// DEBUG START
				std::cout << "Left extremum bar: " << left_extremum_bar_coords << std::endl;
				std::cout << "Left extremum tag: " << left_extremum_tag_coords << std::endl;
				std::cout << "Left extremum NPCF: " << left_extremum_npcf_coords << std::endl;
				std::cout << "Left extremum image: " << left_extremum_image_coords << std::endl;
				std::cout << "Left extremum rotated image: " << left_extremum_rotated_image_coords << std::endl;
				std::cout << "Right extremum bar: " << right_extremum_bar_coords << std::endl;
				std::cout << "Right extremum tag: " << right_extremum_tag_coords << std::endl;
				std::cout << "Right extremum NPCF: " << right_extremum_npcf_coords << std::endl;
				std::cout << "Right extremum image: " << right_extremum_image_coords << std::endl;
				std::cout << "Right extremum rotated image: " << right_extremum_rotated_image_coords << std::endl;
				// DEBUG END
				
				// DEBUG START
				std::cout << "BAR TO TAG:";
				bar_to_tag_transform.Print();
				std::cout << "TAG TO NPCF:";
				tag_to_npcf_transform.Print();
				std::cout << "NPCF TO IMAGE:\n\tNot sure; encapsulated in m_camera" << std::endl;
				std::cout << "IMAGE TO ROTATED IMAGE:";
				image_to_rotated_image_transform.Print();
				// DEBUG END
				
				// trace along the bar until we get to:
				//  - either the right extremum (in which case bar value is 100%)  <-- TODO: deal with this
				//  - or the black-into-white boundary
				// TODO: may need to deal with special case of a value of 0%
				FPoint working_image_coords = left_extremum_image_coords;
				FPoint working_rotated_image_coords = left_extremum_rotated_image_coords;
				bool keep_going = true;
				bool left_pixel;
				bool right_pixel = image.GetPixel(left_extremum_image_coords);
				while (keep_going)
				{
					// move one pixel along the x-axis in the rotated image, then transform to the unrotated image's coordinate system
					working_rotated_image_coords.x(working_rotated_image_coords.x() + 1);
					rotated_image_to_image_transform.Apply(working_rotated_image_coords, working_image_coords);
					
					// DEBUG START
//					std::cout << "Working rotated image coords: " << working_rotated_image_coords << std::endl;
//					std::cout << "Working image coords: " << working_image_coords << std::endl;
					// DEBUG END
					
					// sample the unrotated image at these coordinates
					std::cout << "Sampling analogue tag at " << working_image_coords << std::endl;
					left_pixel = right_pixel;
					right_pixel = image.GetPixel(working_image_coords);
					
					// stop if we've reached the black-into-white boundary
					keep_going = false;// !(left_pixel && !right_pixel);
				}
				FPoint bar_end_image_coords = working_image_coords;
				FPoint bar_end_npcf_coords = bar_end_image_coords;
				m_camera.ImageToNPCF(bar_end_npcf_coords);
				
				// transform the coordinate we stopped at into tag coordinates
				Transform npcf_to_tag_transform = tag_to_npcf_transform;
				npcf_to_tag_transform.Invert();
				FPoint bar_end_tag_coords;
				npcf_to_tag_transform.Apply(bar_end_npcf_coords, bar_end_tag_coords);
				
				// populate the object to be returned
				dest.SetTagPoint(i, bar_end_tag_coords);
				dest.SetValid(true);
			}
			
			return true;
		}
		else
		{
			return false;
		}
	}
	
	
	
#if 0
	template<>
	bool SampleTagAnalogueUsingBinaryChop<2>::operator()(const MonochromeImage& image, const TransformEntity& source, SampleAnalogueEntity<2>& dest) const
	{
		const int NUM_BARS = 2;
		
		const Transform* tag_to_image_transform = source.GetPreferredTransform();
		
		if (tag_to_image_transform)
		{
			for (int i = 0; i < NUM_BARS; i++)
			{
				const AnalogueBarSpec& bar_spec = m_tag.GetBarSpecs()[i];
				const Transform& bar_to_tag_transform = bar_spec.GetTransform();
				
				float left_edge_bar_coords[] = { bar_spec.GetMinVal(), (bar_spec.GetMinY() + bar_spec.GetMaxY()) / 2 };
				float right_edge_bar_coords[] = { bar_spec.GetMaxVal(), (bar_spec.GetMinY() + bar_spec.GetMaxY()) / 2 };
				float left_edge_tag_coords[2];
				float right_edge_tag_coords[2];
				float left_edge_image_coords[2];
				float right_edge_image_coords[2];
				
				bool todo = true;
				do
				{				
					// transform from bar to tag coordinates
					bar_to_tag_transform.Apply(left_edge_bar_coords[0], left_edge_bar_coords[1], left_edge_tag_coords, left_edge_tag_coords + 1);
					bar_to_tag_transform.Apply(right_edge_bar_coords[0], right_edge_bar_coords[1], right_edge_tag_coords, right_edge_tag_coords + 1);
					
					// transform from tag coordinates to image
				}
				while (todo);
				
				dest.SetValid(true);
			}
			
			return true;
		}
		else
		{
			return false;
		}
	}
#endif
	
}
