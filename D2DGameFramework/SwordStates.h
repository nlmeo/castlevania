#pragma once
#include "Fly_Sword.h"

class SwordStates : public GamePlay::StateNode
{
public:
	virtual void OnEnter() = 0;
	virtual void OnUpdate(float delta_time = 0) = 0;
	virtual void OnQuit() = 0;
	virtual void SetOwner(class GamePlay::GameObject* pObj);
protected:
	void InitCachedComponents();
	Fly_Sword* mOwner = nullptr;
	Attributes* mAttributes = nullptr;
	GamePlay::FSM* mFSM = nullptr;
	GamePlay::Transform* mTransform = nullptr;
	GamePlay::Animator* mAnimator = nullptr;
	//GamePlay::GameObject* mAttackCollider = nullptr;
};
#pragma once
