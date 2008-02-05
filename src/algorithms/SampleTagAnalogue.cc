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

#include <cantag/MultipleBasisCoords.hh>
#include <cantag/TagAnalogue.hh>
#include <cantag/algorithms/SampleTagAnalogue.hh>

namespace Cantag
{
	
	/*template<>
	void SampleTagAnalogueBase<2>::Debug_DeleteMeWhenever(const Transform& tag_to_npcf_transform) const
	{
		FPoint debug_tag_coords(-0.625, -0.625, 0);
		FPoint debug_npcf_coords;
		tag_to_npcf_transform.Apply(debug_tag_coords, debug_npcf_coords);
		std::cout << "DEBUG (SampleTagAnalogueUsingLineTrace<2>::operator()) - Using FPoint - Tag coords: " << debug_tag_coords << std::endl;
		std::cout << "DEBUG (SampleTagAnalogueUsingLineTrace<2>::operator()) - Using FPoint - NPCF coords: " << debug_npcf_coords << std::endl;
		float pts[] = { -0.625, -0.625 };
		std::cout << "DEBUG (SampleTagAnalogueUsingLineTrace<2>::operator()) - Using float[] - Tag coords: (" << pts[0] << "," << pts[1] << ")" <<  std::endl;
		tag_to_npcf_transform.Apply(pts[0],pts[1],pts,pts+1);
		std::cout << "DEBUG (SampleTagAnalogueUsingLineTrace<2>::operator()) - Using float[] - Image coords: (" << pts[0] << "," << pts[1] << ")" << std::endl;
	}*/
	
	template<>
	void SampleTagAnalogueBase<2>::DebugPrint(FPoint left_extremum_bar_coords, FPoint left_extremum_tag_coords, FPoint left_extremum_npcf_coords, FPoint left_extremum_image_coords, FPoint left_extremum_rotated_image_coords,
	                                          FPoint right_extremum_bar_coords, FPoint right_extremum_tag_coords, FPoint right_extremum_npcf_coords, FPoint right_extremum_image_coords, FPoint right_extremum_rotated_image_coords,
	                                          Transform bar_to_tag_transform, Transform tag_to_npcf_transform, Transform image_to_rotated_image_transform) const
	{
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
		
		std::cout << "BAR TO TAG:";
		bar_to_tag_transform.Print();
		std::cout << "TAG TO NPCF:";
		tag_to_npcf_transform.Print();
		std::cout << "NPCF TO IMAGE:\n\tNot sure; encapsulated in m_camera" << std::endl;
		std::cout << "IMAGE TO ROTATED IMAGE:";
		image_to_rotated_image_transform.Print();
	}
		                
	template<>
	FriendlyXYRotationTransform SampleTagAnalogueBase<2>::GetImageToRotatedImageTransform(FPoint left_extremum_image_coords, FPoint right_extremum_image_coords) const
	{
		// work out the transform required to rotate the image such that:
		//  - the bar's x-axis is parallel to the image's x-axis
		//  - the left extremum's x-coordinate is less than the right extremum's (i.e. left really is left!)
		typedef FPoint Vector;
		Vector extremum_difference_image_coords(right_extremum_image_coords.x() - left_extremum_image_coords.x(),
		                                        right_extremum_image_coords.y() - left_extremum_image_coords.y(),
		                                        0.0);
		float angle_of_left_extremum_to_right_extremum_vector = atan2(extremum_difference_image_coords.y(), extremum_difference_image_coords.x());
		float rotation_angle = FLT_PI - angle_of_left_extremum_to_right_extremum_vector;
		FriendlyXYRotationTransform image_to_rotated_image_transform(rotation_angle, "Image to rotated image");
		image_to_rotated_image_transform.GetMatrix().Print(std::cout);
		return image_to_rotated_image_transform;
	}
	
	template<>
	bool SampleTagAnalogueUsingLineTrace<2>::operator()(const MonochromeImage& image, const TransformEntity& source, SampleAnalogueEntity<2>& dest) const
	{
		const int NUM_BARS = 2;
		std::cout << "DEBUG080123 In SampleTagAnalogueUsingLineTrace::operator()" << std::endl;
		
#if 0
		std::cout << "m_camera's parameters are:" << std::endl;
		std::cout << "\tprinciple x: " << m_camera.GetPrincipleX() << std::endl;
		std::cout << "\tprinciple y: " << m_camera.GetPrincipleY() << std::endl;
		std::cout << "\tx scale: " << m_camera.GetXScale() << std::endl;
		std::cout << "\ty scale: " << m_camera.GetYScale() << std::endl;
		
		std::cout << "Using m_camera's functions, image to NPCF to image is:" << std::endl;
		FPoint c(1040, 1470, 0);
		std::cout << "\t" << c << " -> ";
		m_camera.ImageToNPCF(c);
		std::cout << c << " -> ";
		m_camera.NPCFToImage(c);
		std::cout << c << std::endl;
		
		std::cout << "Using my matrices, NPCF to image to NPCF is:" << std::endl;
		Transform translation = Transform::Make2DTranslationTransform(-m_camera.GetPrincipleX(), -m_camera.GetPrincipleY());
		Transform scale = Transform::MakeScaleTransform(1.0 / m_camera.GetXScale(), 1.0 / m_camera.GetYScale());
		Transform image_to_npcf = scale * translation;
		Transform npcf_to_image = image_to_npcf;
		npcf_to_image.Invert();
		std::cout << "Image to NPCF is:";
		image_to_npcf.Print();
		std::cout << "NPCF to image (obtained through inversion) is:";
		npcf_to_image.Print();
		FPoint d(1040, 1470, 0);
		std::cout << "\t" << d << " -> ";
		image_to_npcf.Apply(d, d);
		std::cout << d << " -> ";
		npcf_to_image.Apply(d, d);
		std::cout << d << std::endl;
#endif
		
		if (source.GetPreferredTransform() != NULL)
		{
			const FriendlyTransformWrapper tag_to_npcf_transform(*source.GetPreferredTransform(), "Tag to NPCF");
			const Friendly4DMatrixTransform& tag_to_camera_transform = tag_to_npcf_transform.Get4DMatrixTransform();
			const FriendlyZProjectionTransform& camera_to_npcf_transform = tag_to_npcf_transform.GetZProjectionTransform();
//			std::cout << "FriendlyTransformWrapper tag_to_npcf_transform looks like this:" << std::endl;
//			tag_to_npcf_transform.Print(std::cout);
			const FriendlyCameraWrapper npcf_to_image_transform(m_camera, true, "Camera wrapper");
//			Debug_DeleteMeWhenever(tag_to_npcf_transform);
			
			for (int i = 0; i < NUM_BARS; i++)
			{
				std::cout << "DEBUG080123 Dealing with bar " << i << std::endl;
				
				const AnalogueBarSpec& bar_spec = m_tag.GetBarSpecs()[i];
				const FriendlyTransform& bar_to_tag_transform = bar_spec.GetTransform();
				
				// friendly names for our coordinate systems
				enum CoordBase { CB_BAR = 0, CB_TAG, CB_CAMERA, CB_NPCF, CB_IMAGE, CB_ROTATED_IMAGE };
				
				// create an object that packages all the transforms together
				std::cout << "Creating the transforms object" << std::endl;
				MultipleBasisCoords transforms;
				transforms.PushTransform(bar_to_tag_transform);
				transforms.PushTransform(tag_to_camera_transform);
				transforms.PushTransform(camera_to_npcf_transform);
				transforms.PushTransform(npcf_to_image_transform);
				
				// now apply these transforms to the left and right extrema
				std::cout << "DEBUG080123 Creating right_extremum" << std::endl;
				MultipleBasisCoords right_extremum(transforms, CB_BAR, FPoint(bar_spec.GetMaxVal(), (bar_spec.GetMinY() + bar_spec.GetMaxY()) / 2, bar_spec.GetZ()));
				std::cout << "DEBUG080123 Creating left_extremum" << std::endl;
				MultipleBasisCoords left_extremum(transforms, CB_BAR, FPoint(bar_spec.GetMinVal(), (bar_spec.GetMinY() + bar_spec.GetMaxY()) / 2, bar_spec.GetZ()));
				
				// work out the transform required to rotate the image such that:
				//  - the bar's x-axis is parallel to the image's x-axis
				//  - the left extremum's x-coordinate is less than the right extremum's (i.e. left really is left!)
				std::cout << "DEBUG080123 Getting image-to-rotated-image transform(" << left_extremum[CB_IMAGE] << ", " << right_extremum[CB_IMAGE] << ")" << std::endl;
				const FriendlyXYRotationTransform image_to_rotated_image_transform = GetImageToRotatedImageTransform(left_extremum[CB_IMAGE], right_extremum[CB_IMAGE]);
				std::cout << "DEBUG080123 Calling transforms.PushTransform" << std::endl;
				transforms.PushTransform(image_to_rotated_image_transform);
				std::cout << "DEBUG080123 Called right_extremum.PushTransform" << std::endl;
				right_extremum.PushTransform(image_to_rotated_image_transform);
				std::cout << "DEBUG080123 Calling left_extremum.PushTransform" << std::endl;
				left_extremum.PushTransform(image_to_rotated_image_transform);
				
				std::cout << "DEBUG080129 The transforms are as follows:" << std::endl;
				transforms.Print(std::cout);
				
				// trace along the bar in rotated-image coordinates until we get to:
				//  - either the right extremum (in which case bar value is 100%)  <-- TODO: deal with this
				//  - or the black-into-white boundary
				// TODO: may need to deal with special case of a value of 0%
				MultipleBasisCoords working_coords = left_extremum;
				bool keep_going = true;
				bool left_pixel;
				bool right_pixel = image.GetPixel(left_extremum[CB_IMAGE]);
				while (keep_going)
				{
					std::cout << "DEBUG080123 Keeping going" << std::endl;
					
					// move one pixel along the x-axis in the rotated image, then transform to the unrotated image's coordinate system
					// (FIXME because unintuitive) N.B. yes, because of my backwards tag I do mean -1 for the time being!!
					std::cout << "DEBUG080123 Keeping going" << std::endl;
					FPoint prev_coords = working_coords[CB_ROTATED_IMAGE];
					std::cout << "DEBUG080123 Keeping going" << std::endl;
					working_coords.SetCoords(CB_ROTATED_IMAGE, FPoint(prev_coords.x() - 1, prev_coords.y(), prev_coords.z()));
//					rotated_image_to_image_transform.Apply(working_rotated_image_coords, working_image_coords);

					std::cout << "DEBUG080129 Working with rotated-image coords " << working_coords[CB_ROTATED_IMAGE] << ", " <<
					             "image coords " << working_coords[CB_IMAGE] << std::endl;
					
					// DEBUG START
//					std::cout << "Working rotated image coords: " << working_rotated_image_coords << std::endl;
//					std::cout << "Working image coords: " << working_image_coords << std::endl;
					// DEBUG END
					
					// sample the unrotated image at these coordinates
//					std::cout << "Sampling analogue tag at " << working_image_coords << std::endl;
					std::cout << "DEBUG080123 Keeping going" << std::endl;
					left_pixel = right_pixel;
					right_pixel = image.GetPixel(working_coords[CB_IMAGE]);
					
					std::cout << "DEBUG080129 Left pixel is " << left_pixel << ", right pixel is " << right_pixel << std::endl;
					
					// stop if we've reached the black-into-white boundary
					std::cout << "DEBUG080123 Keeping going" << std::endl;
					keep_going = !(!left_pixel && right_pixel);
				}
				MultipleBasisCoords& finished_coords = working_coords;
				
				std::cout << "We stopped at:" << std::endl
				          << "    rotated image " << finished_coords[CB_ROTATED_IMAGE] << std::endl
				          << "    image " << finished_coords[CB_IMAGE] << std::endl
				          << "    NPCF " << finished_coords[CB_NPCF] << std::endl
				          << "    camera " << finished_coords[CB_CAMERA] << std::endl
				          << "    tag " << finished_coords[CB_TAG] << std::endl
				          << "    bar " << finished_coords[CB_BAR] << std::endl;
				
				// populate the object to be returned
				dest.SetTagPoint(i, finished_coords[CB_TAG]);
				dest.SetValid(true);
				
				// DEBUG
				const boost::array<const FPoint*, NUM_BARS>& tag_points = dest.GetTagPoints();
				assert(*tag_points[i] == finished_coords[CB_TAG]);
				// END DEBUG
			}
			
			return true;
		}
		else
		{
			return false;
		}
	}

}
