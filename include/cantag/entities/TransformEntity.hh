/**
 * $Header$
 */

#ifndef TRANSFORM_ENTITY_GUARD
#define TRANSFORM_ENTITY_GUARD

#include <cantag/entities/Entity.hh>
#include <cantag/Transform.hh>


namespace Cantag {
  class TransformEntity : public Entity  {
  private:
    std::list<Transform*> m_transforms;
    Transform* m_preferredTransform;
  public:
    TransformEntity() : Entity() {}
    ~TransformEntity() {
      for(std::list<Transform*>::const_iterator i = m_transforms.begin(); i != m_transforms.end(); ++i) {
	delete *i;
      }
    };

    inline std::list<Transform*>& GetTransforms() { return m_transforms; }
    inline const std::list<Transform*>& GetTransforms() const { return m_transforms; }
    inline Transform* GetTransform() { return *(m_transforms.begin()); }
    inline const Transform* GetTransform() const { return *(m_transforms.begin()); }
  private:
    TransformEntity(const TransformEntity& copyme) {}
  };
}
#endif//TRANSFORM_ENTITY_GUARD
