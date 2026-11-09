#include "GameObject.h"

namespace GamePlay
{
	Component::Component(GameObject* pObject)
	{
		mType = COMTYPE::ComType_None;
		mOwner = pObject;
	}
	Transform* Component::transform() const
	{
		return &mOwner->transform();
	}


	bool ComponentClassType::mIsInitComponent = false;
	std::map<std::string, emComponentType> ComponentClassType::mComponentClassType;

	#define RegType(t) RegComponentClassType(typeid(t).name(),emComponentType::ComType_##t)

	void ComponentClassType::InitAllComponent()
	{
		RegType(Transform);
		RegType(SpriteRender);
		RegType(Camera);
		RegType(Collider);
		RegType(BoxCollider);
		RegType(SphereCollider);
		RegType(RotationBoxCollider);
		RegType(AudioSource);
		RegType(RenderComponent);
		RegType(VideoPlayer);
		RegType(Animator);
		RegType(FSM);
		RegType(AfterImageTrail);
		RegType(HitFlash);
		RegType(AttackCollider);
	}

	emComponentType ComponentClassType::GetComponentClassType(std::string type_name)
	{
		if (!mIsInitComponent)
		{
			InitAllComponent();
			mIsInitComponent = true;
		}

		auto t = mComponentClassType.find(type_name);
		if (t != mComponentClassType.end())
		{
			return t->second;
		}
		return emComponentType::ComType_None;
	}

	void ComponentClassType::RegComponentClassType(std::string type_name, emComponentType t)
	{
		mComponentClassType[type_name] = t;
	}


	RenderComponent::RenderComponent(GameObject* pobj):Component(pobj)
	{
		mType = COMTYPE::ComType_RenderComponent;
	}
}