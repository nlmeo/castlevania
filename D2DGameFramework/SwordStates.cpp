#include "SwordStates.h"
#include "Soma.h"

void SwordStates::SetOwner(GamePlay::GameObject* pObj)
{
	mOwner = static_cast<Fly_Sword*>(pObj);
	InitCachedComponents();
}

void SwordStates::InitCachedComponents()
{
	if (mOwner)
	{
		mFSM = mOwner->GetComponent<GamePlay::FSM>();
		mAnimator = mOwner->GetComponent<GamePlay::Animator>();
		mAttributes = &mOwner->GetAttributes();
		mTransform = &mOwner->transform();
	}
}

