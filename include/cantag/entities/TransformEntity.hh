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
 */

#ifndef TRANSFORM_ENTITY_GUARD
#define TRANSFORM_ENTITY_GUARD

#include <cantag/entities/Entity.hh>
#include <cantag/Transform.hh>


namespace Cantag {
  class CANTAG_EXPORT TransformEntity : public Entity  {
  private:
    std::list<Transform*> m_transforms;
  public:
    TransformEntity() : Entity() {}
    ~TransformEntity() {
      for(std::list<Transform*>::const_iterator i = m_transforms.begin(); i != m_transforms.end(); ++i) {
	delete *i;
      }
    };

    inline std::list<Transform*>& GetTransforms() { return m_transforms; }
    inline const std::list<Transform*>& GetTransforms() const { return m_transforms; }
    inline const Transform* GetPreferredTransform() const { 
      float best = -1e10f;
      Transform* chosen = NULL;
      for(std::list<Transform*>::const_iterator i = m_transforms.begin();i!=m_transforms.end();++i) {
	if ((*i)->GetConfidence() >= best) {
 	  chosen = *i;
 	  best = (*i)->GetConfidence();
 	}
       }
      return chosen;
    }
    inline Transform* GetPreferredTransform() { 
      float best = 0.f;
      Transform* chosen = NULL;
      for(std::list<Transform*>::iterator i = m_transforms.begin();i!=m_transforms.end();++i) {
	if ((*i)->GetConfidence() >= best) {
 	  chosen = *i;
 	  best = (*i)->GetConfidence();
 	}
       }
      return chosen;
    }

    inline const Transform* GetTransform() const { return *(m_transforms.begin()); }
  private:
    TransformEntity(const TransformEntity& copyme) {}
  };
}
#endif//TRANSFORM_ENTITY_GUARD
