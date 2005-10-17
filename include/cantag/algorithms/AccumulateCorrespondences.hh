/*
  Copyright (C) 2005 Andrew C. Rice

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
 */

#ifndef ACCUMULATECORRESPONDENCE_GUARD
#define ACCUMULATECORRESPONDENCE_GUARD

#include <set>

#include <cantag/Config.hh>
#include <cantag/Function.hh>
#include <cantag/entities/TransformEntity.hh>
#include <cantag/entities/DecodeEntity.hh>
#include <cantag/Correspondence.hh>
#include <cantag/TagDictionary.hh>


namespace Cantag {

  template<int PAYLOAD_SIZE>
  class AccumulateCorrespondences : public Function<TL1(TransformEntity),TL1(DecodeEntity<PAYLOAD_SIZE>)>
  {
  private:
    mutable float m_x_est;
    mutable float m_y_est;
    mutable float m_z_est;

    mutable float m_theta_est;
    mutable float m_phi_est;
    mutable float m_psi_est;

    mutable int m_n_est;



    std::list<Correspondence>& m_corr;
    Transform& m_average;
    const TagDictionary<PAYLOAD_SIZE,TL3(LocationElement,PoseElement,SizeElement)>& m_dictionary;

  public:
    AccumulateCorrespondences(std::list<Correspondence>& corr, 
			      Transform& average_transform, 
			      const TagDictionary<PAYLOAD_SIZE,TL3(LocationElement,PoseElement,SizeElement)>& dictionary) 
      : m_corr(corr), 
      m_average(average_transform), 
      m_dictionary(dictionary),
      m_x_est(0.0), 
      m_y_est(0.0), 
      m_z_est(0.0), 
      m_theta_est(0.0), 
      m_phi_est(0.0), 
      m_psi_est(0.0), 
      m_n_est(0) {}


    AccumulateCorrespondences(const AccumulateCorrespondences &ac)
      : m_corr(ac.corr),
      m_average(ac.m_average),
      m_dictionary(ac.dictionary),
      m_x_est(ac.m_x_est), 
      m_y_est(ac.m_y_est), 
      m_z_est(ac.m_z_est), 
      m_theta_est(ac.m_theta_est), 
      m_phi_est(ac.m_phi_est), 
      m_psi_est(ac.m_psi_est), 
      m_n_est(ac.m_n_est) {}

    bool operator()(const TransformEntity& trans, DecodeEntity<PAYLOAD_SIZE>& decode) const;
  };




  template<int PAYLOAD_SIZE> bool AccumulateCorrespondences<PAYLOAD_SIZE>::operator()(const TransformEntity& trans, DecodeEntity<PAYLOAD_SIZE>& decode) const {
    // See if we have the transform Tag->World for this tag
    //    const typename TagDictionary<PAYLOAD_SIZE,TL3(LocationElement,PoseElement,SizeElement)>::Element* element = 
    //  m_dictionary.GetInformation((*(decode.GetPayloads().begin()))->payload);
    const LocationElement* loc_lookup = m_dictionary.GetInformation((*(decode.GetPayloads().begin()))->payload);
    const PoseElement* pose_lookup = m_dictionary.GetInformation((*(decode.GetPayloads().begin()))->payload);
    const SizeElement* size_lookup = m_dictionary.GetInformation((*(decode.GetPayloads().begin()))->payload);
    if (loc_lookup && pose_lookup && size_lookup) {
     
      SizeElement tmp;
      tmp.tag_size=1.0;
      Transform tagToWorld(*loc_lookup, *pose_lookup, tmp);

      const Cantag::Transform *t = trans.GetPreferredTransform();
      
      // take a copy, scale and invert to get camera->Tag in world units
      Transform cameraToTag;
      for (int i=0; i<16;i++) cameraToTag[i] = (*t)[i];
      cameraToTag[3]*=size_lookup->tag_size;
      cameraToTag[7]*=size_lookup->tag_size;
      cameraToTag[11]*=size_lookup->tag_size;

      cameraToTag.Invert();

      Transform camToWorld = tagToWorld*cameraToTag;

      float a,b,g;
      camToWorld.GetAngleRepresentation(&a,&b,&g);

      m_x_est +=camToWorld[3];
      m_y_est +=camToWorld[7];
      m_z_est +=camToWorld[11];
      
      m_theta_est += a;
      m_phi_est += b;
      m_psi_est += g;

      m_n_est++;

      m_average.SetupFromAngles(
				m_x_est/(float)m_n_est,
				m_y_est/(float)m_n_est,
				m_z_est/(float)m_n_est,
				m_theta_est/(float)m_n_est,
				m_phi_est/(float)m_n_est,
				m_psi_est/(float)m_n_est,
				1.0
				);

      // Now figure out the correspondences
      // Want to map tag (0,0) into camera frame
      // then compute NPCF equivalent
      float xx, yy;
      t->Apply((float)0.0,(float)0.0,(float)0.0,&xx,&yy);
      m_corr.push_back(Correspondence(xx,yy,loc_lookup->x,loc_lookup->y,loc_lookup->z));

    }
    else std::cout << "No match " << (*(decode.GetPayloads().begin()))->payload.to_ulong() << std::endl;
  }


}
#endif//ACCUMULATECORRESPONDENCE_GUARD
