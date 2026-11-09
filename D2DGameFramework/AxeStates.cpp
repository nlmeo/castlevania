#include "AxeStates.h"
#include "./GamePlay/GameObject.h"

void AxeStates::SetOwner(GamePlay::GameObject* pObj)
{
	mOwner = static_cast<Axe*>(pObj);
	InitCachedComponents();
}

void AxeStates::InitCachedComponents()
{
	if (mOwner)
	{
		mFSM = mOwner->GetComponent<GamePlay::FSM>();
		mAnimator = mOwner->GetComponent<GamePlay::Animator>();
		mAttributes = &mOwner->GetAttributes();
	}
}
